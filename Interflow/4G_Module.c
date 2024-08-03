#include "4G_Module.h"


// *********************************************************************************
char EC200T_VER[30] = {0};
char EC20T_CCID[30] = {0};
char EC20T_IMEI[30] = {0};
char EC20T_CCLK[22] = {0};
char EC20T_CSQ[5] = {0};
int EC20T_RSSI = 0;
int EC20T_SINR = 0;


// 一些打印
// 打开网络
static const char * QMTOPEN_Str[] = {
    "sucess",	   // 0 打开网络成功
    "err param ",  // 1 参数错误
    "err id",	   // 2 MQTT 标识符被占用
    "err PDP",	   // 3 激活 PDP 失败
    "err url",	   // 4 域名解析失败
    "err disconn", // 5 网络断开导致错误
};

// 连接服务器
static const char * QMTOPEN_CONN_Code[] = {
    "sucess",	 // 0 接受连接
    "err Ver",	 // 1 拒绝连接： 不接受的协议版本
    "err Id",	 // 2 拒绝连接：标识符被拒绝
    "err Host",	 // 3 拒绝连接：服务器不可用
    "err Login", // 4 拒绝连接： 错误的用户名或密码
    "err unLic", // 5 拒绝连接：未授权
};

// status
static const char * QMTSTAT_Code[] = {
    "sucess",		  // 0
    "err open",		  // 1        | 连接被服务器断开或者重置
    "err ping",		  // 2        | 发送 PINGREQ 包超时或者失败
    "err conn",		  // 3        | 发送 CONNECT 包超时或者失败
    "err ack",		  // 4        | 接收 CONNACK 包超时或者失败
    "rec disconn",	  // 5        | 客户端向服务器发送 DISCONNECT 包
    "err send",		  // 6        | 因为发送数据包总是失败，
    "err Host",		  // 7        | 链路不工作或者服务器不可用
    "client disconn", // 8        | 客户端主动断开 MQTT 连接
    // 9~255    | 留作将来使用
};

// *********************************************************************************
// private ***************************************************************************
typedef bool (*MQTT_4G_CMD)(char * data);
typedef struct MQTT_4G_PROCESS_S {
    const char * cmd;				  // 命令
    const char * sucess;				  // 成功返回
    const unsigned int overtime_10ms; // 超时时间
    const int8_t overtime_num;		  // 超时重试次数
    const int8_t sucess_add;		  // 成功后前进步数, 0跳出AT模式
    MQTT_4G_CMD fun_cmd;			  // 自定义命令
    MQTT_4G_CMD fun_sucess;			  // 等待结果
    char * read;						  // 读数据的缓存
} MQTT_4G_PROCESS_T;

// 发送缓存
typedef void (*MQTTSendData)(unsigned char *, unsigned short int);
static MQTTSendData MQTT_4G_TX = Uart0Send;
char MQTT_4G_SendBuf[UART0_MAX];
char MQTT_4G_ReceiveBuf[UART0_MAX];
unsigned short int MQTT_4G_ReceiveBufLen = 0;

unsigned char MQTT_4G_Connect_Flage = 0; // 连接状态：0：断开，1:连接
unsigned char MQTT_4G_History_Confirm = 0;

// 发送函数
static unsigned char MQTT_4G_Has_RST = 1;
// 主机确认标志
static int MQTT_4G_Process_10mS_Delay = 0; // 用于控制AT指令模式下，指令超时
static int MQTT_4G_History_10mS_Delay = 0; // 传输历史数据超时时间
static unsigned char MQTT_4G_History_Repeat_Count = 0;
static unsigned int MQTT_4G_Receive_10mS_Delay = 0;		  // 20ms 一次执行此任务
static unsigned int MQTT_4G_Read_Rssi_10mS_Delay = 0;	  // 在透传模式下1h一次读取信号强度
static unsigned int MQTT_4G_Read_Cclk_10mS_Delay = 0;	  // 1h一次读取系统时钟
static unsigned int MQTT_4G_Heart_Inv_Delay = 0;		  // 心跳间隔
static unsigned int MQTT_4G_Connect_10mS_Delay = 0;		  // 用于确定当前是否连接延时。 CONNECT 后保持5s, 才算连接上
static unsigned int MQTT_4G_OverTime_10mS_Delay = 0;	  // 帧超时
static unsigned int MQTT_4G_Send_OverTime_10mS_Delay = 0; // 发送超时
static unsigned char MQTT_4G_Process_Index = 0xff;		  // 当前 步骤
static unsigned char MQTT_4G_Process_Repeat_Count = 0;	  // 重试次数
static unsigned char MQTT_4G_Process_Reboot_Count = 0;	  // 重启次数

