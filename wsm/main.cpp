#include "resource.h"

#include <Windows.h>

HINSTANCE instance;

BOOL CheckInstanceRunning()
{
    HANDLE handle = CreateMutex(NULL, TRUE, L"me.demirkir.berk.wsm");

    if (handle) {
        DWORD errorCode = GetLastError();
        // Just check if the same mutex exists.
        // Other error cases will be ignored.
        if (errorCode == ERROR_ALREADY_EXISTS)
            return TRUE;
    }

    return FALSE;
}

BOOL AddTaskbarIcon(HWND hwnd)
{
    NOTIFYICONDATA tnid = {};
    tnid.cbSize = sizeof(tnid);
    tnid.hWnd = hwnd;
    tnid.uID = IDI_APPICON;
    tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
    tnid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
    tnid.szTip[0] = '\0';
    return Shell_NotifyIcon(NIM_ADD, &tnid);
}

BOOL DeleteTaskbarIcon(HWND hwnd)
{
    NOTIFYICONDATA tnid = {};
    tnid.cbSize = sizeof(tnid);
    tnid.hWnd = hwnd;
    tnid.uID = IDI_APPICON;
    return Shell_NotifyIcon(NIM_DELETE, &tnid);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static UINT taskbarCreatedMessage = WM_NULL;

    switch (uMsg) {
    case WM_CREATE:
        RegisterWindowMessage(L"TaskbarCreated");
        AddTaskbarIcon(hwnd);
        break;
    case WM_DESTROY:
        DeleteTaskbarIcon(hwnd);
        PostQuitMessage(WM_QUIT);
        break;
    default:
        if (taskbarCreatedMessage != WM_NULL && uMsg == taskbarCreatedMessage) {
            AddTaskbarIcon(hwnd);
        } else {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        break;
    }
    return 0;
}

// Helper function to get read-only resource string
LPCWSTR LoadString(HINSTANCE hInstance, UINT uID)
{
    LPWSTR buffer = 0;
    LoadString(hInstance, uID, (LPWSTR)&buffer, 0);
    return buffer;
}

void CreateMainWindow()
{
    WNDCLASSEX wcx;
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = &WindowProc;
    wcx.cbClsExtra = 0;
    wcx.cbWndExtra = 0;
    wcx.hInstance = instance;
    wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
    wcx.lpszMenuName = 0;
    wcx.lpszClassName = L"WSMMainWindow";
    wcx.hIconSm = LoadIcon(instance, MAKEINTRESOURCE(IDI_APPICON));

    if (!RegisterClassEx(&wcx))
        return;

    LPCWSTR buffer = LoadString(instance, IDS_APPNAME);
    HWND mainWindow = CreateWindow(L"WSMMainWindow", buffer, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, instance, NULL);

    if (!mainWindow)
        return;

    ShowWindow(mainWindow, SW_SHOW);
    UpdateWindow(mainWindow);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // We won't process command line arguments. At least not for now.
    UNREFERENCED_PARAMETER(lpCmdLine);
    // We don't respect show commands. This is a tray application.
    UNREFERENCED_PARAMETER(nCmdShow);

    instance = hInstance;

    // TODO Localization
    //SetThreadUILanguage(LANG_TURKISH);

    // hPrevInstance is always zero. So this won't hurt us at all.
    // http://blogs.msdn.com/b/oldnewthing/archive/2004/06/15/156022.aspx
    if (CheckInstanceRunning() || hPrevInstance)
        return ERROR_SINGLE_INSTANCE_APP;

    CreateMainWindow();

    BOOL ret;
    MSG msg;
    while ((ret = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (ret == -1) {
            // handle the error and possibly exit
            break;
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Return with exit code.
    return (int)msg.wParam;
}