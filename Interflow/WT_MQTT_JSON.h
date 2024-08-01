#ifndef _WT_MQTT_JSON_H
#define _WT_MQTT_JSON_H

#include "StrLib.h"


extern void WT_JSON_Send_Main_Copy_Meter_Data(char * JSON_TTL_Buff, unsigned char * data, unsigned char lentgh);             // 组织上报仪表数据
extern void WT_JSON_Send_Immediately_Main_Copy_Meter_Data(char * JSON_TTL_Buff, unsigned char * data, unsigned char lentgh); // 组织透传数据上报
extern void WT_MQTT_JSON_Analysis(char * data);

#endif

/*
{
    "type": 1,
    "eo": "02345678903",
    "list": [
        {
            "Ttag": 1,
            "v": "1",
            "P103": "001"
        },
        {
            "Ttag": 1,
            "v": "1",
            "P103": "002"
        }
    ]
}


热表设备//6855
    R51	    是	string	    热表编码
    L21	    是	double	    供水温度
    L22	    是	double	    回水温度
    L1000	是	Double	    温差
    L26	    是	double	    瞬时热量
    L33	    是	double	    累计热量
    L25	    是	double	    瞬时流量
    L34	    是	double	    累计流量
    J1000	是	string	    采集时间(yyyy-MM-dd HH:mm:ss)

温控设备//6853
    R53 	是	string	    温控阀编码
    S13	    是	double	    设定温度
    S3	    是	double	    室内温度
    L21	    是	double	    供水温度(无数据则为nult)
    L22	    是	double	    回水温度
    L27	    是	double	    阀门开度(92.5%传92.5)
    J11	    是	double	    电量(单位%如50%那么数值就是50)
    R64	    是	string	    控制策略(0:开度给定,1:室温控制,2:圆温控制)
    R65	    是	string	    故障信息
    J1000	是	string	    采集时间(yy-MM-dd HH:mm:ss)

楼前设备
    P103	是	string	    编码
    J100	是	int	        温控阀类型1，户表，2楼表，12:楼前阀16:用户阀
    S13		否  double	    设定温度
    S3		否  double	    室内温度
    L22	    否	double	    实际回水温度
    L27	    是	double	    实际阀位(92.5%传92.5)
    L29	    否	int	        通讯状态
    L622	否	double	    目标回水温度
    L627	否	double	    目标阀位
    J111	否	double	    电池电量
    J112	否	double	    信号强度
    179		否  Datetime	标准数据时间(秒)
    L38		否  double	    最大开度
    L39	    否	double	    最小开度
    R64	    否	string	    控制策略反馈(0:开度给定,1:室温控制2:套湿控制)
    R65	    否	string	    故障信息

未知设备
    J91	    是	否	string	    设备编码
    S3	    是	否	double	    室内温度
    J111    否	否	double	    电池电量
    J112    否	是  double	    信号强度
    S62	    否	是	string	    故障状态
    S66	    是	否	datetime	数据采集时间

*/

/*  // WT:7.2
{
    "code": 1,
    "list": [
        {
            "T8": 1,
            "T10": 1,
            "R53": "001",
            "R64": 1,
            "L32": 1,
            "P32": 1,
            "L20": 1.0,
            "J79": "2022-05-18 13:22:23",
        }
    ]
}
指令下发
    T8	    是  int	        控制指令编码
    T10	    是  int	        控制指令状态1:执行中,-1:失败,2:成功
    R53     是  string      温控阀编码
    R64     是  int         控制权限（0：公司控制（只能公司控制）,1：用户控制（只能用户控制）
    L32     是  int         控制策略（0：开度给定，1：室温控制，2：回温控制）
    P32     是  int         控制下发反馈（0：开度给定，1：室温控制，2：回温控制）
    L20     是  double      下发值
    J79     是  string      控制指令下发时间
*/
