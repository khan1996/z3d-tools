// EnumD3D.h: interface for the CEnumD3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENUMD3D_H__9D5F8359_25F1_49DD_8C6A_CADE38529740__INCLUDED_)
#define AFX_ENUMD3D_H__9D5F8359_25F1_49DD_8C6A_CADE38529740__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <d3d8.h>
#include <vector>

//-----------------------------------------------------------------------------
// Name: struct D3DModeInfo
// Desc: Structure for holding information about a display mode
//-----------------------------------------------------------------------------
struct D3DModeInfo
{
    DWORD      Width;      // Screen width in this mode
    DWORD      Height;     // Screen height in this mode
    D3DFORMAT  Format;     // Pixel format in this mode
    DWORD      dwBehavior; // Hardware / Software / Mixed vertex processing
	DWORD      dwRefresh[50];
    D3DFORMAT  DepthStencilFormat; // Which depth/stencil format to use with this mode
};




//-----------------------------------------------------------------------------
// Name: struct D3DDeviceInfo
// Desc: Structure for holding information about a Direct3D device, including
//       a list of modes compatible with this device
//-----------------------------------------------------------------------------
struct D3DDeviceInfo
{
    // Device data
    D3DDEVTYPE   DeviceType;      // Reference, HAL, etc.
    D3DCAPS8     d3dCaps;         // Capabilities of this device
    const TCHAR* strDesc;         // Name of this device
    BOOL         bCanDoWindowed;  // Whether this device can work in windowed mode

    // Modes for this device
    DWORD        dwNumModes;
    D3DModeInfo  modes[150];

    // Current state
    DWORD        dwCurrentMode;
    BOOL         bWindowed;
    D3DMULTISAMPLE_TYPE MultiSampleType;
};




//-----------------------------------------------------------------------------
// Name: struct D3DAdapterInfo
// Desc: Structure for holding information about an adapter, including a list
//       of devices available on this adapter
//-----------------------------------------------------------------------------
struct D3DAdapterInfo
{
    // Adapter data
    D3DADAPTER_IDENTIFIER8 d3dAdapterIdentifier;
    D3DDISPLAYMODE d3ddmDesktop;      // Desktop display mode for this adapter

    // Devices for this adapter
    DWORD          dwNumDevices;
    D3DDeviceInfo  devices[5];

    // Current state
    DWORD          dwCurrentDevice;
};

/* Display Modes */
typedef struct _CD3DDISPLAYMODE
{
    UINT            Width;
    UINT            Height;
    UINT            RefreshRate[50];
    D3DFORMAT       Format;
} CD3DDISPLAYMODE;

class CEnumD3D  
{	    
    static BOOL              m_bWindowed;	
	static long				 m_dwMinDepthBits;
	static long				 m_dwMinStencilBits;
public:	 
	static long m_nAdapter,m_nDevice,m_nMode;
	static LPDIRECT3D8		 m_pD3D;
	static DWORD             m_dwNumAdapters;
	static BOOL				 m_bUseDepthBuffer;
	static DWORD             m_dwAdapter;
	static D3DAdapterInfo    m_Adapters[10];
	static TCHAR             m_strSoundCard[64];
	static TCHAR             m_strSoundCardDrv[64];

	static HRESULT ConfirmDevice(D3DCAPS8*,DWORD,D3DFORMAT);
	static BOOL FindDepthStencilFormat(UINT iAdapter, D3DDEVTYPE DeviceType, D3DFORMAT TargetFormat, D3DFORMAT *pDepthStencilFormat);
	static HRESULT Enum();	
	CEnumD3D();
	virtual ~CEnumD3D();
};

#endif // !defined(AFX_ENUMD3D_H__9D5F8359_25F1_49DD_8C6A_CADE38529740__INCLUDED_)
