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

HWND g_hPnlCmp = NULL; //CMP container, at startup it should be at the very top of the program, width full, height 50
HWND g_hPnlSplashScreen = NULL; //[Splash Ad]
HWND g_hPnlBanner = NULL; //[Banner]
HWND g_hPnlCoupletLeft = NULL; //[Couplet left]
HWND g_hPnlCoupletRight = NULL; //[Couplet right]
HWND g_hPnlInterstitial = NULL;//[Interstitial] 
HWND g_hPnlReward = NULL;//[Rewarded]
HWND g_hPnlFeed = NULL;//[Feed]
HWND g_hPnlEmbedded = NULL;//[Embedded]

// Custom Message
#define WM_SHOW_ADVERT (WM_USER + 300)
#define WM_SHOW_OPENSCREEN_ADVERT (WM_USER + 301)
#define WM_DESTROY_CMP (WM_USER + 310)
#define WM_DESTROY_ADVERT (WM_USER + 311)

int g_cmpChangedWidth = 0;
int g_cmpChangedHeight = 0;
int g_cmpSdkControlHandle = 0;

//1.Splash Ad 2.Exit Ad 3.Banner 4.Interstitial 5.Couplet 6.Rewarded 7.Feed 8.Embedded
const char* YourAppId = "69316b6861328938223cc124";
const char* YourSecretKey = "MIGTAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBHkwdwIBAQQgZgULOuiIDYZyGiUyYdGr3odHVN6ebZ1uDwXx7PXiHh2gCgYIKoZIzj0DAQehRANCAASf1FWCfsSn/tXFVRt04C7JkpRG12KSC3wnaJRWb5QWin9dsBk1OR31BCsELMYtWsFhA7e6Q6Fi4Mi6+ub24O5a";
const char* SplashAdUnitId = "b871f83c5e8845f1b43325561bcdd6c7";             //Splash Ad:1920 x 1080
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
    CreateWindowW(L"BUTTON", L"Splash Ad", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y, 130, 30, hWnd, (HMENU)ID_BTN_AD1, hInst, NULL);
    CreateWindowW(L"BUTTON", L"Exit Ad", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_EXITAD, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Banner", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD3, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Interstitial", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD4, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Interstitial Video", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD41, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Interstitial Web", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD42, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Couplet", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD5, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Rewarded", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD6, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Rewarded Web", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD61, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Feed", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD7, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Embedded", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_AD8, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Close CMP", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 130, 30, hWnd, (HMENU)ID_BTN_DELCMP, hInst, NULL);

    // Log
    g_hwndLog = CreateWindow(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
        300, 10, 1200, 200, hWnd, (HMENU)ID_TXT_LOG, hInst, NULL);

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
        if (auto func = (CmpSizeChangedEvent)GetProcAddress(hDLL, "CmpSizeChangedEvent")) // CMP container size changed
            func(onCmpSizeChangedEvent);
        if (auto func = (CmpClosedEvent)GetProcAddress(hDLL, "CmpClosedEvent")) //CMP closed
            func(onCmpClosedEvent);

        // Register the initialisation completion callback event
        if (auto func = (InitCompleteEvent)GetProcAddress(hDLL, "InitCompleteEvent")) //Callback function after initialization is complete
            func(onInitCompleteEvent);
        if (auto func = (AdCloseEvent)GetProcAddress(hDLL, "AdCloseEvent")) //Callback function for ad close event
            func(onAdCloseEvent);

        //1.Set parameters
        setAppId(hDLL, YourAppId, YourSecretKey);

        //2.First, determine whether to pop up CMP
        if (isOpenCmp(hDLL))
        {
            RECT clientRect;
            if (GetClientRect(hWnd, &clientRect)) {
                int panelWidth = clientRect.right - clientRect.left;
                int panelHeight = clientRect.bottom - clientRect.top;

                //2.1.Create CMP container
                HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
                g_hPnlCmp = CreateWindowW(L"STATIC", L"This is a panel for CMP", WS_CHILD | WS_VISIBLE, 0, 0, panelWidth, 50, hWnd, (HMENU)3011, minstance, NULL);
                BringWindowToTop(g_hPnlCmp);

                //2.2.Show CMP
                nlohmann::json json_obj = {
                 {"handle", reinterpret_cast<int>(g_hPnlCmp)},
                 {"parentWidth", panelWidth},
                 {"parentHeight", panelHeight}
                };
                std::string jsonStr = json_obj.dump();
                openCmp(hDLL, jsonStr.c_str());//Returns the handle of the CMP control in the SDK, saved using the global variable g_cmpSdkControlHandle.
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
        {//Splash Ad
            RECT clientRect;
            if (GetClientRect(hWnd, &clientRect)) {
                int clientWidth = clientRect.right - clientRect.left;
                int clientHeight = clientRect.bottom - clientRect.top;
                nlohmann::json json_obj = {
                 {"unitId", SplashAdUnitId},
                 {"appType", 1},
                 {"adType", 1},//1.Splash Ad 2.Exit Ad 3.Banner 4.Interstitial 5.Couplet 6.Rewarded 7.Feed 8.Embedded
                 {"handle", reinterpret_cast<int>(g_hwndMain)},
                 {"width", clientWidth},//For splash ad, the program's width and height must be provided.
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
                {"media", "web"},//Only image-type creatives,support media types (image, video, web). Can be left empty.
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
             {"unitId", SplashAdUnitId},
             {"appType", 1},
             {"adType", 1},
             {"handle", reinterpret_cast<int>(g_hPnlSplashScreen)},
             {"width", clientWidth},//Splash ad requires passing in the program's width and height.
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
                if (unitId == SplashAdUnitId)
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
    {
        if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)
        {
            int parentWidth = LOWORD(lParam);
            int parentHeight = HIWORD(lParam);
            if (g_hPnlCmp != NULL)//When the window size changes, the CMP container's size needs to be modified.
            {
                RECT cmpRect;
                GetClientRect(g_hPnlCmp, &cmpRect);
                int cmpWidth = cmpRect.right - cmpRect.left;
                int cmpHeight = cmpRect.bottom - cmpRect.top;
                if (cmpHeight > 50)
                {
                    //1.In the CMP interface, when the user selects "Custom", the CMP interface changes from the top banner to a rectangle in the middle of the program.
                    int x = (parentWidth - cmpWidth) / 2;
                    int y = (parentHeight - cmpHeight) / 2;
                    SetWindowPos(g_hPnlCmp, NULL, x, y, cmpWidth, cmpHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
                }
                else
                {
                    //2.CMP original interface. The height is fixed at 50px, and the width remains full across the App.
                    if (cmpWidth != parentWidth)
                    {
                        //Modify the size of the CMP panel.
                        SetWindowPos(g_hPnlCmp, NULL, 0, 0, parentWidth, cmpHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
                        if (g_cmpSdkControlHandle > 0)
                        {
                            //Modify the size of the CMP control within the SDK. This Handle is returned by the OpenCmp interface.
                            HWND hWndSdkControl = (HWND)(INT_PTR)g_cmpSdkControlHandle;
                            SetWindowPos(hWndSdkControl, NULL, 0, 0, parentWidth, cmpHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
                        }
                    }
                }
            }

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
为实现CMP功能，开发者需按以下步骤创建并管理控件：
1.初始化创建：开发者需在应用启动时，创建CMP控件并将其置于视图顶层。控件初始尺寸应设为：宽度与App等宽，高度为50 px。
2.响应应用布局变化：当应用窗口尺寸改变时，开发者需手动更新CMP控件的宽度，确保其始终与新的可用宽度保持一致。
3.完成与清理：当用户完成授权(即CMP关闭)后，开发者必须在 CmpClosedEvent 回调事件中，销毁CMP控件实例，以释放资源。

To implement CMP functionality, developers must create and manage the control following these steps:
1. Initialization and Creation: Developers must create the CMP control upon application launch and place it at the top layer of the view. The control's initial dimensions should be set to: width equal to the App's width, height set to 50 px.
2. Respond to Layout Changes: When the application window resizes, developers must manually update the CMP control's width to ensure it consistently matches the new available width.
3. Completion and Cleanup: After the user completes authorization (i.e., the CMP closes), developers must destroy the CMP control instance within the CmpClosedEvent callback to release resources.
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
        g_cmpSdkControlHandle = func(jsonParam);//Return the handle of the CMP control in the SDK
    }
    catch (const std::exception&)
    {
    }
}

/*
为确保CMP界面交互期间布局正确：
在触发onCmpSizeChangedEvent(即CPP界面改变时)，开发者应同步调整Banner广告控件的尺寸，并确保其在父容器中保持居中。
在触发onCmpClosedEvent(即用户完成权限操作，CMP关闭后)，开发者需将Banner广告控件恢复至初始的尺寸与位置。
*注意：此适配仅针对横幅、信息流、嵌入式广告。

To ensure correct layout during CMP interface interactions:
When triggering onCmpSizeChangedEvent (i.e., when the CPP UI changes), developers should synchronously adjust the Banner ad control's dimensions and ensure it remains centered within its parent container.
When triggering onCmpClosedEvent (i.e., after the CMP closes), developers must restore the Banner ad control to its initial size and position.
*Note: This adaptation applies only to banner, feed, and in-app ads.
*/

// Cmp size changed callback function
void onCmpSizeChangedEvent(char* s) {
    try
    {
        nlohmann::json json_obj = nlohmann::json::parse(s);//{"width":900,"height":440}
        int width = (int)json_obj["width"];
        int height = (int)json_obj["height"];
        int cmpOrigin = (int)json_obj["cmpOrigin"];//1.From CMP control  2.From Ad control 
        if (cmpOrigin == 1)
        {
            AppendLog(L"Cmp size changed  %hs", s);
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
        else {
            //In Google Ads' CMP, when users click to authorize, developers need to modify the size and positioning of the ad control
            AppendLog(L"Cmp ad size changed  %hs", s);
            std::string unitId = json_obj["unitId"];
            int cppHandle = (int)json_obj["cppHandle"];
            if (cppHandle > 0)
            {
                RECT parentRect;
                if (GetClientRect(g_hwndMain, &parentRect)) {
                    int parentWidth = parentRect.right - parentRect.left;
                    int parentHeight = parentRect.bottom - parentRect.top;
                    int x = (parentWidth - width) / 2;
                    int y = (parentHeight - height) / 2;

                    HWND cppHwnd = reinterpret_cast<HWND>(cppHandle);
                    SetWindowPos(cppHwnd, NULL, x, y, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);
                }
            }
        }
    }
    catch (const std::exception&)
    {
    }
}
void onCmpClosedEvent(char* s) {
    try
    {
        nlohmann::json json_obj = nlohmann::json::parse(s);
        int cmpOrigin = (int)json_obj["cmpOrigin"];//1.From CMP control   2.From Ad control
        if (cmpOrigin == 1)
        {
            g_cmpChangedWidth = 0;
            g_cmpChangedHeight = 0;

            //Remove CMP control
            /* DestroyWindow(g_hPnlCmp);
            g_hPnlCmp = NULL;*/
            PostMessage(g_hwndMain, WM_DESTROY_CMP, 0, NULL);

            AppendLog(L"CMP has been removed.");
        }
        else
        {
            //In Google Ads' CMP, after the user grants permission, the developer needs to restore the size and positioning of the ad controls
            AppendLog(L"Cmp ad reset size %hs", s);

            std::string unitId = json_obj["unitId"];
            int cppHandle = (int)json_obj["cppHandle"];
            if (unitId == BannerUnitId)
            {
                RECT parentRect;
                if (GetClientRect(g_hwndMain, &parentRect)) {
                    int width = 728;
                    int height = 90; // Banner 728x90
                    int x = (parentRect.right - parentRect.left - width) / 2;
                    int y = (parentRect.bottom - parentRect.top - height - 50);
                    SetWindowPos(g_hPnlBanner, NULL, x, y, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);
                }
            }
            else if (unitId == FeedUnitId)
            {
                SetWindowPos(g_hPnlFeed, NULL, 10, 545, 400, 50, SWP_NOZORDER | SWP_SHOWWINDOW);
            }
            else if (unitId == EmbeddedUnitId)
            {
                SetWindowPos(g_hPnlEmbedded, NULL, 10, 605, 200, 200, SWP_NOZORDER | SWP_SHOWWINDOW);
            }
        }
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