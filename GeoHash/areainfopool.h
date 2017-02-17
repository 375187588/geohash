#ifndef _AREAINFOPOOL_H
#define _AREAINFOPOOL_H

//�����ڴ��
//add by freeeyes
#include "common.h"

class CAreaInfoPool
{
public:
	CAreaInfoPool();
	~CAreaInfoPool();
	
	size_t Init(int nPoolCount, char* pData);
	size_t Load(int nPoolCount, char* pData);
	void   Close();
	
	size_t GetSize(int nCount);
	
	_Area_Info* Create();
	int Get_Node_Offset(_Area_Info* pWordInfo);
	_Area_Info* Get_NodeOffset_Ptr(int nOffset);
	bool Delete(_Area_Info* pWordInfo);
	
	_Area_Info* Get(int nIndex);
	
	_Pool_Info Get_Pool_State();
	
private:
	char*          m_pBase;           //�����ڴ濪ʼ��ַ
	int*           m_pUsedCount;      //��ǰ�ڴ���Ѿ�ʹ�ö������ 
	int*           m_pPoolCount;      //�ڴ�ص�ǰ����
	int*           m_pCurrIndex;      //��ǰ�ڵ�λ��
	_Area_Info*    m_AreaInfoList;    //�ڴ���б�			
};


#endif
