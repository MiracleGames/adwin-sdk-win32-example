// CppApp.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "CppApp.h"
#include <nlohmann/json.hpp>
#include <objbase.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//MG Ads SDK相关的全局变量
HINSTANCE hDLL = NULL;
int g_nCmdShow = 0;
HWND g_hwndLog = NULL;
HWND g_hwndMain = NULL;
bool g_comInitialized = false;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    // 初始化 COM 为 STA
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr)) {
        g_comInitialized = true;
    }
    else {
        MessageBox(nullptr, L"COM初始化失败", L"错误", MB_ICONERROR);
        return FALSE;
    }

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CPPAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CPPAPP));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // 清理 COM
    if (g_comInitialized) {
        CoUninitialize();
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CPPAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CPPAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   //HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 1500, 900, nullptr, nullptr, hInstance, nullptr);
    
   if (!hWnd)
   {
      return FALSE;
   }

   g_hwndMain = hWnd;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// App Log
void AppendLog(const wchar_t* format, ...) {
    if (!g_hwndLog) return;

    wchar_t buffer[1024];
    va_list args;
    va_start(args, format);
    vswprintf_s(buffer, format, args);
    va_end(args);

    int len = GetWindowTextLength(g_hwndLog);
    SendMessage(g_hwndLog, EM_SETSEL, len, len);
    SendMessage(g_hwndLog, EM_REPLACESEL, FALSE, (LPARAM)buffer);
    SendMessage(g_hwndLog, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
}
std::wstring UTF8ToWide(const std::string& utf8str) {
    if (utf8str.empty()) return L"";
    int required = MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, nullptr, 0);
    if (required == 0) {
        // 转换失败
        return L"";
    }
    std::vector<wchar_t> buffer(required);
    MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, buffer.data(), required);
    return std::wstring(buffer.data());
}

HWND g_hPnlSplashScreen = NULL;//开屏(全屏)
HWND g_hPnlBanner = NULL;
HWND g_hPnlCoupletLeft = NULL;
HWND g_hPnlCoupletRight = NULL;
HWND g_hPnlInterstitial = NULL;//插屏  
HWND g_hPnlReward = NULL;//激励视频  
HWND g_hPnlInformationFlow = NULL;//信息流
HWND g_hPnlEmbedded = NULL;//嵌入式

// 定义自定义消息
#define WM_DESTROY_ADVERT (WM_USER + 300)
#define WM_SHOW_OPENSCREEN_ADVERT (WM_USER + 301)
#define WM_SHOW_ADVERT (WM_USER + 310)

//广告业务参数
const char* YourAppId = "9NC834S3H84L";
const char* YourSecretKey = "MIGTAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBHkwdwIBAQQgiJm0JnjgpDjxEKKzH/7kc3N8r+nvmHko1EPV6My6WG6gCgYIKoZIzj0DAQehRANCAAR2z1Eih/EOFjBMbpgMdvfYjUqFEVaRbnEeYEYZrp4K3pGj1YoY0/dmRRQ58OaHfxKotbFDMwNDBpuHwtxTqGE6";
const char* SplashScreenUnitId = "768338453d614f3aad85eea7e3916e7e";     //开屏:1920 x 1080
const char* ExitScreenUnitId = "7cdc7614b69c4118933e2067e6e14d01";       //退屏:1920 x 1080
const char* BannerUnitId = "e9b34829a2ad4a959874f9a180278bfe";           //横幅:728 x 90
const char* InterstitialUnitId = "e333abaf22404c4a8d382c1e7ba42076";     //插屏:1024 x 768
const char* CoupletUnitId = "c68cd45e8e374ccd98a704887e5b3582";          //对联:300 x 600
const char* RewardUnitId = "0f505442fac84f098e81d6f2ca04abe1";           //激励广告:1024x768
const char* InformationFlowUnitId = "6fab0e0912db497cbf886c2c4a9b131c";  //信息流，由开发者维护广告控件
const char* EmbeddedUnitId = "e065e44302314b888dcb6074fa6efd69";         //嵌入式，由开发者维护广告控件

