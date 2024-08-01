#include "WT_MQTT_JSON.h"
#include "NumberBaseLib.h"
#include "StrLib.h"
#include "Uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "AT24C0256.h"
#include "RTC.h"
#include "TimerCopyReading.h"
#include "TimerMainReading.h"
#include "ImmediatelyReading.h"
#include "JsonDataDoneLib.h"
 // 主表控制数据 // 二网阀请求下发示例： 控制6855 1.3
struct _CtrlMainMeter_Manager {
    int T8;        // 控制指令编码
    int T10;       // 控制指令状态1:执行中,-1:失败,2:成功
    char P103[30]; // 阀门设备编码
    double L20;    // 下发值
    char J79[20];  // 控制指令下发时间 (yyyy-MM-dd HH:mm:ss)
} CtrlMainMeter_Manager;
// 主表数据
struct _InputMainMeter_Manager {
    char MeterID[30];              // 采集表号
    double InputWaterTemperature;  // 供水温度 ℃
    double ReturnWaterTemperature; // 回水温度 ℃
    double TemperatureDifference;  // 温差     ℃
    double InstantaneousHeat;      // 瞬时热量 GJ/h
    double AccumulatedHeat;        // 累计热量 GJ
    double InstantaneousFlow;      // 瞬时流量 m³/h
    double AccumulatedFlow;        // 累计流量 t
    double ValveOpenDeg;           // 阀门开度(92.5%传92.5)
    double setValveOpenDeg;        // 设定阀门开度(92.5%传92.5)
    char Name[30];                 // 固定字段
    char CollectionTime[20];       // 采集时间 (yyyy-MM-dd HH:mm:ss)
} InputMainMeter_Manager;
 // 副表控制数据 // 户阀（温控阀）请求示例： 控制6853 1.2
struct _CtrlCopyMeter_Manager {
    int T8;         // 控制指令编码
    int T10;        // 控制指令状态1:执行中,-1:失败,2:成功
    char R53[30];   // 温控阀编码
    int R64;        // 控制权限(0:公司控制(只能公司控制),1:用户控制(只能用户控制))
    int L32;        // 控制策略(0:开度给定,1:室温控制,2:圆温控制)
    double L20;     // 下发值
    char J79[20];   // 控制指令下发时间 (yyyy-MM-dd HH:mm:ss)
} CtrlCopyMeter_Manager;
// 副表数据
struct _InputCopyMeter_Manager {
    char MeterID[30];              // 温控阀编码
    double SetRoomTemperature;     // 设定温度
    double RoomTemperature;        // 室内温度
    double InputWaterTemperature;  // 供水温度(无数据则为nult)
    double OutputWaterTemperature; // 回水温度
    double ValveOpenDeg;           // 阀门开度(92.5%传92.5)
    double Power;                  // 电量(单位%如50%那么数值就是50)
    char ControlStrategy[30];      // 控制策略(0:开度给定,1:室温控制,2:圆温控制)
    char FaultInformation[100];    // 故障信息
    char CollectionTime[20];       // 采集时间 (yyyy-MM-dd HH:mm:ss)
    char Name[30];                 // 固定字段
} InputCopyMeter_Manager;

