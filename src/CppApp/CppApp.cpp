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

//MG Ads SDK相关的全局变量 (Global variables related to MG Ads SDK)
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

    // 初始化 COM 为 STA (Initialize COM as STA)
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
    if (!InitInstance(hInstance, nCmdShow))
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

    // 清理 COM  (Clean up COM)
    if (g_comInitialized) {
        CoUninitialize();
    }

    return (int)msg.wParam;
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

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CPPAPP));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CPPAPP);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
        // Conversion failed
        return L"";
    }
    std::vector<wchar_t> buffer(required);
    MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, buffer.data(), required);
    return std::wstring(buffer.data());
}

HWND g_hPnlCmp = NULL; //CMP容器，启动时在程序的最上面，宽度铺满，高度50 (CMP container, at startup it should be at the very top of the program, width full, height 50)
HWND g_hPnlSplashScreen = NULL; //开屏 [Splash Screen]
HWND g_hPnlBanner = NULL; //横幅 [Banner]
HWND g_hPnlCoupletLeft = NULL; //对联左 [Couplet left]
HWND g_hPnlCoupletRight = NULL; //对联右 [Couplet right]
HWND g_hPnlInterstitial = NULL;//插屏 [Interstitial] 
HWND g_hPnlReward = NULL;//激励视频 [Rewarded Video]
HWND g_hPnlInformationFlow = NULL;//信息流 [Information flow]
HWND g_hPnlEmbedded = NULL;//嵌入式 [Embedded]

// 自定义消息
#define WM_SHOW_ADVERT (WM_USER + 300)
#define WM_SHOW_OPENSCREEN_ADVERT (WM_USER + 301)
#define WM_DESTROY_CMP (WM_USER + 310)
#define WM_DESTROY_ADVERT (WM_USER + 311)

int g_cmpChangedWidth = 0;
int g_cmpChangedHeight = 0;
int g_cmpSdkControlHandle = 0;

//1.开屏[Splash Screen] 2.退屏[Exit Screen] 3.Banner 4.插屏[Interstitial] 5.对联[Couplet] 6.激励视频[Rewarded Video] 7.信息流[Information flow] 8.嵌入式[Embedded]
const char* YourAppId = "69316b6861328938223cc124";
const char* YourSecretKey = "MIGTAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBHkwdwIBAQQgZgULOuiIDYZyGiUyYdGr3odHVN6ebZ1uDwXx7PXiHh2gCgYIKoZIzj0DAQehRANCAASf1FWCfsSn/tXFVRt04C7JkpRG12KSC3wnaJRWb5QWin9dsBk1OR31BCsELMYtWsFhA7e6Q6Fi4Mi6+ub24O5a";
const char* SplashScreenUnitId = "b871f83c5e8845f1b43325561bcdd6c7";     //Splash Screen:1920 x 1080
const char* ExitScreenUnitId = "5076eab6ae1042b6b92f73ea01981475";       //Exit Screen:1920 x 1080
const char* BannerUnitId = "cb7d9688a2d9499992febb6b642b3625";           //Banner:728 x 90
const char* InterstitialUnitId = "2cb66a1301404561881a3f26b6ce5ba7";     //Interstitial:1024 x 768
const char* CoupletUnitId = "b502f6e6281c43e4b28ea22503471039";          //Couplet:300 x 600
const char* RewardUnitId = "2ae60936ba664fbfb7d92ce3a19c2915";           //Rewarded Video:1024x768
const char* InformationFlowUnitId = "f152f6caf7a8440f8510bc31534baf4e";  //Information flow,Ad control is maintained by the developer.
const char* EmbeddedUnitId = "4192966a9db343f48dd2f6308ea9ec30";         //Embedded,Ad control is maintained by the developer.

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
    RECT rect;
    if (GetClientRect(hWnd, &rect))
    {
        int panelWidth = 728;
        int panelHeight = 90; // Banner 728x90
        int topMargin = (rect.bottom - rect.top - panelHeight - 50);
        int leftMargin = (rect.right - rect.left - panelWidth) / 2;

        HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
        g_hPnlBanner = CreateWindowW(L"STATIC", L"This is a panel for banner advertising", WS_CHILD | WS_VISIBLE, leftMargin, topMargin, 728, 90, hWnd, (HMENU)2031, minstance, NULL);
    }
}

