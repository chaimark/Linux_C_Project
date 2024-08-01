#include "NetProt_Module.h"
#include "ATCmd_FunctionSum.h"
#include "NumberBaseLib.h"
#include "Uart.h"
#include "RTC.h"
#include "AT24C0256.h"
#include "MQTT_JSON.h"
#include "WT_MQTT_JSON.h"
#include "Gpio.h"
#include "main.h"
#include "StrLib.h"
#include "Define.h"

bool SetDevATCMDModel_ThroughSendData(void);
void setNetArgumentInit(void);
void MQTT_NET_10mS_Timers_Add(void);
bool copyDataForUART(void);

// 检查当前设备 TCP 是否连接
bool TCPLinkOnlineFlga() {
    bool ATConfig_Flag = false;
    if (AT24CXX_Manager.TCPCheckENableFlag == false) {
        return true;
    }

    for (int i = 0; i < 10; i++) {
        if (SetDevATCMDModel_ThroughSendData()) {
            break;
        }
    }
    char cmdStr[2][2][20] = {{"AT+SOCKLKA3\r\n", "CONNECTED"}, {"AT+ENTM\r\n", "\r\n+OK"}};
    for (int i = 0; i < 2; i++) {
        ClearNetDataBuff();
        sendDataByNetProt((unsigned char *)cmdStr[i][0], strlen(cmdStr[i][0]));
        FL_DelayMs(500);
        if (!copyDataForUART()) {
            break;
        }
        if ((i != 0) && (myStrstr(Now_NetDevParameter.NetDataBuff, cmdStr[i][1], Now_NetDevParameter.NetDataBuff_NowLen) == NULL)) {
            sendDataByNetProt((unsigned char *)cmdStr[i][0], strlen(cmdStr[i][0]));
            break; // 没有退出 AT 最后发一次
        }
        if (myStrstr(Now_NetDevParameter.NetDataBuff, cmdStr[i][1], Now_NetDevParameter.NetDataBuff_NowLen) != NULL) {
            ATConfig_Flag = (i == 0 ? true : ATConfig_Flag);
        } else {
            ATConfig_Flag = false; // 没链上TCP
        }
    }
    return ATConfig_Flag;
}
// 判断是否连接上MQTT
bool isMQTTLinkOnleng(void) {
    if (AT24CXX_Manager.TCPCheckENableFlag == false) {
        return true;
    }
    return true;
}

// 处理用户请求/命令
bool doingUserRequest(void) {
    return true;
}
// 处理模组回复,判断指令是否发送成功，或者模组再忙等待返回等操作
bool doingATCmdResult(NetDevATCmd NowATCmd) {
    return NowATCmd.DoingATCmdResData(&NowATCmd);
}
// 发送ATCmd
bool sendATCmdData(NetDevATCmd NowATCmd) {
    char Temp[500] = {0};
    strnew ATCmd_SendBUFF = NEW_NAME(Temp);

    for (int SendCount_i = 0; SendCount_i < NowATCmd.CmdSendCount; SendCount_i++) {
        // 清理接收缓存
        ClearNetDataBuff();
        // 装载指令
        NowATCmd.DataInstallation(ATCmd_SendBUFF, &NowATCmd);
        // 发送指令
        sendDataByNetProt((unsigned char *)ATCmd_SendBUFF.Name._char, strlen(ATCmd_SendBUFF.Name._char));
        FL_DelayMs(500);
        for (int ResCount_i = 0; ResCount_i < NowATCmd.CmsResCount; ResCount_i++) {
            // 模组是否回复
            if (!copyDataForUART()) {
                // 判断是否可以跳过
                if ((NowATCmd.DoingATReceiveFunName == ResetTrueFlagDoingFun) && ((ResCount_i + 1) == NowATCmd.CmsResCount)) {
                    return true; // 发送结束也没收到回复，直接跳过
                }
                continue;
            }
            // 处理模组的回复
            if (doingATCmdResult(NowATCmd) == false) {
                return false; // AT 指令发送失败
            }
            // 该指令是否需要上位机回复，如果不需要直接回复 true
            if (NowATCmd.IsGetComputerResFlag == false) {
                return true; // 上位机回复成功
            }
            return doingUserRequest(); // 上位机回复成功
        }
    }
    return true; // AT 指令发送限制为0,直接跳过该指令
}