void CreateSplashScreenAdPanel(HWND hWnd) {
    RECT rect;
    if (GetClientRect(hWnd, &rect))
    {
        int panelWidth = rect.right - rect.left;
        int panelHeight = rect.bottom - rect.top;
        HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
        g_hPnlSplashScreen = CreateWindowW(L"STATIC", L"This is a panel for Open Screen advertising", WS_CHILD | WS_VISIBLE, 0, 0, panelWidth, panelHeight, hWnd, (HMENU)2011, minstance, NULL);
        BringWindowToTop(g_hPnlSplashScreen);
    }
}

void CreateBannerAdPanel(HWND hWnd) {
    HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    RECT rect;
    GetClientRect(hWnd, &rect);
    int panelWidth = 728;
    int panelHeight = 90; // Banner广告尺寸 728x90
    int topMargin = (rect.bottom - rect.top - panelHeight - 50);
    int leftMargin = (rect.right - rect.left - panelWidth) / 2;

    // 创建Panel的窗口句柄
    g_hPnlBanner = CreateWindowW(L"STATIC", L"This is a panel for banner advertising", WS_CHILD | WS_VISIBLE, leftMargin, topMargin, 728, 90, hWnd, (HMENU)2031, minstance, NULL);
}

void CreateInterstitialAdPannel(HWND hWnd) {
    HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    RECT rect;
    GetClientRect(hWnd, &rect);
    int panelWidth = 1024;
    int panelHeight = 768;
    int topMargin = (rect.bottom - rect.top - panelHeight) / 2;
    int leftMargin = (rect.right - rect.left - panelWidth) / 2;

    g_hPnlInterstitial = CreateWindowW(L"STATIC", L"This is a panel for Interstitial advertising", WS_TABSTOP | WS_CHILD | WS_VISIBLE, leftMargin, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2041, minstance, NULL);
    BringWindowToTop(g_hPnlInterstitial); 
}

void CreateCoupletAdPannel(HWND hWnd) {
    HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    RECT rect;
    GetClientRect(hWnd, &rect);
    int panelWidth = 300;
    int panelHeight = 600; // 对联广告尺寸 300x600
    int topMargin = (rect.bottom - rect.top - panelHeight - 50);
    int leftMargin = (rect.right - rect.left - panelWidth);

    g_hPnlCoupletLeft = CreateWindowW(L"STATIC", L"This is a panel for CoupletLeft advertising", WS_TABSTOP | WS_CHILD | WS_VISIBLE, 0, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2051, minstance, NULL);
    //SetWindowPos(g_hPnlCoupletLeft, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    BringWindowToTop(g_hPnlCoupletLeft);
    g_hPnlCoupletRight = CreateWindowW(L"STATIC", L"This is a panel for CoupletRight advertising", WS_TABSTOP | WS_CHILD | WS_VISIBLE, leftMargin, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2052, minstance, NULL);
    //SetWindowPos(g_hPnlCoupletRight, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    BringWindowToTop(g_hPnlCoupletRight);
}

void CreateRewardAdPannel(HWND hWnd) {
    RECT rect;
    if (GetClientRect(hWnd, &rect))
    {
        int panelWidth = 1024;
        int panelHeight = 768;
        int topMargin = (rect.bottom - rect.top - panelHeight) / 2;
        int leftMargin = (rect.right - rect.left - panelWidth) / 2;

        HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
        g_hPnlReward = CreateWindowW(L"STATIC", L"This is a panel for Reward advertising", WS_TABSTOP | WS_CHILD | WS_VISIBLE, leftMargin, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2061, minstance, NULL);
        BringWindowToTop(g_hPnlReward);
    }
}

void CreateInformationFlowAdPanel(HWND hWnd) {
    HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    RECT rect;
    GetClientRect(hWnd, &rect);
    int panelWidth = 400;
    int panelHeight = 50;
    int topMargin = (rect.bottom - rect.top - panelHeight - 100);
    int leftMargin = 10;

    g_hPnlInformationFlow = CreateWindowW(L"STATIC", L"This is a panel for information flow advertising", WS_CHILD | WS_VISIBLE, leftMargin, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2071, minstance, NULL);
}

