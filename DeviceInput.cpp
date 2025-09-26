// DeviceInput.cpp: implementation of the CDeviceInput class.
//
//////////////////////////////////////////////////////////////////////

#include "DeviceInput.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LPDIRECTINPUT8 CDeviceInput::m_pDI;
LPDIRECTINPUTDEVICE8 CDeviceInput::m_pMouse;	
LPDIRECTINPUTDEVICE8 CDeviceInput::m_pKeyboard;

bool CDeviceInput::m_MouseButton[3];
POINT CDeviceInput::m_MousePos;
bool CDeviceInput::m_isMouseMove;
POINT CDeviceInput::m_MouseMovePos;
bool CDeviceInput::m_isLeftMouseClick;
bool CDeviceInput::m_isRightMouseClick;

bool CDeviceInput::m_isLeftMouseDown;
bool CDeviceInput::m_isRightMouseDown;

bool CDeviceInput::m_isLeftMousePress;
bool CDeviceInput::m_isRightMousePress;

bool CDeviceInput::m_isRightMouseUp;
bool CDeviceInput::m_isMidMousePress;

float CDeviceInput::m_MouseSpeed;
int CDeviceInput::m_MouseZPos;
RECT CDeviceInput::m_rcMouseMove;

char CDeviceInput::m_aryKeyState[256];
char CDeviceInput::m_aryKeyOldState[256];


CDeviceInput::CDeviceInput()
{
	m_pDI=NULL;
	m_pMouse=NULL;
	m_pKeyboard = NULL;
	m_MouseButton[0]=false;
	m_MouseButton[1]=false;
	m_MouseButton[2]=false;

	m_MousePos.x=0;
	m_MousePos.y=0;
	m_MouseZPos=0;

	m_MouseMovePos.x=0;
	m_MouseMovePos.y=0;
	m_isLeftMouseClick=m_isRightMouseClick=false;
	m_isLeftMouseDown=m_isRightMouseDown=false;
	m_isLeftMousePress=m_isRightMousePress=false;
	m_isRightMouseUp=false;
	m_isMouseMove=false;
	m_MouseSpeed=2.0f;

	SetRect(&m_rcMouseMove,0,0,800-16,600-16);

	memset(m_aryKeyState, 0, 256);
	memset(m_aryKeyOldState, 0, 256);
}

CDeviceInput::~CDeviceInput()
{
	if(m_pKeyboard)
		m_pKeyboard->Release();
	if(m_pMouse)
		m_pMouse->Release();
	if(m_pDI)
		m_pDI->Release();	
}

void CDeviceInput::Create(HWND hWnd)
{	
	HRESULT hr;

	if( FAILED(DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                   IID_IDirectInput8, (VOID**)&m_pDI, NULL ) ) )
	{

	}
    
    // Obtain an interface to the system keyboard device.
    if( FAILED(m_pDI->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, NULL ) ) )
	{

	}
    
	if( FAILED( hr = m_pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
	{

	}

	if(FAILED(m_pKeyboard->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND )))
	{

	} 

	m_pKeyboard->Acquire();


    // Obtain an interface to the system mouse device.
    if( FAILED(m_pDI->CreateDevice( GUID_SysMouse, &m_pMouse, NULL ) ) )
	{

	}
    
	if( FAILED( hr = m_pMouse->SetDataFormat( &c_dfDIMouse2 ) ) )
	{

	}

	m_pMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	m_pMouse->Acquire();


	/*
    if(FAILED(DirectInputCreate( (HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE ),
                            DIRECTINPUT_VERSION, &m_pDI, NULL )))
	{
		throw CDeviceInputError("CDeviceInput:Create , Create DirectInput Error");
	}   
    
    if(FAILED(m_pDI->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, NULL )))
	{
		throw CDeviceInputError("CDeviceInput:Create , cannot Get a KeyboardDevice");
	}
    if(FAILED(m_pKeyboard->SetDataFormat( &c_dfDIKeyboard )))
	{
		throw CDeviceInputError("CDeviceInput:Create , Setting DataFormat failed");
	}    
	if(FAILED(m_pKeyboard->SetCooperativeLevel( hWnd,DISCL_EXCLUSIVE | DISCL_FOREGROUND )))
	{
		throw CDeviceInputError("CDeviceInput:Create ,SetCooperativeLevel is failed");
	} 
	*/

}

