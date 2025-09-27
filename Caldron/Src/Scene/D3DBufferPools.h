#pragma once
#include "../../include/Caldron.h"

namespace Caldron {
	namespace Scene {
enum D3DBUFFFEROBJ_TYPE 
{
	D3DBUFFEROBJ_VERTEX = 0,
	D3DBUFFEROBJ_INDEX,
	D3DBUFFEROBJ_TYPENUM,

};
enum D3DBUFFER_SIZE
{
	D3DBUFFER_SIZE1 = 0, // 100
	D3DBUFFER_SIZE2 = 1, // 250
	D3DBUFFER_SIZE3 = 2, // 500
	D3DBUFFER_SIZE4 = 3, // 1000
	D3DBUFFER_SIZE5 = 4, // 3000
	D3DBUFFER_SIZE6 = 5, // 7000
	D3DBUFFER_SIZE7 = 6, // 10000
	D3DBUFFER_SIZE8 = 7, // 20000
	D3DBUFFER_SIZE9 = 8, // Custom Buffer
	D3DBUFFER_SIZENUM = 9,

};
// edith 2010.01.29 D3D 버퍼 크기 조절
const unsigned int ArrBufferSize[8] = {
	100,
	250,
	500,
	1000,
	3000,
	7000,
	10000,
	20000
};
class CD3DBufferObj
{
public:	
	int					m_iBufferObjType;
	LPDIRECT3DRESOURCE8 m_pBuffers;
	unsigned int		m_uiBufferSize;
	DWORD				m_dwValue;


	CD3DBufferObj() : m_pBuffers(NULL),m_uiBufferSize(0),m_dwValue(0){}
	~CD3DBufferObj()
	{
		//SafeRelease(m_pBuffers);
		m_pBuffers = NULL;
	}
	void NullBuffer();
	void CreateBuffer(int iType,unsigned int uiSize,DWORD dwDecl,LPDIRECT3DRESOURCE8 lpBuffer);

};

class CD3DBufferPools
{
public:
	CD3DBufferPools(void);
	~CD3DBufferPools(void);
	void InitBuffer();

	LPDIRECT3DVERTEXBUFFER8 GetVertexBuffer(unsigned int uiSize,DWORD dwDecl,bool bDynamic = false);
	LPDIRECT3DINDEXBUFFER8	GetIndexBuffer(unsigned int uiSize,DWORD dwFormat,bool bDynamic = false);
	void UnRef(unsigned int iType,LPDIRECT3DRESOURCE8 lpBuffer);

	LPDIRECT3DRESOURCE8 CreateBuffer(bool bDynamic,unsigned int iType,int iSizeType,DWORD dwValue,unsigned int uiCustomSize = 0);
	// Dynamic Buffer
	std::queue<CD3DBufferObj *>		m_DVertexBufferPools[D3DBUFFER_SIZENUM -1];
	std::vector<CD3DBufferObj *>	m_DCustomVertexBufferPools;
	unsigned int					m_DVertexBufferNums[D3DBUFFER_SIZENUM];

	std::queue<CD3DBufferObj *>		m_DIndexBufferPools[D3DBUFFER_SIZENUM -1];
	std::vector<CD3DBufferObj *>	m_DCustomIndexBufferPools;
	unsigned int					m_DIndexBufferNums[D3DBUFFER_SIZENUM];
	// Static Buffer
	std::queue<CD3DBufferObj *>		m_SVertexBufferPools[D3DBUFFER_SIZENUM -1];
	std::vector<CD3DBufferObj *>	m_SCustomVertexBufferPools;
	unsigned int					m_SVertexBufferNums[D3DBUFFER_SIZENUM];

	std::queue<CD3DBufferObj *>		m_SIndexBufferPools[D3DBUFFER_SIZENUM -1];
	std::vector<CD3DBufferObj *>	m_SCustomIndexBufferPools;
	unsigned int					m_SIndexBufferNums[D3DBUFFER_SIZENUM];

};


}}
