
#include "Win32TextInput.h"

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

using namespace waseGraphics;

WNDPROC oldEditProc = NULL;

std::string LocalToUTF8(const std::string& localStr)
{
    // ��һ�������ر���ת���ַ�(UTF-16)
    int wideLen = MultiByteToWideChar(CP_ACP, 0, localStr.c_str(), -1, NULL, 0);
    if (wideLen == 0)
        return "";

    wchar_t* wideStr = new wchar_t[wideLen];
    MultiByteToWideChar(CP_ACP, 0, localStr.c_str(), -1, wideStr, wideLen);

    // �ڶ��������ַ�(UTF-16)תUTF-8
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
    if (utf8Len == 0) {
        delete[] wideStr;
        return "";
    }

    char* utf8Str = new char[utf8Len];
    WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, utf8Str, utf8Len, NULL, NULL);

    std::string result(utf8Str);
    delete[] wideStr;
    delete[] utf8Str;

    return result;
}

std::string UTF8ToLocal(const std::string& utf8Str)
{
    // ��һ����UTF-8ת���ַ�(UTF-16)
    int wideLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
    if (wideLen == 0)
        return "";

    wchar_t* wideStr = new wchar_t[wideLen];
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, wideStr, wideLen);

    // �ڶ��������ַ�(UTF-16)ת���ر���
    int localLen = WideCharToMultiByte(CP_ACP, 0, wideStr, -1, NULL, 0, NULL, NULL);
    if (localLen == 0) {
        delete[] wideStr;
        return "";
    }

    char* localStr = new char[localLen];
    WideCharToMultiByte(CP_ACP, 0, wideStr, -1, localStr, localLen, NULL, NULL);

    std::string result(localStr);
    delete[] wideStr;
    delete[] localStr;

    return result;
}

void Win32TextInput::init(HWND hWnd, int type) SC_NOTHROW {
    this->hWnd = hWnd;
    int style = WS_CHILD | WS_VISIBLE;//WS_BORDER | 
    if (type == TextInput::inputTypeMultiLine) {
        style |= ES_MULTILINE | ES_AUTOVSCROLL;
    }
    else if (type == TextInput::inputTypePassword) {
        style |= ES_PASSWORD;
    }
    int X = 0;
    int Y = 0;
    int W = 10;
    int H = 20;
    HWND inputHandle = CreateWindow(
        "EDIT", "", style,
        X, Y, W, H,
        hWnd, (HMENU)ID_INPUTEDIT, GetModuleHandle(0), 0);

    textInputHandle = inputHandle;
    if (custemProc) {
        oldEditProc = (WNDPROC)SetWindowLongPtr(textInputHandle, GWLP_WNDPROC, custemProc);
    }
}

void Win32TextInput::close() SC_NOTHROW {
    ShowWindow(textInputHandle, SW_HIDE);
    DestroyWindow(textInputHandle);
    textInputHandle = 0;
}
void Win32TextInput::setPos(int x, int y, int w, int h) SC_NOTHROW {
    SetWindowPos(textInputHandle, HWND_TOP, x, y, w, h, SWP_ASYNCWINDOWPOS);
}
void Win32TextInput::setStyle(waseGraphics::Font& font, float fontSize, waseGraphics::Color textColor, waseGraphics::Color backgroundColor) SC_NOTHROW {
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);


    ncm.lfMessageFont.lfHeight = fontSize;

    HFONT hNewFont = CreateFontIndirect(&ncm.lfMessageFont);
    if (hNewFont)
    {
        SendMessage(textInputHandle, WM_SETFONT, (WPARAM)hNewFont, TRUE);
    }

    //SetTextColor(textInputHandle, RGB(0, 0, 255));
    //SetBkColor(textInputHandle, RGB(255, 255, 200));
}
void Win32TextInput::setText(const char* text) SC_NOTHROW {
    std::string localStr = UTF8ToLocal(text);
    SetWindowText(textInputHandle, localStr.c_str());
}
void Win32TextInput::setType(int lineNum, bool editable) SC_NOTHROW {
    if (textInputHandle != 0) {
        EnableWindow(textInputHandle, editable);
    }
}
void Win32TextInput::focus() SC_NOTHROW {
    SetFocus(textInputHandle);
    SendMessage(textInputHandle, EM_SETSEL, 0, -1);
}

void Win32TextInput::select(int start, int end) SC_NOTHROW {
    SendMessage(textInputHandle, EM_SETSEL, start, end);
}
int Win32TextInput::caretPos() SC_NOTHROW {
    DWORD dwStart, dwEnd;
    SendMessage(textInputHandle, EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);
    return dwStart;
}