void CreateInterstitialAdPannel(HWND hWnd) {
    RECT rect;
    if (GetClientRect(hWnd, &rect))
    {
        int panelWidth = 1024;
        int panelHeight = 768;
        int topMargin = (rect.bottom - rect.top - panelHeight) / 2;
        int leftMargin = (rect.right - rect.left - panelWidth) / 2;

        HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
        g_hPnlInterstitial = CreateWindowW(L"STATIC", L"This is a panel for Interstitial advertising", WS_TABSTOP | WS_CHILD | WS_VISIBLE, leftMargin, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2041, minstance, NULL);
        BringWindowToTop(g_hPnlInterstitial);
    }
}

void CreateCoupletAdPannel(HWND hWnd) {
    HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    RECT rect;
    if (GetClientRect(hWnd, &rect))
    {
        int panelWidth = 300;
        int panelHeight = 600;
        int topMargin = (rect.bottom - rect.top - panelHeight - 50);
        int leftMargin = (rect.right - rect.left - panelWidth);

        g_hPnlCoupletLeft = CreateWindowW(L"STATIC", L"This is a panel for CoupletLeft advertising", WS_TABSTOP | WS_CHILD | WS_VISIBLE, 0, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2051, minstance, NULL);
        //SetWindowPos(g_hPnlCoupletLeft, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        BringWindowToTop(g_hPnlCoupletLeft);
        g_hPnlCoupletRight = CreateWindowW(L"STATIC", L"This is a panel for CoupletRight advertising", WS_TABSTOP | WS_CHILD | WS_VISIBLE, leftMargin, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2052, minstance, NULL);
        //SetWindowPos(g_hPnlCoupletRight, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        BringWindowToTop(g_hPnlCoupletRight);
    }
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
    RECT rect;
    if (GetClientRect(hWnd, &rect))
    {
        int panelWidth = 400;
        int panelHeight = 50;
        int topMargin = (rect.bottom - rect.top - panelHeight - 100);
        int leftMargin = 10;

        HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
        g_hPnlInformationFlow = CreateWindowW(L"STATIC", L"This is a panel for information flow advertising", WS_CHILD | WS_VISIBLE, leftMargin, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2071, minstance, NULL);
    }
}

void CreateEmbeddedAdPanel(HWND hWnd) {
    RECT rect;
    if (GetClientRect(hWnd, &rect))
    {
        int panelWidth = 200;
        int panelHeight = 200;
        int topMargin = (rect.bottom - rect.top - panelHeight - 300);
        int leftMargin = 10;

        HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
        g_hPnlEmbedded = CreateWindowW(L"STATIC", L"This is a panel for embedded advertising", WS_CHILD | WS_VISIBLE, leftMargin, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2081, minstance, NULL);
    }
}

void CreateControls(HWND hWnd) {
    int y = 10;
    CreateWindowW(L"BUTTON", L"开屏(全屏)", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y, 100, 30, hWnd, (HMENU)ID_BTN_AD1, hInst, NULL);
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

    CreateWindowW(L"BUTTON", L"关闭CMP", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_DELCMP, hInst, NULL);

    // Log
    g_hwndLog = CreateWindow(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
        250, 10, 1200, 550, hWnd, (HMENU)ID_TXT_LOG, hInst, NULL);

    //信息流广告容器 (Information flow ad container)
    g_hPnlInformationFlow = CreateWindowW(L"STATIC", L"This is a panel for information flow advertising", WS_CHILD | WS_VISIBLE, 10, y += 250, 400, 50, hWnd, (HMENU)2071, hInst, NULL);
    BringWindowToTop(g_hPnlInformationFlow);
    //嵌入式广告容器 (Embedded ad container)
    g_hPnlEmbedded = CreateWindowW(L"STATIC", L"This is a panel for embedded advertising", WS_CHILD | WS_VISIBLE, 10, y += 60, 200, 200, hWnd, (HMENU)2081, hInst, NULL);
}

