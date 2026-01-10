# Miracle Games Cpp SDK

**其他语言版本: [English](sdk_cpp.en.md), [中文](sdk_cpp.zh-CN.md).**

**SDK名称：** Miracle Games Cpp SDK

**SDK提供方的公司名称：**  北京奇游灵动科技有限公司

**处理的个人信息类型：** 您主动上传的数据、设备信息（设备型号、操作系统版本号）、网络信息（网络类型）、应用信息

**使用目的及功能场景：** 提供广告等相关功能

**SDK隐私政策：** [Miracle Games 隐私政策](https://www.mguwp.net/developer_privacy.html)

**处理方式：** 1、采用SSL协议加密及HTTPS传输加密技术保障安全； 2、采取加密、去标识化等安全措施脱敏处理

**合规使用说明：** [Miracle Games SDK 合规使用指南](https://www.mguwp.net/developer_compliance.html)

---
# 集成和功能说明
### [1.开发环境配置](#1开发环境配置-1)
### [2.SDK初始化](#2SDK初始化-1)
### [3.广告](#3广告-1)
### [4.联系方式](contact.zh-CN.md)
---

# 1.开发环境配置

      1.下载 MiracleGamesAdSDK，解压到项目文件夹中，如dll文件夹。

      2.在编译后事件中添加命令，将MiracleGamesAdSDK中的所有dll文件，生成到编译文件夹中。

```plaintext
xcopy /y "$(ProjectDir)dll\$(PlatformTarget)\*.dll" "$(OutDir)"
xcopy /y "$(ProjectDir)dll" "$(OutDir)"
xcopy /yei "$(ProjectDir)dll\runtimes" "$(OutDir)runtimes"
```

![cpp_event.png](../images/cpp_event.png)

# 2.SDK初始化

### 2.1.简介

      在接入Miracle Games SDK之前，首先需要进行SDK的初始化，初始化完成后，才可以配合后台系统使用本SDK的全部功能，在进入游戏后即进行SDK初始化。

### 2.2.SDK初始化

```c++
void InitMgAdSdk(HWND hWnd) {
    if (hDLL) return;

    hDLL = LoadLibrary(L"MgAdSDKCSharpDLL.dll");
    if (hDLL) {
        // Register the CMP callback event
        if (auto func = (CmpSizeChangedEvent)GetProcAddress(hDLL, "CmpSizeChangedEvent")) // CMP容器尺寸发生改变
            func(onCmpSizeChangedEvent);
        if (auto func = (CmpClosedEvent)GetProcAddress(hDLL, "CmpClosedEvent")) //CMP已关闭
            func(onCmpClosedEvent);

        // Register the initialisation completion callback event
        if (auto func = (InitCompleteEvent)GetProcAddress(hDLL, "InitCompleteEvent")) // 初始化完成后的回调函数
            func(onInitCompleteEvent);
        if (auto func = (AdCloseEvent)GetProcAddress(hDLL, "AdCloseEvent")) //广告关闭事件的回调函数
            func(onAdCloseEvent);

        //1.设置参数
        setAppId(hDLL, YourAppId, YourSecretKey);

        //2.首先判断是否需要弹出CMP
        if (isOpenCmp(hDLL))
        {
            RECT clientRect;
            if (GetClientRect(hWnd, &clientRect)) {
                int panelWidth = clientRect.right - clientRect.left;
                int panelHeight = clientRect.bottom - clientRect.top;

                //2.1.创建CMP容器
                HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
                g_hPnlCmp = CreateWindowW(L"STATIC", L"This is a panel for CMP", WS_CHILD | WS_VISIBLE, 0, 0, panelWidth, 50, hWnd, (HMENU)3011, minstance, NULL);
                BringWindowToTop(g_hPnlCmp);

                //2.2.弹出CMP
                nlohmann::json json_obj = {
                 {"handle", reinterpret_cast<int>(g_hPnlCmp)},
                 {"parentWidth", panelWidth},
                 {"parentHeight", panelHeight}
                };
                std::string jsonStr = json_obj.dump();
                openCmp(hDLL, jsonStr.c_str());
            }
        }

        //3.SDK initialisation
        initialize(hDLL);
    }
}

#pragma region 1.CMP
void setAppId(HINSTANCE hdll, const char* appId, const char* secretKey) {
    try
    {
        SetAppId func = (SetAppId)GetProcAddress(hdll, "SetAppId");
        func(appId, secretKey);
    }
    catch (const std::exception&)
    {
    }
}
bool isOpenCmp(HINSTANCE hdll) {
    bool result = false;
    try
    {
        IsOpenCmp func = (IsOpenCmp)GetProcAddress(hdll, "IsOpenCmp");
        result = func();
    }
    catch (const std::exception&)
    {
    }
    AppendLog(L"Cmp IsOpen Flag  %hs", result ? "true" : "false");
    return result;
}
void openCmp(HINSTANCE hdll, const char* jsonParam) {
    try
    {
        OpenCmp func = (OpenCmp)GetProcAddress(hdll, "OpenCmp");
        g_cmpSdkControlHandle = func(jsonParam);
    }
    catch (const std::exception&)
    {
    }
}
// Cmp size changed callback functions
void onCmpSizeChangedEvent(char* s) {
    try
    {
        AppendLog(L"Cmp size changed  %hs", s);
        nlohmann::json json_obj = nlohmann::json::parse(s); //{"width":900,"height":440}
        int width = json_obj["width"];
        int height = json_obj["height"];
        g_cmpChangedWidth = width;
        g_cmpChangedHeight = height;

        RECT parentRect;
        if (GetClientRect(g_hwndMain, &parentRect)) {
            int parentWidth = parentRect.right - parentRect.left;
            int parentHeight = parentRect.bottom - parentRect.top;
            int x = (parentWidth - width) / 2;
            int y = (parentHeight - height) / 2;
            SetWindowPos(g_hPnlCmp, NULL, x, y, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);
        }
    }
    catch (const std::exception&)
    {
    }
}
void onCmpClosedEvent(char* s) {
    try
    {
        g_cmpChangedWidth = 0;
        g_cmpChangedHeight = 0;

        //移除CMP容器
        /* DestroyWindow(g_hPnlCmp);
        g_hPnlCmp = NULL;*/
        PostMessage(g_hwndMain, WM_DESTROY_CMP, 0, NULL);

        AppendLog(L"Cmp 容器已销毁");
    }
    catch (const std::exception&)
    {
    }
}
#pragma endregion

#pragma region 2.SDK Initialisation
void initialize(HINSTANCE hdll) {
    try
    {
        Initialize func = (Initialize)GetProcAddress(hdll, "Initialize");
        func();
    }
    catch (const std::exception&)
    {
    }
}

// Initialise callback functions
void onInitCompleteEvent(char* s) {
    try
    {
        nlohmann::json json_obj = nlohmann::json::parse(s); //{"success":true,"data":""}
        bool success = json_obj["success"];
        if (success) {
            AppendLog(L"Initialization successful");

            setupExitAd(hDLL);//Exit Ad

            //PostMessage(g_hwndMain, WM_SHOW_OPENSCREEN_ADVERT, 0, NULL);//Open screen advertisement 
        }
    }
    catch (const std::exception&)
    {
    }
}
#pragma endregion
```

### 2.3.没有初始化成功的错误可能如下

● 网络故障，没有正确的网络支持

　　● 本SDK不支持VPN，本机开启了VPN软件

　　● AppId错误，请登录开发者后台检查应用设置

　　● 服务器问题，请检查result的错误信息，及时[联系技术支持](contact.zh-CN.md)

# 3.广告

### 3.1.简介

      在接入广告之前，首先需要完成SDK的初始化。

      Miracle Games 广告支持【开屏1920\*1080】【横幅728\*90】【插屏1024\*768】【对联300\*600】【激励视频1024\*768】【信息流】【嵌入式】【退屏】

### 3.2.开屏、横幅、插屏、对联、激励视频、信息流、嵌入式广告

```c++
//确保在主线程调用MG SDK广告接口
void showAd(const char* json) {
    ShowAd func = (ShowAd)GetProcAddress(hDLL, "ShowAd");
    if (func) {
        // 确保在调用前COM已初始化
        if (!g_comInitialized) {
            HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
            if (SUCCEEDED(hr)) {
                g_comInitialized = true;
            }
        }

        int result = func(json);
        if (result == 1) {
            // success 
        }
    }
}


//1.开屏广告
{ 
    RECT clientRect;
    if (GetClientRect(hWnd, &clientRect)) {
        int clientWidth = clientRect.right - clientRect.left;
        int clientHeight = clientRect.bottom - clientRect.top;
        nlohmann::json json_obj = {
            {"unitId", SplashAdUnitId},
            {"appType", 1},
            {"adType", 1},//1.开屏 2.退屏 3.Banner 4.插屏 5.对联 6.激励视频 7.信息流 8.嵌入式
            {"handle", reinterpret_cast<int>(g_hwndMain)},
            {"width", clientWidth},//开屏广告，需要传入程序的宽高
            {"height", clientHeight},
            {"parentWidth", clientWidth},
            {"parentHeight", clientHeight}
        };
        std::string jsonStr = json_obj.dump();
        showAd(jsonStr.c_str());
    }
    break;
}
//2.横幅
{
  CreateBannerAdPanel(hWnd); 
  int containerHandle = reinterpret_cast<int>(g_hPnlBanner);
  nlohmann::json json_obj = {
      {"unitId", BannerUnitId},
      {"media", "image"},
      {"appType", 1},
      {"adType", 3},
      {"handle", containerHandle}
  };
  std::string jsonStr = json_obj.dump(); 
  ShowAd(jsonStr.c_str());
}

//3.插屏广告
{
  CreateInterstitialAdPannel(hWnd);//由开发者维护容器
  nlohmann::json json_obj = {
   {"unitId", InterstitialUnitId},
   {"media", "image"},//仅图片类型的素材，支持媒体类型(image,video,web)。可传空
   {"appType", 1},
   {"adType", 4},
   {"handle", reinterpret_cast<int>(g_hPnlInterstitial)}
  };
  std::string jsonStr = json_obj.dump();
  ShowAd(jsonStr.c_str()); 
}
 
//4.对联
{
  CreateCoupletAdPannel(hWnd);
  nlohmann::json json_obj = {
      {"unitId", CoupletUnitId},
      {"appType", 1},
      {"adType", 5},
      {"handle", reinterpret_cast<int>(g_hPnlCoupletLeft)},
      {"handle2", reinterpret_cast<int>(g_hPnlCoupletRight)}
  };
  std::string jsonStr = json_obj.dump();
  ShowAd(jsonStr.c_str()); 
}

//5.激励视频
{
  CreateRewardAdPannel(hWnd);
  nlohmann::json json_obj = {
        {"unitId", RewardedUnitId},
        {"comment", "abc123"},//透传参数，前端需要进行urlEncode；在广告关闭回调事件中会原封不动的返回
        {"appType", 1},
        {"adType", 6},
        {"handle", reinterpret_cast<int>(g_hPnlReward)},
        {"width", 1024},
        {"height", 768}
    };
    std::string jsonStr = json_obj.dump();
    ShowAd(jsonStr.c_str());
}

//6.信息流
{ 
    //需要开发者负责维护广告容器
    int containerHandle = reinterpret_cast<int>(g_hPnlInformationFlow);
    nlohmann::json json_obj = {
        {"unitId", FeedUnitId},
        {"media", "image"},
        {"appType", 1},
        {"adType", 7},//信息流
        {"width", 400},//尺寸自定义(示例400*50)，在MG后台设置
        {"height", 50},
        {"handle", containerHandle}
    };
    std::string jsonStr = json_obj.dump();
    showAd(jsonStr.c_str());  
}

//7.嵌入式
{ 
    //需要开发者负责维护广告容器
    int containerHandle = reinterpret_cast<int>(g_hPnlEmbedded);
    nlohmann::json json_obj = {
        {"unitId", EmbeddedUnitId},
        {"media", "image"},
        {"appType", 1},
        {"adType", 8},//嵌入式
        {"width", 200},//尺寸自定义(示例200*200)，在MG后台设置
        {"height", 200},
        {"handle", containerHandle}
    };
    std::string jsonStr = json_obj.dump();
    showAd(jsonStr.c_str()); 
}
```

### 3.3.退屏广告

      弹屏广告是在退出游戏时触发，为了保证退出游戏时广告的弹出率，MG会分两步完成退屏广告的实现

      1.在初始化完成后，将退屏广告的信息加载到内存中

      2.在退出游戏时，直接打开退屏广告

```c++
//退屏广告
//Step1.初始化成功之后，加载退屏广告资源
void setupExitAd(HINSTANCE hdll) {
    if (auto func = (SetupExitAd)GetProcAddress(hdll, "SetupExitAd")) {
        func(ExitAdUnitId);   
        AppendLog(L"Load the resources for MG exit ad");
    }
}

// 退屏广告
// Step2.在程序关闭时，弹出展示退屏广告
void showExitAdBlocking(HINSTANCE hdll) {
    if (auto func = (ShowExitAdBlocking)GetProcAddress(hdll, "ShowExitAdBlocking")) {
        func();
        AppendLog(L"show fallback screen advert");
    }
}
```

### 3.4.广告关闭事件

      注册广告关闭的回调事件，一般在页面的构造函数中进行

      广告关闭事件参数说明

| 参数名 | 参数描述 | 示例 |
| --- | --- | --- |
| unitId | 开发者传入的广告位ID | e333abaf22404c4a8d382c1e7ba42076 |
| advertStatus | 广告位状态 | 1:广告正常；2:广告被后台关闭；3:没有广告素材 |
| 以下是仅激励视频广告拥有的参数 |  |  |
| completeStatus | 广告的播放状态 | 1:广告播放完毕，可以发奖励；0:广告未播放完毕 |
| comment | 由开发者传入的透传参数，经过 url 编码 | abc%2c123 |
| rewardId | 奖励的MG订单号，游戏发奖后向MG报告核销时使用 | String |
| resourceId | 资源Id | String |
| materialId | 素材 Id | String |

```c++
//注册广告关闭回调函数
void onAdCloseEvent(char* s) {
    AppendLog(L"onAdCloseEvent: %hs", s);
    //...
    // Destroy Ad pannel 
     
    // 发送到主UI线程
    char* jsonCopy = _strdup(s);
    PostMessage(g_hwndMain, WM_DESTROY_ADVERT, 0, reinterpret_cast<LPARAM>(jsonCopy));
}

//激励视频广告，向MG核销订单
bool reportAdRewardFulfillment(const char* unitId, const char* resourceId, const char* materialId, const char* rewardId) {
    ReportAdRewardFulfillment func = (ReportAdRewardFulfillment)GetProcAddress(hDLL, "ReportAdRewardFulfillment");
    if (func) {
        return func(unitId, resourceId, materialId, rewardId);
    }
    return false;
}

//在主UI线程中，实现广告关闭回调函数
case WM_DESTROY_ADVERT: {
    const char* json = reinterpret_cast<const char*>(lParam);
    if (json) {
        try
        {
            nlohmann::json json_obj = nlohmann::json::parse(json);
            std::string unitId = json_obj["unitId"];
            if (unitId == SplashAdUnitId)
                {//删除开屏广告容器
                    DestroyWindow(g_hPnlSplashScreen);
                    g_hPnlSplashScreen = NULL;
            }
            else if (unitId == InterstitialUnitId)
            {//删除插屏广告容器
                DestroyWindow(g_hPnlInterstitial);
                g_hPnlInterstitial = NULL;
            }
            else if (unitId == BannerUnitId)
            {//删除Banner广告容器
                DestroyWindow(g_hPnlBanner);
                g_hPnlBanner = NULL;
            }
            else if (unitId == CoupletUnitId)
            {//删除对联广告容器
                int coupletType = json_obj["coupletType"];
                if (coupletType == 1)//删除左侧容器
                {
                    BOOL result = DestroyWindow(g_hPnlCoupletLeft);
                    g_hPnlCoupletLeft = NULL;
                }
                else
                {
                    BOOL result = DestroyWindow(g_hPnlCoupletRight);
                    g_hPnlCoupletRight = NULL;
                }
            }
            else if (unitId == RewardedUnitId)
            {//激励视频
                DestroyWindow(g_hPnlReward);
                g_hPnlReward = NULL;

                int completeStatus = json_obj["completeStatus"]; 
                if (completeStatus == 1)
                {
                    std::string resourceId = json_obj["resourceId"];
                    std::string materialId = json_obj["materialId"];
                    std::string rewardId = json_obj["rewardId"];

                    //视频播放完毕，下发奖励道具 
                    //... 

                    //向MG核销订单
                    bool res = reportAdRewardFulfillment(unitId.c_str(), resourceId.c_str(), materialId.c_str(), rewardId.c_str());
                    AppendLog(L"reportAdRewardFulfillment: %hs", std::to_string(res).c_str());
                }
            }
        }
        catch (const std::exception&)
        { 
        }
        free((void*)json);
    }
    return 0;
}
```