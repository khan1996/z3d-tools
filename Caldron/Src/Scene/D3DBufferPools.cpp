#include "./d3dbufferpools.h"
#include "SceneManager.h"
#include "GMMemory.h"

using namespace Caldron::Scene;

void CD3DBufferObj::CreateBuffer(int iType,unsigned int uiSize,DWORD dwDecl,LPDIRECT3DRESOURCE8 lpBuffer)
{
	m_iBufferObjType = iType;
	m_uiBufferSize = uiSize;
	m_dwValue = dwDecl;

	m_pBuffers = lpBuffer;

}
void CD3DBufferObj::NullBuffer()
{
	SafeRelease(m_pBuffers);
	m_pBuffers = NULL;
	m_iBufferObjType = 0;
	m_uiBufferSize = 0;
	m_dwValue = 0;


}
CD3DBufferPools::CD3DBufferPools(void)
{
	for(int i = 0; i < D3DBUFFER_SIZENUM - 1; i++ )
	{
		m_DVertexBufferNums[i] = 0;
		m_DIndexBufferNums[i] = 0;
	}

	m_DCustomVertexBufferPools.clear();
	m_DCustomIndexBufferPools.clear();
	m_DIndexBufferNums[D3DBUFFER_SIZENUM - 1] = 0;
	m_DVertexBufferNums[D3DBUFFER_SIZENUM - 1] = 0;
	
	for(int i = 0; i < D3DBUFFER_SIZENUM - 1; i++ )
	{
		m_SVertexBufferNums[i] = 0;
		m_SIndexBufferNums[i] = 0;
	}

	m_SCustomVertexBufferPools.clear();
	m_SCustomIndexBufferPools.clear();
	m_SIndexBufferNums[D3DBUFFER_SIZENUM - 1] = 0;
	m_SVertexBufferNums[D3DBUFFER_SIZENUM - 1] = 0;
}	

