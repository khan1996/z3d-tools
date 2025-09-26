// DeviceInput.h: interface for the CDeviceInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVICEINPUT_H__70A5C6C2_40AB_4919_9E57_25A08751068F__INCLUDED_)
#define AFX_DEVICEINPUT_H__70A5C6C2_40AB_4919_9E57_25A08751068F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DIRECTINPUT_VERSION 0x800

#include <dinput.h>
#include "Exception.h"


class CDeviceInput
{
	static LPDIRECTINPUT8 m_pDI;
	static LPDIRECTINPUTDEVICE8 m_pMouse;
	static LPDIRECTINPUTDEVICE8 m_pKeyboard;

	static bool m_MouseButton[3];
	static POINT m_MousePos;
	static int m_MouseZPos;
	static bool m_isMouseMove;

	static POINT m_MouseMovePos;
	static bool m_isLeftMouseClick,m_isRightMouseClick;
	static bool m_isLeftMouseDown,m_isRightMouseDown;
	static bool m_isLeftMousePress,m_isRightMousePress;
	static bool m_isRightMouseUp;
	static bool m_isMidMousePress;
	static float m_MouseSpeed;
	static RECT m_rcMouseMove;

	static char m_aryKeyState[256];
	static char m_aryKeyOldState[256];

public:
	void UpdateInput();
	static bool GetIsMousePos(){return m_isMouseMove;};
	static void SetIsMousePos(bool isMouseMove){m_isMouseMove=isMouseMove;};
	static bool GetIsLeftMouseClick(){return m_isLeftMouseClick;};
	static bool GetIsRightMouseClick(){return m_isRightMouseClick;};
	static bool GetIsLeftMouseDown(){return m_isLeftMouseDown;};
	static bool GetIsRightMouseDown(){return m_isRightMouseDown;};
	static bool GetIsLeftMousePress(){return m_isLeftMousePress;};
	static bool GetIsRightMousePress(){return m_isRightMousePress;};
	static bool GetIsMidMousePress(){return m_isMidMousePress;};
	static bool GetIsRightMouseUp(){return m_isRightMouseUp;};

	static void Fake_LeftMouseDown(){ m_isLeftMouseDown=true; };

	static POINT GetMousePosition(){return m_MousePos;};

	static void  SetMousePosition(POINT pt){m_MousePos=pt;};
	static POINT GetMouseMove(){return m_MouseMovePos;};
	static int GetMouseZMove(){return m_MouseZPos;};

	static BOOL KeyDown(int KeyCode) { if((m_aryKeyState[KeyCode] & 0x80) && !(m_aryKeyOldState[KeyCode] & 0x80)) return TRUE; else return FALSE; }
	static BOOL KeyHold(int KeyCode) { if(m_aryKeyState[KeyCode] & 0x80) return TRUE; else return FALSE; }
	static BOOL KeyUp(int KeyCode) { if(!(m_aryKeyState[KeyCode] & 0x80) && (m_aryKeyOldState[KeyCode] & 0x80)) return TRUE; else return FALSE; }

	static void InitKey(int KeyCode) { m_aryKeyState[KeyCode] = 0;	m_aryKeyOldState[KeyCode] = 0; }

	static void UnAcquire();
	static void Acquire();
	static void Create(HWND hWnd);
	CDeviceInput();
	virtual ~CDeviceInput();
};

#endif // !defined(AFX_DEVICEINPUT_H__70A5C6C2_40AB_4919_9E57_25A08751068F__INCLUDED_)
