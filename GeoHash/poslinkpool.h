#ifndef _POSLINKPOOL_H
#define _POSLINKPOOL_H

#include "common.h"

//add by freeeyes
//���ڴ�����ڹ�������ָ��Ĵ洢

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
