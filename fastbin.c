#include <windows.h>
#include <shellapi.h>
#include <strsafe.h>

#define ID_TRAY_OPEN   1001
#define ID_TRAY_EMPTY  1002
#define ID_TRAY_EXIT   1003
#define ID_TRAY_SIZE   1004
#define WM_TRAYICON    (WM_USER + 1)
#define TIMER_ID       1
#define TOOLTIP_MAX    128

NOTIFYICONDATA nid;
HMENU hMenu;
HWND hwnd;

HICON hIconEmpty = NULL;
HICON hIconFull = NULL;

void UpdateTooltip() {
    SHQUERYRBINFO qinfo = { sizeof(SHQUERYRBINFO) };
    if (SUCCEEDED(SHQueryRecycleBin(NULL, &qinfo))) {
        // Convert size to MB
        double sizeMB = (double)qinfo.i64Size / (1024.0 * 1024.0);

        char tooltip[TOOLTIP_MAX];
        StringCchPrintfA(tooltip, TOOLTIP_MAX, "FastBin - Recycle Bin: %.2f MB", sizeMB);

        StringCchCopyA(nid.szTip, TOOLTIP_MAX, tooltip);
        Shell_NotifyIcon(NIM_MODIFY, &nid);
    }
}

void UpdateMenuSize() {
    SHQUERYRBINFO qinfo = { sizeof(SHQUERYRBINFO) };
    if (SUCCEEDED(SHQueryRecycleBin(NULL, &qinfo))) {
        double sizeMB = (double)qinfo.i64Size / (1024.0 * 1024.0);

        char sizeText[64];
        sprintf(sizeText, "Size: %.2f MB", sizeMB);

        ModifyMenu(hMenu, ID_TRAY_SIZE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED, ID_TRAY_SIZE, sizeText);
    }
}

void UpdateTrayIcon() {
    SHQUERYRBINFO qinfo = { sizeof(SHQUERYRBINFO) };
    if (SUCCEEDED(SHQueryRecycleBin(NULL, &qinfo))) {
        HICON newIcon = (qinfo.i64Size == 0) ? hIconEmpty : hIconFull;
        if (nid.hIcon != newIcon) {
            nid.hIcon = newIcon;
            Shell_NotifyIcon(NIM_MODIFY, &nid);
        }
    }
}

void ShowMenu(HWND hwnd) {
    UpdateMenuSize();
    UpdateTrayIcon();

    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hwnd);
    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
}

void InitTray(HWND hwnd) {
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = (HICON)LoadImage(NULL, "fastbin.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
    if (nid.hIcon == NULL) {
        // fallback icon if loading fails
        nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    }

    // Initial tooltip
    StringCchCopyA(nid.szTip, TOOLTIP_MAX, "FastBin");

    Shell_NotifyIcon(NIM_ADD, &nid);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) {
            ShowMenu(hwnd);
            return 0;
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_TRAY_OPEN:
            ShellExecute(NULL, "open", "shell:RecycleBinFolder", NULL, NULL, SW_SHOWNORMAL);
            break;

        case ID_TRAY_EMPTY:
            SHEmptyRecycleBin(NULL, NULL, SHERB_NOPROGRESSUI | SHERB_NOSOUND);
            UpdateTooltip();
            UpdateTrayIcon();
            break;

        case ID_TRAY_EXIT:
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_CREATE:
        SetTimer(hwnd, TIMER_ID, 5000, NULL);
        UpdateTooltip();
        UpdateTrayIcon();
        break;

    case WM_TIMER:
        if (wParam == TIMER_ID) {
            UpdateTooltip();
            UpdateTrayIcon();
        }
        break;

    case WM_DESTROY:
        KillTimer(hwnd, TIMER_ID);
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmd, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "FastBinClass";
    RegisterClass(&wc);

    hwnd = CreateWindow("FastBinClass", "FastBin", 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    hIconEmpty = (HICON)LoadImage(NULL, "resources/EmptyBinLight.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
    hIconFull  = (HICON)LoadImage(NULL, "resources/BinLight.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);

    if (!hIconEmpty) hIconEmpty = LoadIcon(NULL, IDI_APPLICATION);
    if (!hIconFull)  hIconFull  = LoadIcon(NULL, IDI_APPLICATION);

    InitTray(hwnd);
    UpdateTrayIcon();

    hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING | MF_DISABLED | MF_GRAYED, ID_TRAY_SIZE, "Size: calculating...");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, ID_TRAY_OPEN, "Open Recycle Bin");
    AppendMenu(hMenu, MF_STRING, ID_TRAY_EMPTY, "Empty Recycle Bin");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, "Exit");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}