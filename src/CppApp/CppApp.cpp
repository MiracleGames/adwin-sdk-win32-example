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

//Global variables related to MG Ads SDK
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

    // Initialize COM as STA
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr)) {
        g_comInitialized = true;
    }
    else {
        MessageBox(nullptr, L"COM Initialize failed", L"Error", MB_ICONERROR);
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

    // Clean up COM
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
        // Conversion failed
        return L"";
    }
    std::vector<wchar_t> buffer(required);
    MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, buffer.data(), required);
    return std::wstring(buffer.data());
}

HWND g_hPnlCmp = NULL; //CMP container, at startup it should be at the very top of the program, width full, height 50
HWND g_hPnlSplashScreen = NULL; //[FullScreen Ad]
HWND g_hPnlBanner = NULL; //[Banner]
HWND g_hPnlCoupletLeft = NULL; //[Couplet left]
HWND g_hPnlCoupletRight = NULL; //[Couplet right]
HWND g_hPnlInterstitial = NULL;//[Interstitial] 
HWND g_hPnlReward = NULL;//[Rewarded]
HWND g_hPnlFeed = NULL;//[Feed]
HWND g_hPnlEmbedded = NULL;//[Embedded]
HWND g_hBtnRewardedAdShow = NULL;
HWND g_hBtnInterstitialAdShow = NULL;
// Custom Message
#define WM_SHOW_ADVERT (WM_USER + 300)
#define WM_SHOW_OPENSCREEN_ADVERT (WM_USER + 301)
#define WM_DESTROY_CMP (WM_USER + 310)
#define WM_DESTROY_ADVERT (WM_USER + 311)

int g_cmpChangedWidth = 0;
int g_cmpChangedHeight = 0;
int g_cmpSdkControlHandle = 0;

/*
* 1.Downlodad the SDK  https://doc.mg-ads.com/en/docs/AdSdkService/CppSdk/SdkDownload
* 2.Reference the SDK  https://doc.mg-ads.com/en/docs/AdSdkService/CppSdk/DevelopmentEnvironmentConfig
*/

