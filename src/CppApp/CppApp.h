#pragma once

#include "resource.h"

// 全局 DLL 句柄
extern HINSTANCE hDLL;

// 回调函数类型定义
typedef void(*CallbackDelegate)(char* s);

// DLL 函数类型定义
typedef char* (WINAPI* GetStr)(const char* appId, const char* secretKey);
typedef void(WINAPI* Initialize)(const char* appId, const char* secretKey);
typedef void(WINAPI* InitCompleteEvent)(CallbackDelegate onInitComplete);
typedef void(WINAPI* SetupExitAd)(char* exitAdUnitId);
typedef void(WINAPI* ShowExitAdBlocking)();
typedef int(WINAPI* ShowAd)(const char* jsonParam);
typedef void(WINAPI* ReportAdRewardFulfillment)(const char* unitId, const char* resourceId, const char* materialId, const char* rewardId);
typedef void(WINAPI* AdCloseEvent)(CallbackDelegate onAdCloseEvent);

// SDK 操作函数声明 
void initialize(HINSTANCE hDLL);
void setupExitAd(HINSTANCE hDLL);
void showExitAdBlocking(HINSTANCE hDLL);
void showAd(const char* s);
void reportAdRewardFulfillment(const char* unitId, const char* resourceId, const char* materialId, const char* rewardId);

// 回调函数声明 
void onInitCompleteEvent(char* s);
void onAdCloseEvent(char* s);

// 定义所使用控件 ID 
// 1.开屏 2.退屏 3.Banner 4.插屏 5.对联 6.激励视频 7.信息流 8.嵌入式
#define ID_TXT_LOG               101
#define ID_BTN_AD1               210 //开屏 
#define ID_BTN_EXITAD            220 //退屏
#define ID_BTN_AD3               230 //Banner
#define ID_BTN_AD4               240 //插屏
#define ID_BTN_AD41              241 //插屏仅视频
#define ID_BTN_AD42              242 //插屏仅Web
#define ID_BTN_AD5               250 //对联
#define ID_BTN_AD6               260 //激励视频
#define ID_BTN_AD61              261 //激励视频仅Web
#define ID_BTN_AD7               270 //信息流
#define ID_BTN_AD8               280 //信息流 