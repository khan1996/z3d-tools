// GrassManager.cpp: implementation of the CGrassManager class.
//
//////////////////////////////////////////////////////////////////////

#include "GrassManager.h"
#include "SceneManager.h"
#include "RenderOption.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGrassManager::CGrassManager()
{
	if(CRenderOption::m_GrassRendering && CRenderOption::m_GrassAnimationUseVertexShader)
	{
		float fAdder=(3.14159f*2.0f)/20.0f;
		for(int i=0;i<20;i++)
		{		
			m_fGrassRot[i]=fAdder*i;
		}
	}
}

CGrassManager::~CGrassManager()
{
	if(m_pGrassNode != NULL)
	{
		delete[] m_pGrassNode;
		m_pGrassNode = NULL;
	}
}
void CGrassManager::Delete() 
{
	if(m_pGrassNode) {
		for(int i=0;i<MAX_GRASSRANGE*MAX_GRASSRANGE;i++) {
			
			m_pGrassNode[i].UnloadData();
		}
	}
	
}
void CGrassManager::Create()
{	
	if(CRenderOption::m_GrassRendering)	
	{
		MAX_GRASSRANGE=CRenderOption::m_GrassRenderRange;

		m_pGrassNode=new CGrassScene[MAX_GRASSRANGE*MAX_GRASSRANGE];
		for(int i=0;i<MAX_GRASSRANGE*MAX_GRASSRANGE;i++)
		{
			m_pGrassNode[i].Create();
		}

		m_NowPositionX=0;
		m_NowPositionY=0;
		for(int ix=-MAX_GRASSRANGE/2,cx=0;ix<MAX_GRASSRANGE/2;ix++,cx++)
		{
			for(int iy=-MAX_GRASSRANGE/2,cy=0;iy<MAX_GRASSRANGE/2;iy++,cy++)
			{
				m_pGrassNode[cx+cy*MAX_GRASSRANGE].Generate(
						ix*LINTERVAL,
						iy*LINTERVAL,10);
			}
		}
	}	
}

void CGrassManager::UpdateRange()
{	
	if(CRenderOption::m_GrassRendering)	
	{
		vector3 *ViewPos;
		vector3 CharPos;
		if(CSceneManager::m_ViewerMode==0 || CSceneManager::m_ViewerMode==2)
			ViewPos=CSceneManager::GetCamera()->GetPosition();
		else
		{		
			CSceneManager::m_CharacterManager.m_CharacterList[0].m_pChrmodel->GetPosition(CharPos.x,CharPos.y,CharPos.z);		
			ViewPos=&CharPos;
		}

		int UpdatePositionX=(int)(ViewPos->x/LINTERVAL);
		int UpdatePositionY=(int)(ViewPos->z/LINTERVAL);

		if( m_NowPositionX == UpdatePositionX &&
			m_NowPositionY == UpdatePositionY) return;

		int WillX=(int)(ViewPos->x/LINTERVAL);
		int WillY=(int)(ViewPos->z/LINTERVAL);
		int MoveX=m_NowPositionX-WillX;
		int MoveY=m_NowPositionY-WillY;
		
		int *UpdateGrass=new int[MAX_GRASSRANGE*MAX_GRASSRANGE];
		int *NotUpdateGrass=new int[MAX_GRASSRANGE*MAX_GRASSRANGE];
		int CountUpdate=0;

		memset(NotUpdateGrass,-1,sizeof(int)*MAX_GRASSRANGE*MAX_GRASSRANGE);
		
		int fx,fy;
		int ix,iy;
		int grassx,grassy;

		for(ix=0;ix<MAX_GRASSRANGE;ix++)
		{
			for(iy=0;iy<MAX_GRASSRANGE;iy++)
			{			
				grassx=m_pGrassNode[ix+iy*MAX_GRASSRANGE].m_GrassPosX;
				grassy=m_pGrassNode[ix+iy*MAX_GRASSRANGE].m_GrassPosY;

				if( (grassx >= WillX-(int)(MAX_GRASSRANGE/2.0f) && grassx <= WillX+(int)(MAX_GRASSRANGE/2)) &&
					(grassy >= WillY-(int)(MAX_GRASSRANGE/2.0f) && grassy <= WillY+(int)(MAX_GRASSRANGE/2)) )
				{
					fx=grassx-WillX+(int)(MAX_GRASSRANGE/2.0f);
					fy=grassy-WillY+(int)(MAX_GRASSRANGE/2.0f);				
					if(NotUpdateGrass[fx+fy*MAX_GRASSRANGE]==0)
					{
						UpdateGrass[CountUpdate++]=ix+iy*MAX_GRASSRANGE;
					}
					else
					{
						NotUpdateGrass[fx+fy*MAX_GRASSRANGE]=0;
					}
				}
				else
					UpdateGrass[CountUpdate++]=ix+iy*MAX_GRASSRANGE;

			}
		}
		if(CountUpdate!=MAX_GRASSRANGE*MAX_GRASSRANGE)
		{
			UpdateGrass[CountUpdate]=-1;
		}
		
		int InnerCount=0;
		int CheckEndUpdate=0;
		for(ix=0;ix<MAX_GRASSRANGE;ix++)
		{
			for(iy=0;iy<MAX_GRASSRANGE;iy++)
			{				
				if(NotUpdateGrass[ix+iy*MAX_GRASSRANGE]==-1)
				{
					CheckEndUpdate=UpdateGrass[InnerCount++];
					if(CheckEndUpdate==-1)
						break;
					m_pGrassNode[CheckEndUpdate].m_GrassPosX=(ix+WillX-(int)(MAX_GRASSRANGE/2.0f));
					m_pGrassNode[CheckEndUpdate].m_GrassPosY=(iy+WillY-(int)(MAX_GRASSRANGE/2.0f));
					m_pGrassNode[CheckEndUpdate].Generate(
						(ix+WillX-(int)(MAX_GRASSRANGE/2.0f))*LINTERVAL,
						(iy+WillY-(int)(MAX_GRASSRANGE/2.0f))*LINTERVAL,30);
				}
				
			}
			if(CheckEndUpdate==-1)
					break;			
		}	
		m_NowPositionX=UpdatePositionX;
		m_NowPositionY=UpdatePositionY;		
		if(UpdateGrass) {
			delete [] UpdateGrass;
			UpdateGrass = NULL;
		}
		if(NotUpdateGrass) {	
			delete [] NotUpdateGrass;
			NotUpdateGrass = NULL;
		}
	}
	//*/
	
}