static bool MQTT_4G_Net_Status(char * data);
static bool MQTT_4G_Get_RSSI(char * data);
static bool MQTT_4G_Open_CMD(char * data);
static bool MQTT_4G_Wait_Open(char * data);
static bool MQTT_4G_Conn_CMD(char * data);
static bool MQTT_4G_Wait_Conn(char * data);
static bool MQTT_4G_Sub_CMD(char * data);
static bool MQTT_4G_Wait_Sub(char * data);
static bool MQTT_4G_LastWds_CMD(char * data);
static bool MQTT_4G_Data_CMD(char * data);
static bool MQTT_4G_Wait_Data(char * data);
static void MQTT_4G_Connect_Task(void);

// 没有 cmd  通过  fun_cmd 返回 cmd命令
// 没有sucess,通过 fun_cmd 判断返回
static MQTT_4G_PROCESS_T MQTT_4G_Pro[] = {
    {"AT\r\n", "OK", 200, 3, 1, NULL, NULL, NULL},									//
    {"ATE0\r\n", "OK", 200, 3, 1, NULL, NULL, NULL},								// 关闭回显
    {"ATI\r\n", "Revision: ", 200, 3, 1, NULL, NULL, EC200T_VER},					// 获取版本号
    {"AT+CPIN?\r\n", "READY", 500, 3, 1, NULL, NULL, NULL},							// pin码是否OK                 ,  20s 重启
    {"AT+CREG?\r\n", NULL, 500, 25, 1, NULL, MQTT_4G_Net_Status, NULL},				// 网络注册是否OK              ,  90s 重启
    {"AT+CGREG?\r\n", NULL, 500, 25, 1, NULL, MQTT_4G_Net_Status, NULL},			// 网络注册是否OK              ,  60s 重启
    {"AT+CCID\r\n", "CCID: ", 200, 3, 1, NULL, NULL, EC20T_CCID},					// 获取CCID
    {"AT+QENG=\"servingcell\"\r\n", NULL, 500, 3, 1, NULL, MQTT_4G_Get_RSSI, NULL}, // 获取信号强度
    {"AT+CSQ\r\n", "CSQ: ", 500, 5, 1, NULL, NULL, EC20T_CSQ},						// 获取CSQ
    {"AT+CCLK?\r\n", "CCLK: ", 500, 3, 1, NULL, NULL, EC20T_CCLK},					// 获取时钟
    {"AT+QMTCFG=\"version\",0,4\r\n", "OK", 200, 3, 1, NULL, NULL, NULL},			// 设备MQTT版本
    {"AT+QMTCFG=\"qmtping\",0,60\r\n", "OK", 200, 3, 1, NULL, NULL, NULL},			// 设置心跳间隔
    {"AT+QMTCFG=\"recv/mode\",0,0,1\r\n", "OK", 200, 3, 1, NULL, NULL, NULL},		// 设置消息上报形式
    {NULL, "OK", 2000, 3, 1, MQTT_4G_LastWds_CMD, NULL, NULL},						// will
    {"AT\r\n", "OK", 200, 3, 1, NULL, NULL, NULL},									// 备用
    {NULL, NULL, 2000, 3, 1, MQTT_4G_Open_CMD, MQTT_4G_Wait_Open, NULL},			// 打开网络， "+QMTOPEN: 0,0"    +QMTSTAT: 0,1 要重新打开
    {NULL, NULL, 2000, 3, 1, MQTT_4G_Conn_CMD, MQTT_4G_Wait_Conn, NULL},			// 连接服务器， +QMTCONN: 0,0,0
    {NULL, NULL, 2000, 3, 0, MQTT_4G_Sub_CMD, MQTT_4G_Wait_Sub, NULL},				// 订阅
    {"AT+QENG=\"servingcell\"\r\n", NULL, 200, 3, 0, NULL, MQTT_4G_Get_RSSI, NULL}, // 获取信号强度
    {NULL, NULL, 500, 1, 0, MQTT_4G_Data_CMD, MQTT_4G_Wait_Data, NULL},				// 发送实际数据
};

