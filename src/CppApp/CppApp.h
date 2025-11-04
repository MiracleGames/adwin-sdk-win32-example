#pragma once

#include "resource.h"

// 全局 DLL 句柄
extern HINSTANCE hDLL;

// 回调函数类型定义
typedef void(*CallbackDelegate)(char* s);

// DLL 函数类型定义
typedef char* (WINAPI* GetStr)(const char* appKey, const char* secretKey);
typedef void(WINAPI* SetupAsync)(const char* appKey, const char* secretKey);
typedef void(WINAPI* OpenMgLog)(bool isError);
typedef void(WINAPI* RegisterInitCompleteEvent)(CallbackDelegate onInitComplete);
typedef void(WINAPI* SetupExitAdUnitId)(char* exitAdUnitId);
typedef void(WINAPI* ShowExitAdvert)();
typedef int(WINAPI* OpenMGAdvert)(const char* jsonParam);
typedef bool(WINAPI* ReportMgRewardFulfillment)(const char* unitId, const char* resourceId, const char* materialId, const char* rewardId);
typedef void(WINAPI* RegisterCloseAdvertEvent)(CallbackDelegate onCloseAdvert);

// SDK 操作函数声明 
void setupAsync(HINSTANCE hDLL);
void openMgLog(HINSTANCE hDLL);
void setupExitAdUnitId(HINSTANCE hDLL);
void showExitAdvert(HINSTANCE hDLL);
void openMGAdvert(const char* s);
bool reportFulfillment(const char* unitId, const char* resourceId, const char* materialId, const char* rewardId);

// 回调函数声明 
void onInitCompleteEvent(char* s);
void onCloseAdvertEvent(char* s);

// 注册函数声明
//void RegisterInitCompleteEvent(HINSTANCE hDLL);
//void RegisterCloseAdvertEvent(HINSTANCE hDLL);

// 定义所使用控件 ID 
#define ID_TXT_LOG               101
#define ID_BTN_AD1               201
#define ID_BTN_AD2               202
#define ID_BTN_AD3               203
#define ID_BTN_AD4               204
#define ID_BTN_AD5               205
#define ID_BTN_EXITAD            206