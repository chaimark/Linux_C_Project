#include "MQTT_JSON.h"
#include "WT_MQTT_JSON.h"
#include "NumberBaseLib.h"
#include "StrLib.h"
#include "NetProt_Module.h"
#include "AT24C0256.h"
#include "RTC.h"

JSON_MATCHING_S json_matching = {
    {"\"msg_id\"", 0}, // 代码升级
    {"\"msg_gw_pr\"", 0},
    {"\"gw_id\"", 0xff},
    {"\"update_gw_id\"", 0xff},
    {"\"username\"", 0xff},
    {"\"password\"", 0xff},
    {"\"url\"", 0xff},
    {"\"port\"", 0},
    {"\"stat\"", 0},
    {"\"CSQ\"", 0xff},
    {"\"RSSI\"", 0},
    {"\"SINR\"", 0},
    {"\"CCLK\"", 0xff},
    {"\"CCID\"", 0xff},
    {"\"MBUS_mV\"", 0},
    {"\"MBUS_mA\"", 0},
    {"\"GW_model\"", 0},
    {"\"GW_Ver\"", 0xff},
    {"\"time\"", 0xff},
    {"\"has_rst\"", 0},
    {"\"not_intimer_interval\"", 0},
    {"\"heating_start\"", 0xff},
    {"\"heating_end\"", 0xff},
    {"\"main_interval\"", 0},
    {"\"main_meter_total\"", 0},
    {"\"main_meter_id\"", 0xff},
    {"\"main_immediately\"", 0},
    {"\"copy_interval\"", 0},
    {"\"copy_groups_num\"", 0},
    {"\"copy_meter_total\"", 0},
    {"\"copy_meter_id\"", 0xff},
    {"\"copy_immediately\"", 0},
    {"\"copy_statistics\"", 0},
    {"\"meter_id\"", 0xff},
    {"\"up_immediately\"", 0},
    {"\"transparent\"", 0xff},
    {"\"transparent_port\"", 0},
    {"\"transparent_count\"", 0},
    {"\"index_row\"", 0},
    {"\"row\"", 1},
    {"\"fw\"", 0xff},
    {"\"upgrades\"", 0xff},
    {"\"program_ver\"", 0xff},
    {"\"program_now_addr\"", 0},
    {"\"WTporjectId\"", 0xff},
    {"\"WTproductkey\"", 0xff},
};

METERID_T Main_MeterId[20] = {0}; // 主表仪表号码缓存  20只
METERID_T Copy_MeterId[20] = {0}; // 辅表仪表号码缓存 20只
unsigned int MQTT_Important_Even_1Second_Count = 0;

// ****** 接收找数据 *******************************************************
// 返回指向节点数据的指针， 如果是字符串，需要赋值len
// 要求 ':'左右不能有空格
char * JSON_Find_Node(char * data, JSONNODE_T * json_node, unsigned short int * len) {
    char * ret = NULL, * p;
    int node_len = strlen(json_node->node);
    p = strstr(data, json_node->node);
    if (p != NULL) {
        // 找数据
        ret = p + node_len;
        while (*ret == ':' || *ret == ' ' || *ret == '"') {
            ret++;
            if (*ret == 0)
                return NULL;
        }
        // 字符串长度
        if (json_node->type == 0xff) {
            p = strchr(ret, '"');
            if (p != NULL)
                *len = p - ret;
            else
                return NULL;
        }
    }
    return ret;
}

// 找一个整数
char * JSON_Find_Int(char * data, JSONNODE_T * json_node, int * data_int) {
    char * p;
    if ((p = JSON_Find_Node(data, json_node, NULL)) != NULL)
        *data_int = atoi(p);
    return p;
}

// 添加一个条目
void JSON_Send_Add_Item(JSONNODE_T * json_node, void * data) {
    if (json_node->type == 0xff) {
        sprintf(JSON_TTL_Buff + strlen(JSON_TTL_Buff), "%s:\"%s\",", json_node->node, (char *)data);
    } else {
        int temp = *((int *)data);
        if (json_node->type == 0)
            sprintf(JSON_TTL_Buff + strlen(JSON_TTL_Buff), "%s:%d,", json_node->node, temp);
        else if (json_node->type == 1) {
            sprintf(JSON_TTL_Buff + strlen(JSON_TTL_Buff), "%s:%d.%d,", json_node->node, (unsigned int)temp / 10, (unsigned int)temp % 10);
        } else if (json_node->type == 2) {
            sprintf(JSON_TTL_Buff + strlen(JSON_TTL_Buff), "%s:%d.%02d,", json_node->node, (unsigned int)temp / 100, (unsigned int)temp % 100);
        }
    }
}

