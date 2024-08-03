#ifndef WEB_NET_PROT_H
#define WEB_NET_PROT_H

#include <stdint.h>
#include <stdbool.h>

#define BuffcheckTime 7 // 50ms 115200 == 11520字节/s

#define sendDataByNetProt Uart0Send // Uart0Send(unsigned char *, unsigned short int)
// 类定义
typedef struct _NetDevParameter {
    uint16_t LineCheckTime; // 检查网络在线的间隔时间
    bool NowNetOnlineFlag;  // 网络在线标记
    bool CheckOnlineFlag;   // 检查网络在线标记
    bool NowTCPLinkFlag;    // TCP连接标记
    bool CheckTCPLinkFlag;  // 检查TCP连接标记

    uint8_t ReBootCount; // 重启次数

    uint8_t MQTT_NET_Receive_checkTime; // 检查接收 buff 的间隔时间
    char NetDataBuff[1024];             // 接收数据或发送数据的缓存空间
    uint16_t NetDataBuff_NowLen;        // Buff的实际长度
} NetDevParameter;
extern NetDevParameter Now_NetDevParameter;

// 公共接口
extern void MOTT_Net_Task(void);
extern bool copyDataForUART(void);
extern void MQTT_NET_10mS_Timers_Add(void);
extern void ClearNetDataBuff(void);
#endif