void CreateEmbeddedAdPanel(HWND hWnd) {
    HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    RECT rect;
    GetClientRect(hWnd, &rect);
    int panelWidth = 200;
    int panelHeight = 200;
    int topMargin = (rect.bottom - rect.top - panelHeight - 300);
    int leftMargin = 10;

    g_hPnlEmbedded = CreateWindowW(L"STATIC", L"This is a panel for embedded advertising", WS_CHILD | WS_VISIBLE, leftMargin, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2081, minstance, NULL);
}

//1.开屏 2.退屏 3.Banner 4.插屏 5.对联 6.激励视频 7.信息流 8.嵌入式
void CreateControls(HWND hWnd) {
    int y = 10;
    CreateWindowW(L"BUTTON", L"开屏(全屏)", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y, 100, 30, hWnd, (HMENU)ID_BTN_AD1, hInst, NULL);
    //退屏广告
    CreateWindowW(L"BUTTON", L"Exit Ad", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_EXITAD, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Banner", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_AD3, hInst, NULL);

    CreateWindowW(L"BUTTON", L"插屏", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_AD4, hInst, NULL);

    CreateWindowW(L"BUTTON", L"插屏仅视频", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_AD41, hInst, NULL);

    CreateWindowW(L"BUTTON", L"插屏仅Web", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_AD42, hInst, NULL);

    CreateWindowW(L"BUTTON", L"对联", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_AD5, hInst, NULL);

    CreateWindowW(L"BUTTON", L"激励视频", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_AD6, hInst, NULL);

    CreateWindowW(L"BUTTON", L"激励视频Web", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_AD61, hInst, NULL);

    CreateWindowW(L"BUTTON", L"信息流", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_AD7, hInst, NULL);

    CreateWindowW(L"BUTTON", L"嵌入式", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_AD8, hInst, NULL);

    // Log
    g_hwndLog = CreateWindow(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
        250, 10, 1200, 550, hWnd, (HMENU)ID_TXT_LOG, hInst, NULL);

    //信息流广告容器
    g_hPnlInformationFlow = CreateWindowW(L"STATIC", L"This is a panel for information flow advertising", WS_CHILD | WS_VISIBLE, 10, y += 250, 400, 50, hWnd, (HMENU)2071, hInst, NULL);
    BringWindowToTop(g_hPnlInformationFlow);
    //嵌入式广告容器
    g_hPnlEmbedded = CreateWindowW(L"STATIC", L"This is a panel for embedded advertising", WS_CHILD | WS_VISIBLE, 10, y += 60, 200, 200, hWnd, (HMENU)2081, hInst, NULL);
}

void InitMgAdSdk() {
    if (hDLL) return;

    hDLL = LoadLibrary(L"MgAdSDKCSharpDLL.dll");
    if (hDLL) {
        // Register the initialisation completion callback event
        if (auto func = (InitCompleteEvent)GetProcAddress(hDLL, "InitCompleteEvent")) // 初始化完成后的回调函数
            func(onInitCompleteEvent);
        if (auto func = (AdCloseEvent)GetProcAddress(hDLL, "AdCloseEvent")) //广告关闭事件的回调函数
            func(onAdCloseEvent);

        initialize(hDLL); // SDK initialisation
    }
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        CreateControls(hWnd);
        InitMgAdSdk();//初始化SDK
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;

        case ID_BTN_AD1:
        {//开屏（全屏）
            RECT clientRect;
            if (GetClientRect(hWnd, &clientRect)) {
                int clientWidth = clientRect.right - clientRect.left;
                int clientHeight = clientRect.bottom - clientRect.top;
                nlohmann::json json_obj = {
                 {"unitId", SplashScreenUnitId},
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
        case ID_BTN_AD3:
        {//Banner
            CreateBannerAdPanel(hWnd);
            int containerHandle = reinterpret_cast<int>(g_hPnlBanner);
            nlohmann::json json_obj = {
                {"unitId", BannerUnitId},
                {"media", "image"},
                {"appType", 1},
                {"adType", 3},//Banner
                {"handle", containerHandle}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD4:
        {//插屏
            CreateInterstitialAdPannel(hWnd);
            nlohmann::json json_obj = {
               {"unitId", InterstitialUnitId},
               {"appType", 1},
               {"adType", 4},
               {"handle", reinterpret_cast<int>(g_hPnlInterstitial)}
               //{"handle", reinterpret_cast<int>(g_hwndMain)}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD41:
        {//插屏广告
            CreateInterstitialAdPannel(hWnd);
            nlohmann::json json_obj = {
               {"unitId", InterstitialUnitId},
               {"media", "video"},//仅视频类型的素材，支持媒体类型(image,video,web)。可传空
               {"appType", 1},
               {"adType", 4},
               {"handle", reinterpret_cast<int>(g_hPnlInterstitial)}
               //{"handle", reinterpret_cast<int>(g_hwndMain)}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD42:
        {//插屏广告
            CreateInterstitialAdPannel(hWnd);
            nlohmann::json json_obj = {
               {"unitId", InterstitialUnitId},
               {"media", "web"},//支持媒体类型(image,video,web)。可传空
               {"appType", 1},
               {"adType", 4},
               {"handle", reinterpret_cast<int>(g_hPnlInterstitial)},
               {"width", 1024 },
               {"height", 768}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD5:
        {//对联
            CreateCoupletAdPannel(hWnd);
            nlohmann::json json_obj = {
                {"unitId", CoupletUnitId},
                {"appType", 1},
                {"adType", 5},
                {"handle", reinterpret_cast<int>(g_hPnlCoupletLeft)},
                {"handle2", reinterpret_cast<int>(g_hPnlCoupletRight)}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD6:
        {
            CreateRewardAdPannel(hWnd);
            nlohmann::json json_obj = {
                 {"unitId", RewardUnitId},
                 {"comment", "abc123"},//透传参数，前端需要进行urlEncode；在广告关闭回调事件中会原封不动的返回
                 {"appType", 1},
                 {"adType", 6},//激励视频
                 {"handle", reinterpret_cast<int>(g_hPnlReward)},
                 {"width", 1024},
                 {"height", 768}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD61:
        {
            CreateRewardAdPannel(hWnd);
            nlohmann::json json_obj = {
                 {"unitId", RewardUnitId},
                 {"comment", "abc123"},//透传参数，前端需要进行urlEncode；在广告关闭回调事件中会原封不动的返回
                 {"appType", 1},
                 {"adType", 6},//激励视频
                 {"media", "web"},
                 {"handle", reinterpret_cast<int>(g_hPnlReward)},
                 {"width", 1024},
                 {"height", 768}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD7:
        {
            int containerHandle = reinterpret_cast<int>(g_hPnlInformationFlow);
            nlohmann::json json_obj = {
                {"unitId", InformationFlowUnitId},
                {"media", "image"},
                {"appType", 1},
                {"adType", 7},//信息流
                {"width", 400},//信息流，需要传入程序的宽高
                {"height", 50},
                {"handle", containerHandle}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD8:
        {
            int containerHandle = reinterpret_cast<int>(g_hPnlEmbedded);
            nlohmann::json json_obj = {
                {"unitId", EmbeddedUnitId},
                {"media", "image"},
                {"appType", 1},
                {"adType", 8},//嵌入式
                {"width", 200},//嵌入式，需要传入程序的宽高
                {"height", 200},
                {"handle", containerHandle}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_EXITAD:
            showExitAdBlocking(hDLL);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_SHOW_OPENSCREEN_ADVERT:
    {
        CreateSplashScreenAdPanel(g_hwndMain);//创建开屏广告容器
        RECT clientRect;
        if (GetClientRect(g_hwndMain, &clientRect)) {
            int clientWidth = clientRect.right - clientRect.left;
            int clientHeight = clientRect.bottom - clientRect.top;
            nlohmann::json json_obj = {
             {"unitId", SplashScreenUnitId},
             {"appType", 1},
             {"adType", 1},
             {"handle", reinterpret_cast<int>(g_hPnlSplashScreen)},
             {"width", clientWidth},//开屏广告，需要传入程序的宽高
             {"height", clientHeight},
             {"parentWidth", clientWidth},
             {"parentHeight", clientHeight}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
        }
        return 0;
    }
    case WM_SHOW_ADVERT: {
        const char* json = reinterpret_cast<const char*>(lParam);
        if (json) {
            showAd(json);
            free((void*)json);
        }
        return 0;
    }
    case WM_DESTROY_ADVERT: {
        const char* json = reinterpret_cast<const char*>(lParam);
        if (json) {
            try
            {
                nlohmann::json json_obj = nlohmann::json::parse(json);
                std::string unitId = json_obj["unitId"];
                if (unitId == SplashScreenUnitId)
                {//删除开屏广告容器
                    DestroyWindow(g_hPnlSplashScreen);
                    g_hPnlSplashScreen = NULL;
                }
                if (unitId == InterstitialUnitId)
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
                else if (unitId == RewardUnitId)
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
                        reportAdRewardFulfillment(unitId.c_str(), resourceId.c_str(), materialId.c_str(), rewardId.c_str());
                        AppendLog(L"reportAdRewardFulfillment Async: %hs", rewardId.c_str());
                    }
                }
            }
            catch (const std::exception& ex)
            {
            }
            free((void*)json);
        }
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

#pragma region 1.SDK Initialisation
void initialize(HINSTANCE hdll) {
    try
    {
        Initialize func = (Initialize)GetProcAddress(hdll, "Initialize");
        func(YourAppId, YourSecretKey);
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

            PostMessage(g_hwndMain, WM_SHOW_OPENSCREEN_ADVERT, 0, NULL);//Open screen advertisement 
        }
    }
    catch (const std::exception&)
    { 
    } 
}
#pragma endregion

#pragma region 2.AD
//1.Load the advert on the fallback screen, after SDK initialisation is complete
void setupExitAd(HINSTANCE hdll) {
    if (auto func = (SetupExitAd)GetProcAddress(hdll, "SetupExitAd")) {
        func(const_cast<char*>(ExitScreenUnitId)); // passing in the key value of the advert's primary key that was created in the mg backend.

        AppendLog(L"Load the resources for MG exit ad");
    }
}

//2.Turn on the fallback screen advert
void showExitAdBlocking(HINSTANCE hdll) {
    try
    {
        if (auto func = (ShowExitAdBlocking)GetProcAddress(hdll, "ShowExitAdBlocking")) {
            func();
            AppendLog(L"show fallback screen advert");
        } 
    }
    catch (const std::exception&)
    {
    }
}

//3.Register Callback event to close the advert
void onAdCloseEvent(char* s) {
    AppendLog(L"onAdCloseEvent: %hs", s);
    //...
    // Destroy Ad pannel 

    // 发送到主UI线程
    char* jsonCopy = _strdup(s);
    PostMessage(g_hwndMain, WM_DESTROY_ADVERT, 0, reinterpret_cast<LPARAM>(jsonCopy));
}
//4.Open MG Advert window
void showAd(const char* json) {
    try
    {
        // 确保在调用前COM已初始化
        if (!g_comInitialized) {
            HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
            if (SUCCEEDED(hr)) {
                g_comInitialized = true;
            }
        }

        ShowAd func = (ShowAd)GetProcAddress(hDLL, "ShowAd");
        if (func) {
            int result = func(json);
            if (result == 1) {
                // success 
            }
        }
    }
    catch (const std::exception& ex)
    {
    }
}
//5.Report to MG
void reportAdRewardFulfillment(const char* unitId, const char* resourceId, const char* materialId, const char* rewardId) {
    ReportAdRewardFulfillment func = (ReportAdRewardFulfillment)GetProcAddress(hDLL, "ReportAdRewardFulfillment");
    if (func) {
        func(unitId, resourceId, materialId, rewardId);
    }
}
#pragma endregion

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}