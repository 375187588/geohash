#ifndef _POSLINKPOOL_H
#define _POSLINKPOOL_H

#include "stdio.h"

//add by freeeyes
//���ڴ�����ڹ�������ָ��Ĵ洢

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

class CPosLinkPool
{
public:
	CPosLinkPool();
	~CPosLinkPool();
	
	void Close();
	
	size_t Init(int nPoolCount, char* pData);
	size_t Load(int nPoolCount, char* pData);	
	
	_PosLink_Info* Create();
	bool Delete(_PosLink_Info* pPosLinkInfo);
	
	_PosLink_Info* Get(int nIndex);
	
private:
	char*          m_pBase;          //�����ڴ濪ʼ��ַ
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_PosLink_Info* m_PosLinkList;    //�ڴ���б�			
};

#endif
