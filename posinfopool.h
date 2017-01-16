#ifndef _POSINFOPOOL_H
#define _POSINFOPOOL_H

//��λ���ڴ��
//add by freeeyes

#include "stdio.h"
#include "time.h"

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

class CPosInfoPool
{
public:
	CPosInfoPool();
	~CPosInfoPool();
	
	size_t Init(int nPoolCount, char* pData);
	size_t Load(int nPoolCount, char* pData);
	void   Close();
	
	_Pos_Info* Create();
	int Get_Node_Offset(_Pos_Info* pWordInfo);
	_Pos_Info* Get_NodeOffset_Ptr(int nOffset);
	bool Delete(_Pos_Info* pWordInfo);
	
private:
	char*          m_pBase;           //�����ڴ濪ʼ��ַ
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_Pos_Info*     m_PosInfoList;    //�ڴ���б�			
};

#endif