void InitMgAdSdk(HWND hWnd) {
    if (hDLL) return;

    hDLL = LoadLibrary(L"MgAdSDKCSharpDLL.dll");
    if (hDLL) {
        // Register the CMP callback event
        if (auto func = (CmpSizeChangedEvent)GetProcAddress(hDLL, "CmpSizeChangedEvent")) // CMP容器尺寸发生改变 (CMP container size changed)
            func(onCmpSizeChangedEvent);
        if (auto func = (CmpClosedEvent)GetProcAddress(hDLL, "CmpClosedEvent")) //CMP窗口已关闭 (CMP closed)
            func(onCmpClosedEvent);

        // Register the initialisation completion callback event
        if (auto func = (InitCompleteEvent)GetProcAddress(hDLL, "InitCompleteEvent")) // 初始化完成后的回调函数 (Callback function after initialization is complete)
            func(onInitCompleteEvent);
        if (auto func = (AdCloseEvent)GetProcAddress(hDLL, "AdCloseEvent")) //广告关闭事件的回调函数 (Callback function for ad close event)
            func(onAdCloseEvent);

        //1.设置参数 (Set parameters)
        setAppId(hDLL, YourAppId, YourSecretKey);

        //2.首先判断是否需要弹出CMP (First, determine whether to pop up CMP)
        if (isOpenCmp(hDLL))
        {
            RECT clientRect;
            if (GetClientRect(hWnd, &clientRect)) {
                int panelWidth = clientRect.right - clientRect.left;
                int panelHeight = clientRect.bottom - clientRect.top;

                //2.1.创建CMP容器 (Create CMP container)
                HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
                g_hPnlCmp = CreateWindowW(L"STATIC", L"This is a panel for CMP", WS_CHILD | WS_VISIBLE, 0, 0, panelWidth, 50, hWnd, (HMENU)3011, minstance, NULL);
                BringWindowToTop(g_hPnlCmp);

                //2.2.弹出CMP (Show CMP)
                nlohmann::json json_obj = {
                 {"handle", reinterpret_cast<int>(g_hPnlCmp)},
                 {"parentWidth", panelWidth},
                 {"parentHeight", panelHeight}
                };
                std::string jsonStr = json_obj.dump();
                openCmp(hDLL, jsonStr.c_str());//返回SDK中CMP控件的句柄，使用全局变量g_cmpSdkControlHandle保存 (Returns the handle of the CMP control in the SDK, saved using the global variable g_cmpSdkControlHandle.)
            }
        }

        //3.SDK initialisation
        initialize(hDLL);
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
        InitMgAdSdk(hWnd);//SDK initialize
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
        {//开屏 [Splash Screen]
            RECT clientRect;
            if (GetClientRect(hWnd, &clientRect)) {
                int clientWidth = clientRect.right - clientRect.left;
                int clientHeight = clientRect.bottom - clientRect.top;
                nlohmann::json json_obj = {
                 {"unitId", SplashScreenUnitId},
                 {"appType", 1},
                 {"adType", 1},//1.开屏[Splash Screen] 2.退屏[Exit Screen] 3.Banner 4.插屏[Interstitial] 5.对联[Couplet] 6.激励视频[Rewarded Video] 7.信息流[Information flow] 8.嵌入式[Embedded]
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
        {//插屏 [Interstitial]
            CreateInterstitialAdPannel(hWnd);
            nlohmann::json json_obj = {
               {"unitId", InterstitialUnitId},
               {"appType", 1},
               {"adType", 4},//Interstitial
               {"handle", reinterpret_cast<int>(g_hPnlInterstitial)}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD41:
        {//Interstitial
            CreateInterstitialAdPannel(hWnd);
            nlohmann::json json_obj = {
               {"unitId", InterstitialUnitId},
               {"media", "video"},//Only video-type creatives,support media types (image, video, web). Can be left empty.
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
        {//Interstitial
            CreateInterstitialAdPannel(hWnd);
            nlohmann::json json_obj = {
               {"unitId", InterstitialUnitId},
               {"media", "web"},//Supports media types (image, video, web). Can be left empty.
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
        {//对联[Couplet]
            CreateCoupletAdPannel(hWnd);
            nlohmann::json json_obj = {
                {"unitId", CoupletUnitId},
                {"appType", 1},
                {"adType", 5},//Couplet
                {"handle", reinterpret_cast<int>(g_hPnlCoupletLeft)},
                {"handle2", reinterpret_cast<int>(g_hPnlCoupletRight)}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD6:
        {//激励视频[Rewarded Video]
            CreateRewardAdPannel(hWnd);
            nlohmann::json json_obj = {
                 {"unitId", RewardUnitId},
                 {"comment", "abc123"},//透传参数，前端需要进行urlEncode；在广告关闭回调事件中会原封不动的返回 (Passthrough parameter, the frontend needs to perform urlEncode; it will be returned unchanged in the ad close callback event.)
                 {"appType", 1},
                 {"adType", 6},//Rewarded Video
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
                 {"comment", "abc123"},
                 {"appType", 1},
                 {"adType", 6},
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
        {//信息流[Information flow] 
            int containerHandle = reinterpret_cast<int>(g_hPnlInformationFlow);
            nlohmann::json json_obj = {
                {"unitId", InformationFlowUnitId},
                {"media", "image"},
                {"appType", 1},
                {"adType", 7},//Information flow
                {"width", 400},//信息流，需要传入容器的宽高 (Information flow requires passing in the container's width and height.)
                {"height", 50},
                {"handle", containerHandle}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD8:
        {//嵌入式[Embedded]
            int containerHandle = reinterpret_cast<int>(g_hPnlEmbedded);
            nlohmann::json json_obj = {
                {"unitId", EmbeddedUnitId},
                {"media", "image"},
                {"appType", 1},
                {"adType", 8},//Embedded
                {"width", 200},//嵌入式，需要传入容器的宽高 (Embedded requires passing in the container's width and height.)
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
    case WM_SHOW_OPENSCREEN_ADVERT://在UI线程中显示开屏广告 (Display the splash screen ad in the UI thread.)
    {
        CreateSplashScreenAdPanel(g_hwndMain);
        RECT clientRect;
        if (GetClientRect(g_hwndMain, &clientRect)) {
            int clientWidth = clientRect.right - clientRect.left;
            int clientHeight = clientRect.bottom - clientRect.top;
            nlohmann::json json_obj = {
             {"unitId", SplashScreenUnitId},
             {"appType", 1},
             {"adType", 1},
             {"handle", reinterpret_cast<int>(g_hPnlSplashScreen)},
             {"width", clientWidth},//开屏广告，需要传入程序的宽高 (Splash screen ad requires passing in the program's width and height.)
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
    case WM_DESTROY_CMP: {
        try
        {
            DestroyWindow(g_hPnlCmp);
            g_hPnlCmp = NULL;
        }
        catch (const std::exception& ex)
        {
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
                {//删除开屏广告容器 (Delete splash screen ad container)
                    DestroyWindow(g_hPnlSplashScreen);
                    g_hPnlSplashScreen = NULL;
                }
                else if (unitId == InterstitialUnitId)
                {//删除插屏广告容器 (Delete interstitial ad container)
                    DestroyWindow(g_hPnlInterstitial);
                    g_hPnlInterstitial = NULL;
                }
                else if (unitId == BannerUnitId)
                {//删除Banner广告容器 (Delete banner ad container)
                    DestroyWindow(g_hPnlBanner);
                    g_hPnlBanner = NULL;
                }
                else if (unitId == CoupletUnitId)
                {//删除对联广告容器 (Delete couplet ad container)
                    int advertStatus = json_obj["advertStatus"];
                    if (advertStatus == 1)//广告已显示，由用户关闭 (Ad already displayed, closed by user)
                    {
                        int coupletType = json_obj["coupletType"];
                        if (coupletType == 1)//删除左侧容器 (Delete left container)
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
                    else
                    {//没有获取到广告素材，不显示广告。删除已创建的容器 (No ad creatives obtained, ad not displayed. Delete created container)
                        BOOL result = DestroyWindow(g_hPnlCoupletLeft);
                        g_hPnlCoupletLeft = NULL;

                        BOOL result2 = DestroyWindow(g_hPnlCoupletRight);
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
                        //Video playback finished, issue reward items
                        //... 

                        //向MG核销订单 (Report order fulfillment to MG)
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
    case WM_SIZE:
        if (g_hPnlCmp != NULL) //App窗口尺寸发生改变时，需修改CMP容器的尺寸 (When the window size changes, the CMP container's size needs to be modified.)
        {
            if (g_cmpChangedWidth > 0 && g_cmpChangedHeight > 0)
            {
                //1.CMP界面中用户选择"自定义"，CMP界面由上部的横幅改变为程序中间的矩形 (In the CMP interface, when the user selects "Custom", the CMP interface changes from the top banner to a rectangle in the middle of the program.)
                RECT parentRect;
                if (GetClientRect(g_hwndMain, &parentRect)) {
                    int parentWidth = parentRect.right - parentRect.left;
                    int parentHeight = parentRect.bottom - parentRect.top;
                    int x = (parentWidth - g_cmpChangedWidth) / 2;
                    int y = (parentHeight - g_cmpChangedHeight) / 2;
                    SetWindowPos(g_hPnlCmp, NULL, x, y, g_cmpChangedWidth, g_cmpChangedHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
                }
            }
            else
            {
                //2.CMP原始界面。高度固定50px，宽度保持铺满App (CMP original interface. The height is fixed at 50px, and the width remains full across the App.)
                RECT cmpRect;
                GetClientRect(g_hPnlCmp, &cmpRect);
                int cmpWidth = cmpRect.right - cmpRect.left;
                int cmpHeight = cmpRect.bottom - cmpRect.top;
                RECT parentRect;
                GetClientRect(g_hwndMain, &parentRect);
                int parentWidth = parentRect.right - parentRect.left;
                if (cmpWidth != parentWidth)
                {
                    //修改CMP容器的尺寸 (Modify the size of the CMP container.)
                    SetWindowPos(g_hPnlCmp, NULL, 0, 0, parentWidth, cmpHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
                    if (g_cmpSdkControlHandle > 0)
                    {
                        //修改SDK中CMP控件的尺寸。此Handle是OpenCmp接口返回 (Modify the size of the CMP control within the SDK. This Handle is returned by the OpenCmp interface.)
                        HWND hWndSdkControl = (HWND)(INT_PTR)g_cmpSdkControlHandle;
                        SetWindowPos(hWndSdkControl, NULL, 0, 0, parentWidth, cmpHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
                    }
                }
            }
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
#pragma region 1.CMP
/*
业务流程：
1.在App启动时，先设置appId和secretKey。
2.判断是否需要显示CMP。
    若需要则显示CMP，OpenCmp接口会返回SDK中的CMP控件句柄。
    在程序尺寸发生改变时，需要维护CMP容器跟随发生改变。
3.CMP初始窗口的高度为50px，固定在App顶部。当用户选择"自定义"，CMP界面将改变为显示在程序中间的矩形区域。
4.当用户完成选择之后，需要移除CMP容器。
5.CMP界面仅在App首次运行时会显示。

Business Process:
1.When the application starts, first set the appId and secretKey.
2.Determine whether to display the CMP.
    If required, display the CMP. The OpenCmp interface will return the handle of the CMP control within the SDK.
    When the program size changes, the CMP container must be adjusted accordingly.
3.The initial CMP window has a fixed height of 50px and is positioned at the top of the application. If the user selects "Custom", the CMP interface will change to a rectangular area displayed in the middle of the program.
4.After the user completes their selection, remove the CMP container.
5.The CMP interface will only be presented during the first launch of the App.
*/
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
        g_cmpSdkControlHandle = func(jsonParam);//返回SDK中CMP控件的句柄
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

        //移除CMP容器 (Remove CMP container)
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
/*
业务流程
1.在CMP之后，紧接着进行SDK的初始化。
2.SDK初始化完成之后，根据需要设置退屏广告单元Id、显示开屏广告。
    请注意：开屏广告需要在UI线程中创建广告容器。

Business Process:

1. After the CMP, proceed immediately with the initialization of the SDK.
2. Upon successful SDK initialization, set up the exit screen ad unit ID and display the splash screen ad as needed.
   Please note: The splash screen ad requires creating the ad container within the UI thread.
*/
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

            PostMessage(g_hwndMain, WM_SHOW_OPENSCREEN_ADVERT, 0, NULL);//Open screen advertisement 
        }
    }
    catch (const std::exception&)
    {
    }
}
#pragma endregion

#pragma region 3.AD
//1.Load the advert on the fallback screen, after SDK initialisation is complete
void setupExitAd(HINSTANCE hdll) {
    if (auto func = (SetupExitAd)GetProcAddress(hdll, "SetupExitAd")) {
        func(ExitScreenUnitId); // passing in the key value of the advert's primary key that was created in the mg backend.

        AppendLog(L"Load the resources for MG exit ad");
    }
}

//2.Turn on the fallback screen advert
void showExitAdBlocking(HINSTANCE hdll) {
    if (auto func = (ShowExitAdBlocking)GetProcAddress(hdll, "ShowExitAdBlocking")) {
        func();
        AppendLog(L"show fallback screen advert");
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