// 添加网关信息
static void JSON_Send_Add_GW(unsigned char id) {
    char temp_char[24] = {0};
    int temp;
    // 头
    JSON_TTL_Buff[0] = 0;
    strcat(JSON_TTL_Buff, "{\"gw\":{");
    // id
    temp = id;
    JSON_Send_Add_Item(&json_matching.msg_id, &temp);
    // no
    temp = 0;
    JSON_Send_Add_Item(&json_matching.msg_gw_pr, &temp);
    // gw
    sprintf(temp_char, "%01x%02x%02x%02x%02x%02x", AT24CXX_Manager.gw_id[0], AT24CXX_Manager.gw_id[1], AT24CXX_Manager.gw_id[2], AT24CXX_Manager.gw_id[3], AT24CXX_Manager.gw_id[4], AT24CXX_Manager.gw_id[5]);
    JSON_Send_Add_Item(&json_matching.gw_id, temp_char);
    // 尾
    temp = strlen(JSON_TTL_Buff);
    JSON_TTL_Buff[temp - 1] = 0;
    strcat(JSON_TTL_Buff, "},");
}

// 添加公共信息
void JSON_Send_Add_Begin(unsigned char id) {
    JSON_Send_Add_GW(id);
    strcat(JSON_TTL_Buff, "\"data\":{");
}

// 添加结尾
void JSON_Send_Add_End(void) {
    int temp;
    temp = strlen(JSON_TTL_Buff);
    JSON_TTL_Buff[temp - 1] = 0;
    strcat(JSON_TTL_Buff, "}}");
}

// 添加结尾
static void JSON_Send_Add_End_Func(void) {
    int temp;
    temp = strlen(JSON_TTL_Buff);
    JSON_TTL_Buff[temp - 1] = 0;
    strcat(JSON_TTL_Buff, "]}}");
}

