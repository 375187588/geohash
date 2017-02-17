#ifndef GEO_COMMON_H
#define GEO_COMMON_H

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "stdio.h"
#include "time.h"

//������Ҫ�Ľṹ�嶼�����ﶨ��
//add by freeeyes

//pos����
struct _Pos_Info
{
	char   m_szMsisdn[15];       //SIM����
	double m_dPosLatitude;       //γ��
	double m_dPosLongitude;      //����
	time_t m_ttCurrTime;         //��ǰ��ʱ��
	char   m_cUsed;              //�Ƿ�ʹ�ã�0Ϊδ��ʹ�ã�1Ϊ��ʹ�� 
	int    m_nIndex;             //��ǰ��Pool����ID
	
	void Init()
	{
		m_dPosLatitude   = 0.0f;
		m_dPosLongitude  = 0.0f;
		m_cUsed          = 0;
		m_nIndex         = 0;
	}
	
	void Set_Index(int nIndex)
	{
		m_nIndex = nIndex;
	}
	
	int Get_Index()
	{
		return m_nIndex;
	}
};

//�������
struct _PosLink_Info
{
	int  m_nPosOffset;              //Pos�����ָ��ƫ��ֵ
	int  m_nIndex;                  //��ǰ������
	int  m_nAreaIndex;              //��ǰ������
	char m_cUsed;                   //�Ƿ�ʹ�ã�0Ϊδ��ʹ�ã�1Ϊ��ʹ�� 
	_PosLink_Info* m_pNext;         //Next
	
	
	_PosLink_Info()
	{
		Init();
	}
	
	void Init()
	{
		m_nPosOffset   = 0;
		m_nIndex       = 0;
		m_nAreaIndex   = 0;
		m_cUsed        = 0;
		m_pNext        = NULL;		
	}
	
	void Clear()
	{
		m_nPosOffset   = 0;
		m_nAreaIndex   = 0;
		m_cUsed        = 0;
		m_pNext        = NULL;			
	}
	
	void Set_Index(int nIndex)
	{
		m_nIndex = nIndex;
	}
	
	int Get_Index()
	{
		return m_nIndex;
	}		
};


//area����
#define MAX_GEO_SIZE          10

struct _Area_Info
{
	pthread_rwlock_t     m_rwLock;                     //�����д��
	char                 m_szHashGeo[MAX_GEO_SIZE];    //geo�ַ���
	_PosLink_Info*       m_pPosList;                   //Pos������Ϣ
	char                 m_cUsed;                      //�Ƿ�ʹ�ã�0Ϊδ��ʹ�ã�1Ϊ��ʹ�� 
	int                  m_nIndex;                     //��ǰ��Pool����ID
	
	void Init()
	{
		m_szHashGeo[0] = '\0';
		m_pPosList     = NULL;
		
		pthread_rwlockattr_t rwLockAttr;                 
		//��ʼ����д��
		if(0 != pthread_rwlockattr_init(&rwLockAttr))
		{
			printf("[_Area_Info::Init]init rwlockattr attribute is failed ! erro reason is %s!\n", strerror(errno));
		}
		
    if(pthread_rwlockattr_setpshared(&rwLockAttr, PTHREAD_PROCESS_SHARED)!=0)
    {
        printf("[_Area_Info::Init]set rw lockattrite value is failed ! error reason is %s!\n",strerror(errno));
    }
    if(pthread_rwlock_init(&m_rwLock,&rwLockAttr)!=0)
    {
        printf("[_Area_Info::Init]init rw lock  attribute is failed !error reason is %s\n",strerror(errno));
    }	
	}
	
	void Load()
	{
		m_pPosList = NULL;
		//ϵͳ�������ͷ�һ��д������ֹϵͳ������
		//pthread_rwlock_unlock(&m_rwLock);
		pthread_rwlock_destroy(&m_rwLock);
		
		pthread_rwlockattr_t rwLockAttr;                 
		//��ʼ����д��
		if(0 != pthread_rwlockattr_init(&rwLockAttr))
		{
			printf("[_Area_Info::Init]init rwlockattr attribute is failed ! erro reason is %s!\n", strerror(errno));
		}
		
    if(pthread_rwlockattr_setpshared(&rwLockAttr, PTHREAD_PROCESS_SHARED)!=0)
    {
        printf("[_Area_Info::Init]set rw lockattrite value is failed ! error reason is %s!\n",strerror(errno));
    }
    if(pthread_rwlock_init(&m_rwLock,&rwLockAttr)!=0)
    {
        printf("[_Area_Info::Init]init rw lock  attribute is failed !error reason is %s\n",strerror(errno));
    }			
	}
	
	void Clear()
	{
		m_szHashGeo[0] = '\0';
		m_cUsed        = 0;
		
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
				//printf("[Get]pTail->m_nPosOffset=%d.\n", pTail->m_nPosOffset);
				if(pTail->m_nPosOffset == nPosOffset)
				{
					return pTail;
				}
				
				pTail = pTail->m_pNext;
			}
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
					//printf("[Delete]m_pPosList=0x%08x.\n", m_pPosList);
					//printf("[Delete]pPosInfo=0x%08x.\n", pPosInfo);
					//printf("[Delete]pBefore=0x%08x.\n", pBefore);
					//printf("[Delete]pTail=0x%08x.\n", pTail);
					//printf("[_Area_Info::Delete]pTail=0x%08x.\n", pTail);
					if(NULL == pBefore)
					{
						//�ǵ�һ��
						m_pPosList = m_pPosList->m_pNext;
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
	
	bool Add(_PosLink_Info* pPosInfo, char* pHashGeo)
	{
		sprintf(m_szHashGeo, "%s", pHashGeo);
		if(m_pPosList == NULL)
		{
			//���������ĵ�һ��
			m_pPosList = pPosInfo;
		}
		else
		{
			_PosLink_Info* pTail = m_pPosList;
			while(NULL != pTail->m_pNext)
			{
				pTail = pTail->m_pNext;
			}
			
			pTail->m_pNext = pPosInfo;
		}
		
		return false;		
	}
};


#endif
