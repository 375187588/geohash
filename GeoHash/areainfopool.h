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
