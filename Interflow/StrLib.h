
#ifndef STRLIB_H // 如果 STRLIB_H 未定义
#define STRLIB_H // 定义 STRLIB_H

#include <string.h>
#include <stdbool.h>
/*-----------------------------------定义数组类----------------------------------*/
// #define ThisObj &##ThisObj
#define This (*this)
#define ARR_SIZE(ArrName) sizeof(ArrName) / sizeof(ArrName[0]) // 计算数组元素个数
#define WIFI_BUFF_MAXLEN 1024                                  // 接收缓存空间
#define ONCE_PACKAGE_LEN 1000                                  // http分块发送的单块size
/*
数组类，包含数组指针和长度
需要注意的是
函数需要返回该类型时
该指针指向的数组需要定义在全局空间
该类型容易误认为是实体对象，需要注意
可以定义一个全局空间，用于暂时存放 strnew 对象的数据
或需要使用时直接定义数据，并作为参数传入
*/
// 类定义
typedef struct New_Arr {
    union _newclass {
        void * _void;           // 无定义形指针
        char * _char;           // 字符型指针
        short * _short;         // 短整型指针
        int * _int;             // 整型指针
        long * _long;           // 长整型指针
        long long * _LLong;     // 长长整型指针
        float * _float;         // 单精度浮点型指针
        double * _double;       // 双精度浮点型指针
        long double * _Ldouble; // 扩展精度浮点型指针
    } Name;
    int MaxLen;
} strnew;
// 建立对象
#define NEW_NAME(ArrName) New_Str_Obj(ArrName, ARR_SIZE(ArrName))
extern strnew New_Str_Obj(void * Master, int SizeNum); // 建立对象的函数
extern char TempBuff[WIFI_BUFF_MAXLEN];

/*-----------------------------------外部接口----------------------------------*/
extern int catString(char * OutStr, char * IntStr, int MaxSize, int IntSize);
extern bool copyString(char * OutStr, char * IntStr, int MaxSize, int IntSize);
extern char * myStrstr(char * MotherStr, char * SonStr, int MotherMaxSize);
extern char * myStrstrCont(char * MotherStr, char * SonStr, int MotherMaxSize, int ContNum);
extern void swapChr(char * a, char * b);
extern void swapStr(char * IntputStr, int StrLen);
extern char swapLowHight_Of_Char(char InputCh);

#endif
