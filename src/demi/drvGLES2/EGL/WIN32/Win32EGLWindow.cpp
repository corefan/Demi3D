/**********************************************************************
This source file is a part of Demi3D
   __  ___  __  __  __
  |  \|_ |\/||   _)|  \ 
  |__/|__|  ||  __)|__/ 

Copyright (c) 2013-2014 Demi team
https://github.com/wangyanxing/Demi3D

Released under the MIT License
https://github.com/wangyanxing/Demi3D/blob/master/License.txt
***********************************************************************/

#include "DrvGLES2Pch.h"
#include "Win32EGLWindow.h"
#include "Win32EGLContext.h"
#include "RenderWindow.h"
#include "RenderTarget.h"
#include "Command.h"
#include "GfxDriver.h"

namespace Demi
{
    static const char *gWindowClass = "_DEMI3D_WND";
    static const DWORD gWindowStyle = WS_OVERLAPPEDWINDOW;
    static const DWORD gFullscreenStyle = WS_POPUP;

    static INT_PTR CALLBACK windowProc(HWND hwnd, uint32 msg, WPARAM wParam, LPARAM lParam)
    {
        DiWin32EGLWindow* window = (DiWin32EGLWindow*)LongToPtr(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        DiRenderWindow* renderWnd = DiBase::Driver->FindRenderWindow(DiWndHandle(hwnd));

        if (window && window->DestroyingWindow())
            return 0;

        switch (msg)
        {
        case WM_CREATE:
            ::UpdateWindow(hwnd);
            break;

        case WM_CLOSE:
            DiBase::CommandMgr->ExecuteCommand("quit");
            break;
        case WM_ACTIVATE:
        {
            bool active = (LOWORD(wParam) != WA_INACTIVE);
            if (active)
            {
                if (renderWnd)
                    renderWnd->GetRenderBuffer()->SetActive(true);
            }
            else
            {
                if (renderWnd)
                    renderWnd->GetRenderBuffer()->SetActive(false);
            }
            break;
        }
    
        case WM_COPYDATA:
        {
            const char* str = (const char*)(((COPYDATASTRUCT*)lParam)->lpData);
            DiString s(str, ((COPYDATASTRUCT*)lParam)->cbData);
            DiBase::CommandMgr->ExecuteCommand(s.c_str());
            break;
        }

        default:
            return ::DefWindowProc(hwnd, msg, wParam, lParam);
        }
        return 0;
    }

    static ATOM registerWindowClass(HINSTANCE hInstance)
    {
        static ATOM atom = 0;
        if (!atom)
        {
            WNDCLASSEX wcex;
            wcex.cbSize = sizeof(WNDCLASSEX);
            wcex.style = CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc = (WNDPROC)windowProc;
            wcex.cbClsExtra = 0;
            wcex.cbWndExtra = sizeof(void*);
            wcex.hInstance = hInstance;
            wcex.hIcon = ::LoadIcon(hInstance, (LPCTSTR)0);
            wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
            wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            wcex.lpszMenuName = 0;
            wcex.lpszClassName = gWindowClass;
            wcex.hIconSm = ::LoadIcon(wcex.hInstance, (LPCTSTR)0);
            atom = ::RegisterClassEx(&wcex);
        }
        return atom;
    }

    //////////////////////////////////////////////////////////////////////////

    DiWin32EGLWindow::DiWin32EGLWindow(DiWin32EGLUtil* util)
        : DiEGLWindow(util)
    {
        mGLSupport = util;
        mNativeDisplay = util->GetNativeDisplay();
        mHandlingmsg = false;
        mDestroyWindow = false;
    }

    DiWin32EGLWindow::~DiWin32EGLWindow()
    {
    }

    bool DiWin32EGLWindow::Create(uint32& width, uint32& height, const DiString& title, bool fullscreen)
    {
        ::EGLContext eglContext = 0;
        if (!mEglConfig)
        {
            uint32 samples = 0;
            int minAttribs[] = 
            {
                EGL_LEVEL, 0,
                EGL_DEPTH_SIZE, 16,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_NATIVE_RENDERABLE, EGL_FALSE,
                EGL_DEPTH_SIZE, EGL_DONT_CARE,
                EGL_NONE
            };

            int maxAttribs[] = 
            {
                EGL_SAMPLES, samples,
                EGL_STENCIL_SIZE, INT_MAX,
                EGL_NONE
            };

            mEglConfig = mGLSupport->SelectGLConfig(minAttribs, maxAttribs);
        }

        bool ret = _Create(width, height, title, fullscreen);

        mContext = CreateEGLContext();
        mContext->BeginContext();
        ::EGLSurface oldDrawableDraw = eglGetCurrentSurface(EGL_DRAW);
        ::EGLSurface oldDrawableRead = eglGetCurrentSurface(EGL_READ);
        ::EGLContext oldContext = eglGetCurrentContext();

        int glConfigID;
        mGLSupport->GetGLConfigAttrib(mEglConfig, EGL_CONFIG_ID, &glConfigID);

        DI_INFO("EGLWindow::create used FBConfigID = %d", glConfigID);
        return ret;
    }

    void DiWin32EGLWindow::Update()
    {
        DI_ASSERT(mWndHandle);
        if (IsOpen())
        {
            mHandlingmsg = true;
            MSG msg;

            while (IsOpen() && ::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }

            mHandlingmsg = false;
            if (mWndHandle && mDestroyWindow)
            {
                ::DestroyWindow((HWND)mWndHandle);
                mWndHandle = nullptr;
                mDestroyWindow = false;
            }
        }
    }

    bool DiWin32EGLWindow::Close()
    {
        if (mWndHandle)
        {
            if (mHandlingmsg)
                mDestroyWindow = true;
            else
            {
                ::DestroyWindow((HWND)mWndHandle);
                mWndHandle = nullptr;
            }
        }

        return true;
    }

    bool DiWin32EGLWindow::IsOpen()
    {
        return mWndHandle ? true : false;
    }

    void DiWin32EGLWindow::SetWindowSize(uint32 width, uint32 height)
    {
        bool fullscreen = false;
        DI_ASSERT(mWndHandle);
        if (mWndHandle)
        {
            RECT rect;
            ::GetWindowRect((HWND)mWndHandle, &rect);
            rect.right = (LONG)(rect.left + width);
            rect.bottom = (LONG)(rect.top + height);
            RECT oldrect = rect;
            DWORD dwstyle = (fullscreen ? gFullscreenStyle : gWindowStyle);
            ::AdjustWindowRect(&rect, dwstyle, 0);
            ::MoveWindow((HWND)mWndHandle, (int)oldrect.left, (int)oldrect.top,
                (int)(rect.right - rect.left), (int)(rect.bottom - rect.top), 1);
        }
    }

    void DiWin32EGLWindow::GetWindowSize(uint32& width, uint32& height)
    {
        if (mWndHandle)
        {
            RECT rect;
            GetClientRect((HWND)mWndHandle, &rect);
            width = (uint32)(rect.right - rect.left);
            height = (uint32)(rect.bottom - rect.top);
        }
    }

    void DiWin32EGLWindow::GetTitle(char *title, uint32 maxLength) const
    {
        DI_ASSERT(mWndHandle);
        if (mWndHandle)
            GetWindowTextA((HWND)mWndHandle, title, maxLength);
    }

    void DiWin32EGLWindow::SetTitle(const char *title)
    {
        DI_ASSERT(mWndHandle);
        if (mWndHandle)
            ::SetWindowTextA((HWND)mWndHandle, title);
    }

    bool DiWin32EGLWindow::_Create(uint32& width, uint32& height, const DiString& title, bool fullscreen)
    {
        bool ok = false;
        DI_ASSERT(!mWndHandle);

        registerWindowClass((HINSTANCE)::GetModuleHandle(0));
        RECT winRect;
        winRect.left = 0;
        winRect.top = 0;
        winRect.right = width;
        winRect.bottom = height;
        DWORD dwstyle = (fullscreen ? gFullscreenStyle : gWindowStyle);
        uint32  offset = fullscreen ? 0 : 50;
        ::AdjustWindowRect(&winRect, dwstyle, 0);
        mWndHandle = ::CreateWindowA(gWindowClass, title.c_str(), dwstyle,
            offset, offset,
            winRect.right - winRect.left, winRect.bottom - winRect.top,
            0, 0, 0, 0);
        DI_ASSERT(mWndHandle);
        mWindow = (NativeWindowType)mWndHandle;

        if (mWndHandle)
        {
            ok = true;
            ShowWindow((HWND)mWndHandle, SW_SHOWNORMAL);
            ::SetFocus((HWND)mWndHandle);
            SetWindowLongPtr((HWND)mWndHandle, GWLP_USERDATA, PtrToLong(this));

            mHDC = ::GetDC((HWND)mWndHandle);
        }

        RAWINPUTDEVICE rawInputDevice;
        rawInputDevice.usUsagePage = 1;
        rawInputDevice.usUsage = 6;
        rawInputDevice.dwFlags = 0;
        rawInputDevice.hwndTarget = NULL;

        BOOL status = RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice));
        if (status != TRUE)
        {
            DI_ERROR("RegisterRawInputDevices failed: %d", GetLastError());
        }

        mNativeDisplay = GetDC(mWindow);
        mEglDisplay = eglGetDisplay(mNativeDisplay);

        // fallback for some emulations 
        if (mEglDisplay == EGL_NO_DISPLAY)
        {
            mEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        }

        eglInitialize(mEglDisplay, NULL, NULL);

        eglBindAPI(EGL_OPENGL_ES_API);

        mGLSupport->SetGLDisplay(mEglDisplay);
        mEglSurface = CreateSurfaceFromWindow(mEglDisplay, mWindow);

        return ok;
    }

    DiEGLContext* DiWin32EGLWindow::CreateEGLContext() const
    {
        return DI_NEW DiWin32EGLContext(mEglDisplay, mGLSupport, mEglConfig, mEglSurface);
    }
}