void setInputMainMeter_Manager(char * DevIDStartAddrOfBuff) {

    char * TempP = DevIDStartAddrOfBuff;
    char TempHexbufft[30] = {0};
    strnew TempString = NEW_NAME(TempHexbufft);
    strnew IN_Str;
    int64_t TempNum = 0;
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    HEX2ToASCII((char *)TempP, 4, InputMainMeter_Manager.MeterID, (4 * 2)); // 27 10 05 83 00 11 11：仪表表号：27100583 制造商代号：001111        ----chai 1
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    TempP = DevIDStartAddrOfBuff + 12; // 跳转到累计热量 12 23 53 04 05：累计热量： 4532312 kW*h ----chai 6
    swapStr((char *)TempP, 4);
    IN_Str.Name._char = (char *)TempP;
    IN_Str.MaxLen = 4;
    TempNum = shortChStrToDoubleChStr(IN_Str, TempString);
    TempNum = anyBaseArrayToAnyBaseNumber(TempString.Name._char, TempNum, 10, 16);
    TempP = DevIDStartAddrOfBuff + 16;
    switch ((*TempP)) {                                                                      // 识别单位并同时处理 TempNum 的转换
        case 0x05:                                                             // 1 KW*h
            InputMainMeter_Manager.AccumulatedHeat = (double)TempNum / 277.78; // 转换为其他的单位
            break;
        case 0x0E:                                                             // 1 MJ
            InputMainMeter_Manager.AccumulatedHeat = (double)TempNum / 1000.0; // 转换为其他的单位
            break;
        case 0x0F:                                                            // 10 MJ
            InputMainMeter_Manager.AccumulatedHeat = (double)TempNum / 100.0; // 转换为其他的单位
            break;
        case 0x10:                                                           // 100 MJ
            InputMainMeter_Manager.AccumulatedHeat = (double)TempNum / 10.0; // 转换为其他的单位
            break;
    }
    TempNum = 0;
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    TempP = DevIDStartAddrOfBuff + 17; // 跳转到瞬时流量 78 56 34 12 32：瞬时流量：12345.678 m³/h ----chai 7
    swapStr((char *)TempP, 4);
    memset(TempString.Name._char, 0, TempString.MaxLen); // 清空
    IN_Str.Name._char = (char *)TempP;
    IN_Str.MaxLen = 4;
    TempNum = shortChStrToDoubleChStr(IN_Str, TempString);
    if (TempString.Name._char[0] == 0x0F) {
        TempString.Name._char[0] = 0x00;
        TempNum = anyBaseArrayToAnyBaseNumber(TempString.Name._char, TempNum, 10, 16);
        TempNum = 0 - TempNum;
    } else {
        TempNum = anyBaseArrayToAnyBaseNumber(TempString.Name._char, TempNum, 10, 16);
    }
    TempP = DevIDStartAddrOfBuff + 21;
    switch ((*TempP)) {          // 识别单位并同时处理 TempNum 的转换
        case 0x32: // 1 L/h
            InputMainMeter_Manager.InstantaneousFlow = (double)TempNum / 1000.0;
            break;
        case 0x33: // 10 L/h
            InputMainMeter_Manager.InstantaneousFlow = (double)TempNum / 100.0;
            break;
        case 0x34: // 100 L/h
            InputMainMeter_Manager.InstantaneousFlow = (double)TempNum / 10.0;
            break;
    }
    TempNum = 0;
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    TempP = DevIDStartAddrOfBuff + 22; // 跳转到累计流量 78 56 34 00 2A：累计流量：3456.78 m³ ----chai 8
    swapStr((char *)TempP, 4);
    memset(TempString.Name._char, 0, TempString.MaxLen); // 清空
    IN_Str.Name._char = (char *)TempP;
    IN_Str.MaxLen = 4;
    TempNum = shortChStrToDoubleChStr(IN_Str, TempString);
    TempNum = anyBaseArrayToAnyBaseNumber(TempString.Name._char, TempNum, 10, 16);
    TempP = DevIDStartAddrOfBuff + 26;
    switch ((*TempP)) {          // 识别单位并同时处理 TempNum 的转换
        case 0x2A: // 10 L
            InputMainMeter_Manager.AccumulatedFlow = (double)TempNum / 100.0;
            break;
        case 0x2B: // 100 L
            InputMainMeter_Manager.AccumulatedFlow = (double)TempNum / 10.0;
            break;
        case 0x2C: // 1000 L
            InputMainMeter_Manager.AccumulatedFlow = (double)TempNum;
            break;
    }
    TempNum = 0;
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    TempP = DevIDStartAddrOfBuff + 27; // 跳转到进水温度 17 23 00：进水温度：0023.17 ℃        ----chai 2
    swapStr((char *)TempP, 3);
    memset(TempString.Name._char, 0, TempString.MaxLen); // 清空
    IN_Str.Name._char = (char *)TempP;
    IN_Str.MaxLen = 3;
    TempNum = shortChStrToDoubleChStr(IN_Str, TempString);
    InputMainMeter_Manager.InputWaterTemperature = (double)anyBaseArrayToAnyBaseNumber(TempString.Name._char, TempNum, 10, 16) / 100.0;
    TempNum = 0;
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    TempP = DevIDStartAddrOfBuff + 30; // 跳转到回水温度 93 23 00：回水温度：0023.93 ℃        ----chai 3
    swapStr((char *)TempP, 3);
    memset(TempString.Name._char, 0, TempString.MaxLen); // 清空
    IN_Str.Name._char = (char *)TempP;
    IN_Str.MaxLen = 3;
    TempNum = shortChStrToDoubleChStr(IN_Str, TempString);
    InputMainMeter_Manager.ReturnWaterTemperature = (double)anyBaseArrayToAnyBaseNumber(TempString.Name._char, TempNum, 10, 16) / 100.0;
    TempNum = 0;
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    InputMainMeter_Manager.TemperatureDifference = InputMainMeter_Manager.InputWaterTemperature - InputMainMeter_Manager.ReturnWaterTemperature; // 温差    ----chai 4
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    TempP = DevIDStartAddrOfBuff + 38; // 瞬时热量            ----chai 5
    swapStr((char *)TempP, 4);
    memset(TempString.Name._char, 0, TempString.MaxLen); // 清空
    IN_Str.Name._char = (char *)TempP;
    IN_Str.MaxLen = 4;
    TempNum = shortChStrToDoubleChStr(IN_Str, TempString);
    TempNum = anyBaseArrayToAnyBaseNumber(TempString.Name._char, TempNum, 10, 16);
    TempP = DevIDStartAddrOfBuff + 42;
    switch ((*TempP)) {          // 识别单位并同时处理 TempNum 的转换
        case 0x15: // 10 W
            InputMainMeter_Manager.InstantaneousHeat = (double)TempNum / 100.0;
            break;
    }
    InputMainMeter_Manager.InstantaneousHeat /= 277.78; // 转换为GJ
    TempNum = 0;
    //////////////////////////------------------------------------------------------------------------//////////////////////////

    TempP = DevIDStartAddrOfBuff + 47;
    TempNum = TempP[1];
    TempNum = TempNum << 8 | TempP[0];
    InputMainMeter_Manager.ValveOpenDeg = (double)TempNum / 10.0;
    TempNum = 0;
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    TempP = DevIDStartAddrOfBuff + 49;
    TempNum = TempP[1];
    TempNum = TempNum << 8 | TempP[0];
    InputMainMeter_Manager.setValveOpenDeg = (double)TempNum / 10.0;

    char NowTime[20] = {0};
    getStrNowDataAndTimeByRTC(NowTime);
    strcpy(InputMainMeter_Manager.CollectionTime, NowTime); // 采集时间  ----chai 9
}
void setInputCopyMeter_Manager(char * DevIDStartAddrOfBuff) {
    char * TempP = DevIDStartAddrOfBuff;
    int64_t TempNum = 0;
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    HEX2ToASCII((char *)TempP, 4, InputCopyMeter_Manager.MeterID, (4 * 2)); // 27 10 05 83 00 11 11：仪表表号：27100583 制造商代号：001111        ----chai 1
    TempP = DevIDStartAddrOfBuff + 17;                                      // 跳转到室内温度 DC 00：室内温度：0x00DC(HEX)=220(BCD)=+22.0℃ 0 表示+温度 F表示-温度        ----chai 3
    TempNum = TempP[1];
    TempNum = TempNum << 8 | TempP[0];
    InputCopyMeter_Manager.RoomTemperature = (double)TempNum / 10.0;
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    TempP = DevIDStartAddrOfBuff + 19; // 跳转到室内设定温度 DC 00：室内设定温度：0x00DC(HEX)=220(BCD)=22.0℃        ----chai 2
    TempNum = TempP[1];
    TempNum = TempNum << 8 | TempP[0];
    InputCopyMeter_Manager.SetRoomTemperature = (double)TempNum / 10.0;
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    TempP = DevIDStartAddrOfBuff + 22; // 64：当前角度百分比：0x64(HEX)=100(BCD)=100%        ----chai 6 阀门开度
    InputCopyMeter_Manager.ValveOpenDeg = (double)TempP[0];
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    TempP = DevIDStartAddrOfBuff + 31; // 00 F9：进水温度：0x00F9(HEX)=249(BCD)=+24.9℃ 0 表示+温度 F表示-温度        ----chai 4
    TempNum = TempP[0];
    TempNum = TempNum << 8 | TempP[1];
    InputCopyMeter_Manager.InputWaterTemperature = (double)TempNum / 10.0;
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    TempP = DevIDStartAddrOfBuff + 33; // 00 FB：回水温度：0x00FB(HEX)=251(BCD)=+25.1℃ 0 表示+温度 F表示-温度        ----chai 5
    TempNum = TempP[0];
    TempNum = TempNum << 8 | TempP[1];
    InputCopyMeter_Manager.OutputWaterTemperature = (double)TempNum / 10.0;
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    TempP = DevIDStartAddrOfBuff + 44; // B4：阀门锂电池电压：0xB4(HEX)=180(BCD)*2/100=3.6V        ----chai 7
    InputCopyMeter_Manager.Power = ((double)((TempP[0] * 2.0) / 100) / 3.6) * 100;
    strcpy(InputCopyMeter_Manager.ControlStrategy, "0"); // ----chai 8 控制策略(0:开度给定,1:室温控制,2:圆温控制)
    //////////////////////////------------------------------------------------------------------------//////////////////////////
    TempP = DevIDStartAddrOfBuff + 53; // 40：设备状态：(厂商内部)        ----chai 9 非00有故障
                                       // InputCopyMeter_Manager.FaultInformation[0] = (char)TempNum;
    #warning "告警暂时不传";
    InputCopyMeter_Manager.FaultInformation[0] = 0x00;
    // 把设备状态转为字符串
    HEX2ToASCII(InputCopyMeter_Manager.FaultInformation, 1, InputCopyMeter_Manager.FaultInformation, 2);
    char NowTime[20] = {0};
    getStrNowDataAndTimeByRTC(NowTime);
    strcpy(InputCopyMeter_Manager.CollectionTime, NowTime); // 采集时间  ----chai 10
}

