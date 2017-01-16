#ifndef _MAPINFO_H
#define _MAPINFO_H

#include "HashTable.h"
#include "geohash.h"
#include "time.h"

//�й���ͼ��Χ�ڵ��������򣬰���500�׵ķ�Χ�ָ�����빲���ڴ档
//add by freeeyes

#define MAX_GEO_SIZE          10
#define MAX_AREA_OBJECT_COUNT 200

struct _POS_INFO
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

class CPosInfoPool
{
public:
	CPosInfoPool();
	~CPosInfoPool();
	
	size_t Init(int nPoolCount, char* pData);
	size_t Load(int nPoolCount, char* pData);
	void   Close();
	
	char* GetCryptTable();
	
	_POS_INFO* Create();
	int Get_Node_Offset(_POS_INFO* pWordInfo);
	_POS_INFO* Get_NodeOffset_Ptr(int nOffset);
	bool Delete(_POS_INFO* pWordInfo);
	
private:
	char*          m_pBase;           //�����ڴ濪ʼ��ַ
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_POS_INFO*     m_PosInfoList;    //�ڴ���б�			
};

struct _Area_Info
{
	char       m_szHashGeo[MAX_GEO_SIZE];
	int        m_objPosList[MAX_AREA_OBJECT_COUNT];
	
	void Init()
	{
		m_szHashGeo[0] = '\0';
		for(int i = 0; i < MAX_AREA_OBJECT_COUNT; i++)
		{
			//�����¼����ƫ�ƾ���
			m_objPosList[i] = -1;
		}
	}
};

class CMapInfo
{
public:
	CMapInfo();
	~CMapInfo();
	
	void Init(char* pData);
	void Load(char* pData);
	
	bool AddPos(const char* pMsisdn, double dPosLatitude, double dPosLongitude, time_t ttPos);
	
	size_t GetSize(int nMaxCount);
	
private:
	void prepareCryptTable();	
	
private:	
	char*        m_pCryptTable;     //hash�ʵ��㷨��
	CHashTable   m_objHashArea;     //Hash��ͼ 
	_Area_Info*  m_pAreaInfo;       //���������ͼ
	int          m_AreaCount;       //���е�������� 
	CPosInfoPool m_objPosInfoList;  //��ǰ������
};

#endif
