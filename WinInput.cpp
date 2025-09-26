// WinInput.cpp: implementation of the CWinInput class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseGraphicsLayer.h"
#include "WinInput.h"
#include "GMMemory.h"

CWinInput g_DeviceInput;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWinInput::CWinInput()
{
	m_ptPreMousePos.x = m_ptMousePos.x = 0;
	m_ptPreMousePos.y = m_ptMousePos.y = 0;

	Init();

	m_bFocus = TRUE;
	m_hWnd = 0;
	m_hEditWnd = 0;

	m_lCursorCount = 0;
}

CWinInput::~CWinInput()
{

}

void CWinInput::UpdateInfo()
{
	m_ptPreMousePos = m_ptMousePos;
	GetCursorPos(&m_ptMousePos);
	m_ptLocalPos = m_ptMousePos;
	ScreenToClient(m_hWnd, &m_ptLocalPos);

	if(m_hWnd && m_hEditWnd && GetFocus() != m_hWnd && GetFocus() != m_hEditWnd)
	{
		Init();
		m_bFocus = FALSE;
		return;
	}

	if(InRect(0, 0, BaseGraphicsLayer::m_lScreenSx, BaseGraphicsLayer::m_lScreenSy))
	{
		m_ptMovePos.x = m_ptPreMousePos.x - m_ptMousePos.x;
		m_ptMovePos.y = m_ptPreMousePos.y - m_ptMousePos.y;

		memcpy(m_aryKeyOldState, m_aryKeyState, 256);
		ZeroMemory(m_aryKeyState, 256);
		for(int i = 0; i < 256; i++)
		{
			if(HIBYTE(GetAsyncKeyState(i))) m_aryKeyState[i] = (char)0x80;
		}

		m_bLButtonOldState = m_bLButtonState;
		if(m_aryKeyState[VK_LBUTTON]) 
			m_bLButtonState = TRUE; 
		else 
			m_bLButtonState = FALSE;

		m_bLButtonDBClick = FALSE;
		if(m_bLButtonDBState)
		{
			m_bLButtonDBClick = TRUE;
		}
		m_bLButtonDBState = FALSE;

		m_bRButtonOldState = m_bRButtonState;
		if(m_aryKeyState[VK_RBUTTON])
			m_bRButtonState = TRUE;
		else
			m_bRButtonState = FALSE;

		m_bRButtonDBClick = FALSE;
		if(m_bRButtonDBState)
		{
			m_bRButtonDBClick = TRUE;
		}
		m_bRButtonDBState = FALSE;

		m_bMButtonOldState = m_bMButtonState;
		if(m_aryKeyState[VK_MBUTTON]) m_bMButtonState = TRUE; else m_bMButtonState = FALSE;

		m_lMMoveZ = m_lMStoreMoveZ;
		m_lMStoreMoveZ = 0;


/*		if(GetIsRightMousePress())
		{
			POINT ptMousePos;
			GetCursorPos(&ptMousePos);

			m_lRStoreMoveX = ptMousePos.x - 400;
			m_lRStoreMoveY = ptMousePos.y - 300;

			SetCursorPos(400, 300);
		}

		m_lRMoveX = m_lRStoreMoveX;
		m_lRMoveY = m_lRStoreMoveY;
		m_lRMoveZ = m_lRStoreMoveZ;

		m_lRStoreMoveX = m_lRStoreMoveY = m_lRStoreMoveZ = 0;

		if(GetIsMiddleMousePress())
		{
			POINT ptMousePos;
			GetCursorPos(&ptMousePos);

			m_lMStoreMoveX = ptMousePos.x - 400;
			m_lMStoreMoveY = ptMousePos.y - 300;

			SetCursorPos(400, 300);
		}

		m_lMMoveX = m_lMStoreMoveX;
		m_lMMoveY = m_lMStoreMoveY;
		m_lMMoveZ = m_lMStoreMoveZ;

		m_lMStoreMoveX = m_lMStoreMoveY = m_lMStoreMoveZ = 0;*/

		if(m_bFocus == FALSE)
		{
			//::SetFocus(*m_hWnd);
			m_bFocus = TRUE;
		}
	} else
	{
		Init();
		m_bFocus = FALSE;
	}
}

void CWinInput::GetMouseState(unsigned int uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case 0x020A:		// WM_MOUSEWHEEL
			{
				m_lMStoreMoveZ = ((short)HIWORD(wParam)) / 120;
			}
			break;

		case WM_LBUTTONDBLCLK:
			m_bLButtonDBState = TRUE;
			break;

		case WM_RBUTTONDBLCLK:
			m_bRButtonDBState = TRUE;
			break;
/*		case WM_RBUTTONDOWN:
			m_lMStoreX = LOWORD(lParam);
			m_lMStoreY = HIWORD(lParam);
			SetCursorPos(400, 300);
			ShowCursor(FALSE);

		case WM_RBUTTONUP:
//			SetCursorPos(m_lMStoreX, m_lMStoreY);
//			ShowCursor(TRUE);
			break;*/

		case WM_MBUTTONDBLCLK:
			m_bMButtonDBState = TRUE;
			break;
/*		case WM_MBUTTONDOWN:
			m_lMStoreX = LOWORD(lParam);
			m_lMStoreY = HIWORD(lParam);
			SetCursorPos(400, 300);
			ShowCursor(FALSE);

		case WM_MBUTTONUP:
//			SetCursorPos(m_lMStoreX, m_lMStoreY);
//			ShowCursor(TRUE);
			break;*/
	}
}

void CWinInput::Init()
{
	m_bLButtonOldState = m_bLButtonState = m_bLButtonDBState = FALSE;
	m_bRButtonOldState = m_bRButtonState = m_bRButtonDBState = FALSE;
	m_bMButtonOldState = m_bMButtonState = m_bMButtonDBState = FALSE;
	m_bLButtonDBClick = m_bRButtonDBClick = m_bMButtonDBClick = FALSE;

	m_ptMovePos.x = 0;
	m_ptMovePos.y = 0;

	m_lRStoreMoveX = m_lRStoreMoveY = m_lRStoreMoveZ = 0;
	m_lMStoreMoveX = m_lMStoreMoveY = m_lMStoreMoveZ = 0;

	ShowCursor(TRUE);
}