CD3DBufferPools::~CD3DBufferPools(void)
{
	// Dynamic Buffer Clear
	for(int i = 0; i < D3DBUFFER_SIZENUM - 1; i++ )
	{
		while(!m_DVertexBufferPools[i].empty())
		{
			CD3DBufferObj *pObj = m_DVertexBufferPools[i].front();
			m_DVertexBufferPools[i].pop();
			if(pObj)
			{
				pObj->NullBuffer();
				SafeDelete(pObj);

			}
		}
		while(!m_DIndexBufferPools[i].empty())
		{
			CD3DBufferObj *pObj = m_DIndexBufferPools[i].front();
			m_DIndexBufferPools[i].pop();
			if(pObj)
			{
				pObj->NullBuffer();
				SafeDelete(pObj);

			}
		}
	
		m_DVertexBufferNums[i] = 0;
		m_DIndexBufferNums[i] = 0;
	}

	for(int i = 0; i < (int)m_DVertexBufferNums[D3DBUFFER_SIZENUM - 1]; i++ )
	{	
		if(m_DCustomVertexBufferPools[i])
		{
			m_DCustomVertexBufferPools[i]->NullBuffer();
			SafeDelete(m_DCustomVertexBufferPools[i]);
			m_DCustomVertexBufferPools[i] = NULL;
		}
	}
	m_DCustomVertexBufferPools.clear();

	for(int i = 0; i < (int)m_DIndexBufferNums[D3DBUFFER_SIZENUM - 1]; i++ )
	{	
		if(m_DCustomIndexBufferPools[i])
		{
			m_DCustomIndexBufferPools[i]->NullBuffer();
			SafeDelete(m_DCustomIndexBufferPools[i]);
			m_DCustomIndexBufferPools[i] = NULL;
		}
	}
	m_DCustomIndexBufferPools.clear();
	
	// Static Buffer Clear
	for(int i = 0; i < D3DBUFFER_SIZENUM - 1; i++ )
	{
		while(!m_SVertexBufferPools[i].empty())
		{
			CD3DBufferObj *pObj = m_SVertexBufferPools[i].front();
			m_SVertexBufferPools[i].pop();
			if(pObj)
			{
				pObj->NullBuffer();
				SafeDelete(pObj);

			}
		}
		while(!m_SIndexBufferPools[i].empty())
		{
			CD3DBufferObj *pObj = m_SIndexBufferPools[i].front();
			m_SIndexBufferPools[i].pop();
			if(pObj)
			{
				pObj->NullBuffer();
				SafeDelete(pObj);

			}
		}

	
		m_SVertexBufferNums[i] = 0;
		m_SIndexBufferNums[i] = 0;
	}

	for(int i = 0; i < (int)m_SVertexBufferNums[D3DBUFFER_SIZENUM - 1]; i++ )
	{	
		if(m_SCustomVertexBufferPools[i])
		{
			m_SCustomVertexBufferPools[i]->NullBuffer();
			SafeDelete(m_SCustomVertexBufferPools[i]);
			m_SCustomVertexBufferPools[i] = NULL;
		}
	}
	m_SCustomVertexBufferPools.clear();

	for(int i = 0; i < (int)m_SIndexBufferNums[D3DBUFFER_SIZENUM - 1]; i++ )
	{	
		if(m_SCustomIndexBufferPools[i])
		{
			m_SCustomIndexBufferPools[i]->NullBuffer();
			SafeDelete(m_SCustomIndexBufferPools[i]);
			m_SCustomIndexBufferPools[i] = NULL;
		}
	}
	m_SCustomIndexBufferPools.clear();


}
LPDIRECT3DVERTEXBUFFER8 CD3DBufferPools::GetVertexBuffer(unsigned int uiSize,DWORD dwDecl,bool bDynamic)
{
	LPDIRECT3DVERTEXBUFFER8 pReturn = NULL;
	int i,j;
	if(bDynamic)
	{
		for(i = 0;i < D3DBUFFER_SIZENUM - 1; i++)
		{
			if(uiSize <= ArrBufferSize[i])
			{
				if(m_DVertexBufferNums[i])
				{
					CD3DBufferObj *pObj = m_DVertexBufferPools[i].front();
					m_DVertexBufferPools[i].pop();
					m_DVertexBufferNums[i]--;
					if(pObj)
					{
						pReturn = (LPDIRECT3DVERTEXBUFFER8) pObj->m_pBuffers;
						SafeDelete(pObj);
						return pReturn;
					}
					return NULL;
				}
				return (pReturn = (LPDIRECT3DVERTEXBUFFER8)CreateBuffer(bDynamic,Scene::D3DBUFFEROBJ_VERTEX,i,dwDecl));	

			}
		}
		// Custom Size
		if(m_DVertexBufferNums[D3DBUFFER_SIZENUM - 1])
		{
			for(j = 0; j < (int)m_DVertexBufferNums[D3DBUFFER_SIZENUM - 1]; j++ )
			{

				if((m_DCustomVertexBufferPools[j]->m_uiBufferSize) >= uiSize) 
				{
					pReturn = ((LPDIRECT3DVERTEXBUFFER8)m_DCustomVertexBufferPools[j]->m_pBuffers);	
					SafeDelete(m_DCustomVertexBufferPools[j]);
					m_DCustomVertexBufferPools.erase(m_DCustomVertexBufferPools.begin() + j);
					m_DVertexBufferNums[D3DBUFFER_SIZENUM - 1]--;

					return pReturn;
				}

			}
		}
	}
	else	//static 
	{
		for(i = 0;i < D3DBUFFER_SIZENUM - 1; i++)
		{
			if(uiSize <= ArrBufferSize[i])
			{
				if(m_SVertexBufferNums[i])
				{
					CD3DBufferObj *pObj = m_SVertexBufferPools[i].front();
					m_SVertexBufferPools[i].pop();
					m_SVertexBufferNums[i]--;
					if(pObj)
					{
						pReturn = (LPDIRECT3DVERTEXBUFFER8) pObj->m_pBuffers;
						SafeDelete(pObj);
						return pReturn;
					}
					return NULL;
				}
				return (pReturn = (LPDIRECT3DVERTEXBUFFER8)CreateBuffer(bDynamic,Scene::D3DBUFFEROBJ_VERTEX,i,dwDecl));	

			}
		}
		// Custom Size
		if(m_SVertexBufferNums[D3DBUFFER_SIZENUM - 1])
		{
			for(j = 0; j < (int)m_SVertexBufferNums[D3DBUFFER_SIZENUM - 1]; j++ )
			{

				if((m_SCustomVertexBufferPools[j]->m_uiBufferSize) >= uiSize) 
				{
					pReturn = ((LPDIRECT3DVERTEXBUFFER8)m_SCustomVertexBufferPools[j]->m_pBuffers);	
					SafeDelete(m_SCustomVertexBufferPools[j]);
					m_SCustomVertexBufferPools.erase(m_SCustomVertexBufferPools.begin() + j);
					m_SVertexBufferNums[D3DBUFFER_SIZENUM - 1]--;

					return pReturn;
				}

			}
		}
	}
	return (pReturn = (LPDIRECT3DVERTEXBUFFER8)CreateBuffer(bDynamic,Scene::D3DBUFFEROBJ_VERTEX,-1,dwDecl,uiSize));	

}