void MOTT_Net_Task(void) {
    if (Now_NetDevParameter.CheckTCPLinkFlag == true) { // 检查当前设备 TCP 是否连接
        if (!TCPLinkOnlineFlga()) {                                                 // 检查当前设备 TCP 是否连接
            Now_NetDevParameter.NowNetOnlineFlag = false; // 标记不在线，下次运行这个任务时重启
            Now_NetDevParameter.NowTCPLinkFlag = false;   // 标记不在线，下次运行这个任务时重启
            Now_NetDevParameter.ReBootCount++;
        } else
            Now_NetDevParameter.CheckTCPLinkFlag = false; // 已在线，不用再检查TCP
    } else if (Now_NetDevParameter.CheckOnlineFlag == true) { // 检查当前设备 network 是否在线
        if (!isMQTTLinkOnleng()) {
            Now_NetDevParameter.NowNetOnlineFlag = false; // 标记不在线，下次运行这个任务时重启
            Now_NetDevParameter.NowTCPLinkFlag = false;   // 标记不在线，下次运行这个任务时重启
            Now_NetDevParameter.ReBootCount++;
        } else
            Now_NetDevParameter.CheckOnlineFlag = false; // 已在线，不用再检查 network
    }
    // 不在线或无连接，重新联网通信
    if ((Now_NetDevParameter.NowNetOnlineFlag == false) || (Now_NetDevParameter.NowTCPLinkFlag == false)) {
        setNetArgumentInit();
        // 设置模组进入 AT 模式
        for (int i = 0; i < 10; i++) {
            if (SetDevATCMDModel_ThroughSendData()) {
                break;
            }
        }
        int NowStep = 0;
        while (NowStep != -1) {
            // 一个是否执行的开关，后续可扩展为指令控制
            if (NetDevice_ATData[NowStep].RunFlag) {
                for (int i = 0; i < 3; i++) {
                    if (sendATCmdData(NetDevice_ATData[NowStep])) {
                        break;
                    }
                    // 联网遇到问题，中断联网进程
                    if (i == 2) {
                        Now_NetDevParameter.CheckTCPLinkFlag = false; // TCP 连接失败，不需要检查，直接准备重启
                        Now_NetDevParameter.CheckOnlineFlag = false;  // TCP 连接失败，不需要检查，直接准备重启
                        Now_NetDevParameter.NowNetOnlineFlag = false; // 标记不在线，下次运行这个任务时重启
                        Now_NetDevParameter.NowTCPLinkFlag = false;   // 标记不在线，下次运行这个任务时重启
                        Now_NetDevParameter.ReBootCount++;            // 重连一次
                        printf("The network connection has been interrupted : %s\r\n",NetDevice_ATData[NowStep].ATCmd);
                        return;
                    }
                }
            }
            NowStep = NetDevice_ATData[NowStep].Next_CmdID;
        }
        // 检查当前设备 TCP 是否连接，低电平有效
        if (TCPLinkOnlineFlga()) {
            Now_NetDevParameter.CheckTCPLinkFlag = false; // TCP 连接成功后暂时不需要检查
            Now_NetDevParameter.CheckOnlineFlag = false;  // TCP 连接成功后暂时不需要检查
            Now_NetDevParameter.NowTCPLinkFlag = true;    // TCP 已连接
            printf("Successful TCP connection\r\n");
            // 判断 MQTT 是否在线
            if (isMQTTLinkOnleng()) {
                Now_NetDevParameter.NowNetOnlineFlag = true; // 设备已在线
                Now_NetDevParameter.ReBootCount = 0;         // 连接成功，重器计数清零
                // JSON_Send_GW_Infor((Now_NetDevParameter.ReBootCount > 0 ? true : false));
                printf("Now the gateway is online\r\n");
            }
            Now_NetDevParameter.ReBootCount = 0; // 复位重启计数器
        }
        ClearNetDataBuff();
    } else { // 设备在线进入数据透传模式
        if (Now_NetDevParameter.MQTT_NET_Receive_checkTime == 0) {
            Now_NetDevParameter.MQTT_NET_Receive_checkTime = BuffcheckTime; // 10*BuffcheckTime ms check 一次，后获取数据
            if (copyDataForUART()) { // 从 UART0Ddata 获取数据
                char * AddrStart = NULL;
                if (strstr(Now_NetDevParameter.NetDataBuff, "{\"gw\":") != NULL) {
                    if (NULL == (AddrStart = strstr(Now_NetDevParameter.NetDataBuff, "{\"gw\":"))) {
                        ClearNetDataBuff();
                        return;
                    }
                    if (NULL == strstr(AddrStart, "}}")) {
                        return;
                    }
                    printf("Received Command of HuiYun\r\n");
                    MQTT_JSON_Analysis(AddrStart); // 解析收到的数据，找到 josn 数据
                    ClearNetDataBuff();
                } else if (strstr(Now_NetDevParameter.NetDataBuff, "[{") != NULL) {
                    if (NULL == (AddrStart = strstr(Now_NetDevParameter.NetDataBuff, "[{"))) {
                        ClearNetDataBuff();
                        return;
                    }
                    if (NULL == strstr(AddrStart, "}]")) {
                        return;
                    }
                    printf("Received Command of HuiYun\r\n");
                    WT_MQTT_JSON_Analysis(AddrStart); // 解析收到的数据，找到 josn 数据
                    ClearNetDataBuff();
                }
            }
        }
    }
}