// 组织上报仪表数据
void WT_JSON_Send_Main_Copy_Meter_Data(char * JSON_TTL_Buff, unsigned char * data, unsigned char lentgh) {
    WT_MQTT_FLAG = 0; // WT 的指令
    struct _InputMainMeter_Manager ZeroMainData;
    struct _InputCopyMeter_Manager ZeroCopyData;

    InputMainMeter_Manager = ZeroMainData; // 初始化
    InputCopyMeter_Manager = ZeroCopyData; // 初始化

    newJsonList WTMqttJsonData = NEW_JSON_LIST(&WTMqttJsonData);

    char ArrayID[14] = {0};
    HEX2ToASCII(AT24CXX_Manager.gw_id, 6, ArrayID, 12);
    char TEMPSPACE[300] = {0};
    strnew OutputJsonStr = NEW_NAME(TEMPSPACE);
    JsonItem WTRequestData[4] = {0};

    char * TempP = strchr((char *)data, 0x68);
    TempP += 2; // 跳转转到表号

    if ((*(TempP - 1)) == 0x55) {
        if ((TempP[7] != 0x81) || (TempP[8] != 0x3B) || (TempP[9] != 0x90) || (TempP[10] != 0x1F) || (TempP[11] != 0x00)) { // 81 3B 90 1F 00
            return;
        }
        // 组织 WT 1.1
        JsonItem MainlistItem[9] = {0};
        setInputMainMeter_Manager(TempP);
        memcpy(InputMainMeter_Manager.Name, "Value_Hour", strlen("Value_Hour"));
        /************************************************/
        setJsonItemData(&WTRequestData[0], "eo:%s;%d", &ArrayID[1], 0); // 02345678903
        setJsonItemData(&WTRequestData[1], "Type:%d;%d", 2, 0);         // 仪表类型 栋阀
        JsonItem ClassItem = {0};
        setJsonItemData(&WTRequestData[2], "list:%p;%d", ClassItem, 0);                                     // List 数组
        setJsonItemData(&ClassItem, ":%lp;%d", MainlistItem, 1);                                            // List 中的对象
        setJsonItemData(&MainlistItem[0], "R51:%s;%d", InputMainMeter_Manager.MeterID, 2);                  // R51    是	string	    热表编码
        setJsonItemData(&MainlistItem[1], "L21:%lf;%d", InputMainMeter_Manager.InputWaterTemperature, 2);   // L21    是	double	    供水温度
        setJsonItemData(&MainlistItem[2], "L22:%lf;%d", InputMainMeter_Manager.ReturnWaterTemperature, 2);  // L22    是	double	    回水温度
        setJsonItemData(&MainlistItem[3], "L1000:%lf;%d", InputMainMeter_Manager.TemperatureDifference, 2); // L1000  是	double	    温差
        setJsonItemData(&MainlistItem[4], "L26:%lf;%d", InputMainMeter_Manager.InstantaneousHeat, 2);       // L26    是	double	    瞬时热量
        setJsonItemData(&MainlistItem[5], "L33:%lf;%d", InputMainMeter_Manager.AccumulatedHeat, 2);         // L33    是	double	    累计热量
        setJsonItemData(&MainlistItem[6], "L25:%lf;%d", InputMainMeter_Manager.InstantaneousFlow, 2);       // L25    是	double	    瞬时流量
        setJsonItemData(&MainlistItem[7], "L34:%lf;%d", InputMainMeter_Manager.AccumulatedFlow, 2);         // L34    是	double	    累计流量
        setJsonItemData(&MainlistItem[8], "J1000:%s;%d", InputMainMeter_Manager.CollectionTime, 2);         // J1000  是	string	    采集时间(yyyy-MM-dd HH:mm:ss)
        /************************************************/

        WTMqttJsonData.OutPushJsonString(OutputJsonStr, &WTMqttJsonData);
        memcpy(JSON_TTL_Buff, OutputJsonStr.Name._char, strlen(OutputJsonStr.Name._char));
        sendDataByNetProt((unsigned char *)JSON_TTL_Buff, strlen(JSON_TTL_Buff));
        memset(JSON_TTL_Buff, 0, UART0_MAX);
        memset(WTRequestData, 0, sizeof(WTRequestData)); // 清空 WTRequestData
        memset(&ClassItem, 0, sizeof(ClassItem));        // 清空 ClassItem
        memset(MainlistItem, 0, sizeof(MainlistItem));   // 清空 CopylistItem
        /**************************/
        // 组织 WT 1.3
        setJsonItemData(&WTRequestData[0], "Name:%s;%d", InputMainMeter_Manager.Name, 0);
        setJsonItemData(&WTRequestData[1], "eo:%s;%d", &ArrayID[1], 0);                     // 02345678903
        setJsonItemData(&WTRequestData[2], "list:%p;%d", ClassItem, 0);                     // List 数组
        setJsonItemData(&ClassItem, ":%lp;%d", MainlistItem, 1);                            // List 中的对象
        setJsonItemData(&MainlistItem[0], "P103:%s;%d", InputMainMeter_Manager.MeterID, 2); // P103    是	string      温控阀编码
        setJsonItemData(&MainlistItem[1], "J100:%d;%d", 12, 2);                             // 楼前阀
        setJsonItemData(&MainlistItem[2], "L27:%lf;%d", InputMainMeter_Manager.ValveOpenDeg, 2);
        setJsonItemData(&MainlistItem[3], "L627:%lf;%d", InputMainMeter_Manager.setValveOpenDeg, 2);
        /**************************/
    } else {
        if ((TempP[7] != 0x81) || (TempP[8] != 0x2D) || (TempP[9] != 0x90) || (TempP[10] != 0x1F)) { // 81 2D 90 1F
            return;
        }
        // 组织 WT 1.2
        JsonItem CopylistItem[10] = {0};
        setInputCopyMeter_Manager(TempP);
        memcpy(InputCopyMeter_Manager.Name, "tcv_hour", strlen("tcv_hour")); // 户表
        /**************************/
        setJsonItemData(&WTRequestData[0], "Name:%s;%d", InputCopyMeter_Manager.Name, 0);
        setJsonItemData(&WTRequestData[1], "eo:%s;%d", &ArrayID[1], 0); // 02345678903
        JsonItem ClassItem = {0};
        setJsonItemData(&WTRequestData[2], "list:%p;%d", ClassItem, 0);                                    // List 数组
        setJsonItemData(&ClassItem, ":%lp;%d", CopylistItem, 1);                                           // List 中的对象
        setJsonItemData(&CopylistItem[0], "R53:%s;%d", InputCopyMeter_Manager.MeterID, 2);                 // RS3    是	string      温控阀编码
        setJsonItemData(&CopylistItem[1], "S13:%lf;%d", InputCopyMeter_Manager.SetRoomTemperature, 2);     // S13    是	double	    设定温度
        setJsonItemData(&CopylistItem[2], "S3:%lf;%d", InputCopyMeter_Manager.RoomTemperature, 2);         // 53     是	double	    室内温度
        setJsonItemData(&CopylistItem[3], "L21:%lf;%d", InputCopyMeter_Manager.InputWaterTemperature, 2);  // L21    是	double	    供水温度(无数据则为nult)
        setJsonItemData(&CopylistItem[4], "L22:%lf;%d", InputCopyMeter_Manager.OutputWaterTemperature, 2); // L22    是	double	    回水温度
        setJsonItemData(&CopylistItem[5], "L27:%lf;%d", InputCopyMeter_Manager.ValveOpenDeg, 2);           // L27    是	double	    阀门开度(92.5%传92.5)
        setJsonItemData(&CopylistItem[6], "J111:%lf;%d", InputCopyMeter_Manager.Power, 2);                 // J11    是	double	    电量(单位%如50%那么数值就是50)
        setJsonItemData(&CopylistItem[7], "R64:%s;%d", InputCopyMeter_Manager.ControlStrategy, 2);         // R64    是	string	    控制策略(0:开度给定,1:室温控制,2:圆温控制)
        setJsonItemData(&CopylistItem[8], "R65:%s;%d", InputCopyMeter_Manager.FaultInformation, 2);        // R65    是	string	    故障信息
        setJsonItemData(&CopylistItem[9], "J1000:%s;%d", InputCopyMeter_Manager.CollectionTime, 2);        // J1000  是	string	    采集时间(yy-MM-dd HH:mm:ss)
    }

    WTMqttJsonData.OutPushJsonString(OutputJsonStr, &WTMqttJsonData);
    memcpy(JSON_TTL_Buff, OutputJsonStr.Name._char, strlen(OutputJsonStr.Name._char));
    return;
}

