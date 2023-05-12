#pragma once
// Windows Header Files:
#include <windows.h>

#pragma warning(push) // Disable warning emitted by VersionHelper.h when using latest toolchain
#pragma warning(disable: 4800)
#include <VersionHelpers.h>
#pragma warning(pop)

#include "stringutil.h"

// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
#include <sstream>
#include <map>
#include <chrono>


#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif



std::string TouchEventToString(DWORD val)
{
	std::stringstream ss;
		if((val & TOUCHEVENTF_MOVE) == TOUCHEVENTF_MOVE)
			ss << "TOUCHEVENTF_MOVE ";
		if((val & TOUCHEVENTF_DOWN) == TOUCHEVENTF_DOWN)
			ss << "TOUCHEVENTF_DOWN ";
		if((val & TOUCHEVENTF_UP) == TOUCHEVENTF_UP)
			ss << "TOUCHEVENTF_UP ";
		if((val & TOUCHEVENTF_INRANGE)  == TOUCHEVENTF_INRANGE)
			ss << "TOUCHEVENTF_INRANGE ";
		if((val & TOUCHEVENTF_PRIMARY)  == TOUCHEVENTF_PRIMARY)
			ss << "TOUCHEVENTF_PRIMARY ";
		if((val & TOUCHEVENTF_NOCOALESCE) == TOUCHEVENTF_NOCOALESCE)
			ss << "TOUCHEVENTF_NOCOALESCE ";
		if((val & TOUCHEVENTF_PALM)  == TOUCHEVENTF_PALM)
			ss << "TOUCHEVENTF_PALM ";
	return ss.str();
}

enum MouseButton
{
	Left = 1,
	Right = 2,
	Middle = 3,
	None = 0
};

enum KeyboardModifiers
{
	Control = 1<<1,
	LControl = 1<<2,
	RControl = 1<<3,
	Alt = 1<<4,
	AltGr = 1<<5 | Alt,
	Shift = 1<<6,
	LShift = 1<<7 | Shift,
	RShift = 1<<8 | Shift,
	CapsLock = 1<<9 | Shift
};

struct WindowInfo
{
	LONG style;
	LONG ex_style;
	bool maximized;
	RECT window_rect;
};

static const unsigned int CLICK_THROTTLE_LIMIT_MS = 200U;
static const unsigned int MOVE_THROTTLE_LIMIT_MS  = 100U;

class SimpleWindow
{
private:
	HWND m_hwnd;
	WindowInfo			m_saved_window_info = { 0 };
	UINT				m_uiFrameBufferWidth;
	UINT				m_uiFrameBufferHeight;
	std::wstring		m_WindowName;
	std::string			m_WindowNameCstr;
	WNDPROC				m_hookedProc;
	HWND				m_parentHwnd;
	bool				m_fullScreen;
	bool				m_treat_touch_as_mouse;
	bool				m_activeTouch;
	std::chrono::steady_clock::time_point m_lastTouchReleaseTime;
	std::chrono::steady_clock::time_point m_lastTouchMoveTime;
	unsigned int		m_throttleTouchClickInMs;
	unsigned int		m_throttleTouchMoveInMs;
public:
	SimpleWindow() :
		m_hwnd(NULL),
		m_hookedProc(nullptr),
		m_parentHwnd(NULL),
		m_fullScreen(false),
		m_treat_touch_as_mouse(true),
		m_activeTouch(false),
		m_throttleTouchClickInMs(0U),
		m_throttleTouchMoveInMs(0U)
	{
		m_lastTouchReleaseTime = std::chrono::steady_clock::now();
		m_lastTouchMoveTime = std::chrono::steady_clock::now();
	}
	