//1.FullScreen Ad 2.Exit Ad 3.Banner 4.Interstitial 5.Couplet 6.Rewarded 7.Feed 8.Embedded
const char* YourAppId = "69316b6861328938223cc124";
const char* YourSecretKey = "MIGTAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBHkwdwIBAQQgZgULOuiIDYZyGiUyYdGr3odHVN6ebZ1uDwXx7PXiHh2gCgYIKoZIzj0DAQehRANCAASf1FWCfsSn/tXFVRt04C7JkpRG12KSC3wnaJRWb5QWin9dsBk1OR31BCsELMYtWsFhA7e6Q6Fi4Mi6+ub24O5a";
const char* FullScreenAdUnitId = "b871f83c5e8845f1b43325561bcdd6c7";             //FullScreen Ad:1920 x 1080
const char* ExitAdUnitId = "5076eab6ae1042b6b92f73ea01981475";                 //Exit Ad:1920 x 1080
const char* BannerUnitId = "cb7d9688a2d9499992febb6b642b3625";               //Banner Ad:728 x 90
const char* InterstitialUnitId = "2cb66a1301404561881a3f26b6ce5ba7";           //Interstitial Ad:1024 x 768
const char* CoupletUnitId = "b502f6e6281c43e4b28ea22503471039";              //Couple Ad:300 x 600
const char* RewardedUnitId = "2ae60936ba664fbfb7d92ce3a19c2915";           //Rewarded Ad:1024x768
const char* FeedUnitId = "f152f6caf7a8440f8510bc31534baf4e";                      //Feed，Developers need to maintain the advertising control.
const char* EmbeddedUnitId = "4192966a9db343f48dd2f6308ea9ec30";         //Embedded，Developers need to maintain the advertising control.

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
        BringWindowToTop(g_hPnlBanner);
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
        g_hPnlReward = CreateWindowW(L"STATIC", L"This is a panel for Rewarded advertising", WS_TABSTOP | WS_CHILD | WS_VISIBLE, leftMargin, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2061, minstance, NULL);
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
        g_hPnlFeed = CreateWindowW(L"STATIC", L"This is a panel for feed advertising", WS_CHILD | WS_VISIBLE, leftMargin, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2071, minstance, NULL);
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
    CreateWindowW(L"BUTTON", L"FullScreen Ad", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y, 130, 30, hWnd, (HMENU)ID_BTN_AD1, hInst, NULL);
    CreateWindowW(L"BUTTON", L"Exit Ad", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_EXITAD, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Banner", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD3, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Interstitial", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD4, hInst, NULL);
    CreateWindowW(L"BUTTON", L"I Preload ad", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        150, y, 130, 30, hWnd, (HMENU)ID_BTN_AD4PRELOAD, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Interstitial Video", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD41, hInst, NULL);
    g_hBtnInterstitialAdShow = CreateWindowW(L"BUTTON", L"I Preload show", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        150, y, 130, 30, hWnd, (HMENU)ID_BTN_AD4PRESHOW, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Interstitial Web", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD42, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Couplet", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD5, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Rewarded", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD6, hInst, NULL);
    CreateWindowW(L"BUTTON", L"Preload rewarded", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        150, y, 130, 30, hWnd, (HMENU)ID_BTN_AD6PRELOAD, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Rewarded Web", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD61, hInst, NULL);
    g_hBtnRewardedAdShow = CreateWindowW(L"BUTTON", L"Preload show", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        150, y, 130, 30, hWnd, (HMENU)ID_BTN_AD6PRESHOW, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Feed", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD7, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Embedded", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD8, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Open CMP again", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_OPENCMP, hInst, NULL);

    // Log
    g_hwndLog = CreateWindow(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
        300, 10, 1200, 200, hWnd, (HMENU)ID_TXT_LOG, hInst, NULL);

    EnableWindow(g_hBtnInterstitialAdShow, FALSE);
    EnableWindow(g_hBtnRewardedAdShow, FALSE);
    //Feed ad container
    g_hPnlFeed = CreateWindowW(L"STATIC", L"This is a panel for Feed advertising", WS_CHILD | WS_VISIBLE, 10, y += 150, 400, 50, hWnd, (HMENU)2071, hInst, NULL);
    BringWindowToTop(g_hPnlFeed);
    //Embedded ad container
    g_hPnlEmbedded = CreateWindowW(L"STATIC", L"This is a panel for embedded advertising", WS_CHILD | WS_VISIBLE, 10, y += 60, 200, 200, hWnd, (HMENU)2081, hInst, NULL);
    BringWindowToTop(g_hPnlEmbedded);
}

void InitMgAdSdk(HWND hWnd) {
    if (hDLL) return;

    hDLL = LoadLibrary(L"MgAdSDKCSharpDLL.dll");
    if (hDLL) {
        // Register the CMP callback event
        if (auto func = (CmpClosedEvent)GetProcAddress(hDLL, "CmpClosedEvent")) //CMP closed
            func(onCmpClosedEvent);

        // Register the initialisation completion callback event
        if (auto func = (InitCompleteEvent)GetProcAddress(hDLL, "InitCompleteEvent")) //Callback function after initialization is complete
            func(onInitCompleteEvent);
        if (auto func = (AdCloseEvent)GetProcAddress(hDLL, "AdCloseEvent")) //Callback function for ad close event
            func(onAdCloseEvent);
        if (auto func = (AdPreloadEvent)GetProcAddress(hDLL, "AdPreloadEvent")) //Callback function for ad preload event
            func(onAdPreloadEvent);
        if (auto func = (AdShowPreloadEvent)GetProcAddress(hDLL, "AdShowPreloadEvent")) //Callback function for show ad preload event
            func(onAdShowPreloadEvent);
        

        //1.Set parameters
        setAppId(hDLL, YourAppId, YourSecretKey);

        //2.Show CMP 
        nlohmann::json json_obj = {
        {"ignoreExpiredCheck",false},// false (recommended): Popup will not appear again after user's first choice, compliant with GDPR; true: Popup appears on every launch, suitable for test environment
        {"width", 900},//Optional; default value is 900
        {"height", 500}//Optional; default value is 900
        };
        std::string jsonStr = json_obj.dump();
        openCmp(hDLL, jsonStr.c_str());
        //3.Initialize the SDK after the CMP call completes.
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
        {//FullScreen Ad
            RECT clientRect;
            if (GetClientRect(hWnd, &clientRect)) {
                int clientWidth = clientRect.right - clientRect.left;
                int clientHeight = clientRect.bottom - clientRect.top;
                nlohmann::json json_obj = {
                 {"unitId", FullScreenAdUnitId},
                 {"appType", 1},
                 {"adType", 1},//1.FullScreen Ad 2.Exit Ad 3.Banner 4.Interstitial 5.Couplet 6.Rewarded 7.Feed 8.Embedded
                 {"handle", reinterpret_cast<int>(g_hwndMain)},
                 //{"width", clientWidth},//For splash ad, the program's width and height must be provided.
                 //{"height", clientHeight},
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
                //{"media", "web"},//Only image-type creatives,support media types (image, video, web). Can be left empty.
                {"appType", 1},
                {"adType", 3},//Banner
                {"handle", containerHandle}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD4:
        {//Interstitial
            CreateInterstitialAdPannel(hWnd);
            nlohmann::json json_obj = {
               {"unitId", InterstitialUnitId},
               {"appType", 1},
               {"adType", 4},
               {"handle", reinterpret_cast<int>(g_hPnlInterstitial)}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD4PRELOAD: {
            nlohmann::json json_obj = {
                  {"unitId", InterstitialUnitId},
                  {"appType", 1},
                  {"adType", 4}
            };
            std::string jsonStr = json_obj.dump();
            preloadAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD4PRESHOW: {
            CreateInterstitialAdPannel(g_hwndMain);
            nlohmann::json json_obj = {
                  {"unitId", InterstitialUnitId},
                  {"appType", 1},
                  {"adType", 4},
                  {"handle", reinterpret_cast<int>(g_hPnlInterstitial)}
            };
            std::string jsonStr = json_obj.dump();
            showPreloadAd(jsonStr.c_str(), g_hBtnInterstitialAdShow);
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
        {//Couplet
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
        {//Rewarded
            CreateRewardAdPannel(hWnd);
            nlohmann::json json_obj = {
                 {"unitId", RewardedUnitId},
                 {"comment", "abc123"},//Passthrough parameter, the frontend needs to perform urlEncode; it will be returned unchanged in the ad close callback event.
                 {"appType", 1},
                 {"adType", 6},
                 {"handle", reinterpret_cast<int>(g_hPnlReward)},
                 {"width", 1024},
                 {"height", 768}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD6PRELOAD: {
            nlohmann::json json_obj = {
                 {"unitId", RewardedUnitId},
                 {"comment", "abc123"},
                 {"appType", 1},
                 {"adType", 6},
                 //{"media", "web"},
                 {"width", 1024},
                 {"height", 768}
            };
            std::string jsonStr = json_obj.dump();
            preloadAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD6PRESHOW: {
            CreateRewardAdPannel(hWnd);
            nlohmann::json json_obj = {
                 {"unitId", RewardedUnitId},
                 {"comment", "abc123"},
                 {"appType", 1},
                 {"adType", 6},
                 //{"media", "web"},
                 {"handle", reinterpret_cast<int>(g_hPnlReward)},
                 {"width", 1024},
                 {"height", 768}
            };
            std::string jsonStr = json_obj.dump();
            showPreloadAd(jsonStr.c_str(), g_hBtnRewardedAdShow);
            break;
        }
        case ID_BTN_AD61:
        {
            CreateRewardAdPannel(hWnd);
            nlohmann::json json_obj = {
                 {"unitId", RewardedUnitId},
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
        {//Feed
            int containerHandle = reinterpret_cast<int>(g_hPnlFeed);
            nlohmann::json json_obj = {
                {"unitId", FeedUnitId},
                {"media", "web"},
                {"appType", 1},
                {"adType", 7},
                {"width", 400},//Feed requires passing in the container's width and height.
                {"height", 50},
                {"handle", containerHandle}
            };
            std::string jsonStr = json_obj.dump();
            showAd(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD8:
        {//Embedded
            int containerHandle = reinterpret_cast<int>(g_hPnlEmbedded);
            nlohmann::json json_obj = {
                {"unitId", EmbeddedUnitId},
                {"media", "web"},
                {"appType", 1},
                {"adType", 8},
                {"width", 200},//Embedded requires passing in the container's width and height.
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
        case ID_BTN_OPENCMP: {
            bool needcmp = getUserRegionCmpRequirement(hDLL);//This method relies on the store package and always returns false when debugging locally.
            if (needcmp)
            {
                AppendLog(L"CMP will open");
                nlohmann::json json_obj = {
                   {"ignoreExpiredCheck",true},// false (recommended): Popup will not appear again after user's first choice, compliant with GDPR; true: Popup appears on every launch, suitable for test environment
                   {"width", 900},//Optional; default value is 900
                   {"height", 500}//Optional; default value is 500
                };
                std::string jsonStr = json_obj.dump();
                openCmp(hDLL, jsonStr.c_str());
            }
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_SHOW_OPENSCREEN_ADVERT://Display the splash screen ad in the UI thread.
    {
        CreateSplashScreenAdPanel(g_hwndMain);
        RECT clientRect;
        if (GetClientRect(g_hwndMain, &clientRect)) {
            int clientWidth = clientRect.right - clientRect.left;
            int clientHeight = clientRect.bottom - clientRect.top;
            nlohmann::json json_obj = {
             {"unitId", FullScreenAdUnitId},
             {"appType", 1},
             {"adType", 1},
             {"handle", reinterpret_cast<int>(g_hPnlSplashScreen)},
             {"width", clientWidth},//FullScreen ad requires passing in the program's width and height.
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
   /* case WM_DESTROY_CMP: {
        try
        {
            DestroyWindow(g_hPnlCmp);
            g_hPnlCmp = NULL;
        }
        catch (...)
        {
        }
        return 0;
    }*/
    case WM_DESTROY_ADVERT: {
        const char* json = reinterpret_cast<const char*>(lParam);
        if (json) {
            try
            {
                nlohmann::json json_obj = nlohmann::json::parse(json);
                std::string unitId = json_obj["unitId"];
                if (unitId == FullScreenAdUnitId)
                {//Delete splash ad container
                    DestroyWindow(g_hPnlSplashScreen);
                    g_hPnlSplashScreen = NULL;
                }
                else if (unitId == InterstitialUnitId)
                {//Delete interstitial ad container
                    DestroyWindow(g_hPnlInterstitial);
                    g_hPnlInterstitial = NULL;
                }
                else if (unitId == BannerUnitId)
                {//Delete banner ad container
                    DestroyWindow(g_hPnlBanner);
                    g_hPnlBanner = NULL;
                }
                else if (unitId == CoupletUnitId)
                {//Delete couplet ad container
                    int advertStatus = json_obj["advertStatus"];
                    if (advertStatus == 1)//Ad already displayed, closed by user
                    {
                        int coupletType = json_obj["coupletType"];
                        if (coupletType == 1)//Delete left container
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
                    {//No ad creatives obtained, ad not displayed. Delete created container
                        BOOL result = DestroyWindow(g_hPnlCoupletLeft);
                        g_hPnlCoupletLeft = NULL;

                        BOOL result2 = DestroyWindow(g_hPnlCoupletRight);
                        g_hPnlCoupletRight = NULL;
                    }
                }
                else if (unitId == RewardedUnitId)
                {//Rewarded
                    DestroyWindow(g_hPnlReward);
                    g_hPnlReward = NULL;

                    int completeStatus = json_obj["completeStatus"];
                    if (completeStatus == 1)
                    {
                        std::string resourceId = json_obj["resourceId"];
                        std::string materialId = json_obj["materialId"];
                        std::string rewardId = json_obj["rewardId"];

                        //Video playback finished, issue reward items
                        //... 

                        //Report order fulfillment to MG
                        reportAdRewardFulfillment(unitId.c_str(), resourceId.c_str(), materialId.c_str(), rewardId.c_str());
                        AppendLog(L"reportAdRewardFulfillment Async: %hs", rewardId.c_str());
                    }
                }
            }
            catch (...)
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
    {
        if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)
        {
            int parentWidth = LOWORD(lParam);
            int parentHeight = HIWORD(lParam);
            //if (g_hPnlCmp != NULL)//When the window size changes, the CMP container's size needs to be modified.
            //{
            //    RECT cmpRect;
            //    GetClientRect(g_hPnlCmp, &cmpRect);
            //    int cmpWidth = cmpRect.right - cmpRect.left;
            //    int cmpHeight = cmpRect.bottom - cmpRect.top;
            //    if (cmpHeight > 50)
            //    {
            //        //1.In the CMP interface, when the user selects "Custom", the CMP interface changes from the top banner to a rectangle in the middle of the program.
            //        int x = (parentWidth - cmpWidth) / 2;
            //        int y = (parentHeight - cmpHeight) / 2;
            //        SetWindowPos(g_hPnlCmp, NULL, x, y, cmpWidth, cmpHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
            //    }
            //    else
            //    {
            //        //2.CMP original interface. The height is fixed at 50px, and the width remains full across the App.
            //        if (cmpWidth != parentWidth)
            //        {
            //            //Modify the size of the CMP panel.
            //            SetWindowPos(g_hPnlCmp, NULL, 0, 0, parentWidth, cmpHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
            //            if (g_cmpSdkControlHandle > 0)
            //            {
            //                //Modify the size of the CMP control within the SDK. This Handle is returned by the OpenCmp interface.
            //                HWND hWndSdkControl = (HWND)(INT_PTR)g_cmpSdkControlHandle;
            //                SetWindowPos(hWndSdkControl, NULL, 0, 0, parentWidth, cmpHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
            //            }
            //        }
            //    }
            //}

            if (g_hPnlBanner != NULL) {
                RECT adcRect;
                GetClientRect(g_hPnlBanner, &adcRect);
                int adWidth = adcRect.right - adcRect.left;
                int adHeight = adcRect.bottom - adcRect.top;
                if (adHeight == 90)
                {
                    int x = (parentWidth - 728) / 2;
                    int y = parentHeight - 90 - 50;
                    SetWindowPos(g_hPnlBanner, NULL, x, y, 728, 90, SWP_NOZORDER | SWP_SHOWWINDOW);
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
CMP和初始化的完整流程
1.在UI线程调用CMP
2.在CMP回调事件中初始化SDK
3.在SDK初始化完成事件中调用开屏广告，广告需要在UI线程中调用

Complete Process for CMP and Initialization
1. Call CMP on the UI thread
2. Initialize the SDK in the CMP callback event
3. Call the splash screen ad in the SDK initialization complete event; the ad must be called on the UI thread
*/
void setAppId(HINSTANCE hdll, const char* appId, const char* secretKey) {
    try
    {
        SetAppId func = (SetAppId)GetProcAddress(hdll, "SetAppId");
        func(appId, secretKey);
    }
    catch (...)
    {
    }
}
void openCmp(HINSTANCE hdll, const char* jsonParam) {
    try
    {
        OpenCmp func = (OpenCmp)GetProcAddress(hdll, "OpenCmp");
        func(jsonParam);//Return the handle of the CMP control in the SDK
    }
    catch (...)
    {
    }
}
void onCmpClosedEvent(char* s) {
    try
    { 
        AppendLog(L"CMP result: %hs", s);
        
        initialize(hDLL); 
    }
    catch (...)
    {
    }
}

bool getUserRegionCmpRequirement(HINSTANCE hdll)
{
    bool result = false;
    try
    {
        GetUserRegionCmpRequirement func = (GetUserRegionCmpRequirement)GetProcAddress(hdll, "GetUserRegionCmpRequirement");
        result =  func();//Get regional CMP requirement (Additional interface, for specific scenarios)
        //This method relies on the store package and always returns false when debugging locally.
        std::string resultString = result ? "true" : "false";
        AppendLog(L"Get regional CMP requirement, result = %hs", resultString.c_str());
    }
    catch (...)
    {
    }
    return result; 
}
#pragma endregion

#pragma region 2.SDK Initialisation
void initialize(HINSTANCE hdll) {
    try
    {
        Initialize func = (Initialize)GetProcAddress(hdll, "Initialize");
        func();
    }
    catch (...)
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
    catch (...)
    {
    }
}
#pragma endregion

#pragma region 3.AD
//1.Load the advert on the fallback screen, after SDK initialisation is complete
void setupExitAd(HINSTANCE hdll) {
    if (auto func = (SetupExitAd)GetProcAddress(hdll, "SetupExitAd")) {
        func(ExitAdUnitId); // passing in the key value of the advert's primary key that was created in the mg backend.

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

    // Send to the main UI thread
    char* jsonCopy = _strdup(s);
    PostMessage(g_hwndMain, WM_DESTROY_ADVERT, 0, reinterpret_cast<LPARAM>(jsonCopy));
}
//4.Open MG Advert window
void showAd(const char* json) {
    try
    {
        // Ensure that COM has been initialized before calling.
        if (!g_comInitialized) {
            HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
            if (SUCCEEDED(hr)) {
                g_comInitialized = true;
            }
        }

        ShowAd func = (ShowAd)GetProcAddress(hDLL, "ShowAd");
        if (func) {
            func(json);
        }
    }
    catch (...)
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

void preloadAd(const char* json)
{
    try
    {
        // 确保在调用前COM已初始化
        if (!g_comInitialized) {
            HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
            if (SUCCEEDED(hr)) {
                g_comInitialized = true;
            }
        }
        PreloadAd func = (PreloadAd)GetProcAddress(hDLL, "PreloadAd");
        if (func) {
            func(json);
        }
    }
    catch (...)
    {
    }
}

void onAdPreloadEvent(char* s) {
    AppendLog(L"onAdPreloadEvent: %hs", s);
    //eg.s = {"unitId":"a9bd7d57faef4f8cb016979284c86102","advertStatus":1,"displayStatus":0}
    //...

    nlohmann::json json_obj = nlohmann::json::parse(s);
    std::string unitId = json_obj["unitId"];
    std::int32_t adStatus = json_obj["advertStatus"];
    if (adStatus == 1)
    {   // The button show the advert can be clicked.
        if (unitId == InterstitialUnitId)
        {  
            EnableWindow(g_hBtnInterstitialAdShow, TRUE);
        }
        else  if (unitId == RewardedUnitId)
        { 
            EnableWindow(g_hBtnRewardedAdShow, TRUE);
        }
    }
}

int showPreloadAd(const char* json,HWND adBntHwnd)
{ 
    try
    {
        if (!g_comInitialized) {
            HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
            if (SUCCEEDED(hr)) {
                g_comInitialized = true;
            }
        }
        ShowPreloadAd func = (ShowPreloadAd)GetProcAddress(hDLL, "ShowPreloadAd");
        if (func) {
             func(json); 

             AppendLog(L"show mg ad: success");
             EnableWindow(adBntHwnd, FALSE);
        }
    }
    catch (...)
    {
    }
    return 0;
}
void onAdShowPreloadEvent(char* s) {
    AppendLog(L"onAdShowPreloadEvent: %hs", s);
    //eg.s = {"unitId":"a9bd7d57faef4f8cb016979284c86102","advertStatus":1,"displayStatus":1}
    //...

    nlohmann::json json_obj = nlohmann::json::parse(s);
    std::string unitId = json_obj["unitId"];
    std::int32_t adStatus = json_obj["advertStatus"];
    if (adStatus == 1)
    {   
        if (unitId == InterstitialUnitId)
        { 
            //EnableWindow(g_hBtnInterstitialAdShow, FALSE);
        }
        else  if (unitId == RewardedUnitId)
        {
            //EnableWindow(g_hBtnRewardedAdShow, FALSE);
        }
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