// 判断网络状态是否正确
static bool MQTT_4G_Net_Status(char * data) {
    if (strstr(data, ",1\r\n") != NULL)
        return true;
    else
        return false;
}

static bool MQTT_4G_Get_RSSI(char * data) {
    char * p_star = data;
    for (int i = 0; i < 16; i++) {
        p_star = strchr(p_star + 1, ',');
        if (i == 12)
            EC20T_RSSI = atoi(p_star + 1);
        if (p_star == NULL) {
            EC20T_RSSI = 0;
            EC20T_SINR = 0;
            return false;
        }
    }

    EC20T_SINR = atoi(p_star + 1);
    return true;
}

static bool MQTT_4G_Open_CMD(char * data) {
    sprintf(data, "AT+QMTOPEN=0,\"%s\",%d\r\n", AT24CXX_Manager.NET_4G_Remote_Url, AT24CXX_Manager.NET_4G_Remote_Port);
    return true;
}

static bool MQTT_4G_Wait_Open(char * data) {
    unsigned char res;
    char * p = strstr(data, "+QMTOPEN");
    if (p != NULL) {
        p = strchr(p, '\r');
        if (p != NULL) {
            res = *(p - 1) - 0x30;
            if (res == 1 && *(p - 2) == '-')
                printf("mqtt open: fail\r\n"); // -1
            else if (res < 6)
                printf("mqtt open: %s\r\n", QMTOPEN_Str[res]);
            if (res == 0 || res == 2)
                return true; // 标识符被占用，说明已经打开
        }
    }
    return false;
}

static bool MQTT_4G_Conn_CMD(char * data) {
    sprintf(data, "AT+QMTCONN=0,\"%01x%02x%02x%02x%02x%02x\",\"%s\",\"%s\"\r\n", AT24CXX_Manager.gw_id[0], AT24CXX_Manager.gw_id[1], AT24CXX_Manager.gw_id[2], AT24CXX_Manager.gw_id[3], AT24CXX_Manager.gw_id[4], AT24CXX_Manager.gw_id[5], AT24CXX_Manager.username, AT24CXX_Manager.password);
    return true;
}

static bool MQTT_4G_Wait_Conn(char * data) {
    unsigned char res;
    char * p = strstr(data, "+QMTCONN");
    if (p != NULL) {
        p = strchr(p, ',');
        if (p != NULL) {
            res = *(p + 1) - 0x30;
            if (res == 0) {
                printf("mqtt conn: sucess\r\n"); // 连接成功
                return true;
            } else if (*(p + 2) == ',') { // 连接服务器失败
                res = *(p + 3) - 0x30;
                if (res < 6)
                    printf("mqtt conn: fail, %s\r\n", QMTOPEN_CONN_Code[res]);
            }
        }
    }
    return false;
}

static bool MQTT_4G_Sub_CMD(char * data) {
    sprintf(data, "AT+QMTSUB=0,1,\"hy/gw/get/%01x%02x%02x%02x%02x%02x/#\",2\r\n", AT24CXX_Manager.gw_id[0], AT24CXX_Manager.gw_id[1], AT24CXX_Manager.gw_id[2], AT24CXX_Manager.gw_id[3], AT24CXX_Manager.gw_id[4], AT24CXX_Manager.gw_id[5]);
    return true;
}

