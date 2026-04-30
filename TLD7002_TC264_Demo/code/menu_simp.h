#ifndef _MENU_SIMP_H_
#define _MENU_SIMP_H_

#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "zf_common_headfile.h"

#define NUM_VAR 52
#define NUM_PAGE 15
#define PAGES 4
#define OPERATION_REGION 1     
#define STORAGE_START_REGION 2 
#define MAX_REGION 8
#define KEY_CONTINOUS 7
#define name2str(x) #x
#define load(a1, a2, x, y, z)               \
    (a1)[(y)] = &(x);                       \
    strcpy((a2)[(y)], (char *)name2str(x)); \
    (y)++;                                  \
    (z)++;
#define page_alloc(a1, a2, x, y, pn) \
    (a1)[(x)] = (--y);               \
    strcpy((a2)[(x)], (char *)(pn));\
    x++;                                   \
    y = 0;
typedef enum model_t
{
    Menu = 0, Flash,
} model;

// 操作定义
typedef enum menu_opeartion_t
{
    None = 0,
    // 单键
    Key1 = 1000,
    Key2 = 100,
    Key3 = 10,
    Key4 = 1,
    Long_Key1 = 2000,
    Long_Key2 = 200,
    Long_Key3 = 20,
    Long_Key4 = 2,
    // 双键
    Key12 = 1100,
    Key13 = 1010,
    Key14 = 1001,
    Key23 = 110,
    Key24 = 101,
    Key34 = 11,
    Long_Key12 = 2200,
    Long_Key13 = 2020,
    Long_Key14 = 2002,
    Long_Key23 = 220,
    Long_Key24 = 202,
    Long_Key34 = 22,
    Key1_LongKey2 = 1200,
    Key1_LongKey3 = 1020,
    Key1_LongKey4 = 1002,
    Key2_LongKey1 = 2100,
    Key2_LongKey3 = 120,
    Key2_LongKey4 = 102,
    Key3_LongKey1 = 2010,
    Key3_LongKey2 = 210,
    Key3_LongKey4 = 12,
    Key4_LongKey1 = 2001,
    Key4_LongKey2 = 201,
    Key4_LongKey3 = 21,
    // 三键
    Key123 = 1110,
    Key124 = 1101,
    Key134 = 1011,
    Key234 = 111,
    Long_Key123 = 2220,
    Long_Key124 = 2202,
    Long_Key134 = 2022,
    Long_Key234 = 222,
    // 四键
    Key1234 = 1111,
    Long_Key1234 = 2222,
    Undefined
} menu_opeartion;

void menu_simp_init ();
void menu_simp_update ();
#endif
