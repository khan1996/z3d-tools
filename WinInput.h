// WinInput.h: interface for the CWinInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WININPUT_H__79336117_D3DE_4260_99DD_86D3F9B4B1A6__INCLUDED_)
#define AFX_WININPUT_H__79336117_D3DE_4260_99DD_86D3F9B4B1A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

class CWinInput  
{
	protected:
		POINT m_ptPreMousePos, m_ptMousePos, m_ptLocalPos;
		HWND m_hWnd;
		HWND m_hEditWnd;

		BOOL m_bFocus;
		BOOL m_bLButtonDBClick;
		BOOL m_bRButtonDBClick;
		BOOL m_bMButtonDBClick;
		BOOL m_bLButtonState, m_bLButtonOldState, m_bLButtonDBState;
		BOOL m_bRButtonState, m_bRButtonOldState, m_bRButtonDBState;
		BOOL m_bMButtonState, m_bMButtonOldState, m_bMButtonDBState;

		long m_lEStoreX, m_lEStoreY;
		long m_lMStoreX, m_lMStoreY;
		long m_lRStoreMoveX, m_lRStoreMoveY, m_lRStoreMoveZ;
		long m_lMStoreMoveX, m_lMStoreMoveY, m_lMStoreMoveZ;

		long m_lCursorCount;

		char m_aryKeyState[256];
		char m_aryKeyOldState[256];

		BOOL m_bSkillKey;

	public:
		CWinInput();
		~CWinInput();

		POINT m_ptMovePos;
		long m_lRMoveX, m_lRMoveY, m_lRMoveZ;
		long m_lMMoveX, m_lMMoveY, m_lMMoveZ;

		long GetCount(void) { return m_lCursorCount; }
		long ShowCursor(BOOL bShow)
		{
			if(bShow)
			{
				if(m_lCursorCount < 0)
				{
					for(;;)
					{
						m_lCursorCount = ::ShowCursor(TRUE);
						if(m_lCursorCount >= 0) break;
					}
				}
			} else
			{
				if(m_lCursorCount > -1)
				{
					for(;;)
					{
						m_lCursorCount = ::ShowCursor(FALSE);
						if(m_lCursorCount <= -1) break;
					}
				}
			}

			return m_lCursorCount;
		}

		void SetHwnd(HWND hWnd) { m_hWnd = hWnd; }
		void SetEditHwnd(HWND hWnd) { m_hEditWnd = hWnd; }
		POINT *GetMousePosition(void) { return &m_ptMousePos; }
		POINT *GetMouseLocal(void) { return &m_ptLocalPos; }
		void GetMouseState(unsigned int uMsg, WPARAM wParam, LPARAM lParam);
		void UpdateInfo(void);
		void Init(void);
		BOOL InRect(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
		{
			if(x1 <= m_ptLocalPos.x && m_ptLocalPos.x <= x2 && y1 <= m_ptLocalPos.y && m_ptLocalPos.y <= y2)
				return TRUE;
			else
				return FALSE;
		}
		BOOL InRect(RECT &rcRect)
		{
			if(rcRect.left <= m_ptLocalPos.x && m_ptLocalPos.x <= rcRect.right &&
				rcRect.top <= m_ptLocalPos.y && m_ptLocalPos.y <= rcRect.bottom)
				return TRUE;
			else
				return FALSE;
		}
		BOOL GetIsLeftMouseDBClick() { return m_bLButtonDBClick; }
		BOOL GetIsRightMouseDBClick() { return m_bRButtonDBClick; }
		BOOL GetIsMiddleMouseDBClick() 
		{ 
			return m_bMButtonDBClick; 
		}
		BOOL GetIsLeftMouseUp() { if(!m_bLButtonState && m_bLButtonOldState) return TRUE; else return FALSE; }
		BOOL GetIsRightMouseUp() { if(!m_bRButtonState && m_bRButtonOldState) return TRUE; else return FALSE; }
		BOOL GetIsMiddleMouseUp() { if(!m_bMButtonState && m_bMButtonOldState) return TRUE; else return FALSE; }
		BOOL GetIsLeftMouseDown() { if(m_bLButtonState && !m_bLButtonOldState && !m_bLButtonDBClick) return TRUE; else return FALSE; }
		BOOL GetIsRightMouseDown()
		{
			if(m_bRButtonState && !m_bRButtonOldState && !m_bRButtonDBClick)
				return TRUE;
			else
				return FALSE;
		}
		BOOL GetIsMiddleMouseDown()
		{
			if(m_bMButtonState && !m_bMButtonOldState && !m_bMButtonDBClick)
				return TRUE;
			else
				return FALSE;
		}
		BOOL GetIsLeftMousePress() { if(m_bLButtonState) return TRUE; else return FALSE; }
		BOOL GetIsRightMousePress()
		{
			if(m_bRButtonState)
				return TRUE;
			else
				return FALSE;
		}
		BOOL GetIsMiddleMousePress() 
		{ 
			if(m_bMButtonState) 
				return TRUE; 
			else 
				return FALSE; 
		}
		BOOL GetIsFocus(void) { return m_bFocus; }

		BOOL KeyDown(int KeyCode) 
		{ 
			if((m_aryKeyState[KeyCode] & 0x80) && !(m_aryKeyOldState[KeyCode] & 0x80)) 
				return TRUE; 
			else 
				return FALSE; 
		}
		BOOL KeyHold(int KeyCode) { if(m_aryKeyState[KeyCode] & 0x80) return TRUE; else return FALSE; }
		BOOL KeyUp(int KeyCode) { if(!(m_aryKeyState[KeyCode] & 0x80) && (m_aryKeyOldState[KeyCode] & 0x80)) return TRUE; else return FALSE; }

		void InitKey(int KeyCode) { m_aryKeyState[KeyCode] = 0;	m_aryKeyOldState[KeyCode] = 0; }
#ifdef _NEW_KEY_
		BOOL GetSkillKey() { return m_bSkillKey; } 

		void SetSkillKey(BOOL bBool)
		{
			m_bSkillKey = bBool;
		}		

		BOOL GetSkillHold()
		{
//			if (GetIsLeftMousePress()) 
//				return TRUE;

			if(!GetSkillKey())
				return FALSE;

			if (KeyHold('1')) 
				return TRUE;
			else if (KeyHold('2')) 
				return TRUE;
			else if (KeyHold('3')) 
				return TRUE;
			else if (KeyHold('4')) 
				return TRUE;
			else if (KeyHold('5')) 
				return TRUE;
			else if (KeyHold('6')) 
				return TRUE;
			else if (KeyHold('7')) 
				return TRUE;
			else if (KeyHold('8')) 
				return TRUE;
			else if (KeyHold('9')) 
				return TRUE;
			else if (KeyHold('0')) 
				return TRUE;


			return FALSE;
		}
#endif
		void SetCursorPos(int X, int Y)
		{
			m_ptLocalPos.x = X;
			m_ptLocalPos.y = Y;

			RECT rect;
			GetWindowRect(m_hWnd, &rect);

			::SetCursorPos(rect.left+X, rect.top+Y);
			GetCursorPos(&m_ptMousePos);
		}
};

extern CWinInput g_DeviceInput;

#endif // !defined(AFX_WININPUT_H__79336117_D3DE_4260_99DD_86D3F9B4B1A6__INCLUDED_)