LPDIRECT3DINDEXBUFFER8	CD3DBufferPools::GetIndexBuffer(unsigned int uiSize,DWORD dwFormat,bool bDynamic)
{
	LPDIRECT3DINDEXBUFFER8 pReturn = NULL;
	int i,j;
	if(bDynamic)
	{
		for(i = 0;i < D3DBUFFER_SIZENUM - 1; i++)
		{
			if(uiSize <= ArrBufferSize[i])
			{
				if(m_DIndexBufferNums[i])
				{
					CD3DBufferObj *pObj = m_DIndexBufferPools[i].front();
					m_DIndexBufferPools[i].pop();
					m_DIndexBufferNums[i]--;
					if(pObj)
					{
						pReturn = (LPDIRECT3DINDEXBUFFER8) pObj->m_pBuffers;
						SafeDelete(pObj);
						return pReturn;
					}
					return NULL;
				}
				return (pReturn = (LPDIRECT3DINDEXBUFFER8)CreateBuffer(bDynamic,Scene::D3DBUFFEROBJ_INDEX,i,dwFormat));	
				
			}
		}
		// Custom Size
		if(m_DIndexBufferNums[D3DBUFFER_SIZENUM - 1])
		{
			for(j = 0; j < (int)m_DIndexBufferNums[D3DBUFFER_SIZENUM - 1]; j++ )
			{

				if((m_DCustomIndexBufferPools[j]->m_uiBufferSize) >= uiSize) 
				{
					pReturn = ((LPDIRECT3DINDEXBUFFER8)m_DCustomIndexBufferPools[j]->m_pBuffers);	
					SafeDelete(m_DCustomIndexBufferPools[j]);
					m_DCustomIndexBufferPools.erase(m_DCustomIndexBufferPools.begin() + j);
					m_DIndexBufferNums[D3DBUFFER_SIZENUM - 1]--;
					return pReturn;
				}

			}
		}
	}
	else		//Static Buffer
	{
		for(i = 0;i < D3DBUFFER_SIZENUM - 1; i++)
		{
			if(uiSize <= ArrBufferSize[i])
			{
				if(m_SIndexBufferNums[i])
				{
					CD3DBufferObj *pObj = m_SIndexBufferPools[i].front();
					m_SIndexBufferPools[i].pop();
					m_SIndexBufferNums[i]--;
					if(pObj)
					{
						pReturn = (LPDIRECT3DINDEXBUFFER8) pObj->m_pBuffers;
						SafeDelete(pObj);
						return pReturn;
					}
					return NULL;
				}
				return (pReturn = (LPDIRECT3DINDEXBUFFER8)CreateBuffer(bDynamic,Scene::D3DBUFFEROBJ_INDEX,i,dwFormat));	
				
			}
		}
		// Custom Size
		if(m_SIndexBufferNums[D3DBUFFER_SIZENUM - 1])
		{
			for(j = 0; j < (int)m_SIndexBufferNums[D3DBUFFER_SIZENUM - 1]; j++ )
			{

				if((m_SCustomIndexBufferPools[j]->m_uiBufferSize) >= uiSize) 
				{
					pReturn = ((LPDIRECT3DINDEXBUFFER8)m_SCustomIndexBufferPools[j]->m_pBuffers);	
					SafeDelete(m_SCustomIndexBufferPools[j]);
					m_SCustomIndexBufferPools.erase(m_SCustomIndexBufferPools.begin() + j);
					m_SIndexBufferNums[D3DBUFFER_SIZENUM - 1]--;
					return pReturn;
				}

			}
		}


	}
	return (pReturn = (LPDIRECT3DINDEXBUFFER8)CreateBuffer(bDynamic,Scene::D3DBUFFEROBJ_INDEX,-1,dwFormat,uiSize));	

}
void CD3DBufferPools::UnRef(unsigned int iType,LPDIRECT3DRESOURCE8 lpBuffer)
{
	// ppuchuu 2004/11/15
	if( NULL == lpBuffer )
	{
		return;
	}

	// edith 2008.12.12 버퍼풀의 최대개수를 제한한다.
	unsigned int iMaxPoolCount = 100;
	bool bDynamic = false;


	if(iType == Scene::D3DBUFFEROBJ_VERTEX)
	{
		D3DVERTEXBUFFER_DESC VertexBufferDesc;
		((LPDIRECT3DVERTEXBUFFER8)lpBuffer)->GetDesc(&VertexBufferDesc);
		
		if(VertexBufferDesc.Usage & D3DUSAGE_DYNAMIC)
			bDynamic = true;

		if(bDynamic)
		{
			// 최대개수 지정.
			for(int i = 0;i < D3DBUFFER_SIZENUM - 1; i++)
			{
				if(VertexBufferDesc.Size <= ArrBufferSize[i])
				{
					if(m_DVertexBufferNums[i] >= iMaxPoolCount)
					{
						SafeRelease(lpBuffer);
						return;
					}
					break;
				}
			}
		}
		else
		{
			// 최대개수 지정
			for(int i = 0;i < D3DBUFFER_SIZENUM - 1; i++)
			{
				if(VertexBufferDesc.Size <= ArrBufferSize[i])
				{
					if(m_SVertexBufferNums[i] >= iMaxPoolCount)
					{
						SafeRelease(lpBuffer);
						return;
					}
					break;
				}
			}
		}

		CD3DBufferObj *pObj = new CD3DBufferObj;
		pObj->CreateBuffer(iType,VertexBufferDesc.Size,VertexBufferDesc.FVF,lpBuffer);

		if(bDynamic)
		{
			for(int i = 0;i < D3DBUFFER_SIZENUM - 1; i++)
			{
				if(pObj->m_uiBufferSize <= ArrBufferSize[i])
				{
					m_DVertexBufferPools[i].push(pObj);
					m_DVertexBufferNums[i]++;
					return;
				}
			}
			m_DCustomVertexBufferPools.push_back(pObj);
			m_DVertexBufferNums[D3DBUFFER_SIZENUM - 1]++;
		}
		else	// Static Buffer
		{
			for(int i = 0;i < D3DBUFFER_SIZENUM - 1; i++)
			{
				if(pObj->m_uiBufferSize <= ArrBufferSize[i])
				{
					m_SVertexBufferPools[i].push(pObj);
					m_SVertexBufferNums[i]++;
					return;
				}
			}
			m_SCustomVertexBufferPools.push_back(pObj);
			m_SVertexBufferNums[D3DBUFFER_SIZENUM - 1]++;
		}
	}
	else if(iType == Scene::D3DBUFFEROBJ_INDEX)
	{
		D3DINDEXBUFFER_DESC IndexBufferDesc;
		((LPDIRECT3DINDEXBUFFER8)lpBuffer)->GetDesc(&IndexBufferDesc);
		
		if(IndexBufferDesc.Usage & D3DUSAGE_DYNAMIC)
			bDynamic = true;
		
		if(bDynamic)
		{
			// 최대개수 지정.
			for(int i = 0;i < D3DBUFFER_SIZENUM - 1; i++)
			{
				if(IndexBufferDesc.Size <= ArrBufferSize[i])
				{
					if(m_DIndexBufferNums[i] >= iMaxPoolCount)
					{
						SafeRelease(lpBuffer);
						return;
					}
					break;
				}
			}
		}
		else
		{
			// 최대개수 지정
			for(int i = 0;i < D3DBUFFER_SIZENUM - 1; i++)
			{
				if(IndexBufferDesc.Size <= ArrBufferSize[i])
				{
					if(m_SIndexBufferNums[i] >= iMaxPoolCount)
					{
						SafeRelease(lpBuffer);
						return;
					}
					break;
				}
			}
		}

		CD3DBufferObj *pObj = new CD3DBufferObj;
		pObj->CreateBuffer(iType,IndexBufferDesc.Size,IndexBufferDesc.Format,lpBuffer);

		if(bDynamic)
		{
			for(int i = 0;i < D3DBUFFER_SIZENUM - 1; i++)
			{
				if(pObj->m_uiBufferSize <= ArrBufferSize[i])
				{
					m_DIndexBufferPools[i].push(pObj);
					m_DIndexBufferNums[i]++;
					return;
				}
			}
			m_DCustomIndexBufferPools.push_back(pObj);
			m_DIndexBufferNums[D3DBUFFER_SIZENUM - 1]++;
		}
		else	// Static 
		{

			for(int i = 0;i < D3DBUFFER_SIZENUM - 1; i++)
			{
				if(pObj->m_uiBufferSize <= ArrBufferSize[i])
				{
					m_SIndexBufferPools[i].push(pObj);
					m_SIndexBufferNums[i]++;
					return;
				}
			}
			m_SCustomIndexBufferPools.push_back(pObj);
			m_SIndexBufferNums[D3DBUFFER_SIZENUM - 1]++;
		}
	}
	
}
LPDIRECT3DRESOURCE8 CD3DBufferPools::CreateBuffer(bool bDynamic,unsigned int iType,int iSizeType,DWORD dwValue,unsigned int uiCustomSize)
{
	unsigned int uiSize = 0;

	if(iSizeType >= 0)
		uiSize = ArrBufferSize[iSizeType];
	else
		uiSize = uiCustomSize;

	if(iType == Scene::D3DBUFFEROBJ_VERTEX)
	{
		LPDIRECT3DVERTEXBUFFER8 lpVertex;
	
		if(!bDynamic)
		{
			
//			if(Caldron::Failed(CSceneManager::GetDevice()->CreateVertexBuffer(uiSize,D3DUSAGE_WRITEONLY,dwValue,D3DPOOL_DEFAULT,&lpVertex)))
			if(Caldron::Failed(CSceneManager::GetDevice()->CreateVertexBuffer(uiSize,D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&lpVertex)))
			{
				MessageBox(NULL,"Buffer Create Faile(1)","Error",MB_OK);
			}
		}
		else
		{
			if(Caldron::Failed(CSceneManager::GetDevice()->CreateVertexBuffer(uiSize,D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,0,D3DPOOL_DEFAULT,&lpVertex)))
			{
				MessageBox(NULL,"Buffer Create Faile(2)","Error",MB_OK);
			}
		}
	
		return lpVertex;

	}
	else if(iType == Scene::D3DBUFFEROBJ_INDEX)
	{
		LPDIRECT3DINDEXBUFFER8 lpIndex;
		if(!bDynamic)
		{
			if(Caldron::Failed(CSceneManager::GetDevice()->CreateIndexBuffer(uiSize,D3DUSAGE_WRITEONLY,(D3DFORMAT)dwValue,D3DPOOL_MANAGED,&lpIndex)))
//			if(Caldron::Failed(CSceneManager::GetDevice()->CreateIndexBuffer(uiSize,D3DUSAGE_WRITEONLY,(D3DFORMAT)dwValue,D3DPOOL_DEFAULT,&lpIndex)))
			{
				MessageBox(NULL,"Buffer Create Faile(3)","Error",MB_OK);
			}
		}
		else
		{

			if(Caldron::Failed(CSceneManager::GetDevice()->CreateIndexBuffer(uiSize,D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,(D3DFORMAT)dwValue,D3DPOOL_DEFAULT ,&lpIndex)))
			{
				MessageBox(NULL,"Buffer Create Faile(4)","Error",MB_OK);
			}
		}
	
	
	

		return lpIndex;

	}

	return NULL;	

}
void CD3DBufferPools::InitBuffer()
{
	int i,j;

	for( i = 0; i <  8; i++ )
	{
		LPDIRECT3DVERTEXBUFFER8 lpVertexBuffer[80];
		LPDIRECT3DINDEXBUFFER8 lpIndexBuffer[80];

		for( j = 0;j < 80; j++ )
		{
			if(j < 15)
			{
				lpVertexBuffer[j] = GetVertexBuffer(ArrBufferSize[i] - 10,0,false);	
				lpIndexBuffer[j] = GetIndexBuffer(ArrBufferSize[i] - 10,D3DFMT_INDEX16,false);	
			}
			else
			{
				lpVertexBuffer[j] = GetVertexBuffer(ArrBufferSize[i] - 10,0,true);	
				lpIndexBuffer[j] = GetIndexBuffer(ArrBufferSize[i] - 10,D3DFMT_INDEX16,true);
			}
		}
		for(j = 0;j < 80; j++ )
		{
			UnRef(D3DBUFFEROBJ_VERTEX,lpVertexBuffer[j]);
			UnRef(D3DBUFFEROBJ_INDEX,lpIndexBuffer[j]);
			lpVertexBuffer[j] = NULL;
			lpIndexBuffer[j] = NULL;
		}
	}

	for(i = 0; i < 5; i++ )
	{
		LPDIRECT3DVERTEXBUFFER8 lpCustomBuffer[5];
		lpCustomBuffer[0] = GetVertexBuffer(150000,0,true);
		lpCustomBuffer[1] = GetVertexBuffer(100000,0,true);
		lpCustomBuffer[2] = GetVertexBuffer(50000,0,true);
		lpCustomBuffer[3] = GetVertexBuffer(50000,0,true);
		lpCustomBuffer[4] = GetVertexBuffer(150000,0,true);
		for(j = 0; j < 5; j++ )
		{
			UnRef(D3DBUFFEROBJ_VERTEX,lpCustomBuffer[j]);
			lpCustomBuffer[j] = NULL;
		}
	}
}