void MQTT_NET_10mS_Timers_Add(void) {
    if ((Now_NetDevParameter.LineCheckTime = (Now_NetDevParameter.LineCheckTime > 0 ? (Now_NetDevParameter.LineCheckTime - 1) : 0)) == 0) {
        Now_NetDevParameter.LineCheckTime = 6000;       // (LineCheckTime/100) 秒后，判断你是否处于连接状态
        Now_NetDevParameter.CheckOnlineFlag = true;     // 检查网络在线标记
        Now_NetDevParameter.CheckTCPLinkFlag = true;    // 检查TCP连接标记
    }
    // 每 10ms 计数一次
    ((Now_NetDevParameter.MQTT_NET_Receive_checkTime != 0) ? (Now_NetDevParameter.MQTT_NET_Receive_checkTime--) : 0);
}
// 搬运串口数据
bool copyDataForUART(void) {
    if (UART0Ddata.RxLen != 0) {
        NVIC_DisableIRQ(UART0_IRQn); // 关闭中断使能
        if (Now_NetDevParameter.NetDataBuff_NowLen + UART0Ddata.RxLen <= ARR_SIZE(Now_NetDevParameter.NetDataBuff)) {                                                                                                                                 // 追加到 Now_NetDevParameter.NetDataBuff
            memcpy(&Now_NetDevParameter.NetDataBuff[Now_NetDevParameter.NetDataBuff_NowLen], (char *)UART0Ddata.RxBuf, UART0Ddata.RxLen); // 接收数据
            Now_NetDevParameter.NetDataBuff_NowLen += UART0Ddata.RxLen;
        } else {
            memset(Now_NetDevParameter.NetDataBuff, 0, ARR_SIZE(Now_NetDevParameter.NetDataBuff)); // 释放 HTTPBuff_p
            memcpy(Now_NetDevParameter.NetDataBuff, (char *)UART0Ddata.RxBuf, UART0Ddata.RxLen);   // 接收数据
            Now_NetDevParameter.NetDataBuff_NowLen = UART0Ddata.RxLen;
        }
        UART0Ddata.RxLen = 0;
        NVIC_EnableIRQ(UART0_IRQn); // 开启中断使能
        return true;
    } else {
        return false;
    }
}

// 设置模组处于AT模式
bool SetDevATCMDModel_ThroughSendData(void) {
    // 先复位模组 ==========
    NET_RST_OFF;
    FL_DelayMs(500); // 拉低 500 ms
    NET_RST_ON;
    FL_DelayMs(2000); // 延时等待重启结束
    copyDataForUART();
    bool ATConfig_Flag = true;
    // 进入AT模式 ==========
    // {"AT+ENTM\r\n", "\r\n+OK"}
    char cmdStr[2][2][20] = {{"+++", "a"}, {"a", "+ok"}};
    for (int i = 0; i < 2; i++) {
        ClearNetDataBuff(); // 释放 HTTPBuff_p
        sendDataByNetProt((unsigned char *)cmdStr[i][0], strlen(cmdStr[i][0]));
        FL_DelayMs(500);
        if (copyDataForUART()) {
            if (myStrstr(Now_NetDevParameter.NetDataBuff, cmdStr[i][1], Now_NetDevParameter.NetDataBuff_NowLen) != NULL) {
                ATConfig_Flag = true;
                continue;
            } else {
                ATConfig_Flag = false; // 无法进入AT模式，直接退出，等待下一次重启
                break;
            }
        } else {
            if (i == 0) {
                sendDataByNetProt("\r\n", strlen("\r\n"));
                FL_DelayMs(500);
                sendDataByNetProt("AT+ENTM\r\n", strlen("AT+ENTM\r\n"));
                ATConfig_Flag = false; // 无法进入AT模式，直接退出，等待下一次重启
            }
            break;
        }
    }
    if (ATConfig_Flag) {
        sendDataByNetProt("AT+VER\r\n", strlen("AT+VER\r\n"));
        FL_DelayMs(500);
        ATConfig_Flag = copyDataForUART();
        printf("Successfully entered the AT mode\r\n");
    } else {
        printf("Fail to enter the AT mode\r\n");
    }
    return ATConfig_Flag;
}
// AT 参数初始化
void setNetArgumentInit(void) {
    Now_NetDevParameter.MQTT_NET_Receive_checkTime = 5;
    return;
}
void ClearNetDataBuff(void) {
    memset(Now_NetDevParameter.NetDataBuff, 0, ARR_SIZE(Now_NetDevParameter.NetDataBuff)); // 释放 HTTPBuff_p
    Now_NetDevParameter.NetDataBuff_NowLen = 0;
}
