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
#define ID_TXT_LOG               101
#define ID_BTN_AD1               201
#define ID_BTN_AD2               202
#define ID_BTN_AD3               203
#define ID_BTN_AD4               204
#define ID_BTN_AD5               205
#define ID_BTN_EXITAD            206