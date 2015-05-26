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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
        break;
    }
    return 0;
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

    WCHAR buffer[1024];
    LoadString(instance, IDS_APPNAME, buffer, 1024);
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

    // Return the exit code to the system.
    return (int)msg.wParam;
}