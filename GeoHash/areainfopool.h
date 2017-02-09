#ifndef _AREAINFOPOOL_H
#define _AREAINFOPOOL_H

//�����ڴ��
//add by freeeyes
#include "stdio.h"
#include "poslinkpool.h"

#define MAX_GEO_SIZE          10
#define MAX_AREA_OBJECT_COUNT 200

struct _Area_Info
{
	char           m_szHashGeo[MAX_GEO_SIZE];
	_PosLink_Info* m_pPosList;           //Pos������Ϣ
	char           m_cUsed;              //�Ƿ�ʹ�ã�0Ϊδ��ʹ�ã�1Ϊ��ʹ�� 
	int            m_nIndex;             //��ǰ��Pool����ID
	
	void Init()
	{
		m_szHashGeo[0] = '\0';
		m_pPosList     = NULL;
	}
	
	void Set_Index(int nIndex)
	{
		m_nIndex = nIndex;
	}
	
	int Get_Index()
	{
		return m_nIndex;
	}
	
	_PosLink_Info* Get(int nPosOffset)
	{
		if(m_pPosList == NULL)
		{
			//���������ĵ�һ��
			return NULL;
		}
		else
		{
			_PosLink_Info* pTail = m_pPosList;
			while(pTail != NULL)
			{
				pTail->m_nPosOffset == nPosOffset;
				return pTail;
			}
			
			pTail = pTail->m_pNext;
		}
		
		return NULL;	
	}
	
	bool Delete(_PosLink_Info* pPosInfo)
	{
		if(m_pPosList == NULL)
		{
			return false;
		}
		else
		{
			_PosLink_Info* pBefore = NULL;
			_PosLink_Info* pTail   = m_pPosList;
			
			//���������ҵ���ȵĶ���
			while(pTail != NULL)
			{
				if(pTail == pPosInfo)
				{
					if(NULL == pBefore)
					{
						//�ǵ�һ��
						m_pPosList = NULL;
						return true;
					}
					else
					{
						//���м��һ��
						pBefore->m_pNext = pTail->m_pNext;
						return true;
					}
				}
				
				pBefore = pTail;
				pTail   = pTail->m_pNext;
			}
			
		}
		
		return false;
	}
	
	bool Add(_PosLink_Info* pPosInfo)
	{
		if(m_pPosList == NULL)
		{
			//���������ĵ�һ��
			m_pPosList = pPosInfo;
		}
		else
		{
			_PosLink_Info* pTail = m_pPosList;
			while(pTail->m_pNext != NULL)
			{
				pTail = pTail->m_pNext;
			}
			
			pTail->m_pNext = pPosInfo;
		}
		
		return false;		
	}
};

class CAreaInfoPool
{
public:
	CAreaInfoPool();
	~CAreaInfoPool();
	
	size_t Init(int nPoolCount, char* pData);
	size_t Load(int nPoolCount, char* pData);
	void   Close();
	
	_Area_Info* Create();
	int Get_Node_Offset(_Area_Info* pWordInfo);
	_Area_Info* Get_NodeOffset_Ptr(int nOffset);
	bool Delete(_Area_Info* pWordInfo);
	
	_Area_Info* Get(int nIndex);
	
private:
	char*          m_pBase;           //�����ڴ濪ʼ��ַ
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_Area_Info*    m_AreaInfoList;    //�ڴ���б�			
};


#endif
