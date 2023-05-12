#pragma once
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "stringutil.h"
#include <fstream>
#include <map>
#include <iostream>
#pragma comment(lib,"shlwapi")

namespace aristocrat
{
	inline bool has_extension(const char* filepath, const char* ext)
	{
		auto n = strlen(filepath);
		auto p = strlen(ext);
		while (n-- && p--)
		{
			if (filepath[n] != ext[p]) return false;
		}
		return true;
	}

	inline bool get_folder_from_path(char* folder, const char* filepath)
	{
		auto n = strlen(filepath);
		auto i = 0UL;
		while (n--)
		{
			if (filepath[n] == L'\\') break;
		}

		for (i = 0; i < n; ++i)
		{
			folder[i] = filepath[i];
		}
		folder[i] = 0;
		return true;
	}

	inline bool get_filename_from_path(wchar_t* filename, const wchar_t* filepath)
	{
		auto nlen = wcslen(filepath);
		auto n = nlen;
		auto i = size_t{0};
		wchar_t *fn = filename;
		while (n--)
		{
			if (filepath[n] == L'\\') break;
		}

		for (i = n + 1; i < nlen; ++i)
		{
			*fn++ = filepath[i];
		}
		*fn++ = 0;
		return true;
	}

	inline bool get_filename_from_path(char* filename, const char* filepath)
	{
		auto nlen = strlen(filepath);
		auto n = nlen;
		auto i = size_t{0};
		char *fn = filename;
		while (n--)
		{
			if (filepath[n] == '\\') break;
		}

		for (i = n + 1; i < nlen; ++i)
		{
			*fn++ = filepath[i];
		}
		*fn++ = 0;
		return true;
	}

	inline const char* SwitchToExecutablesDirectory(HINSTANCE hInstance)
	{
		static char moduleFileName[MAX_PATH + 1];
		GetModuleFileNameA(hInstance, moduleFileName, MAX_PATH + 1);
		PathRemoveFileSpecA(moduleFileName);
		SetCurrentDirectoryA(moduleFileName);
		return moduleFileName;
	}
    
	inline const char* OpenGameDialog()
	{
		static char game_path[MAX_PATH];
		OPENFILENAMEA ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = 0;
		ofn.lpstrFile = game_path;
		ofn.lpstrFile[0] = 0;
		ofn.nMaxFile = sizeof(game_path);
		ofn.lpstrFilter = "Game Dll\0*.DLL\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.lpstrTitle = "Open GDK Game";
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		if (GetOpenFileNameA(&ofn))
		{
			return game_path;
		}
		return nullptr;
	}

	inline size_t GetFileSize(const char* filename)
	{
		if (!::PathFileExistsA(filename)) return 0;
		FILE* f = fopen(filename, "r");
		fseek(f, 0, SEEK_END);
		size_t fsize = ftell(f);
		fclose(f);
		return fsize;
	}

	// Use LocalFree to delete return value
	inline char* ReadFile(const char* filename)
	{
		if (!::PathFileExistsA(filename)) return nullptr;
		FILE* f = fopen(filename,"r");
		if (f == 0) return nullptr;
		fseek(f, 0, SEEK_END);
		size_t size = ftell(f);
		fseek(f, 0, SEEK_SET);
		char* buffer = (char*)LocalAlloc(LMEM_ZEROINIT, (sizeof(char) * (size)) + 1);
		fread(buffer, 1, size, f);
		fclose(f);

		return buffer;
	}
	
	// not safe storing return pointer.
	inline const char* RelativeToExecutable(const char* filename)
	{
		char moduleFolder[MAX_PATH + 1];
		static char target[MAX_PATH + 1];
		GetModuleFileNameA(NULL, moduleFolder, MAX_PATH + 1);
		PathRemoveFileSpecA(moduleFolder);
		PathCombineA(target, moduleFolder, filename);
		return target;
	}
	inline bool ReadFileKeyValuePairByLine(const char* filename, std::map<std::string, std::string>& map, char delimiter = '=')
	{
		if (!::PathFileExistsA(filename))
		{
			filename = RelativeToExecutable(filename);
			if (!::PathFileExistsA(filename))
				return false;
		}

		std::ifstream map_file(filename, std::ifstream::in);
		for (std::string line; std::getline(map_file, line); )
		{
			if (line.size() <= 0 || line[0] == '#')
			{
				continue;
			}
			auto pos = line.find_first_of(delimiter);
			
			std::string key = line.substr(0, pos);
			map[key] = line.substr(pos+1);
		}
		
		return true;
	}