	~SimpleWindow()
	{
		if (m_hookedProc)
		{
			::RemoveProp(m_hwnd, L"SIMPLE_INSTANCE");
			
			::SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_hookedProc));
			m_hookedProc = nullptr;
		}
		m_hwnd = NULL;
	}
	
    const char* Name()
	{
		return m_WindowNameCstr.c_str();
	}

	HWND WindowHandle() const
	{
		return m_hwnd;
	}

	void FillParent(HWND h = 0)
	{
		if (m_parentHwnd == NULL) return;
		RECT rcParent;
		::GetClientRect(m_parentHwnd, &rcParent);
		if (rcParent.left == rcParent.right || rcParent.top == rcParent.bottom) return;
        if (h)
        {
            RECT rc_child;
            ::GetClientRect(h, &rc_child);
            if (rc_child.left != rcParent.left ||
                rc_child.right != rcParent.right ||
                rc_child.top != rcParent.top ||
                rc_child.bottom != rcParent.bottom)
            {
                SetPosition(&rcParent);
            }
        }
        else
            SetPosition(&rcParent);
	}

   
    void DisableTouchFeedback()
    {
        // For slot games, we do not want the generic windows touch feedback graphics to show.

		// Min support for these APIs.
		if (IsWindows8OrGreater())
		{
			typedef void (WINAPI *PSWFS)(HWND, FEEDBACK_TYPE, DWORD, UINT32, const VOID*);

			if (auto pSetWindowFeedbackSetting = reinterpret_cast<PSWFS>(GetProcAddress(GetModuleHandle(TEXT("user32.dll")), "SetWindowFeedbackSetting")))
			{
				BOOL enabled = false;
				pSetWindowFeedbackSetting(m_hwnd, FEEDBACK_TOUCH_CONTACTVISUALIZATION, 0, sizeof(BOOL), &enabled);
				pSetWindowFeedbackSetting(m_hwnd, FEEDBACK_TOUCH_TAP, 0, sizeof(BOOL), &enabled);
				pSetWindowFeedbackSetting(m_hwnd, FEEDBACK_TOUCH_DOUBLETAP, 0, sizeof(BOOL), &enabled);
				pSetWindowFeedbackSetting(m_hwnd, FEEDBACK_TOUCH_PRESSANDHOLD, 0, sizeof(BOOL), &enabled);
				pSetWindowFeedbackSetting(m_hwnd, FEEDBACK_TOUCH_RIGHTTAP, 0, sizeof(BOOL), &enabled);
				pSetWindowFeedbackSetting(m_hwnd, FEEDBACK_GESTURE_PRESSANDTAP, 0, sizeof(BOOL), &enabled);
				pSetWindowFeedbackSetting(m_hwnd, FEEDBACK_PEN_PRESSANDHOLD, 0, sizeof(BOOL), &enabled);
			}
		}
    }

	int Width() const
	{
		return m_uiFrameBufferWidth;
	}

	int Height() const
	{
		return m_uiFrameBufferHeight;
	}

	bool Attach(HWND hWnd)
	{
		m_hwnd = hWnd;
		::SetProp(m_hwnd, L"SIMPLE_INSTANCE", (HANDLE)this);
		m_hookedProc = reinterpret_cast<WNDPROC>(::GetWindowLongPtr(m_hwnd, GWLP_WNDPROC));
		::SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SimpleWindow::WndProc));

		if(m_parentHwnd != 0)
		{
			SetClassLongPtr(m_parentHwnd, GCLP_HBRBACKGROUND, (LONG)0);
		}
		SetClassLongPtr(m_hwnd, GCLP_HBRBACKGROUND, (LONG)0);
		return true;
	}
    
	bool IsFullscreen() { return m_fullScreen; }
	void FullScreen(bool fullscreen, bool for_metro)
	{
		
		// Save current window state if not already fullscreen.
		if (!m_fullScreen) {
			// Save current window information.  We force the window into restored mode
			// before going fullscreen because Windows doesn't seem to hide the
			// taskbar if the window is in the maximized state.
			m_saved_window_info.maximized = !!::IsZoomed(m_hwnd);
			if (m_saved_window_info.maximized)
				::SendMessage(m_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
			m_saved_window_info.style = GetWindowLong(m_hwnd, GWL_STYLE);
			m_saved_window_info.ex_style = GetWindowLong(m_hwnd, GWL_EXSTYLE);
			GetWindowRect(m_hwnd, &m_saved_window_info.window_rect);
		}

		m_fullScreen = fullscreen;

		if (m_fullScreen) {
			// Set new window style and size.
			SetWindowLong(m_hwnd, GWL_STYLE,
				m_saved_window_info.style & ~(WS_CAPTION | WS_THICKFRAME));
			SetWindowLong(m_hwnd, GWL_EXSTYLE,
				m_saved_window_info.ex_style & ~(WS_EX_DLGMODALFRAME |
					WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));

			// On expand, if we're given a window_rect, grow to it, otherwise do
			// not resize.
			if (!for_metro) {
				MONITORINFO monitor_info;
				monitor_info.cbSize = sizeof(monitor_info);
				GetMonitorInfo(MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST),
					&monitor_info);
				RECT  window_rect(monitor_info.rcMonitor);
				SetWindowPos(m_hwnd, NULL, window_rect.left, window_rect.top,
					window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
					SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
			}
		}
		else {
			// Reset original window style and size.  The multiple window size/moves
			// here are ugly, but if SetWindowPos() doesn't redraw, the taskbar won't be
			// repainted.  Better-looking methods welcome.
			SetWindowLong(m_hwnd, GWL_STYLE, m_saved_window_info.style);
			SetWindowLong(m_hwnd, GWL_EXSTYLE, m_saved_window_info.ex_style);

			if (!for_metro) {
				// On restore, resize to the previous saved rect size.
				RECT new_rect(m_saved_window_info.window_rect);
				SetWindowPos(m_hwnd, NULL, new_rect.left, new_rect.top,
					new_rect.right - new_rect.left, new_rect.bottom - new_rect.top,
					SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
			}
			if (m_saved_window_info.maximized)
				::SendMessage(m_hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		}
		
	}

	HRESULT Create(HWND parent, const wchar_t* szTitleName, int clientWidth, int clientHeight, bool bWindowFrame = true)
	{
		POINT p{ CW_USEDEFAULT,CW_USEDEFAULT };
		return Create(parent, szTitleName, clientWidth, clientHeight, p, bWindowFrame);
	}
	// Register the window class and call methods for instantiating drawing resources
	HRESULT Create(HWND parent, const wchar_t* szTitleName, int clientWidth, int clientHeight, POINT& position, bool bWindowFrame = true)
	{
		m_parentHwnd = parent;
		m_WindowName = szTitleName;
		m_WindowNameCstr = aristocrat::sutils::to_string(szTitleName);
		m_uiFrameBufferWidth = clientWidth;
		m_uiFrameBufferHeight = clientHeight;
		HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

		//if (!SUCCEEDED(CoInitialize(NULL))) return -1;

		// Register the window class.
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		//wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = SimpleWindow::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = HINST_THISCOMPONENT;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
		wcex.lpszClassName = m_parentHwnd==NULL?L"SimpleWindow":L"SimpleChildWindow";

		RegisterClassEx(&wcex);

        DWORD wndStyle = m_parentHwnd == NULL ? (bWindowFrame ? WS_OVERLAPPEDWINDOW : WS_POPUP) : WS_CHILD;

		// If window has UI elements like border and title bar, its dimensions are
		// not the same as the client area.  
		RECT R = { 0, 0, clientWidth, clientHeight };
		//AdjustWindowRect(&R, wndStyle, FALSE);
        if (m_parentHwnd != NULL) 
          ::GetClientRect(m_parentHwnd, &R);

		int wndWidth = R.right - R.left;
		int wndHeight = R.bottom - R.top;

		// Create the window.
		m_hwnd = CreateWindow(
			m_parentHwnd == NULL ? L"SimpleWindow" : L"SimpleChildWindow",
			m_WindowName.c_str(),
			wndStyle,
			position.x,
			position.y,
			wndWidth,
			wndHeight,
			m_parentHwnd,
			NULL,
			HINST_THISCOMPONENT,
			this
			);
        HRESULT hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			::SetProp(m_hwnd, L"ATTACHED", (HANDLE) m_parentHwnd);
			//FillParent();
			//ShowWindow(m_hwnd, SW_HIDE);
			//UpdateWindow(m_hwnd);
            
			int value = GetSystemMetrics(SM_DIGITIZER);
			if (value & NID_READY)
			{ /* stack ready */}
			if (value  & NID_MULTI_INPUT)
			{
				RegisterTouchWindow(m_hwnd,0);
			}

			DisableTouchFeedback();

			ShrinkToFitWorkingArea();
		}

		return hr;
	}

	// Process and dispatch messages
	static bool RunMessageLoopOnce()
	{
        MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
            if (msg.message == WM_QUIT) // will be sent if we are the main window. 
                return false;
            // Translate the message and dispatch it to WindowProc()
            TranslateMessage(&msg);
            DispatchMessage(&msg);
		}

        return true;
	}

	void Resize(UINT width, UINT height)
	{
		m_uiFrameBufferWidth = width;
		m_uiFrameBufferHeight = height;
		FillParent();
	}

	void Dettach()
	{
		::SetProp(m_hwnd, L"SIMPLE_INSTANCE", (HANDLE)nullptr);
	}
	
	void SetTouchClickTrottleMs(unsigned int throttleMs)
	{
		if(throttleMs > CLICK_THROTTLE_LIMIT_MS)
			m_throttleTouchClickInMs = CLICK_THROTTLE_LIMIT_MS;
		else
			m_throttleTouchClickInMs = throttleMs;
	}

	void SetTouchMoveTrottleMs(unsigned int throttleMs)
	{
		if (throttleMs > MOVE_THROTTLE_LIMIT_MS)
			m_throttleTouchMoveInMs = MOVE_THROTTLE_LIMIT_MS;
		else
			m_throttleTouchMoveInMs = throttleMs;
	}

private:

	
	virtual void OnKeyPressed(char, int, int)
	{
	}

    virtual void OnTouch(UINT /*cInput*/, TOUCHINPUT* )
	{
	}

	void OnTouchInternal(UINT cInput, TOUCHINPUT* pInputs)
	{		
		// Currently this is always true, if this is ever made dynamic, the full implementation needs to be reviwed
		if(m_treat_touch_as_mouse)
		{
			// The TOUCHEVENTF_PRIMARY flag identifies the touch that was the first received from a "non-touched" state
			// The order in pInputs is not guaranteed, so while the first touch is likely to be the primary, it is not always true, so check all
			for (UINT i = 0; i < cInput; ++i)
			{
#ifdef _DEBUG
				if (::IsDebuggerPresent())
				{
					std::stringstream dbgTouch;
					dbgTouch << "i: " << i << " cx: " << pInputs[i].cxContact << " cy: " << pInputs[i].cyContact << "x: " << pInputs[i].x << " y: " << pInputs[i].y << " flags: " << TouchEventToString(pInputs[i].dwFlags) << " id: " << pInputs[i].dwID << " mask: " << pInputs[i].dwMask << " extra: " << pInputs[i].dwExtraInfo << std::endl;
					::OutputDebugStringA(dbgTouch.str().c_str());
				}
#endif
				if ((pInputs[i].dwFlags & TOUCHEVENTF_PRIMARY) == TOUCHEVENTF_PRIMARY)
				{
					POINT p = { pInputs[i].x / 100,pInputs[i].y / 100 };
					ScreenToClient(m_hwnd, &p);

					if ((pInputs[i].dwFlags & TOUCHEVENTF_DOWN) == TOUCHEVENTF_DOWN)
					{
						auto elapsed = std::chrono::steady_clock::now() - m_lastTouchReleaseTime;
						using milliseconds = std::chrono::duration<double, std::milli>;
						auto elapsedInMs = std::chrono::duration_cast<milliseconds>(elapsed).count();
						if (elapsedInMs >= m_throttleTouchClickInMs)
						{
							m_activeTouch = true;
							OnMouseClick(p.x, p.y, MouseButton::Left, true);
						}
					}
					
					if (m_activeTouch && (pInputs[i].dwFlags & TOUCHEVENTF_MOVE) == TOUCHEVENTF_MOVE)
					{
						auto elapsed = std::chrono::steady_clock::now() - m_lastTouchMoveTime;
						using milliseconds = std::chrono::duration<double, std::milli>;
						auto elapsedInMs = std::chrono::duration_cast<milliseconds>(elapsed).count();
						if (elapsedInMs >= m_throttleTouchMoveInMs)
						{
							m_lastTouchMoveTime = std::chrono::steady_clock::now();
							OnMouseMove(p.x, p.y, MouseButton::Left, true);
						}
					}

					if (m_activeTouch && (pInputs[i].dwFlags & TOUCHEVENTF_UP) == TOUCHEVENTF_UP)
					{
						m_lastTouchReleaseTime = std::chrono::steady_clock::now();
						OnMouseClick(p.x, p.y, MouseButton::Left, false);
						m_activeTouch = false;
					}

					// since there can be only 1 primary, once it has been processed, exit the loop
					break;
				}
			}
			
			
		}
		else
		{
			OnTouch(cInput, pInputs);
		}
	}

	void OnMouseInternal(int x, int y, MouseButton button, bool pressed, bool move)
	{
		// We should only be processing mouse events when in non-production environments
		// however windows automatically creates both touch and mouse events
		// So ensure that we only use the mouse events when it is not generated from touch code
		if (!m_activeTouch && (!IsTouchEvent() || !IsMultiTouchSupported()))
		{
			if (move)
			{
				OnMouseMove(x, y, button, pressed);
			}
			else
			{
				auto elapsed = std::chrono::steady_clock::now() - m_lastTouchReleaseTime;
				using milliseconds = std::chrono::duration<double, std::milli>;
				auto elapsedInMs = std::chrono::duration_cast<milliseconds>(elapsed).count();
				if (elapsedInMs >= m_throttleTouchClickInMs && button == MouseButton::Left)
				{
					if (!pressed)
					{
						m_lastTouchReleaseTime = std::chrono::steady_clock::now();
					}
					OnMouseClick(x, y, button, pressed);
				}
			}
		}
	}

	virtual void OnMouseClick(int, int, MouseButton, bool)
	{
	}
	
    virtual void OnMouseMove(int, int, MouseButton, bool)
	{
	}
    virtual void OnMouseScroll(int,int,int)
    {
    }
		// Resize the render target.
	virtual void OnResize( int /*width*/, int /*height*/)
	{
	}

    static BOOL CALLBACK DelPropProc(
        HWND h,              // handle of window with property 
        LPCSTR lpszString,   // property string or atom 
        HANDLE hData)        // data handle 
    {
        hData = RemovePropA(h, lpszString);
        //
        // if appropriate, free the handle hData
        //

        return TRUE;
    }
   
	// The windows procedure.
	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
		)
	{
		LRESULT result = 0;

		SimpleWindow *pSw = static_cast<SimpleWindow *>(::GetProp(hWnd, L"SIMPLE_INSTANCE"));

		if (message == WM_CREATE)
		{
			LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
			pSw = (SimpleWindow*)pcs->lpCreateParams;
			::SetProp(hWnd, L"SIMPLE_INSTANCE", (HANDLE)pSw);
			
			result = 1;
		}
		else
		{
			bool wasHandled = false;

			if (pSw != NULL)
			{
				switch (message)
				{
				case WM_SIZE:
				{
					UINT width = LOWORD(lParam);
					UINT height = HIWORD(lParam);
					pSw->OnResize(width, height);
					//InvalidateRect(hWnd, NULL, FALSE);
				}
				result = 0;
				wasHandled = true;
				break;

			/*	case WM_DISPLAYCHANGE:
				{
				//	InvalidateRect(hWnd, NULL, FALSE);
				}
				result = 0;
				wasHandled = true;
				break;*/
                case WM_PAINT:
                {
                    pSw->FillParent(hWnd);
                    ValidateRect(hWnd, NULL);
                }
				result = 0;
				wasHandled = true;
				return 0;
				break;
				case WM_ERASEBKGND:
				{
                    return 1;
				} 
                break;
				case WM_KEYDOWN:
				//if (lParam == 1) // ignore repeated keys
				{
					WORD ch = 0;
					BYTE kbs[256];
					::GetKeyboardState(kbs);
                    UINT param = (UINT) wParam;
					::MapVirtualKey(param, MAPVK_VSC_TO_VK);
					if (kbs[VK_CONTROL] & 0x80)
					{
						kbs[VK_CONTROL] &= 0x7f;
						if (::ToAscii(param, ::MapVirtualKey(param, MAPVK_VSC_TO_VK), kbs, &ch, 0) == 0)
							ch = 0;
						kbs[VK_CONTROL] |= 0x80;
					}
					else if (::ToAscii(param, ::MapVirtualKey(param, MAPVK_VSC_TO_VK), kbs, &ch, 0) == 0)
							ch = 0;
					
					int modifiers = 0;
					if (kbs[VK_LSHIFT] & 0x80)
					{
						modifiers |= KeyboardModifiers::LShift;
					}
					if (kbs[VK_RSHIFT] & 0x80)
					{
						modifiers |= KeyboardModifiers::RShift;
					}
					if (kbs[VK_LCONTROL] & 0x80)
					{
						modifiers |= KeyboardModifiers::LControl;
					}
					if (kbs[VK_RCONTROL] & 0x80)
					{
						modifiers |= KeyboardModifiers::RControl;
					}
					if (kbs[VK_LMENU] & 0x80)
					{
						modifiers |= KeyboardModifiers::Alt;
					}
					if (kbs[VK_RMENU] & 0x80)
					{
						modifiers |= KeyboardModifiers::AltGr;
					}
					if (kbs[VK_CAPITAL] & 0x01)
					{
						modifiers |= KeyboardModifiers::CapsLock;
					}
					
					pSw->OnKeyPressed((char)ch, (int)param,modifiers);

				}
				break;
				
				case WM_LBUTTONDOWN:
				{
					POINTS p = MAKEPOINTS(lParam);
					pSw->OnMouseInternal(p.x, p.y, Left, true, false);
				}
				break;
				
				case WM_LBUTTONUP:
				{
					POINTS p = MAKEPOINTS(lParam);
					pSw->OnMouseInternal(p.x, p.y, Left, false, false);
				}
				break;

				case WM_RBUTTONDOWN:
				{
					POINTS p = MAKEPOINTS(lParam);
					pSw->OnMouseInternal(p.x, p.y, Right, true, false);
				}
				break;
				case WM_RBUTTONUP:
				{
					POINTS p = MAKEPOINTS(lParam);
					pSw->OnMouseInternal(p.x, p.y, Right, false, false);
				}

				break;
                case WM_MOUSEWHEEL:
                {
                    POINTS p = MAKEPOINTS(lParam);
                    POINT pl = {p.x,p.y};
                    ScreenToClient(hWnd,&pl);
                    // DWORD fwKeys = GET_KEYSTATE_WPARAM(wParam);
                    int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
                    pSw->OnMouseScroll(pl.x,pl.y,zDelta/WHEEL_DELTA);
                }
                break;
				case WM_MBUTTONDOWN:
				{
					POINTS p = MAKEPOINTS(lParam);
					pSw->OnMouseInternal(p.x, p.y, Middle, true, false);
				}
				break;
				case WM_MBUTTONUP:
				{
					POINTS p = MAKEPOINTS(lParam);
					pSw->OnMouseInternal(p.x, p.y, Middle, false, false);
				}

				break;
				case WM_MOUSEMOVE:
				{
					POINTS p = MAKEPOINTS(lParam);
					MouseButton mb = None;
					if ((wParam & MK_LBUTTON) == MK_LBUTTON)
						mb = Left;
					else if ((wParam & MK_RBUTTON) == MK_RBUTTON)
						mb = Right;
					else if ((wParam & MK_MBUTTON) == MK_MBUTTON)
						mb = Middle;
					
					// Do not pass on moves that occur without the mouse button being pressed
					if (mb != None)
					{
						pSw->OnMouseInternal(p.x, p.y, mb, true, true);
					}
				}
				break;
				case WM_TOUCH:
				{
					UINT cInputs = LOWORD(wParam);
					PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs];
					if (pInputs)
					{
						if (GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, pInputs, sizeof(TOUCHINPUT)))
						{
							pSw->OnTouchInternal(cInputs, pInputs);
							result = 1;
							CloseTouchInputHandle((HTOUCHINPUT)lParam);
						}
						else
						{
								/* handle the error here */
						}
					}
					
					delete [] pInputs;

				} break;
				
				case WM_NCCALCSIZE:
					break;
				case WM_DESTROY:
				{
                    EnumPropsA(hWnd, DelPropProc);
					PostQuitMessage(0);
					//::Sleep(10); // Let the WM_QUIT get pushed before exit this proc
				}
				result = 1;
				wasHandled = true;
				break;
				}
			}

			if (!wasHandled)
			{
				if(pSw != nullptr && pSw->m_hookedProc)
					result =  CallWindowProc(pSw->m_hookedProc, hWnd, message, wParam, lParam);
				else
					result = DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		
		return result;
	}
	
	static bool IsTouchEvent()
	{
		// As specified in MSDN: https://docs.microsoft.com/en-us/windows/win32/tablet/system-events-and-mouse-messages
		const LONG_PTR c_SIGNATURE_MASK = 0xFFFFFF00;
		const LONG_PTR c_MOUSEEVENTF_FROMTOUCH = 0xFF515700;

		LONG_PTR extraInfo = GetMessageExtraInfo();

		return ((extraInfo & c_SIGNATURE_MASK) == c_MOUSEEVENTF_FROMTOUCH);
	}

	static bool IsMultiTouchSupported()
	{		
		int value = GetSystemMetrics(SM_DIGITIZER);
				
		return (value & NID_MULTI_INPUT) ? true : false;
	}

    void SetPosition(const PRECT rec) const
    {
        SetWindowPos(m_hwnd,
            HWND_TOP,
            rec->left,
            rec->top,
            rec->right,
            rec->bottom,
            SWP_DEFERERASE | SWP_NOREDRAW | SWP_NOOWNERZORDER | SWP_NOCOPYBITS | SWP_NOACTIVATE);

        /*MoveWindow(m_hwnd,
            rec.left,
            rec.top,
            rec.right,
            rec.bottom,
            true);*/
    }

    // Shrink the window to fit the working area if it is too large.
    void ShrinkToFitWorkingArea()
    {
        // Get window size
        RECT window_rect;
        GetWindowRect(m_hwnd, &window_rect);
        auto wndWidth = window_rect.right - window_rect.left;
        auto wndHeight = window_rect.bottom - window_rect.top;

        // Get working area size
        MONITORINFO monitor_info;
        monitor_info.cbSize = sizeof(monitor_info);
        GetMonitorInfo(MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST),
                       &monitor_info);
        RECT working_area(monitor_info.rcWork);
        auto wrkWidth = working_area.right - working_area.left;
        auto wrkHeight = working_area.bottom - working_area.top;

        if ((wrkWidth == wndWidth) && (wrkHeight == wndHeight))
            return;

        // Calculate ratio; return if window is already fitted in.
        auto ratioWidth = static_cast<double>(wndWidth) / wrkWidth;
        auto ratioHeight = static_cast<double>(wndHeight) / wrkHeight;
        auto ratio = ratioWidth > ratioHeight ? ratioWidth : ratioHeight;
        if (ratio < 1.0)
        {
            return;
        }

        // Shrink window size to fit working area
        UINT newWndWidth = static_cast<UINT>(wndWidth / ratio);
        UINT newWndHeight = static_cast<UINT>(wndHeight / ratio);
        Resize(newWndWidth, newWndHeight);
        SetWindowPos(m_hwnd, NULL, window_rect.left, window_rect.top,
                     newWndWidth, newWndHeight,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
};