static bool MQTT_4G_Wait_Sub(char * data) {
    unsigned char res;
    char * p = strstr(data, "+QMTSUB");
    if (p != NULL) {
        p = strchr(p, ',');
        if (p != NULL && *(p + 2) == ',') {
            p += 2;
            res = *(p + 1) - 0x30;
            if (res == 0) {
                printf("mqtt sub: sucess\r\n"); // 连接成功
                return true;
            }
            printf("mqtt sub: fail.\r\n");
        }
    }
    return false;
}

// 发送报文
static unsigned char MQTT_4G_Sendcmd[64];
static char MQTT_4G_publish[24];
bool MQTT_4G_Send_Wait(void) {
    MQTT_4G_Send_OverTime_10mS_Delay = 0;
    while (MQTT_4G_Send_OverTime_10mS_Delay < 10) {
        FL_DelayMs(5);
        NVIC_DisableIRQ(UART0_IRQn);
        if (UART0Ddata.RxLen > 0) {
            UART0Ddata.RxBuf[UART0Ddata.RxLen] = 0;
            if ((strstr((char *)UART0Ddata.RxBuf, "\r\n>")) != NULL) {
                NVIC_EnableIRQ(UART0_IRQn);
                return true;
            }
        }
        NVIC_EnableIRQ(UART0_IRQn);
    }
    printf("no1 > \r\n");
    return false;
}

bool MQTT_4G_Send_Wait_after(void) {
    unsigned short int tempLen = UART0Ddata.RxLen;
    MQTT_4G_Send_OverTime_10mS_Delay = 0;
    while (MQTT_4G_Send_OverTime_10mS_Delay < 30) {
        FL_DelayMs(5);
        NVIC_DisableIRQ(UART0_IRQn);
        if (UART0Ddata.RxLen > 0) {
            UART0Ddata.RxBuf[UART0Ddata.RxLen] = 0;
            if ((strstr((char *)(UART0Ddata.RxBuf + tempLen), "QMTPUBEX")) != NULL) {
                NVIC_EnableIRQ(UART0_IRQn);
                return true;
            }
        }
        NVIC_EnableIRQ(UART0_IRQn);
    }
    printf("no2 > \r\n");
    return false;
}

void MQTT_4G_Send_Protocol_To_TTL(char * txBuffer, unsigned short int length) {
    sprintf((char *)MQTT_4G_Sendcmd, "AT+QMTPUBEX=0,0,0,0,\"%s\",%d\r\n", MQTT_4G_publish, length);
    MQTT_4G_TX(MQTT_4G_Sendcmd, strlen((char *)MQTT_4G_Sendcmd));
    MQTT_4G_Send_Wait();
    MQTT_4G_TX((unsigned char *)MQTT_4G_SendBuf, length);
    MQTT_4G_Send_Wait_after();
}

// 特殊处理，发送参数
static bool MQTT_4G_LastWds_CMD(char * data) {
    char addr[16] = {0};
    sprintf(addr, "%01x%02x%02x%02x%02x%02x", AT24CXX_Manager.gw_id[0], AT24CXX_Manager.gw_id[1], AT24CXX_Manager.gw_id[2], AT24CXX_Manager.gw_id[3], AT24CXX_Manager.gw_id[4], AT24CXX_Manager.gw_id[5]);
    sprintf((char *)MQTT_4G_Sendcmd, "AT+QMTCFG=\"willex\",0,1,1,0,\"hy/gw/set/%s/\",74\r\n", addr);
    MQTT_4G_TX(MQTT_4G_Sendcmd, strlen((char *)MQTT_4G_Sendcmd));
    sprintf(data, "{\"gw\":{\"msg_id\":70,\"msg_gw_pr\":0,\"gw_id\":\"%s\"},\"data\":{\"stat\":0}}", addr);
    MQTT_4G_Send_Wait();
    MQTT_4G_TX((unsigned char *)data, 74);
    MQTT_4G_Send_Wait_after();
    return false;
}

static bool MQTT_4G_Data_CMD(char * data) {
    return false;
}