// 具体发送报文 **************
// 发送状态报文
static void JSON_Send_Add_Stat(unsigned char id, unsigned int stat) {
    int temp;
    JSON_Send_Add_Begin(id);
    temp = stat;
    JSON_Send_Add_Item(&json_matching.stat, &temp);
    JSON_Send_Add_End();
    if (UP_Mode_NET_ON == 1) {
        sendDataByNetProt((unsigned char *)JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    } else {
        MQTT_4G_Send_Protocol_To_TTL(JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    }
    // MQTT_4G_Send_Protocol_To_TTL(JSON_TTL_Buff, strlen(JSON_TTL_Buff));
}

// 添加一个条目
static void JSON_Send_Add_Item_Func(JSONNODE_T * json_node, void * data) {
    if (json_node->type == 0xff) {
        sprintf(JSON_TTL_Buff + strlen(JSON_TTL_Buff), "%s:[\"%s\",", json_node->node, (char *)data);
    } else {
        int temp = *((int *)data);
        if (json_node->type == 0)
            sprintf(JSON_TTL_Buff + strlen(JSON_TTL_Buff), "%s:%d,", json_node->node, temp);
        else if (json_node->type == 1) {
            sprintf(JSON_TTL_Buff + strlen(JSON_TTL_Buff), "%s:%d.%d,", json_node->node, (unsigned int)temp / 10, (unsigned int)temp % 10);
        } else if (json_node->type == 2) {
            sprintf(JSON_TTL_Buff + strlen(JSON_TTL_Buff), "%s:%d.%02d,", json_node->node, (unsigned int)temp / 100, (unsigned int)temp % 100);
        }
    }
}

void JSON_Init_Confirm(unsigned char confirm) {
    MQTT_4G_History_Confirm = confirm;
}

// ****** 接收找数据 *******************************************************
// 返回指向节点数据的指针， 如果是字符串，需要赋值len
// 要求 ':'左右不能有空格
static char * JSON_Find_Node_Func(char * data, JSONNODE_T * json_node, unsigned short int * len) {
    char * ret = NULL, * p;
    int node_len = strlen(json_node->node);
    p = strstr(data, json_node->node);
    if (p != NULL) {
        // 找数据
        ret = p + node_len;
        while (*ret == ':' || *ret == ' ' || *ret == '"' || *ret == '[') {
            ret++;
            if (*ret == 0)
                return NULL;
        }
        // 字符串长度
        if (json_node->type == 0xff) {
            p = strchr(ret, '"');
            if (p != NULL)
                *len = p - ret;
            else
                return NULL;
        }
    }
    return ret;
}
// 心跳上报信息
void JSON_Send_GW_Infor(bool hasRST) {
    char temp_char[6] = {0};
    int temp = 0;
    JSON_Send_Add_Begin(1);
    temp = 2;
    JSON_Send_Add_Item(&json_matching.stat, &temp); // 添加stats
    if (UP_Mode_NET_ON == 1) {
        // 信号强度
        JSON_Send_Add_Item(&json_matching.CSQ, "99,99");
        temp = -100;
        JSON_Send_Add_Item(&json_matching.RSSI, &temp);
        temp = 0;
        JSON_Send_Add_Item(&json_matching.SINR, &temp);
        // ccid
        JSON_Send_Add_Item(&json_matching.CCID, "00000000000000000000");
    } else {
        // 信号强度
        JSON_Send_Add_Item(&json_matching.CSQ, EC20T_CSQ);
        temp = EC20T_RSSI;
        JSON_Send_Add_Item(&json_matching.RSSI, &temp);
        temp = EC20T_SINR;
        JSON_Send_Add_Item(&json_matching.SINR, &temp);
        // ccid
        JSON_Send_Add_Item(&json_matching.CCID, EC20T_CCID);
    }
    // 版本号
    sprintf(temp_char, "V%d.%d", SOFT_VERSION / 10, SOFT_VERSION % 10);
    JSON_Send_Add_Item(&json_matching.GW_Ver, temp_char);
    JSON_Send_Add_Item(&json_matching.has_rst, &hasRST);                // 是否重启
    JSON_Send_Add_Item(&json_matching.MBUS_mV, &Current_MBUS_MV_Value); // MBUS电压
    JSON_Send_Add_Item(&json_matching.MBUS_mA, &Current_MBUS_MA_Value); // MBUS电流
    temp = AT24CXX_Manager.Save_Working_Mode;                           // 端口 要完善
    JSON_Send_Add_Item(&json_matching.GW_model, &temp);                 // 网关当前开启的端口  MBUS及485区别
    temp = 0;
    temp = EEprom_Byte1Read(EEPROM_MAIN_METER_TOTAL_ADDR);
    JSON_Send_Add_Item(&json_matching.main_meter_total, &temp);
    temp = 0;
    temp = EEprom_Byte1Read(EEPROM_COPY_METER_TOTAL_ADDR);
    JSON_Send_Add_Item(&json_matching.copy_meter_total, &temp);
    JSON_Send_Add_End();

    if (UP_Mode_NET_ON == 1) {
        sendDataByNetProt((unsigned char *)JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    } else {
        MQTT_4G_Send_Protocol_To_TTL(JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    }
}

// 读上报周期
static void JSON_Send_GW_Read_Interval(unsigned char id) {
    int temp;
    JSON_Send_Add_Begin(id);
    temp = 2;
    JSON_Send_Add_Item(&json_matching.stat, &temp); // 添加stats
    temp = AT24CXX_Manager.not_intimer_interval;
    JSON_Send_Add_Item(&json_matching.not_intimer_interval, &temp);
    JSON_Send_Add_Item(&json_matching.heating_start, AT24CXX_Manager.heating_start);
    JSON_Send_Add_Item(&json_matching.heating_end, AT24CXX_Manager.heating_end);
    temp = AT24CXX_Manager.main_interval;
    JSON_Send_Add_Item(&json_matching.main_interval, &temp);
    temp = AT24CXX_Manager.copy_interval;
    JSON_Send_Add_Item(&json_matching.copy_interval, &temp);
    temp = AT24CXX_Manager.copy_statistics;
    JSON_Send_Add_Item(&json_matching.copy_statistics, &temp);
    JSON_Send_Add_End();
    if (UP_Mode_NET_ON == 1) {
        sendDataByNetProt((unsigned char *)JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    } else {
        MQTT_4G_Send_Protocol_To_TTL(JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    }
}

// 读主表表号
static void JSON_Send_Read_Main_Meter_Id(unsigned char id) {
    int temp;
    char temp_char[24] = {0};
    char i, j;
    JSON_Send_Add_Begin(id);
    temp = 32;
    JSON_Send_Add_Item(&json_matching.stat, &temp); // 添加stats
    temp = EEprom_Byte1Read(EEPROM_MAIN_METER_TOTAL_ADDR);
    JSON_Send_Add_Item(&json_matching.main_meter_total, &temp);
    for (i = 0; i < 20; i++) {
        for (j = 0; j < 7; j++)
            Main_MeterId[i].meterIds[j] = EEprom_Byte1Read(EEPROM_MAIN_METER_START_ADDR + i * 60 + j);
    }
    sprintf(temp_char, "%01x%02x%02x%02x%02x%02x%02x", Main_MeterId[0].meterIds[0], Main_MeterId[0].meterIds[1], Main_MeterId[0].meterIds[2], Main_MeterId[0].meterIds[3], Main_MeterId[0].meterIds[4], Main_MeterId[0].meterIds[5], Main_MeterId[0].meterIds[6]);
    if ((Main_MeterId[0].meterIds[0] == 0x00) || (Main_MeterId[0].meterIds[0] == 0xFF)) {   // 无仪表号码
        JSON_Send_Add_Stat(id, 31); // 失败
    } else {
        JSON_Send_Add_Item_Func(&json_matching.main_meter_id, temp_char);
        for (i = 0; i < 19; i++) {
            if ((Main_MeterId[i + 1].meterIds[0] != 0x00) && (Main_MeterId[i + 1].meterIds[0] != 0xFF)) {
                sprintf(temp_char, "%01x%02x%02x%02x%02x%02x%02x", Main_MeterId[i + 1].meterIds[0], Main_MeterId[i + 1].meterIds[1],
                    Main_MeterId[i + 1].meterIds[2], Main_MeterId[i + 1].meterIds[3], Main_MeterId[i + 1].meterIds[4], Main_MeterId[i + 1].meterIds[5], Main_MeterId[i + 1].meterIds[6]);
                strcat(JSON_TTL_Buff, "\"");
                strcat(JSON_TTL_Buff, temp_char);
                strcat(JSON_TTL_Buff, "\",");
            }
        }
        JSON_Send_Add_End_Func();
        if (UP_Mode_NET_ON == 1) {
            sendDataByNetProt((unsigned char *)JSON_TTL_Buff, strlen(JSON_TTL_Buff));
        } else {
            MQTT_4G_Send_Protocol_To_TTL(JSON_TTL_Buff, strlen(JSON_TTL_Buff));
        }
    }
}

// 读辅表表号
static void JSON_Send_Read_Copy_Meter_Id(unsigned char id, unsigned char num) {
    int temp;
    char temp_char[24] = {0};
    char i, j;
    JSON_Send_Add_Begin(id);
    temp = 42;
    JSON_Send_Add_Item(&json_matching.stat, &temp); // 添加stats
    temp = EEprom_Byte1Read(EEPROM_COPY_METER_TOTAL_ADDR);
    JSON_Send_Add_Item(&json_matching.copy_meter_total, &temp);
    temp = num;
    JSON_Send_Add_Item(&json_matching.copy_groups_num, &temp);
    for (i = 0; i < 20; i++) {
        for (j = 0; j < 7; j++)
            Copy_MeterId[i].meterIds[j] = EEprom_Byte1Read(EEPROM_COPY_METER_START_ADDR + (num - 1) * 1200 + i * 60 + j);
    }
    sprintf(temp_char, "%01x%02x%02x%02x%02x%02x%02x", Copy_MeterId[0].meterIds[0], Copy_MeterId[0].meterIds[1], Copy_MeterId[0].meterIds[2], Copy_MeterId[0].meterIds[3], Copy_MeterId[0].meterIds[4], Copy_MeterId[0].meterIds[5], Copy_MeterId[0].meterIds[6]);
    if ((Copy_MeterId[0].meterIds[0] == 0x00) || (Copy_MeterId[0].meterIds[0] == 0xFF)) {   // 无仪表号码
        JSON_Send_Add_Stat(id, 41); // 失败
    } else {
        JSON_Send_Add_Item_Func(&json_matching.copy_meter_id, temp_char);
        for (i = 0; i < 19; i++) {
            if ((Copy_MeterId[i + 1].meterIds[0] != 0x00) && (Copy_MeterId[i + 1].meterIds[0] != 0xFF)) {
                sprintf(temp_char, "%01x%02x%02x%02x%02x%02x%02x", Copy_MeterId[i + 1].meterIds[0], Copy_MeterId[i + 1].meterIds[1], Copy_MeterId[i + 1].meterIds[2], Copy_MeterId[i + 1].meterIds[3], Copy_MeterId[i + 1].meterIds[4], Copy_MeterId[i + 1].meterIds[5], Copy_MeterId[i + 1].meterIds[6]);
                strcat(JSON_TTL_Buff, "\"");
                strcat(JSON_TTL_Buff, temp_char);
                strcat(JSON_TTL_Buff, "\",");
            }
        }
        JSON_Send_Add_End_Func();
        if (UP_Mode_NET_ON == 1) {
            sendDataByNetProt((unsigned char *)JSON_TTL_Buff, strlen(JSON_TTL_Buff));
        } else {
            MQTT_4G_Send_Protocol_To_TTL(JSON_TTL_Buff, strlen(JSON_TTL_Buff));
        }
    }
}

// 副标周期内统计
void JSON_Send_Copy_Statistics(void) {
    int temp;
    JSON_Send_Add_Begin(45);
    temp = AT24CXX_Manager.copy_statistics;
    JSON_Send_Add_Item(&json_matching.copy_statistics, &temp);
    JSON_Send_Add_End();
    if (UP_Mode_NET_ON == 1) {
        sendDataByNetProt((unsigned char *)JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    } else {
        MQTT_4G_Send_Protocol_To_TTL(JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    }
}

// 读MQTT信息
static void JSON_Send_Read_MQTT_Parameter(unsigned char id) {
    int temp;
    JSON_Send_Add_Begin(id);
    temp = 2;
    JSON_Send_Add_Item(&json_matching.stat, &temp); // 添加stats
    JSON_Send_Add_Item(&json_matching.username, AT24CXX_Manager.username);
    JSON_Send_Add_Item(&json_matching.password, AT24CXX_Manager.password);
    JSON_Send_Add_Item(&json_matching.url, AT24CXX_Manager.NET_4G_Remote_Url);
    temp = AT24CXX_Manager.NET_4G_Remote_Port;
    JSON_Send_Add_Item(&json_matching.port, &temp);
    JSON_Send_Add_End();
    if (UP_Mode_NET_ON == 1) {
        sendDataByNetProt((unsigned char *)JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    } else {
        MQTT_4G_Send_Protocol_To_TTL(JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    }
}

// 上报仪表数据
void JSON_Send_Main_Copy_Meter_Data(unsigned char id, unsigned char * addr, unsigned char * data, unsigned char lentgh) {
    if (WT_MQTT_FLAG == 1) {
        char temp_char[270] = {0};
        JSON_Send_Add_Begin(id);
        sprintf(temp_char, "%01x%02x%02x%02x%02x%02x%02x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6]);
        JSON_Send_Add_Item(&json_matching.meter_id, temp_char);
        strcat(JSON_TTL_Buff, "\"meter_data\":[\"");
        HEX2ToASCII((char *)data, lentgh, temp_char, (lentgh * 2));
        strcat(JSON_TTL_Buff, temp_char);
        strcat(JSON_TTL_Buff, "\",");
        JSON_Send_Add_End_Func();
    } else {
        memset(JSON_TTL_Buff, 0, UART0_MAX);
        WT_JSON_Send_Main_Copy_Meter_Data(JSON_TTL_Buff, data, lentgh);
    }
    if (UP_Mode_NET_ON == 1) {
        sendDataByNetProt((unsigned char *)JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    } else {
        MQTT_4G_Send_Protocol_To_TTL(JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    }
}

// 透传数据上报
void JSON_Send_Immediately_Main_Copy_Meter_Data(unsigned char id, unsigned char port_num, unsigned char * data, unsigned char lentgh) {
    if (WT_MQTT_FLAG == 1) {
        char temp_char[240] = {0};
        JSON_Send_Add_Begin(id);
        JSON_Send_Add_Item(&json_matching.transparent_port, &port_num);
        strcat(JSON_TTL_Buff, "\"meter_data\":[\"");
        HEX2ToASCII((char *)data, lentgh, temp_char, (lentgh * 2));
        strcat(JSON_TTL_Buff, temp_char);
        strcat(JSON_TTL_Buff, "\",");
        JSON_Send_Add_End_Func();
    } else {
        memset(JSON_TTL_Buff, 0, UART0_MAX);
        WT_JSON_Send_Immediately_Main_Copy_Meter_Data(JSON_TTL_Buff, data, lentgh);
    }
    if (UP_Mode_NET_ON == 1) {
        sendDataByNetProt((unsigned char *)JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    } else {
        MQTT_4G_Send_Protocol_To_TTL(JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    }
}
// 请求平台下发升级包
void JSON_GetDataForUpgradeProgram(unsigned char id) {
    // int temp;
    // JSON_Send_Add_Begin(id);

    // temp = 2;
    // JSON_Send_Add_Item(&json_matching.stat, &temp); // 添加stats
    // JSON_Send_Add_Item(&json_matching.program_now_addr, &Up_Param_Infomation.UpProgram_NowAddr);
    // JSON_Send_Add_Item(&json_matching.program_ver, &Up_Param_Infomation.Program_ver);

    // JSON_Send_Add_End();
    // if (UP_Mode_NET_ON == 1) {
    //     sendDataByNetProt((unsigned char *)JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    // } else {
    //     MQTT_4G_Send_Protocol_To_TTL(JSON_TTL_Buff, strlen(JSON_TTL_Buff));
    // }
}
// {"gw":{"msg_id":80,"msg_gw_pr":0,"gw_id":"02345678903"},"data":{"porjectId":"AAAAA","productkey":"BBBBB"}} 
int MQTT_JSON_Analysis(char * data) {
    WT_MQTT_FLAG = 1;   // HY 的指令
    char * p;
    unsigned short int len;
    char temp_char[12] = {0};
    int temp, temp1;
    unsigned char change_char[13] = {0};
    char i, j;
    // Id
    int id;
    if ((p = JSON_Find_Int(data, &json_matching.msg_id, &id)) == NULL)
        return 0;
    printf("json: id = %d\r\n", id);
    // gw
    if ((p = JSON_Find_Node(p, &json_matching.gw_id, &len)) == NULL || len != JSON_GW_SIZE)
        return 0;
    sprintf(temp_char, "%01x%02x%02x%02x%02x%02x", AT24CXX_Manager.gw_id[0], AT24CXX_Manager.gw_id[1], AT24CXX_Manager.gw_id[2], AT24CXX_Manager.gw_id[3], AT24CXX_Manager.gw_id[4], AT24CXX_Manager.gw_id[5]);
    if (strncmp(temp_char, p, JSON_GW_SIZE) != 0 && strncmp(p, "FFFFFFFFFFF", JSON_GW_SIZE) != 0)
        return 0;
    printf("json: gw ok\r\n");
    switch (id) {
        case 1: // 读MQTT参数
            JSON_Send_GW_Infor(0);
            break;
        case 2: // 读抄表周期
            JSON_Send_GW_Read_Interval(id);
            break;
        case 3: // 读存储主表信息
            JSON_Send_Read_Main_Meter_Id(id);
            break;
        case 4: // 读存储辅表信息
            if ((JSON_Find_Int(p, &json_matching.copy_groups_num, &temp1)) == NULL)
                temp1 = -1; // 组数
            if ((temp1 != -1) && (temp1 <= 12))
                JSON_Send_Read_Copy_Meter_Id(id, temp1);
            break;
        case 5: // 读MQTT信息
            JSON_Send_Read_MQTT_Parameter(id);
            break;
        case 12: // 写抄表周期
            if ((JSON_Find_Int(p, &json_matching.main_interval, &temp)) == NULL)
                temp = -1;
            if (temp != -1) {
                AT24CXX_Manager.main_interval = temp;
                EEprom_JSON_Write((unsigned char *)(&AT24CXX_Manager.main_interval), 2);
            }
            if ((JSON_Find_Int(p, &json_matching.copy_interval, &temp1)) == NULL)
                temp1 = -1;
            if (temp1 != -1) {
                AT24CXX_Manager.copy_interval = temp1;
                EEprom_JSON_Write((unsigned char *)(&AT24CXX_Manager.copy_interval), 2);
            }
            if ((p = JSON_Find_Node(p, &json_matching.heating_start, &len)) == NULL)
                temp1 = -1;
            if (temp1 != -1) {
                copyString(AT24CXX_Manager.heating_start, p, 15, len);
                EEprom_JSON_Write((unsigned char *)(&AT24CXX_Manager.heating_start), 10);
            }
            if ((p = JSON_Find_Node(p, &json_matching.heating_end, &len)) == NULL)
                temp1 = -1;
            if (temp1 != -1) {
                copyString(AT24CXX_Manager.heating_end, p, 15, len);
                EEprom_JSON_Write((unsigned char *)(&AT24CXX_Manager.heating_end), 10);
            }
            if (temp != -1 || temp1 != -1)
                JSON_Send_Add_Stat(id, 2);
            else
                JSON_Send_Add_Stat(id, 1);
            break;
        case 13: // 添加主表号码
            if ((JSON_Find_Int(p, &json_matching.main_meter_total, &temp1)) == NULL)
                temp1 = -1;
            if ((temp1 != -1) && (temp1 <= 20)) { // 最多20个
                EEprom_Byte1Write(EEPROM_MAIN_METER_TOTAL_ADDR, temp1);
            }
            if ((JSON_Find_Int(p, &json_matching.main_meter_id, &temp1)) != NULL) { // 号码
                memset(&Main_MeterId, 0x00, sizeof(Main_MeterId));
                if ((p = JSON_Find_Node_Func(p, &json_matching.main_meter_id, &len)) == NULL || len != JSON_ME_SIZE) {
                    JSON_Send_Add_Stat(id, 131);
                } else {
                    for (i = 0; i < 20; i++) {
                        if ((strstr((char *)p, "\r\n>")) != NULL)
                            break;
                        ASCIIToHEX2((char *)p, strlen((char *)p) - strlen("\"]}}"), (char *)Main_MeterId[i].meterIds, 7);
                        p += 17;
                    }
                    JSON_Send_Add_Stat(id, 132);
                    for (i = 0; i < 20; i++) { // 保存仪表号码
                        for (j = 0; j < 7; j++)
                            EEprom_Byte1Write(EEPROM_MAIN_METER_START_ADDR + i * 60 + j, Main_MeterId[i].meterIds[j]);
                        FL_IWDT_ReloadCounter(IWDT); // 喂狗
                    }
                }
            } else
                JSON_Send_Add_Stat(id, 131);
            break;
        case 14: // 添加辅表号码
            if ((JSON_Find_Int(p, &json_matching.copy_meter_total, &temp1)) == NULL)
                temp1 = -1;
            if ((temp1 != -1) && (temp1 <= 240)) { // 最多240个
                EEprom_Byte1Write(EEPROM_COPY_METER_TOTAL_ADDR, temp1);
            }
            if ((JSON_Find_Int(p, &json_matching.copy_groups_num, &temp1)) == NULL)
                temp1 = -1; // 组数
            if ((temp1 != -1) && (temp1 <= 12)) { // 每组20只  最多12组
                MQTT_Important_Even_1Second_Count = 1;
                memset(&Copy_MeterId, 0x00, sizeof(Copy_MeterId));
                if ((p = JSON_Find_Node_Func(p, &json_matching.copy_meter_id, &len)) == NULL || len != JSON_ME_SIZE) {
                    JSON_Send_Add_Stat(id, 141);
                } else {
                    for (i = 0; i < 20; i++) {
                        if ((strstr((char *)p, "\r\n>")) != NULL)
                            break;
                        ASCIIToHEX2((char *)p, strlen((char *)p) - strlen("\"]}}"), (char *)Copy_MeterId[i].meterIds, 7);
                        p += 17;
                    }
                    JSON_Send_Add_Stat(id, 142);
                    for (i = 0; i < 20; i++) { // 保存仪表号码
                        for (j = 0; j < 7; j++)
                            EEprom_Byte1Write(EEPROM_COPY_METER_START_ADDR + (temp1 - 1) * 1200 + i * 60 + j, Copy_MeterId[i].meterIds[j]);
                        FL_IWDT_ReloadCounter(IWDT); // 喂狗
                    }
                }
            } else
                JSON_Send_Add_Stat(id, 141);
            break;
        case 15: // 修改服务器地址
            temp = 0;
            if (temp == 0 && (p = JSON_Find_Node(p, &json_matching.username, &len)) != NULL && len < sizeof(AT24CXX_Manager.username)) {
                memcpy(AT24CXX_Manager.username, p, len);
                AT24CXX_Manager.username[len] = 0;
                temp++;
            }
            if (temp == 1 && (p = JSON_Find_Node(p, &json_matching.password, &len)) != NULL && len < sizeof(AT24CXX_Manager.password)) {
                memcpy(AT24CXX_Manager.password, p, len);
                AT24CXX_Manager.password[len] = 0;
                temp++;
            }
            if (temp == 2 && (p = JSON_Find_Node(p, &json_matching.url, &len)) != NULL && len < sizeof(AT24CXX_Manager.NET_4G_Remote_Url)) {
                memcpy(AT24CXX_Manager.NET_4G_Remote_Url, p, len);
                AT24CXX_Manager.NET_4G_Remote_Url[len] = 0;
                temp++;
            }
            if (temp == 3 && (p = JSON_Find_Int(p, &json_matching.port, &temp1)) != NULL) {
                AT24CXX_Manager.NET_4G_Remote_Port = temp1;
                temp++;
            }
            if (temp == 4) {
                EEprom_JSON_Write((unsigned char *)(&AT24CXX_Manager.NET_4G_Remote_Url), 57);
                JSON_Send_Add_Stat(id, 2);
            } else { // 失败
                EEprom_MQTT_Parameter_Init();
                JSON_Send_Add_Stat(id, 1);
            }
            break;
        case 16: // 修改网关标识
            if ((p = JSON_Find_Node(p, &json_matching.update_gw_id, &len)) == NULL || len != JSON_GW_SIZE) {
                JSON_Send_Add_Stat(id, 1);
            } else {
                int Len = JSON_GW_SIZE;
                JSON_Send_Add_Stat(id, 2);
                #warning "测试"
                    copyString((char *)change_char, (char *)p, ARR_SIZE(change_char), Len);
                swapStr((char *)change_char, Len);
                if ((Len % 2) != 0) {
                    change_char[strlen((char *)change_char)] = '0';
                    Len++;
                }
                swapStr((char *)change_char, Len);
                ASCIIToHEX2((char *)change_char, strlen((char *)change_char), (char *)change_char, (Len / 2));
                for (int i = 0; ((i < (Len / 2)) && (i < 6)); i++) {
                    AT24CXX_Manager.gw_id[i] = change_char[i];
                }
                EEprom_JSON_Write((unsigned char *)(&AT24CXX_Manager.gw_id), 6);
            }
            break;
        case 17: // 修改网关时间
            if ((p = JSON_Find_Node(p, &json_matching.time, &len)) == NULL || len != JSON_TIME_SIZE) {
                JSON_Send_Add_Stat(id, 1);
            } else {
                strnew SteStrRTCData;
                SteStrRTCData.Name._char = p;
                SteStrRTCData.MaxLen = len;
                setRtcDate(SteStrRTCData);
                FL_DelayMs(1200); // 等时间刷新
                JSON_Send_Add_Stat(id, 2);
            }
            break;
        case 18: // 立即启动抄表  上传
            if ((JSON_Find_Int(p, &json_matching.main_immediately, &temp1)) == NULL)
                temp1 = -1;
            if (temp1 == 1)
                read_Main_Meter_In_Timer_In_Main_Flage = 1; // 立即抄阀门 //启动抄阀门标志
            else if (temp1 == 0)
                read_Main_Meter_In_Timer_In_Main_Flage = 0; // 停止抄阀门
            if ((JSON_Find_Int(p, &json_matching.copy_immediately, &temp1)) == NULL)
                temp1 = -1;
            if (temp1 == 1)
                read_Copy_Meter_In_Timer_In_Main_Flage = 1; // 启动抄表标志 //立即抄阀门
            else if (temp1 == 0)
                read_Copy_Meter_In_Timer_In_Main_Flage = 0; // 停止抄阀门
            if ((JSON_Find_Int(p, &json_matching.up_immediately, &temp1)) == NULL)
                temp1 = -1;
            if (temp1 == 1)
                up_Immediately_History_In_Timer_In_Main_Flage = 1; // 启动上报标志 //立即上报
            else if (temp1 == 0)
                up_Immediately_History_In_Timer_In_Main_Flage = 0; // 停止上报
            JSON_Send_Add_Stat(id, 2);
            break;
        case 30: // 重启
            JSON_Send_Add_Stat(id, 2);
            EC20_ReBoot_In_RTC_By_1Second(2);
            break;
        case 40: // 仪表透传数据上传/透传仪表操作命令
            if ((JSON_Find_Int(p, &json_matching.transparent_port, &temp1)) == NULL)
                temp1 = -1; // 操作端口
            if (temp1 <= 5) { // 端口有效  0：mbus   1:485_1   2:485_2;
                imm_Read.immediately_Ports = temp1;
                if ((p = JSON_Find_Node_Func(p, &json_matching.transparent, &len)) != NULL) {
                    imm_Read.immediately_Exec_Flage = 1;
                    imm_Read.immediately_Reading_Current_Count = 0;
                    imm_Read.immediately_TX_Len = strlen(p);
                    char * P_end = myStrstr(p, "\"", imm_Read.immediately_TX_Len);
                    if (P_end == NULL) {
                        return 1;
                    }
                    imm_Read.immediately_TX_Len = (P_end - p) / 2;
                    ASCIIToHEX2((char *)p, len, (char *)imm_Read.immediately_TXBuffer, imm_Read.immediately_TX_Len);
                    if ((JSON_Find_Int(p, &json_matching.transparent_count, &temp1)) == NULL)
                        temp1 = -1;
                    if ((temp1 > 0) && (temp1 <= 20)) {
                        imm_Read.immediately_ReReading_Count = temp1;
                    } else
                        imm_Read.immediately_ReReading_Count = 5;
                    read_Main_Meter_In_Timer_In_Main_Flage = 0;
                    read_Copy_Meter_In_Timer_In_Main_Flage = 0;
                    JSON_Send_Add_Stat(id, 2);
                } else {
                    JSON_Send_Add_Stat(id, 401);
                }
            } else
                JSON_Send_Add_Stat(id, 401);
            break;
        case 50: // 序言，上电自动上报
            break;
        case 60: // 升级
            if ((p = JSON_Find_Int(p, &json_matching.row, &temp)) == NULL) {
                JSON_Send_Add_Stat(id, 601);
                return 0;
            }
            if ((p = JSON_Find_Node(p, &json_matching.fw, &len)) == NULL) {
                JSON_Send_Add_Stat(id, 601);
                return 0;
            }
            MQTT_Important_Even_1Second_Count = 1;
            // 升级代码，耦合大
            if (*(p - 1) == ':') {
                p--;
                len++;
            }
            temp1 = UpData_Receive_Hex((unsigned short int)temp, (unsigned char *)p, (unsigned short int)len);
            if (temp1 == 0) {
                printf("updata rec ok\r\n");
                JSON_Send_Add_Stat(id, 602);
                Now_NetDevParameter.LineCheckTime = 6000; // 成功则跟新检查时间 防止打断升级
            } else if (temp1 == 0x55) {
                printf("updata rec complete, and reboot\r\n");
                JSON_Send_Add_Stat(id, 603);
                EC20_ReBoot_In_RTC_By_1Second(2);
            } else if (temp1 == -1) {
                printf("updata resend\r\n");
                JSON_Send_Add_Stat(id, 601);
            } else if (temp1 == -2) {
                printf("updata error\r\n");
                JSON_Send_Add_Stat(id, 601);
            }
            break;
        case 70: // 遗言
            break;
        case 80:    //{"gw": {"msg_id": 80,"msg_gw_pr": 0,"gw_id": "02345678903","data": {"porjectId": "AAAAA","productkey": "BBBBB"}}
            if ((p = JSON_Find_Node(p, &json_matching.WTporjectId, &len)) == NULL) {
                JSON_Send_Add_Stat(id, 601);
                return 0;
            }
            memset(AT24CXX_Manager.porjectId, 0, ARR_SIZE(AT24CXX_Manager.porjectId)); // AT24CXX_Manager.porjectId
            memcpy(AT24CXX_Manager.porjectId, p, len);
            if ((p = JSON_Find_Node(p, &json_matching.WTproductkey, &len)) == NULL) {
                JSON_Send_Add_Stat(id, 601);
                return 0;
            }
            memset(AT24CXX_Manager.productkey, 0, ARR_SIZE(AT24CXX_Manager.productkey)); // AT24CXX_Manager.productkey
            memcpy(AT24CXX_Manager.productkey, p, len);
            EEprom_JSON_Write((unsigned char *)AT24CXX_Manager.NET_Local_GATEWAY, sizeof(AT24CXX_Manager.NET_Local_GATEWAY));
            break;
        default:
            break;
    }
    WT_MQTT_FLAG = 0;   // 改回 WT 的指令
    return 1;
}