void CDeviceInput::Acquire()
{
	if(m_pKeyboard)
		m_pKeyboard->Acquire();
	if(m_pMouse)
		m_pMouse->Acquire();
}

void CDeviceInput::UnAcquire()
{	
}
void CDeviceInput::UpdateInput()
{
	HRESULT hr;

	memcpy(m_aryKeyOldState, m_aryKeyState, 256);
	hr = m_pKeyboard->GetDeviceState(256, (void *)&m_aryKeyState); 
	if(FAILED(hr))
	{
		memcpy(m_aryKeyState, m_aryKeyOldState, 256);
		return;
	} 

	DIMOUSESTATE2 dims2;      // DirectInput mouse state structure
	ZeroMemory( &dims2, sizeof(dims2) );	
    hr = m_pMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &dims2 );	

	if( FAILED(hr) ) 
	{
		m_MouseMovePos.x=0;
		m_MouseMovePos.y=0;		
		m_MouseZPos=0;
		m_MouseButton[0]=false;
		m_MouseButton[1]=false;
		m_MouseButton[2]=false;
		return;
	}

	m_MouseMovePos.x=dims2.lX;
	m_MouseMovePos.y=dims2.lY;
	m_MouseZPos=dims2.lZ;
	bool oldMouseButton[3];
	oldMouseButton[0]=m_MouseButton[0];
	oldMouseButton[1]=m_MouseButton[1];
	oldMouseButton[2]=m_MouseButton[2];
    m_MouseButton[0]=(dims2.rgbButtons[0] & 0x80) ? true : false;
    m_MouseButton[1]=(dims2.rgbButtons[1] & 0x80) ? true : false;
    m_MouseButton[2]=(dims2.rgbButtons[2] & 0x80) ? true : false;
	if(m_isMouseMove)
	{
		m_MousePos.x+=(int)(m_MouseMovePos.x*m_MouseSpeed);
		m_MousePos.y+=(int)(m_MouseMovePos.y*m_MouseSpeed);

		if(m_MousePos.x<=m_rcMouseMove.left)
			m_MousePos.x=m_rcMouseMove.left;

		if(m_MousePos.y<=m_rcMouseMove.top)
			m_MousePos.y=m_rcMouseMove.top;

		if(m_MousePos.x>=m_rcMouseMove.right)
			m_MousePos.x=m_rcMouseMove.right;
		if(m_MousePos.y>=m_rcMouseMove.bottom)
			m_MousePos.y=m_rcMouseMove.bottom;
	}
	if(oldMouseButton[0]==true && m_MouseButton[0]==false)
		m_isLeftMouseClick=true;	
	else	
		m_isLeftMouseClick=false;	

	if(oldMouseButton[1]==true && m_MouseButton[1]==false)
		m_isRightMouseClick=true;	
	else	
		m_isRightMouseClick=false;	

	if(oldMouseButton[0]==false && m_MouseButton[0]==true)
		m_isLeftMouseDown=true;
	else 
		m_isLeftMouseDown=false;
	if(oldMouseButton[1]==false && m_MouseButton[1]==true)
		m_isRightMouseDown=true;
	else 
		m_isRightMouseDown=false;

	if(oldMouseButton[1]==true && m_MouseButton[1]==false)
		m_isRightMouseUp=true;
	else 
		m_isRightMouseUp=false;

	if(m_MouseButton[0]==true)	
		m_isLeftMousePress=true;
	else
		m_isLeftMousePress=false;
	if(m_MouseButton[1]==true)
		m_isRightMousePress=true;
	else
		m_isRightMousePress=false;

	if(m_MouseButton[2]==true)
		m_isMidMousePress=true;
	else
		m_isMidMousePress=false;
}
