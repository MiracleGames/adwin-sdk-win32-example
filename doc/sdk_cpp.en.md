# Miracle Games Cpp SDK

**Other language versions: [English](sdk_cpp.en.md), [中文](sdk_cpp.zh-CN.md).**

**SDK Name:** Miracle Games Cpp SDK

**Company Name of SDK Provider:** Beijing Qiyou Lingdong Technology Co., Ltd.

**Types of Personal Information Processed:** Data actively uploaded by you, device information (device model, operating system version number), network information (network type), application information.

**Purpose of Use and Functional Scenarios:** Provides login, payment, and other related functions.

**SDK Privacy Policy:** [Miracle Games Privacy Policy](https://www.mguwp.net/developer_privacy.html)

**Processing Method:** 1. Ensures security through SSL protocol encryption and HTTPS transmission encryption technology; 2. Processes data with de-identification measures such as encryption and anonymization.

**Compliance Usage Instructions:** [Miracle Games SDK Compliance Usage Guide](https://www.mguwp.net/developer_compliance.html)

---
# Integration and Function Description
### [1. Development Environment Configuration](#1-development-environment-configuration)
### [2. SDK Initialization](#2-sdk-initialization-1)
### [3. Advertisements](#3advertisements)
### [Contact Information](contact.en.md)
---

# 1.Development Environment Configuration

      1.Download MiracleGamesAdSDK and extract it to your project folder, such as the dll folder.

      2.Add commands in the post-build event to copy all dll files from MiracleGamesAdSDK to the build output folder.

```plaintext
xcopy /y "$(ProjectDir)dll\$(PlatformTarget)\*.dll" "$(OutDir)"
xcopy /y "$(ProjectDir)dll" "$(OutDir)"
xcopy /yei "$(ProjectDir)dll\runtimes" "$(OutDir)runtimes"
```

![cpp_event.png](../images/cpp_event.png)

# 2. SDK Initialization

### 2.1.Introduction

       Before integrating the Miracle Games SDK, you need to initialize the SDK first. Only after initialization is completed can you use all the features of this SDK in conjunction with the backend system. SDK initialization should be performed when the game starts.

### 2.2.SDK Initialization

```c++
void InitMgAdSdk(HWND hWnd) {
    if (hDLL) return;

    hDLL = LoadLibrary(L"MgAdSDKCSharpDLL.dll");
    if (hDLL) {
        // Register the CMP callback event
        if (auto func = (CmpSizeChangedEvent)GetProcAddress(hDLL, "CmpSizeChangedEvent")) // CMP container size changed
            func(onCmpSizeChangedEvent);
        if (auto func = (CmpClosedEvent)GetProcAddress(hDLL, "CmpClosedEvent")) // CMP closed
            func(onCmpClosedEvent);

        // Register the initialisation completion callback event
        if (auto func = (InitCompleteEvent)GetProcAddress(hDLL, "InitCompleteEvent")) // Callback function after initialization is complete
            func(onInitCompleteEvent);
        if (auto func = (AdCloseEvent)GetProcAddress(hDLL, "AdCloseEvent")) // Callback function for ad close event
            func(onAdCloseEvent);

        //1. Set parameters
        setAppId(hDLL, YourAppId, YourSecretKey);

        //2. First, determine whether to pop up CMP
        if (isOpenCmp(hDLL))
        {
            RECT clientRect;
            if (GetClientRect(hWnd, &clientRect)) {
                int panelWidth = clientRect.right - clientRect.left;
                int panelHeight = clientRect.bottom - clientRect.top;

                //2.1. Create CMP container
                HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
                g_hPnlCmp = CreateWindowW(L"STATIC", L"This is a panel for CMP", WS_CHILD | WS_VISIBLE, 0, 0, panelWidth, 50, hWnd, (HMENU)3011, minstance, NULL);
                BringWindowToTop(g_hPnlCmp);

                //2.2. Pop up CMP
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

        // Remove CMP container
        /* DestroyWindow(g_hPnlCmp);
        g_hPnlCmp = NULL;*/
        PostMessage(g_hwndMain, WM_DESTROY_CMP, 0, NULL);

        AppendLog(L"Cmp container destroyed");
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

### 2.3.Possible reasons for initialization failure include:

● Network failure, no proper network connection

● This SDK does not support VPN; VPN software is enabled on the device

● Incorrect AppId, please check your application settings in the developer backend

● Server issues, please check the error information in the result and [contact technical support](contact.en.md) promptly

# 3.Advertisements

### 3.1.Introduction

      Before integrating advertisements, you must first complete the SDK initialization.

      Miracle Games advertisements support 【Splash Ad 1920*1080】【Banner 728*90】【Interstitial 1024*768】【Couplet 300*600】【Rewarded 1024*768】【Feed】【Embedded】【Exit Ad】

### 3.2.Splash Screen, Banner, Interstitial, Couplet, and Rewarded Video, Information flow, Embedded Ads

```c++
//Ensure MG SDK ad interfaces are called in the main thread
void showAd(const char* json) {
    ShowAd func = (ShowAd)GetProcAddress(hDLL, "ShowAd");
    if (func) {
        // Ensure COM is initialized before calling
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


//1. Splash screen ad
{ 
  RECT clientRect;
  if (GetClientRect(g_hwndMain, &clientRect)) {
    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;
    nlohmann::json json_obj = {
     {"unitId", SplashAdUnitId},
     {"appType", 1},
     {"adType", 1},
     {"handle", reinterpret_cast<int>(g_hwndMain)},
     {"width", clientWidth},//For splash screen ads, the program's width and height must be provided.
     {"height", clientHeight},
     {"parentWidth", clientWidth},
     {"parentHeight", clientHeight}
    };
    std::string jsonStr = json_obj.dump();
    ShowAd(jsonStr.c_str()); 
  }
}

//2. Banner
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

//3. Interstitial ad
{
  CreateInterstitialAdPannel(hWnd);//Container maintained by developer
  nlohmann::json json_obj = {
   {"unitId", InterstitialUnitId},
   {"media", "image"},//Only image type materials, supported media types (image,video,web). Can be empty
   {"appType", 1},
   {"adType", 4},
   {"handle", reinterpret_cast<int>(g_hPnlInterstitial)}
  };
  std::string jsonStr = json_obj.dump();
  ShowAd(jsonStr.c_str()); 
} 

//4. Couplet
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

//5. Rewarded video
{
  CreateRewardAdPannel(hWnd);
  nlohmann::json json_obj = {
     {"unitId", RewardedUnitId},
     {"comment", "abc123"},//Pass-through parameter, frontend needs to urlEncode; will be returned unchanged in the ad close callback event
     {"appType", 1},
     {"adType", 7},
     {"handle", reinterpret_cast<int>(g_hPnlReward)},
     {"width", 1024},
     {"height", 768}
   };
   std::string jsonStr = json_obj.dump();
   ShowAd(jsonStr.c_str());
}

//6.Information flow
{ 
    //Developers are responsible for maintaining the ad container.
    int containerHandle = reinterpret_cast<int>(g_hPnlInformationFlow);
    nlohmann::json json_obj = {
        {"unitId", FeedUnitId},
        {"media", "image"},
        {"appType", 1},
        {"adType", 7},//Information flow
        {"width", 400},//Customize dimensions (e.g., 400×50), set in MG backend
        {"height", 50},
        {"handle", containerHandle}
    };
    std::string jsonStr = json_obj.dump();
    showAd(jsonStr.c_str());  
}

//7.Embedded
{ 
    //Developers are responsible for maintaining the ad container.
    int containerHandle = reinterpret_cast<int>(g_hPnlEmbedded);
    nlohmann::json json_obj = {
        {"unitId", EmbeddedUnitId},
        {"media", "image"},
        {"appType", 1},
        {"adType", 8},//Embedded
        {"width", 200},//Customize dimensions (e.g., 200×200), set in MG backend
        {"height", 200},
        {"handle", containerHandle}
    };
    std::string jsonStr = json_obj.dump();
    showAd(jsonStr.c_str()); 
}
```

### 3.3.Exit Screen Ad

      The exit screen ad is triggered when exiting the game. To ensure the display rate of the ad when exiting the game, MG implements the exit screen ad in two steps:

      1. After initialization is completed, load the exit screen ad information into memory.

      2. When exiting the game, directly display the exit screen ad.

```c++
//Exit screen ad
//Step 1. Load exit ad resources after successful initialization
void setupExitAd(HINSTANCE hdll) {
    if (auto func = (SetupExitAd)GetProcAddress(hdll, "SetupExitAd")) {
        func(ExitAdUnitId);   
        AppendLog(L"Load the resources for MG exit ad");
    }
}

//Exit screen ad
//Step 2. Display the exit screen ad when the application is closing
void showExitAdBlocking(HINSTANCE hdll) {
    if (auto func = (ShowExitAdBlocking)GetProcAddress(hdll, "ShowExitAdBlocking")) {
        func();
        AppendLog(L"show fallback screen advert");
    }
}
```

### 3.4.Ad Close Event

      Register the callback event for ad closure, typically done in the page constructor.

      Ad close event parameter description:

| Parameter Name | Parameter Description | Example |
| --- | --- | --- |
| unitId | Developer-provided Ad Slot ID | e333abaf22404c4a8d382c1e7ba42076 |
| advertStatus | Ad Slot Status | 1:Ad normal; 2:Ad closed by backend; 3:No ad material |
| The following parameters are only available for rewarded video ads |  |  |
| completeStatus | Ad playback status | 1:Ad playback completed, reward can be issued; 0:Ad playback not completed |
| comment | Pass-through parameter provided by the developer, URL encoded | abc%2c123 |
| rewardId | MG Order ID for the reward, used when the game reports verification after issuing the reward | String |
| resourceId | Resource Id | String |
| materialId | Material Id | String |

```c++
//Ad close callback function
void onAdCloseEvent(char* s) {
    AppendLog(L"onAdCloseEvent: %hs", s);
    //...
    // Destroy Ad pannel 
     
    // Send to main UI thread
    char* jsonCopy = _strdup(s);
    PostMessage(g_hwndMain, WM_DESTROY_ADVERT, 0, reinterpret_cast<LPARAM>(jsonCopy));
}

//Rewarded video ad, report order verification to MG
bool reportAdRewardFulfillment(const char* unitId, const char* resourceId, const char* materialId, const char* rewardId) {
    ReportAdRewardFulfillment func = (ReportAdRewardFulfillment)GetProcAddress(hDLL, "ReportAdRewardFulfillment");
    if (func) {
        return func(unitId, resourceId, materialId, rewardId);
    }
    return false;
}

//Implement ad close callback function in the main UI thread
case WM_DESTROY_ADVERT: {
    const char* json = reinterpret_cast<const char*>(lParam);
    if (json) {
        try
        {
            nlohmann::json json_obj = nlohmann::json::parse(json);
            std::string unitId = json_obj["unitId"];
            if (unitId == SplashAdUnitId)
            {//Remove the splash screen ad container
                DestroyWindow(g_hPnlSplashScreen);
                g_hPnlSplashScreen = NULL;
            }
            else if (unitId == InterstitialUnitId)
            {//Destroy interstitial ad container
                DestroyWindow(g_hPnlInterstitial);
                g_hPnlInterstitial = NULL;
            }
            else if (unitId == BannerUnitId)
            {//Destroy Banner ad container
                DestroyWindow(g_hPnlBanner);
                g_hPnlBanner = NULL;
            }
            else if (unitId == CoupletUnitId)
            {//Destroy couplet ad container
                int coupletType = json_obj["coupletType"];
                if (coupletType == 1)//Destroy left container
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
            {//Rewarded video
                DestroyWindow(g_hPnlReward);
                g_hPnlReward = NULL;

                int completeStatus = json_obj["completeStatus"]; 
                if (completeStatus == 1)
                {
                    std::string resourceId = json_obj["resourceId"];
                    std::string materialId = json_obj["materialId"];
                    std::string rewardId = json_obj["rewardId"];

                    //Video playback completed, issue reward items 
                    //... 

                    //Report order verification to MG
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