	inline bool ReadBinaryFile(const char* filename, char* buffer, size_t buffer_size)
	{
		if (!::PathFileExistsA(filename)) 
			return false;
		FILE* f = fopen(filename, "rb");
		if (f == 0) 
			return false;
		
		fseek(f, 0, SEEK_END);
		size_t size = ftell(f);
		if(size > buffer_size) 
		{
			fclose(f);
			return false;
		}
		fseek(f, 0, SEEK_SET);
		bool success = fread(buffer, 1, size, f) == size;
		fclose(f);
		return success;
	}

	inline bool WriteFile(const char* filename, const char* data, size_t size)
	{
		FILE* f = fopen(filename, "wb");
		if (f == 0) return false;
		
		fwrite(data, 1, size, f);
		fflush(f);
		fclose(f);
		return GetFileSize(filename) == size;
	}

	inline LPSTR* CommandLineToArgv(int* argc, bool ignoreFirst, const char* cmdline)
	{
		static char args[4096]; 
		char* argp = &args[0];
		std::wstring cmdlinew = sutils::to_wstring(cmdline);
		auto argvw = CommandLineToArgvW(cmdlinew.c_str(), argc);

		LPSTR* argv;
		argv = (LPSTR*)LocalAlloc(LMEM_ZEROINIT, sizeof(char*) * (*argc + 1));
		int di = 0;
		for (int i = ignoreFirst ? 1 : 0; i < *argc; ++i)
		{
			std::string str = sutils::to_string(argvw[i]);
			strcpy(argp, str.c_str());
			argv[di++] = argp;
			argp += str.size();
			*argp++ = 0;
		}

		argv[*argc] = 0;
		LocalFree(argvw);
		*argc = ignoreFirst ? *argc - 1 : *argc;

		return argv;
	}

	inline LPSTR* CommandLineToArgv(int* argc, bool ignoreFirst)
	{
		static char args[4096];
		char* argp = &args[0];
		
		auto argvw = CommandLineToArgvW(GetCommandLineW(), argc);



		LPSTR* argv;
		argv = (LPSTR*)LocalAlloc(LMEM_ZEROINIT, sizeof(char*) * (*argc + 1));
		int di = 0;
		for (int i = ignoreFirst ? 1 : 0; i < *argc; ++i)
		{
			std::string str = sutils::to_string(argvw[i]);
			strcpy(argp, str.c_str());
			argv[di++] = argp;
			argp += str.size();
			*argp++ = 0;
		}

		argv[*argc] = 0;
		LocalFree(argvw);
		*argc = ignoreFirst ? *argc - 1 : *argc;

		return argv;
	}