// 组织透传数据上报
void WT_JSON_Send_Immediately_Main_Copy_Meter_Data(char * JSON_TTL_Buff, unsigned char * data, unsigned char lentgh) {
    WT_MQTT_FLAG = 0; // WT 的指令
    char TEMPSPACE[300] = {0};
    newJsonList WTMqttJsonData = NEW_JSON_LIST(&WTMqttJsonData);
    strnew OutputJsonStr = NEW_NAME(TEMPSPACE);

    char MetterId[10] = {0};
    char * Start68chr = strchr((char *)data, 0x68);
    char * TempP = Start68chr;
    TempP = Start68chr + 2; // 跳转转到表号

    if ((*(TempP - 1)) == 0x55) {
        HEX2ToASCII((char *)TempP, 4, MetterId, (4 * 2));
        strcpy(CtrlMainMeter_Manager.P103, MetterId);
        int tempT10 = 2;
        tempT10 = (*(Start68chr + 12) == 0x2F ? 2 : (*(Start68chr + 12) == 0x1F ? 1 : -1)); // 获取指令执行结果 

        char NowTime[20] = {0};
        getStrNowDataAndTimeByRTC(NowTime);
        strcpy(CtrlMainMeter_Manager.J79, NowTime); // 回复时间 

        JsonItem WTMainRequestData[6] = {0};
        JsonItem ClassMainItem = {0};
        JsonItem MainlistItem[4] = {0};
        // 组织 WT 1.3 回复
        setJsonItemData(&WTMainRequestData[0], "code:%d;%d", 2, 0);
        setJsonItemData(&WTMainRequestData[1], "list:%p;%d", ClassMainItem, 0);                     // List 数组
        setJsonItemData(&ClassMainItem, ":%lp;%d", MainlistItem, 1);                            // List 中的对象
        setJsonItemData(&MainlistItem[0], "T8:%d;%d", CtrlMainMeter_Manager.T8, 2);
        setJsonItemData(&MainlistItem[1], "T10:%d;%d", tempT10, 2);
        setJsonItemData(&MainlistItem[2], "P103:%s;%d", CtrlMainMeter_Manager.P103, 2);
        setJsonItemData(&MainlistItem[3], "J79:%s;%d", CtrlMainMeter_Manager.J79, 2);
        if (tempT10 == 2) {
            setJsonItemData(&WTMainRequestData[2], "data:%s;%d", "执行成功", 0);
            setJsonItemData(&WTMainRequestData[3], "error:%ls;%d", "null", 0);
        } else if (tempT10 == 1) {
            setJsonItemData(&WTMainRequestData[2], "data:%s;%d", "正在执行", 0);
            setJsonItemData(&WTMainRequestData[3], "error:%ls;%d", "null", 0);
        } else {
            setJsonItemData(&WTMainRequestData[2], "data:%s;%d", "执行异常", 0);
            setJsonItemData(&WTMainRequestData[3], "error:%ls;%d", "未知", 0);
        }
        setJsonItemData(&WTMainRequestData[4], "Total:%d;%d", 1, 0);
        setJsonItemData(&WTMainRequestData[5], "AutoNumberIDs:%ls;%d", "null", 0);
    } else {
        HEX2ToASCII((char *)TempP, 4, MetterId, (4 * 2));
        strcpy(CtrlCopyMeter_Manager.R53, MetterId);

        char NowTime[20] = {0};
        getStrNowDataAndTimeByRTC(NowTime);
        strcpy(CtrlCopyMeter_Manager.J79, NowTime); // 回复时间 

        JsonItem WTCopyRequestData[6] = {0};
        JsonItem ClassCopyItem = {0};
        JsonItem CopylistItem[4] = {0};
        // 组织 WT 1.2 回复
        setJsonItemData(&WTCopyRequestData[0], "list:%p;%d", ClassCopyItem, 0);                     // List 数组
        setJsonItemData(&ClassCopyItem, ":%lp;%d", CopylistItem, 1);                            // List 中的对象
        setJsonItemData(&CopylistItem[0], "T8:%d;%d", CtrlCopyMeter_Manager.T8, 2);
        setJsonItemData(&CopylistItem[1], "T10:%d;%d", 2, 2);
        setJsonItemData(&CopylistItem[2], "R53:%s;%d", CtrlCopyMeter_Manager.R53, 2);
        setJsonItemData(&CopylistItem[3], "J79:%s;%d", CtrlCopyMeter_Manager.J79, 2);
        setJsonItemData(&WTCopyRequestData[1], "code:%d;%d", 1, 0);
        setJsonItemData(&WTCopyRequestData[2], "data:%s;%d", "执行成功", 0);
        setJsonItemData(&WTCopyRequestData[3], "error:%ls;%d", "null", 0);
        setJsonItemData(&WTCopyRequestData[4], "Total:%d;%d", 1, 0);
        setJsonItemData(&WTCopyRequestData[5], "AutoNumberIDs:%ls;%d", "null", 0);
    }
    WTMqttJsonData.OutPushJsonString(OutputJsonStr, &WTMqttJsonData);
    memcpy(JSON_TTL_Buff, OutputJsonStr.Name._char, strlen(OutputJsonStr.Name._char));
    return;
}

