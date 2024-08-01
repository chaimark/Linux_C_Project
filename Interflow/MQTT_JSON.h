#ifndef __MQTT_JSON_H
#define __MQTT_JSON_H

#include <string.h>
#include <stdbool.h>
#include "main.h"
#include "Define.h"
#include "TimerCopyReading.h"
#include "TimerMainReading.h"
#include "ImmediatelyReading.h"
#include "4G_Module.h"
#include "AT24C0256.h"
#include "TimerMainReading.h"


typedef struct JSONNODE_S {
    const char * node;         // 节点信息
    const unsigned char type; // 0:整数，其他保留：几位位小数，  0xaa：字符串
} JSONNODE_T;

#define JSON_GW_SIZE 11
#define JSON_TIME_SIZE 19
#define JSON_ME_SIZE 14
#define JSON_NODE_T_SIZE sizeof(JSONNODE_T)

typedef struct JSON_MATCHING_S {
    JSONNODE_T msg_id;
    JSONNODE_T msg_gw_pr;
    JSONNODE_T gw_id;
    JSONNODE_T update_gw_id;
    JSONNODE_T username;
    JSONNODE_T password;
    JSONNODE_T url;
    JSONNODE_T port;
    JSONNODE_T stat;
    JSONNODE_T CSQ;
    JSONNODE_T RSSI;
    JSONNODE_T SINR;
    JSONNODE_T CCLK;
    JSONNODE_T CCID;
    JSONNODE_T MBUS_mV;
    JSONNODE_T MBUS_mA;
    JSONNODE_T GW_model;
    JSONNODE_T GW_Ver;
    JSONNODE_T time;
    JSONNODE_T has_rst;
    JSONNODE_T not_intimer_interval;
    JSONNODE_T heating_start;
    JSONNODE_T heating_end;
    JSONNODE_T main_interval;
    JSONNODE_T main_meter_total;
    JSONNODE_T main_meter_id;
    JSONNODE_T main_immediately;
    JSONNODE_T copy_interval;
    JSONNODE_T copy_groups_num;
    JSONNODE_T copy_meter_total;
    JSONNODE_T copy_meter_id;
    JSONNODE_T copy_immediately;
    JSONNODE_T copy_statistics;
    JSONNODE_T meter_id;
    JSONNODE_T up_immediately;
    JSONNODE_T transparent;
    JSONNODE_T transparent_port;
    JSONNODE_T transparent_count;
    JSONNODE_T index_row;
    JSONNODE_T row;
    JSONNODE_T fw;
    JSONNODE_T upgrades;
    JSONNODE_T program_ver;
    JSONNODE_T program_now_addr;
    JSONNODE_T WTporjectId;
    JSONNODE_T WTproductkey;
} JSON_MATCHING_S;

typedef struct METERID_S {
    unsigned char meterIds[7]; // 节点信息
} METERID_T;

extern JSON_MATCHING_S json_matching;


extern void JSON_Send_Add_Item(JSONNODE_T * json_node, void * data);
extern void JSON_Send_Add_Begin(unsigned char id);
extern void JSON_Send_Add_End(void);
extern char * JSON_Find_Node(char * data, JSONNODE_T * json_node, unsigned short int * len); // 找一个字符串
extern char * JSON_Find_Int(char * data, JSONNODE_T * json_node, int * data_int);            // 找一个整数
extern void JSON_GetDataForUpgradeProgram(unsigned char id);

extern void JSON_Send_Copy_Statistics(void);
extern void JSON_Send_Main_Copy_Meter_Data(unsigned char id, unsigned char * addr, unsigned char * data, unsigned char lentgh);
extern void JSON_Send_Immediately_Main_Copy_Meter_Data(unsigned char id, unsigned char port_num, unsigned char * data, unsigned char lentgh);

extern unsigned int MQTT_Important_Even_1Second_Count;
extern int MQTT_JSON_Analysis(char * data);
extern void JSON_Init_Confirm(unsigned char confirm);
extern void JSON_Send_GW_Infor(bool hasRST);

#endif