static bool MQTT_4G_Wait_Data(char * data) {
    unsigned char res;
    char * p = strstr(data, "+QMTPUBEX");
    if (p != NULL) {
        p = strchr(p, ',');
        if (p != NULL) {
            res = *(p + 1) - 0x30;
            if (res == 0) {
                printf("mqtt pub: sucess\r\n"); // 连接成功
                return true;
            }
        }
    }
    return false;
}

// 返回下一次重启间隔， 3次以后，2min,每次间隔增加一倍
// 最多间隔 24h
static unsigned int MQTT_4G_Set_ReBoot_Inv(void) {
    unsigned int temp = 12000;
    temp = MQTT_4G_Process_Reboot_Count < 3 ? 1200 : (temp << MQTT_4G_Process_Reboot_Count - 3);
    if (temp >= MQTT_4G_REBOOT_INV_MAX)
        temp = MQTT_4G_REBOOT_INV_MAX;
    else
        MQTT_4G_Process_Reboot_Count++;
    return temp;
}

void MQTT_4G_10mS_Timers_Add(void) {
    MQTT_4G_Receive_10mS_Delay++;
    MQTT_4G_Read_Cclk_10mS_Delay++;
    MQTT_4G_Process_10mS_Delay--;
    MQTT_4G_Read_Rssi_10mS_Delay++;
    // MQTT_4G_Heart_Inv_Delay ++;
    MQTT_4G_Connect_10mS_Delay++;
    MQTT_4G_History_10mS_Delay--;
    MQTT_4G_OverTime_10mS_Delay++;
    MQTT_4G_Send_OverTime_10mS_Delay++;
}
// public **********************************************************************************
void MQTT_4G_Init(void) {
    JSON_Init_Confirm(MQTT_4G_History_Confirm);
    sprintf(MQTT_4G_publish, "hy/gw/set/%01x%02x%02x%02x%02x%02x", AT24CXX_Manager.gw_id[0], AT24CXX_Manager.gw_id[1], AT24CXX_Manager.gw_id[2], AT24CXX_Manager.gw_id[3], AT24CXX_Manager.gw_id[4], AT24CXX_Manager.gw_id[5]);
}

