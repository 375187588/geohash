#ifndef _AREAINFOPOOL_H
#define _AREAINFOPOOL_H

//�����ڴ��
//add by freeeyes
#include "stdio.h"

#define MAX_GEO_SIZE          10
#define MAX_AREA_OBJECT_COUNT 200

struct _Area_Info
{
	char       m_szHashGeo[MAX_GEO_SIZE];
	int        m_objPosList[MAX_AREA_OBJECT_COUNT];
	char       m_cUsed;              //�Ƿ�ʹ�ã�0Ϊδ��ʹ�ã�1Ϊ��ʹ�� 
	int        m_nIndex;             //��ǰ��Pool����ID
	
	void Init()
	{
		m_szHashGeo[0] = '\0';
		for(int i = 0; i < MAX_AREA_OBJECT_COUNT; i++)
		{
			//�����¼����ָ��ƫ�ƾ���
			m_objPosList[i] = -1;
		}
	}
	
	void Set_Index(int nIndex)
	{
		m_nIndex = nIndex;
	}
	
	int Get_Index()
	{
		return m_nIndex;
	}	
	
	bool Delete(int nPosOffset)
	{
		for(int i = 0; i < MAX_AREA_OBJECT_COUNT; i++)
		{
			if(m_objPosList[i] == nPosOffset)
			{
				printf("[Delete]nPosOffset=%d OK.\n", nPosOffset);
				m_objPosList[i] = -1;
				break;
			}
		}
		
		return true;
	}
	
	bool Add(int nPosOffset)
	{
		for(int i = 0; i < MAX_AREA_OBJECT_COUNT; i++)
		{
			if(m_objPosList[i] == -1)
			{
				m_objPosList[i] = nPosOffset;
				printf("[Add]nPosOffset=%d OK.\n", nPosOffset);
				return true;
			}
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
	
private:
	char*          m_pBase;           //�����ڴ濪ʼ��ַ
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_Area_Info*    m_AreaInfoList;    //�ڴ���б�			
};


#endif