// 处理 WT的JSON 指令
void WT_MQTT_JSON_Analysis(char * data) {
    char TempHex[50] = {0};
    char WebCtrlMeterFlag = 0;
    if (strstr(data, "P103") != NULL) {
        WebCtrlMeterFlag = 0x55;
        char * Pend = NULL;
        char * P_TempStart = NULL;
        if ((P_TempStart = strstr(data, "\"T8\":")) != NULL) {
            P_TempStart += strlen("\"T8\":");
            Pend = strchr(P_TempStart, ',');
            *Pend = 0;      // 结束
            CtrlMainMeter_Manager.T8 = atoi(P_TempStart);
            *Pend = ',';    // 复原
        }
        if ((P_TempStart = strstr(data, "\"T10\":")) != NULL) {
            P_TempStart += strlen("\"T10\":");
            Pend = strchr(P_TempStart, ',');
            *Pend = 0;      // 结束
            CtrlMainMeter_Manager.T10 = atoi(P_TempStart);
            *Pend = ',';    // 复原
        }
        if ((P_TempStart = strstr(data, "\"P103\":\"")) != NULL) {
            P_TempStart += strlen("\"P103\":\"");
            Pend = strchr(P_TempStart, '\"');
            *Pend = 0;      // 结束
            strcpy(CtrlMainMeter_Manager.P103, P_TempStart);
            CtrlMainMeter_Manager.P103[strlen(CtrlMainMeter_Manager.P103)] = 0;
            *Pend = '\"';    // 复原
        }
        if ((P_TempStart = strstr(data, "\"L20\":")) != NULL) {
            P_TempStart += strlen("\"L20\":");
            Pend = strchr(P_TempStart, ',');
            *Pend = 0;      // 结束
            CtrlMainMeter_Manager.L20 = doneAsciiToDouble(P_TempStart);
            *Pend = ',';    // 复原
        }
        if ((P_TempStart = strstr(data, "\"J79\":\"")) != NULL) {
            P_TempStart += strlen("\"J79\":\"");
            Pend = strchr(P_TempStart, '\"');
            *Pend = 0;      // 结束
            strcpy(CtrlMainMeter_Manager.J79, P_TempStart);
            CtrlMainMeter_Manager.J79[strlen(CtrlMainMeter_Manager.J79)] = 0;
            *Pend = '\"';    // 复原
        }
        if (strlen(CtrlMainMeter_Manager.P103) != 8) { // 表号不符合标号长度
            return;
        }

        // 组装 68 指令控制被采集设备
        imm_Read.immediately_Exec_Flage = 1;
        imm_Read.immediately_Reading_Current_Count = 0;
        memset(imm_Read.immediately_TXBuffer, 0, ARR_SIZE(imm_Read.immediately_TXBuffer));
        strcpy((char *)imm_Read.immediately_TXBuffer, "FEFEFE6855FFFFFFFF0011111605902F01FFFFFF16"); // FEFEFE6855 27100583 0011111 605902F01 FFFF CS16
        imm_Read.immediately_TX_Len = strlen((char *)imm_Read.immediately_TXBuffer);
        imm_Read.immediately_ReReading_Count = 5;
        read_Main_Meter_In_Timer_In_Main_Flage = 0;
        read_Copy_Meter_In_Timer_In_Main_Flage = 0;

        TempHex[4] = WebCtrlMeterFlag;
        memset(TempHex, 0, ARR_SIZE(TempHex));
        imm_Read.immediately_TX_Len = ASCIIToHEX2((char *)imm_Read.immediately_TXBuffer, imm_Read.immediately_TX_Len, TempHex, ARR_SIZE(TempHex));
        char MetterId[5] = {0};
        if (strlen(CtrlMainMeter_Manager.P103) == 8) { // 表号符合标号长度
            int P103Len = strlen(CtrlMainMeter_Manager.P103);
            ASCIIToHEX2(CtrlMainMeter_Manager.P103, strlen(CtrlMainMeter_Manager.P103), MetterId, ARR_SIZE(MetterId));
            memcpy(&TempHex[5], MetterId, 4);
            numberArrayToStrArray(CtrlMainMeter_Manager.P103, CtrlMainMeter_Manager.P103, P103Len);
        }
        TempHex[17] = ((unsigned char)CtrlMainMeter_Manager.L20 * 10);
        TempHex[18] = ((unsigned char)CtrlMainMeter_Manager.L20 * 10) >> 8;
        int NowLen = imm_Read.immediately_TX_Len - 2 - 3;

        int Cs = 0;
        for (int i = 0; i < NowLen; i++) { // 计算CS
            Cs += TempHex[3 + i];
        }
        NowLen += 3;
        TempHex[NowLen++] = Cs;
        TempHex[NowLen++] = 0x16;
        memset(imm_Read.immediately_TXBuffer, 0, ARR_SIZE(imm_Read.immediately_TXBuffer));
        memcpy(imm_Read.immediately_TXBuffer, TempHex, NowLen);
        imm_Read.immediately_TX_Len = NowLen;
    } else if (strstr(data, "R53") != NULL) {
        WebCtrlMeterFlag = 0x53;
        char * Pend = NULL;
        char * P_TempStart = NULL;
        if ((P_TempStart = strstr(data, "\"T8\":")) != NULL) {
            P_TempStart += strlen("\"T8\":");
            Pend = strchr(P_TempStart, ',');
            *Pend = 0;      // 结束
            CtrlCopyMeter_Manager.T8 = atoi(P_TempStart);
            *Pend = ',';    // 复原
        }
        if ((P_TempStart = strstr(data, "\"T10\":")) != NULL) {
            P_TempStart += strlen("\"T10\":");
            Pend = strchr(P_TempStart, ',');
            *Pend = 0;      // 结束
            CtrlCopyMeter_Manager.T10 = atoi(P_TempStart);
            *Pend = ',';    // 复原
        }
        if ((P_TempStart = strstr(data, "\"R53\":\"")) != NULL) {
            P_TempStart += strlen("\"R53\":\"");
            Pend = strchr(P_TempStart, '\"');
            *Pend = 0;      // 结束
            memcpy(CtrlCopyMeter_Manager.R53, P_TempStart, strlen(P_TempStart));
            CtrlCopyMeter_Manager.R53[strlen(CtrlCopyMeter_Manager.R53)] = 0;
            *Pend = '\"';    // 复原
        }
        if ((P_TempStart = strstr(data, "\"R64\":")) != NULL) {
            P_TempStart += strlen("\"R64\":");
            Pend = strchr(P_TempStart, ',');
            *Pend = 0;      // 结束
            CtrlCopyMeter_Manager.R64 = atoi(P_TempStart);
            *Pend = ',';    // 复原   
        }
        if ((P_TempStart = strstr(data, "\"L32\":")) != NULL) {
            P_TempStart += strlen("\"L32\":");
            Pend = strchr(P_TempStart, ',');
            *Pend = 0;      // 结束
            CtrlCopyMeter_Manager.L32 = atoi(P_TempStart);
            *Pend = ',';    // 复原
        }
        if ((P_TempStart = strstr(data, "\"L20\":")) != NULL) {
            P_TempStart += strlen("\"L20\":");
            Pend = strchr(P_TempStart, ',');
            *Pend = 0;      // 结束
            CtrlCopyMeter_Manager.L20 = doneAsciiToDouble(P_TempStart);
            *Pend = ',';    // 复原
        }
        if ((P_TempStart = strstr(data, "\"J79\":\"")) != NULL) {
            P_TempStart += strlen("\"J79\":\"");
            Pend = strchr(P_TempStart, '\"');
            *Pend = 0;      // 结束
            memcpy(CtrlCopyMeter_Manager.J79, P_TempStart, strlen(P_TempStart));
            CtrlCopyMeter_Manager.J79[strlen(CtrlCopyMeter_Manager.J79)] = 0;
            *Pend = '\"';    // 复原
        }
        if (CtrlCopyMeter_Manager.L32 != 0) { // 只有 0:开度给定 才可以处理
            return;
        }
        if (strlen(CtrlCopyMeter_Manager.R53) != 8) { // 表号不符合标号长度
            return;
        }

        // 组装 68 指令控制被采集设备
        imm_Read.immediately_Exec_Flage = 1;
        imm_Read.immediately_Reading_Current_Count = 0;
        memset(imm_Read.immediately_TXBuffer, 0, ARR_SIZE(imm_Read.immediately_TXBuffer));
        strcpy((char *)imm_Read.immediately_TXBuffer, "FEFEFE6853FFFFFFFF0011110404A01900FFFF16"); // FEFEFE6853 34567891 001111 0404A019 0064 9516
        imm_Read.immediately_TX_Len = strlen((char *)imm_Read.immediately_TXBuffer);
        imm_Read.immediately_ReReading_Count = 5;
        read_Main_Meter_In_Timer_In_Main_Flage = 0;
        read_Copy_Meter_In_Timer_In_Main_Flage = 0;

        TempHex[4] = WebCtrlMeterFlag;
        memset(TempHex, 0, ARR_SIZE(TempHex));
        imm_Read.immediately_TX_Len = ASCIIToHEX2((char *)imm_Read.immediately_TXBuffer, imm_Read.immediately_TX_Len, TempHex, ARR_SIZE(TempHex));
        char MetterId[5] = {0};
        if (strlen(CtrlCopyMeter_Manager.R53) == 8) { // 表号符合标号长度
            int R53Len = strlen(CtrlCopyMeter_Manager.R53);
            ASCIIToHEX2(CtrlCopyMeter_Manager.R53, strlen(CtrlCopyMeter_Manager.R53), MetterId, ARR_SIZE(MetterId));
            memcpy(&TempHex[5], MetterId, 4);
            numberArrayToStrArray(CtrlCopyMeter_Manager.R53, CtrlCopyMeter_Manager.R53, R53Len);
        }
        TempHex[17] = (unsigned char)CtrlCopyMeter_Manager.L20;
        int NowLen = imm_Read.immediately_TX_Len - 2 - 3;

        int Cs = 0;
        for (int i = 0; i < NowLen; i++) { // 计算CS
            Cs += TempHex[3 + i];
        }
        NowLen += 3;
        TempHex[NowLen++] = Cs;
        TempHex[NowLen++] = 0x16;
        memset(imm_Read.immediately_TXBuffer, 0, ARR_SIZE(imm_Read.immediately_TXBuffer));
        memcpy(imm_Read.immediately_TXBuffer, TempHex, NowLen);
        imm_Read.immediately_TX_Len = NowLen;
    }
}