void MQTT_4G_Task(void) {
    char * p_star = NULL; // 开始
    char * p_end = NULL;	 // 结束
    unsigned short int temp;
    unsigned short int tempLen;

    if (MQTT_4G_Receive_10mS_Delay >= 4 ) { // 20ms check 一次
        MQTT_4G_Receive_10mS_Delay = 0;
        // 收到数据
        if (UART0Ddata.RxLen != 0) {
            // 拷贝串口 buf 中的数据
            NVIC_DisableIRQ(UART0_IRQn);
            if (MQTT_4G_ReceiveBufLen + UART0Ddata.RxLen > UART0_MAX) {
                memset(MQTT_4G_ReceiveBuf, 0x00, UART0_MAX);
                MQTT_4G_ReceiveBufLen = 0;
            }
            memcpy(MQTT_4G_ReceiveBuf + MQTT_4G_ReceiveBufLen, UART0Ddata.RxBuf, UART0Ddata.RxLen);
            MQTT_4G_ReceiveBufLen += UART0Ddata.RxLen;
            Clear_UART0_Data();
            UART0Ddata.RxLen = 0;
            NVIC_EnableIRQ(UART0_IRQn);
            MQTT_4G_ReceiveBuf[MQTT_4G_ReceiveBufLen] = 0;
            MQTT_4G_OverTime_10mS_Delay = 0;
            // 接收
            if (strstr((char *)MQTT_4G_ReceiveBuf, "+QMTRECV:") != NULL) {
                EC20_Dealy_GPIO_GSM_On(100);
                if ((p_star = strstr((char *)MQTT_4G_ReceiveBuf, "hy/gw/get/")) != NULL && (p_star = strstr(p_star, "\",")) != NULL) { // 收到完整的 topic
                    p_star += 2;
                    temp = atoi(p_star); // 理论数据长度
                    if ((p_star = strchr(p_star, '"')) != NULL) {				 // 数据开始
                        p_star += 1; // 数据起始地址                     // 判断是否收完
                        if (p_star - MQTT_4G_ReceiveBuf < MQTT_4G_ReceiveBufLen) {																	 // 收到了数据
                            tempLen = MQTT_4G_ReceiveBufLen - (p_star - MQTT_4G_ReceiveBuf); // 收到的数据长度
                            if (tempLen >= temp + 1) { // 数据长度足够
                                if (*(p_star + temp) == '"') { // 数据结构正确
                                    MQTT_JSON_Analysis(p_star);
                                } else {
                                    printf("mqtt no \": %d, %d\r\n", temp, tempLen);
                                }
                                memset(MQTT_4G_ReceiveBuf, 0x00, UART0_MAX);
                                MQTT_4G_ReceiveBufLen = 0;
                            }
                        }
                    }
                }
                // 收到 rec 直接跳到连接模式
                if (MQTT_4G_Process_Index != 0x55) {
                    printf("mqtt connect 1: %s\r\n", AT24CXX_Manager.NET_4G_Remote_Url);
                    MQTT_4G_Heart_Inv_Delay = MQTT_4G_HEART_INV;
                    MQTT_4G_History_Confirm = 2;
                    // 连接
                    MQTT_4G_Connect_Flage = 0xaa; // 状态改为待连接
                    MQTT_4G_Connect_10mS_Delay = 0;
                    EC20_Dealy_GPIO_GSM_On(0);
                    // 清状态
                    MQTT_4G_Process_Reboot_Count = 0; // 重启次数清零
                    MQTT_4G_Read_Rssi_10mS_Delay = 0; // 获取信号强度延时
                    MQTT_4G_Read_Cclk_10mS_Delay = 0;
                    MQTT_4G_Process_Index = 0x55;
                    MQTT_4G_Process_10mS_Delay = 0; // 在AT模式下, 用于计算命令间的延时时间
                }
                MQTT_4G_Process_Repeat_Count = 0;
            } else if (MQTT_4G_Process_Index < MQTT_4G_PROCESS_MAX) {
                // 与反馈的字符一致，则进入下一个命令
                // 没有反馈字符，则通过函数来判断
                if ((MQTT_4G_Pro[MQTT_4G_Process_Index].sucess != NULL && (p_star = strstr(MQTT_4G_ReceiveBuf, MQTT_4G_Pro[MQTT_4G_Process_Index].sucess)) != NULL) || (MQTT_4G_Pro[MQTT_4G_Process_Index].sucess == NULL && MQTT_4G_Pro[MQTT_4G_Process_Index].fun_sucess(MQTT_4G_ReceiveBuf))) {
                    // 如果是获取的数据，则拷贝到目标地址
                    if (MQTT_4G_Pro[MQTT_4G_Process_Index].read != NULL && p_star != NULL) { // 拷贝数据
                        p_star += strlen(MQTT_4G_Pro[MQTT_4G_Process_Index].sucess);
                        p_end = strchr(p_star, '\r');
                        if (p_end != NULL) {
                            temp = p_end - p_star;
                            strncpy(MQTT_4G_Pro[MQTT_4G_Process_Index].read, p_star, temp);
                            MQTT_4G_Pro[MQTT_4G_Process_Index].read[temp] = 0;
                        }
                    }
                    // 一条at指令处理完成
                    if (MQTT_4G_Pro[MQTT_4G_Process_Index].sucess_add != 0) {
                        MQTT_4G_Process_Index += MQTT_4G_Pro[MQTT_4G_Process_Index].sucess_add;
                    } else { // ********************这里跳到透传模式*********************************************************************
                        if (MQTT_4G_Process_Index == MQTT_4G_CONNECT_INDEX) {
                            printf("mqtt connect: %s\r\n", AT24CXX_Manager.NET_4G_Remote_Url);
                            MQTT_4G_Heart_Inv_Delay = MQTT_4G_HEART_INV;
                            MQTT_4G_History_Confirm = 2;
                            MQTT_4G_Connect_Flage = 0xaa; // 状态改为待连接
                            MQTT_4G_Connect_10mS_Delay = 0;
                            EC20_Dealy_GPIO_GSM_On(0);
                        }
                        // 清状态
                        MQTT_4G_Process_Reboot_Count = 0; // 重启次数清零
                        MQTT_4G_Read_Rssi_10mS_Delay = 0; // 获取信号强度延时
                        MQTT_4G_Read_Cclk_10mS_Delay = 0;
                        MQTT_4G_Process_Index = 0x55;
                    }
                    MQTT_4G_Process_Repeat_Count = 0;
                    MQTT_4G_Process_10mS_Delay = 0; // 在AT模式下, 用于计算命令间的延时时间
                    MQTT_4G_ReceiveBufLen = 0;
                }
            } else if (strstr((char *)MQTT_4G_ReceiveBuf, "+QMTSTAT:") != NULL) { // 已经连接上，收到状态改变
                unsigned char res = 1;
                p_star = strchr(MQTT_4G_ReceiveBuf, ',');
                if (p_star != NULL) {
                    res = *(p_star + 1) - 0x30;
                    if (res < 9)
                        printf("mqtt stat: %s\r\n", QMTSTAT_Code[res]);
                }
                // 状态不是0, 则重启
                if (res != 0) {
                    MQTT_4G_Process_Index = 0xff; // 重启模块
                    EC20_Dealy_GPIO_Err_On(1000);
                    MQTT_4G_Process_Reboot_Count = 0; // 收到主动断开，可以直接重启
                }
                MQTT_4G_ReceiveBufLen = 0;
            } else if (strstr((char *)MQTT_4G_ReceiveBuf, "RDY\r\n") != NULL || strstr((char *)MQTT_4G_ReceiveBuf, "ERROR\r\n") != NULL) {
                printf("mqtt RDY/Err\r\n");
                MQTT_4G_Process_Index = 0xff; // 重启模块
                EC20_Dealy_GPIO_Err_On(1000);
                MQTT_4G_Process_Reboot_Count = 0; // 收到主动断开，可以直接重启
                MQTT_4G_ReceiveBufLen = 0;
            }
        } else {
            // 未收到数据
            if (MQTT_4G_ReceiveBufLen != 0 && MQTT_4G_OverTime_10mS_Delay > 100) { // 帧间隔不超过1s
                printf("mqtt frame overtime %d = %s\r\n", MQTT_4G_ReceiveBufLen, MQTT_4G_ReceiveBuf);
                MQTT_4G_ReceiveBufLen = 0;
            }
            // 连接状态延迟
            if (MQTT_4G_Connect_Flage == 0xaa && MQTT_4G_Connect_10mS_Delay > 500) {
                MQTT_4G_Connect_Flage = 1;
                EC20_Dealy_GPIO_GSM_On(100);
            }
            if (MQTT_4G_Process_Index == 0xff) { // 重启模块
                EC20_RST_ON();
                FL_DelayMs(300);
                EC20_RST_OFF();
                MQTT_4G_Process_Index = 0;
                MQTT_4G_Process_Repeat_Count = 0;
                MQTT_4G_Process_10mS_Delay = MQTT_4G_Set_ReBoot_Inv(); // 计算下一次重启间隔
                printf("mqtt reboot = %d\r\n", MQTT_4G_Process_10mS_Delay);
                MQTT_4G_Connect_Flage = 0; // 状态改为断开
                EC20_Dealy_GPIO_GSM_On(0);
            } else if (MQTT_4G_Process_Index < MQTT_4G_PROCESS_MAX) {
                if (MQTT_4G_Process_10mS_Delay <= 0) {
                    if (MQTT_4G_Process_Repeat_Count >= MQTT_4G_Pro[MQTT_4G_Process_Index].overtime_num) {								  // 三次失败重启模块
                        MQTT_4G_Process_Index = 0xff; // 重启模块
                        EC20_Dealy_GPIO_Err_On(1000);
                    } else {
                        MQTT_4G_Process_Repeat_Count++;
                        MQTT_4G_Process_10mS_Delay = MQTT_4G_Pro[MQTT_4G_Process_Index].overtime_10ms;
                        // 发送命令分两种：1,直接发送命令，2,命令较复杂通过函数返回命令
                        if (MQTT_4G_Pro[MQTT_4G_Process_Index].cmd != NULL) {
                            strcpy(MQTT_4G_SendBuf, MQTT_4G_Pro[MQTT_4G_Process_Index].cmd);
                            tempLen = strlen(MQTT_4G_SendBuf);
                            MQTT_4G_TX((unsigned char *)MQTT_4G_SendBuf, tempLen);
                            if (MQTT_4G_Process_Repeat_Count > 1)
                                printf("mqtt retry %d: %s", MQTT_4G_Process_Repeat_Count, MQTT_4G_Pro[MQTT_4G_Process_Index].cmd);
                        } else {
                            if (MQTT_4G_Pro[MQTT_4G_Process_Index].fun_cmd(MQTT_4G_SendBuf)) { // 获取命令成功
                                tempLen = strlen(MQTT_4G_SendBuf);
                                MQTT_4G_TX((unsigned char *)MQTT_4G_SendBuf, tempLen);
                                if (MQTT_4G_Process_Repeat_Count > 1)
                                    printf("mqtt retry %d: %s", MQTT_4G_Process_Repeat_Count, MQTT_4G_SendBuf);
                            }
                        }
                    }
                }
            } else if (MQTT_4G_Read_Rssi_10mS_Delay > MQTT_4G_READ_ISSI_INV && MQTT_4G_ReceiveBufLen == 0) { // 读信号强度
                MQTT_4G_Read_Rssi_10mS_Delay = 0;
                MQTT_4G_Process_Index = MQTT_4G_READ_RSSI_INDEX; // 进入 读信号强度的流程
                MQTT_4G_Process_10mS_Delay = 0;
                MQTT_4G_Heart_Inv_Delay = MQTT_4G_HEART_INV; // 读到以后发心跳
            } else if (MQTT_4G_Read_Cclk_10mS_Delay > MQTT_4G_READ_CCLK_INV && MQTT_4G_ReceiveBufLen == 0) { // 读系统时钟
                MQTT_4G_Read_Cclk_10mS_Delay = 0;
                MQTT_4G_Process_Index = MQTT_4G_READ_CCLK_INDEX; // 进入 读系统时钟的流程
                MQTT_4G_Process_10mS_Delay = 0;
                MQTT_4G_Heart_Inv_Delay = MQTT_4G_HEART_INV; // 读到以后发心跳
                execute_Set_RX8025T_By_MQTT_CCLK_1Second_Count = 1;
            } else if (MQTT_4G_Heart_Inv_Delay >= MQTT_4G_HEART_INV && MQTT_4G_ReceiveBufLen == 0) {
                MQTT_4G_Heart_Inv_Delay = 0;
                JSON_Send_GW_Infor(MQTT_4G_Has_RST);
                MQTT_4G_Has_RST = 0;
            } else if (MQTT_4G_Connect_Flage == 1) { // 确定连接上了
                MQTT_4G_Connect_Task();
            }
        }
    }
}

// 是否已经连接
unsigned char MQTT_4G_IS_Connect(void) {
    return (unsigned char)(MQTT_4G_Connect_Flage == 1);
}

static void MQTT_4G_Connect_Task(void) {
    if (MQTT_4G_History_Confirm == 2 || MQTT_4G_History_10mS_Delay <= 0) {
        if ((MQTT_4G_History_Confirm == 2 && MQTT_4G_History_Repeat_Count != 0) || (MQTT_4G_History_Confirm != 2 && MQTT_4G_History_Repeat_Count >= 3)) {
            MQTT_4G_History_Repeat_Count = 0;
            MQTT_4G_History_Confirm = 2;
        }
    }
}
