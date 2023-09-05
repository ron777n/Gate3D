#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include "Definitions.hpp"
#include "Main.h"
#include "Renderer.h"
#include <Windowsx.h>

static bool quit = false;

LRESULT CALLBACK WindowProcessMessage(HWND, UINT, WPARAM, LPARAM);

static BITMAPINFO frame_bitmap_info;
static HBITMAP frame_bitmap = 0;
static HDC frame_device_context = 0;

Program program;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {
    // setup window
    static WNDCLASS window_class = { 0 };
    const wchar_t window_class_name[] = L"My Window";
    static HWND window_handle;
    {
        window_class.lpfnWndProc = WindowProcessMessage;
        window_class.hInstance = hInstance;
        window_class.lpszClassName = (LPCWSTR)window_class_name;
        RegisterClass(&window_class);

        frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
        frame_bitmap_info.bmiHeader.biPlanes = 1;
        frame_bitmap_info.bmiHeader.biBitCount = 32;
        frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
        frame_device_context = CreateCompatibleDC(0);

        window_handle = CreateWindow(window_class_name, L"Drawing Pixels", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            640, 300, 640, 480, NULL, NULL, hInstance, NULL);
        if (window_handle == NULL) { return -1; }
    }

    program.start();

    // main loop
    while (!quit) {
        static MSG message = { 0 };
        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) { DispatchMessage(&message); }

        InvalidateRect(window_handle, NULL, FALSE);
        UpdateWindow(window_handle);
    }

    return 0;
}

LRESULT CALLBACK WindowProcessMessage(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_QUIT:
    case WM_DESTROY: {
        quit = true;
    } break;

    case WM_PAINT: {
        static PAINTSTRUCT paint;
        static HDC device_context;
        device_context = BeginPaint(window_handle, &paint);
        program.update();
        BitBlt(device_context,
            paint.rcPaint.left, paint.rcPaint.top,
            paint.rcPaint.right - paint.rcPaint.left, paint.rcPaint.bottom - paint.rcPaint.top,
            frame_device_context,
            paint.rcPaint.left, paint.rcPaint.top,
            SRCCOPY);
        EndPaint(window_handle, &paint);
    } break;

    case WM_SIZE: {
        frame_bitmap_info.bmiHeader.biWidth = LOWORD(lParam);
        frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

        if (frame_bitmap) DeleteObject(frame_bitmap);
        frame_bitmap = CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS, (void**)View.GetPixelsArrayPointer(), 0, 0);
        SelectObject(frame_device_context, frame_bitmap);

        View.reSize(LOWORD(lParam), HIWORD(lParam));


    } break;

    case WM_KEYDOWN:
    {
        program.onKeyDown(wParam);
    } break;

    case WM_MOUSEWHEEL:
    {
        int deltaScroll = GET_WHEEL_DELTA_WPARAM(wParam);
        int fwKeys = GET_KEYSTATE_WPARAM(wParam);
        program.scroll(deltaScroll, fwKeys);
    } break;
    case WM_MOUSEMOVE:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        program.onMouseMoved(xPos, yPos, wParam);
    } break;

    default: {
        // std::stringstream name;
        // name << "Function: " << message << std::endl;
        // Debug(name.str().c_str());
        return DefWindowProc(window_handle, message, wParam, lParam);
    }
    }
    return 0;
}

