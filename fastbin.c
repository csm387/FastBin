#include <windows.h>
#include <shellapi.h>

#define ID_TRAY_OPEN   1001
#define ID_TRAY_EMPTY  1002
#define ID_TRAY_EXIT   1003
#define WM_TRAYICON    (WM_USER + 1)

NOTIFYICONDATA nid;
HMENU hMenu;
HWND hwnd;

void ShowMenu(HWND hwnd) {
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
    nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);
    lstrcpy(nid.szTip, "FastBin");
    Shell_NotifyIcon(NIM_ADD, &nid);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TRAYICON && lParam == WM_RBUTTONUP) {
        ShowMenu(hwnd);
        return 0;
    }
    if (msg == WM_COMMAND) {
        switch (LOWORD(wParam)) {
            case ID_TRAY_OPEN:
                ShellExecute(NULL, "open", "shell:RecycleBinFolder", NULL, NULL, SW_SHOWNORMAL);
                break;
            case ID_TRAY_EMPTY:
                if (MessageBox(NULL, "Empty Recycle Bin?", "Confirm", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                    SHEmptyRecycleBin(NULL, NULL, SHERB_NOPROGRESSUI | SHERB_NOSOUND);
                }
                break;
            case ID_TRAY_EXIT:
                Shell_NotifyIcon(NIM_DELETE, &nid);
                PostQuitMessage(0);
                break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmd, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "FastBinClass";
    RegisterClass(&wc);

    hwnd = CreateWindow("FastBinClass", "FastBin", 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    InitTray(hwnd);

    hMenu = CreatePopupMenu();
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
