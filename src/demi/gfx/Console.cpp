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
#include "GfxPch.h"

#if DEMI_PLATFORM == DEMI_PLATFORM_WIN32

#include "Console.h"
#include "GfxDriver.h"
#include "PathLib.h"

namespace Demi
{
    Demi::DiString DiConsoleLogger::s_sConsoleWndName = "The console of PhiloTrinity";

    DiConsoleLogger::DiConsoleLogger(bool createconsole):DiLogger()
        ,mWnd(NULL)
    {
        mConsoleFile = DiPathLib::GetApplicationPath() + "Console.exe";

        Init(createconsole);
    }

    DiConsoleLogger::DiConsoleLogger( bool createconsole, const DiString& consoleFile ):DiLogger()
        ,mWnd(NULL),mConsoleFile(consoleFile)
    {
        Init(createconsole);
    }

    DiConsoleLogger::~DiConsoleLogger()
    {

    }

    void DiConsoleLogger::OutputLog( const char* szMessage, const char* levelInfo, const char*, long )
    {
        OutputLog(szMessage, levelInfo);
    }

    void DiConsoleLogger::OutputLog(const char* szMessage, const char* levelInfo)
    {
        static DiLogInfo logInfo;
        if (::IsWindow(mWnd))
        {
            DiString msg = szMessage;
            DiString lv = levelInfo;

            logInfo.type = LOG_LEVEL_LOG;
            if (lv == "error")
                logInfo.type = LOG_LEVEL_ERROR;
            else if (lv == "info")
                logInfo.type = LOG_LEVEL_LOG;
            else if (lv == "debug")
                logInfo.type = LOG_LEVEL_DEBUG;
            else if (lv == "warning")
                logInfo.type = LOG_LEVEL_WARNING;
            else
                logInfo.type = 0xFFFF;

            if (Driver)
                logInfo.hwnd = mMainHwnd;

            ZeroMemory(logInfo.message, sizeof(logInfo.message));
            strcpy_s(logInfo.message, 4096, msg.c_str());
            COPYDATASTRUCT cpd;
            cpd.dwData = 0;
            cpd.cbData = sizeof(logInfo);
            cpd.lpData = &logInfo;
            ::SendMessage(mWnd, WM_COPYDATA, 0, (LPARAM)(LPVOID)&cpd);
        }
    }

    void DiConsoleLogger::Init( bool createConsole )
    {
        mWnd = ::FindWindowA(NULL,s_sConsoleWndName.c_str());
        if (NULL == mWnd)
        {
            if (createConsole)
            {
                ::WinExec(mConsoleFile.c_str(), SW_SHOW);
                mWnd = ::FindWindowA(NULL,s_sConsoleWndName.c_str());
            }
        }
    }

    void DiConsoleLogger::CreateConsoleWindow()
    {
        mWnd = ::FindWindowA(NULL,s_sConsoleWndName.c_str());
        if (NULL == mWnd)
        {
            ::WinExec(mConsoleFile.c_str(), SW_SHOW);
            mWnd = ::FindWindowA(NULL,s_sConsoleWndName.c_str());
        }
    }
}

#endif