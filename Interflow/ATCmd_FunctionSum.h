#ifndef ATCMD_FUNCTIONSUM_H
#define ATCMD_FUNCTIONSUM_H

#include <stdint.h>
#include "StrLib.h"

#define This (*this)

// 所有 AT指令 《发送时》 需要的用于 《组织指令》 的函数名
typedef enum _SendFun {                                      // 发送AT指令时调用什么函数发送，对应的函数名
    flagChangeUartIDFun = 0,           // 控制修改 AT 指令需要的串口号
    easyATCmdDataLoadFun = 1,          // 简单AT 指令装载
    setBandDataLoadFun = 2,            // 波特率装载设置
    setWannDataLoadFun = 3,            // 网口设置指令装载
    setRemoteIPDataLoadFun = 4,        // 目标IP设置指令装载
    setUserAndPassWordDataLoadFun = 5, // 设置用户名和密码
    setMQTTMassagePUBFun = 6,          // MQTT 发布主题
    setMQTTMassageSUBFun = 7,          // MQTT 订阅主题
    setMQTTUserIDDataLoadFun = 8,      // MQTT 客户端ID
} SendFun;

// 所有 AT指令 《回复时》 需要的用于 《处理回复》 的函数名
typedef enum _receiveFun {                              // 发送AT指令时调用什么函数发送，对应的函数名
    ResetTrueFlagDoingFun = 0, // 作为特殊标记，用于忽略模组返回强制跳过某条指令
    easyATCmdDataDoingFun = 1, // 简单AT 指令处理
    ReBootDevFun = 2,          // 重启命令特殊处理
} receiveFun;

// 类定义
typedef struct _NetDevATCmdData {                         // 成员
    int8_t CmdID;         // 指令ID
    int8_t Next_CmdID;    // 下一条指令ID
    char * ATCmd;         // AT指令
    char * SucessStr;     // 成功返回的字符串
    bool RunFlag;         // 是否执行当前指令
    uint8_t CmsResCount;  // 回复查询次数限制
    uint8_t CmdSendCount; // 指令发送次数限制

    bool IsGetComputerResFlag; // 是否执行当前指令

    SendFun LoadATSendFunName;        // 装载数据的函数名
    receiveFun DoingATReceiveFunName; // 处理返回结果的函数名

    void (*DataInstallation)(strnew OutStr, struct _NetDevATCmdData This);
    bool (*DoingATCmdResData)(struct _NetDevATCmdData This);
} NetDevATCmd;
// 类方法
void _DataInstallation(strnew OutStr, struct _NetDevATCmdData This); // 设置或查询 AT指令 的装载方法
bool _DoingATCmdResData(struct _NetDevATCmdData This);               // 处理指令返回的方法
// 建立对象
#define ATCMD_MAXNUMBER 44
extern NetDevATCmd NetDevice_ATData[ATCMD_MAXNUMBER];
extern void MQTT_NetAT_Init(void);

#endif
