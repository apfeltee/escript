// Win32Lib.cpp
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifdef _WIN32

    #include "escript.h"
    #include "escript.h"

    #include <windows.h>
    #include <iostream>

namespace EScript
{
    //! (static)
    void Win32Lib::init(EScript::Namespace* globals)
    {
        Namespace* lib = new Namespace;
        declareConstant(globals, "Win32", lib);

        //! [ESF]	void setClipboard( string )
        ES_FUN(lib, "setClipboard", 1, 1, (Win32Lib::setClipboard(parameter[0].toString()), RtValue(nullptr)))

        //! [ESF]	string getClipboard( )
        ES_FUN(lib, "getClipboard", 0, 0, Win32Lib::getClipboard())

        //! [ESF]	bool loadLibrary(string )
        ES_FUNCTION(lib, "loadLibrary", 1, 1,
                    {
                        HINSTANCE hDLL;
                        libInitFunction* f;// Function pointer

                        hDLL = LoadLibrary(parameter[0].toString().c_str());
                        if(hDLL == nullptr)
                            return false;

                        f = reinterpret_cast<libInitFunction*>(GetProcAddress(hDLL, "init"));
                        if(!f)
                        {
                            // handle the error
                            FreeLibrary(hDLL);
                            return false;
                        }
                        EScript::initLibrary(f);

                        return true;
                    })
    }

    //! (static)
    void Win32Lib::setClipboard(const std::string& s)
    {
        if(OpenClipboard(nullptr))
        {
            EmptyClipboard();
            HGLOBAL hClipboardData;
            hClipboardData = GlobalAlloc(GMEM_DDESHARE, s.size() + 1);
            // strData.GetLength()+1);
            char* pchData;
            pchData = reinterpret_cast<char*>(GlobalLock(hClipboardData));

            strncpy(pchData, s.c_str(), s.size());// LPCSTR(strData));
            GlobalUnlock(hClipboardData);
            SetClipboardData(CF_TEXT, hClipboardData);
            CloseClipboard();
        }
        else
            std::cout << "Could not open Clipboard!\n";
    }

    //! (static)
    std::string Win32Lib::getClipboard()
    {
        if(!OpenClipboard(nullptr))
        {
            std::cout << "Could not open Clipboard!\n";
            return "";
        }

        HGLOBAL hClipboardData;
        char* lpstr;

        hClipboardData = GetClipboardData(CF_TEXT);
        lpstr = reinterpret_cast<char*>(GlobalLock(hClipboardData));

        std::string s = std::string(lpstr);
        //std::cout << s;
        // ...

        GlobalUnlock(hClipboardData);
        CloseClipboard();
        return s;
    }

}// namespace EScript
#endif