void CGrassManager::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{	
	if(CRenderOption::m_GrassRendering)	// 풀그리기
	{	
		if(CRenderOption::m_GrassAnimationUseVertexShader)		
		{
			for(int i=0;i<20;i++)
			{
				float frate=sinf(m_fGrassRot[i]);
				if(frate>0.98f)
					frate=0.98f;
				if(frate<-0.98f)
					frate=-0.98f;
				
				vector3 vecRotAxis;
				vecRotAxis=vector3(1.0f,0.0f,0.0f);		
				vecRotAxis.Normalize();
				matrix matRot;
				matRot.Rotation(vecRotAxis,frate*0.003f);		
				//matRot.Rotation(vecRotAxis,frate);	
				
				float f=tan(0.003f)*(20000.0f+100.0f);
				
				if(i==0)
				{
					matRot.MakeIdent();
				}
				
				
				matRot.Transpose();				
				pd3dDevice->SetVertexShaderConstant(15+4*i,&matRot,4);
			}			

		}
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_DIFFUSE);	
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
		//

		//pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
		/*pd3dDevice->LightEnable(0,FALSE);	
		pd3dDevice->LightEnable(1,FALSE);	
		pd3dDevice->LightEnable(2,FALSE);	
		pd3dDevice->LightEnable(3,FALSE);	
		pd3dDevice->LightEnable(4,FALSE);	
		pd3dDevice->LightEnable(5,FALSE);	
		pd3dDevice->LightEnable(6,FALSE);	
		pd3dDevice->LightEnable(7,FALSE);	*/

		pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterFurGrassColor.c);	
		//
		matrix matPos;
		matPos.MakeIdent();
		pd3dDevice->SetTransform(D3DTS_WORLD,matPos);
		for(int ix=0;ix<MAX_GRASSRANGE;ix++)
		{
			for(int iy=0;iy<MAX_GRASSRANGE;iy++)
			{
				m_pGrassNode[ix+iy*MAX_GRASSRANGE].Render(pd3dDevice);
			}
		}
	}
	//*/
}

void CGrassManager::Update(float fUpdate)
{
	//*
	if(CRenderOption::m_GrassRendering)
	{
		for(int i=0;i<20;i++)
		{
			m_fGrassRot[i]+=fUpdate;
		}
		for(int ix=0;ix<MAX_GRASSRANGE;ix++)
		{
			for(int iy=0;iy<MAX_GRASSRANGE;iy++)
			{
				m_pGrassNode[ix+iy*MAX_GRASSRANGE].Update(fUpdate);
			}
		}
	}
	//*/
}

void CGrassManager::RenderShadow(LPDIRECT3DDEVICE8 pd3dDevice,CRenderTexture &pShadowTexture)
{
	if(CRenderOption::m_GrassRendering)
	{
		for(int ix=0;ix<MAX_GRASSRANGE;ix++)
		{
			for(int iy=0;iy<MAX_GRASSRANGE;iy++)
			{
				m_pGrassNode[ix+iy*MAX_GRASSRANGE].RenderShadow(pd3dDevice,pShadowTexture);
			}

		}
	}
}