    inline LPCSTR WindowMessageToString(UINT message)
    {
        #define DEFINE_MESSAGE(wm){ wm, #wm }
        struct MAP_MESSAGE
        {
	        UINT    nMsg;
	        LPCSTR  lpszMsg;
        };
        static const MAP_MESSAGE __allMessages[] =
        {
            DEFINE_MESSAGE(NULL),
	        DEFINE_MESSAGE(WM_CREATE),
	        DEFINE_MESSAGE(WM_DESTROY),
	        DEFINE_MESSAGE(WM_MOVE),
	        DEFINE_MESSAGE(WM_SIZE),
	        DEFINE_MESSAGE(WM_ACTIVATE),
	        DEFINE_MESSAGE(WM_SETFOCUS),
	        DEFINE_MESSAGE(WM_KILLFOCUS),
	        DEFINE_MESSAGE(WM_ENABLE),
	        DEFINE_MESSAGE(WM_SETREDRAW),
	        DEFINE_MESSAGE(WM_SETTEXT),
	        DEFINE_MESSAGE(WM_GETTEXT),
	        DEFINE_MESSAGE(WM_GETTEXTLENGTH),
	        DEFINE_MESSAGE(WM_PAINT),
	        DEFINE_MESSAGE(WM_CLOSE),
	        DEFINE_MESSAGE(WM_QUERYENDSESSION),
	        DEFINE_MESSAGE(WM_QUIT),
	        DEFINE_MESSAGE(WM_QUERYOPEN),
	        DEFINE_MESSAGE(WM_ERASEBKGND),
	        DEFINE_MESSAGE(WM_SYSCOLORCHANGE),
	        DEFINE_MESSAGE(WM_ENDSESSION),
	        DEFINE_MESSAGE(WM_SHOWWINDOW),
	        DEFINE_MESSAGE(WM_CTLCOLORMSGBOX),
	        DEFINE_MESSAGE(WM_CTLCOLOREDIT),
	        DEFINE_MESSAGE(WM_CTLCOLORLISTBOX),
	        DEFINE_MESSAGE(WM_CTLCOLORBTN),
	        DEFINE_MESSAGE(WM_CTLCOLORDLG),
	        DEFINE_MESSAGE(WM_CTLCOLORSCROLLBAR),
	        DEFINE_MESSAGE(WM_CTLCOLORSTATIC),
	        DEFINE_MESSAGE(WM_WININICHANGE),
	        DEFINE_MESSAGE(WM_SETTINGCHANGE),
	        DEFINE_MESSAGE(WM_DEVMODECHANGE),
	        DEFINE_MESSAGE(WM_ACTIVATEAPP),
	        DEFINE_MESSAGE(WM_FONTCHANGE),
	        DEFINE_MESSAGE(WM_TIMECHANGE),
	        DEFINE_MESSAGE(WM_CANCELMODE),
	        DEFINE_MESSAGE(WM_SETCURSOR),
	        DEFINE_MESSAGE(WM_MOUSEACTIVATE),
	        DEFINE_MESSAGE(WM_CHILDACTIVATE),
	        DEFINE_MESSAGE(WM_QUEUESYNC),
	        DEFINE_MESSAGE(WM_GETMINMAXINFO),
	        DEFINE_MESSAGE(WM_ICONERASEBKGND),
	        DEFINE_MESSAGE(WM_NEXTDLGCTL),
	        DEFINE_MESSAGE(WM_SPOOLERSTATUS),
	        DEFINE_MESSAGE(WM_DRAWITEM),
	        DEFINE_MESSAGE(WM_MEASUREITEM),
	        DEFINE_MESSAGE(WM_DELETEITEM),
	        DEFINE_MESSAGE(WM_VKEYTOITEM),
	        DEFINE_MESSAGE(WM_CHARTOITEM),
	        DEFINE_MESSAGE(WM_SETFONT),
	        DEFINE_MESSAGE(WM_GETFONT),
	        DEFINE_MESSAGE(WM_QUERYDRAGICON),
	        DEFINE_MESSAGE(WM_COMPAREITEM),
	        DEFINE_MESSAGE(WM_COMPACTING),
	        DEFINE_MESSAGE(WM_NCCREATE),
	        DEFINE_MESSAGE(WM_NCDESTROY),
	        DEFINE_MESSAGE(WM_NCCALCSIZE),
	        DEFINE_MESSAGE(WM_NCHITTEST),
	        DEFINE_MESSAGE(WM_NCPAINT),
	        DEFINE_MESSAGE(WM_NCACTIVATE),
	        DEFINE_MESSAGE(WM_GETDLGCODE),
	        DEFINE_MESSAGE(WM_NCMOUSEMOVE),
	        DEFINE_MESSAGE(WM_NCLBUTTONDOWN),
	        DEFINE_MESSAGE(WM_NCLBUTTONUP),
	        DEFINE_MESSAGE(WM_NCLBUTTONDBLCLK),
	        DEFINE_MESSAGE(WM_NCRBUTTONDOWN),
	        DEFINE_MESSAGE(WM_NCRBUTTONUP),
	        DEFINE_MESSAGE(WM_NCRBUTTONDBLCLK),
	        DEFINE_MESSAGE(WM_NCMBUTTONDOWN),
	        DEFINE_MESSAGE(WM_NCMBUTTONUP),
	        DEFINE_MESSAGE(WM_NCMBUTTONDBLCLK),
	        DEFINE_MESSAGE(WM_KEYDOWN),
	        DEFINE_MESSAGE(WM_KEYUP),
	        DEFINE_MESSAGE(WM_CHAR),
	        DEFINE_MESSAGE(WM_DEADCHAR),
	        DEFINE_MESSAGE(WM_SYSKEYDOWN),
	        DEFINE_MESSAGE(WM_SYSKEYUP),
	        DEFINE_MESSAGE(WM_SYSCHAR),
	        DEFINE_MESSAGE(WM_SYSDEADCHAR),
	        DEFINE_MESSAGE(WM_KEYLAST),
	        DEFINE_MESSAGE(WM_INITDIALOG),
	        DEFINE_MESSAGE(WM_COMMAND),
	        DEFINE_MESSAGE(WM_SYSCOMMAND),
	        DEFINE_MESSAGE(WM_TIMER),
	        DEFINE_MESSAGE(WM_HSCROLL),
	        DEFINE_MESSAGE(WM_VSCROLL),
	        DEFINE_MESSAGE(WM_INITMENU),
	        DEFINE_MESSAGE(WM_INITMENUPOPUP),
	        DEFINE_MESSAGE(WM_MENUSELECT),
	        DEFINE_MESSAGE(WM_MENUCHAR),
	        DEFINE_MESSAGE(WM_ENTERIDLE),
	        DEFINE_MESSAGE(WM_MOUSEWHEEL),
	        DEFINE_MESSAGE(WM_MOUSEMOVE),
	        DEFINE_MESSAGE(WM_LBUTTONDOWN),
	        DEFINE_MESSAGE(WM_LBUTTONUP),
	        DEFINE_MESSAGE(WM_LBUTTONDBLCLK),
	        DEFINE_MESSAGE(WM_RBUTTONDOWN),
	        DEFINE_MESSAGE(WM_RBUTTONUP),
	        DEFINE_MESSAGE(WM_RBUTTONDBLCLK),
	        DEFINE_MESSAGE(WM_MBUTTONDOWN),
	        DEFINE_MESSAGE(WM_MBUTTONUP),
	        DEFINE_MESSAGE(WM_MBUTTONDBLCLK),
	        DEFINE_MESSAGE(WM_PARENTNOTIFY),
	        DEFINE_MESSAGE(WM_MDICREATE),
	        DEFINE_MESSAGE(WM_MDIDESTROY),
	        DEFINE_MESSAGE(WM_MDIACTIVATE),
	        DEFINE_MESSAGE(WM_MDIRESTORE),
	        DEFINE_MESSAGE(WM_MDINEXT),
	        DEFINE_MESSAGE(WM_MDIMAXIMIZE),
	        DEFINE_MESSAGE(WM_MDITILE),
	        DEFINE_MESSAGE(WM_MDICASCADE),
	        DEFINE_MESSAGE(WM_MDIICONARRANGE),
	        DEFINE_MESSAGE(WM_MDIGETACTIVE),
	        DEFINE_MESSAGE(WM_MDISETMENU),
	        DEFINE_MESSAGE(WM_CUT),
	        DEFINE_MESSAGE(WM_COPYDATA),
	        DEFINE_MESSAGE(WM_COPY),
	        DEFINE_MESSAGE(WM_PASTE),
	        DEFINE_MESSAGE(WM_CLEAR),
	        DEFINE_MESSAGE(WM_UNDO),
	        DEFINE_MESSAGE(WM_RENDERFORMAT),
	        DEFINE_MESSAGE(WM_RENDERALLFORMATS),
	        DEFINE_MESSAGE(WM_DESTROYCLIPBOARD),
	        DEFINE_MESSAGE(WM_DRAWCLIPBOARD),
	        DEFINE_MESSAGE(WM_PAINTCLIPBOARD),
	        DEFINE_MESSAGE(WM_VSCROLLCLIPBOARD),
	        DEFINE_MESSAGE(WM_SIZECLIPBOARD),
	        DEFINE_MESSAGE(WM_ASKCBFORMATNAME),
	        DEFINE_MESSAGE(WM_CHANGECBCHAIN),
	        DEFINE_MESSAGE(WM_HSCROLLCLIPBOARD),
	        DEFINE_MESSAGE(WM_QUERYNEWPALETTE),
	        DEFINE_MESSAGE(WM_PALETTEISCHANGING),
	        DEFINE_MESSAGE(WM_PALETTECHANGED),
	        DEFINE_MESSAGE(WM_DDE_INITIATE),
	        DEFINE_MESSAGE(WM_DDE_TERMINATE),
	        DEFINE_MESSAGE(WM_DDE_ADVISE),
	        DEFINE_MESSAGE(WM_DDE_UNADVISE),
	        DEFINE_MESSAGE(WM_DDE_ACK),
	        DEFINE_MESSAGE(WM_DDE_DATA),
	        DEFINE_MESSAGE(WM_DDE_REQUEST),
	        DEFINE_MESSAGE(WM_DDE_POKE),
	        DEFINE_MESSAGE(WM_DDE_EXECUTE),
	        DEFINE_MESSAGE(WM_DROPFILES),
	        DEFINE_MESSAGE(WM_POWER),
	        DEFINE_MESSAGE(WM_WINDOWPOSCHANGED),
	        DEFINE_MESSAGE(WM_WINDOWPOSCHANGING),
        // MFC specific messages
	        DEFINE_MESSAGE(WM_HELP),
	        DEFINE_MESSAGE(WM_NOTIFY),
	        DEFINE_MESSAGE(WM_CONTEXTMENU),
	        DEFINE_MESSAGE(WM_TCARD),
	        DEFINE_MESSAGE(WM_MDIREFRESHMENU),
	        DEFINE_MESSAGE(WM_MOVING),
	        DEFINE_MESSAGE(WM_STYLECHANGED),
	        DEFINE_MESSAGE(WM_STYLECHANGING),
	        DEFINE_MESSAGE(WM_SIZING),
	        DEFINE_MESSAGE(WM_SETHOTKEY),
	        DEFINE_MESSAGE(WM_PRINT),
	        DEFINE_MESSAGE(WM_PRINTCLIENT),
	        DEFINE_MESSAGE(WM_POWERBROADCAST),
	        DEFINE_MESSAGE(WM_HOTKEY),
	        DEFINE_MESSAGE(WM_GETICON),
	        DEFINE_MESSAGE(WM_EXITMENULOOP),
	        DEFINE_MESSAGE(WM_ENTERMENULOOP),
	        DEFINE_MESSAGE(WM_DISPLAYCHANGE),
	        DEFINE_MESSAGE(WM_STYLECHANGED),
	        DEFINE_MESSAGE(WM_STYLECHANGING),
	        DEFINE_MESSAGE(WM_GETICON),
	        DEFINE_MESSAGE(WM_SETICON),
	        DEFINE_MESSAGE(WM_SIZING),
	        DEFINE_MESSAGE(WM_MOVING),
	        DEFINE_MESSAGE(WM_CAPTURECHANGED),
	        DEFINE_MESSAGE(WM_DEVICECHANGE),
	        DEFINE_MESSAGE(WM_PRINT),
	        DEFINE_MESSAGE(WM_PRINTCLIENT),
            
	        { 0, NULL, }    // end of message list
        };
        const MAP_MESSAGE* pMapMsg = __allMessages;
	    for (/*null*/; pMapMsg->lpszMsg != NULL; pMapMsg++)
	    {
		    if (pMapMsg->nMsg == message )
		    {
			    return (char *)pMapMsg->lpszMsg;
		    }
	    }
	    return "";
    }
}