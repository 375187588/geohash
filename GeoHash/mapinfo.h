#ifndef _MAPINFO_H
#define _MAPINFO_H

#include "imapinfo.h"
#include "HashTable.h"
#include "geohash.h"
#include "poslinkpool.h"
#include "areainfopool.h"

//�й���ͼ��Χ�ڵ��������򣬰���500�׵ķ�Χ�ָ�����빲���ڴ档
//add by freeeyes

#define GEO_PERSITION 12   

class CMapInfo : public IMapInfo
{
public:
	CMapInfo();
	~CMapInfo();
	
	void Init(char* pData);
	void Load(char* pData);
	
	bool AddPos(const char* pMsisdn, double dPosLatitude, double dPosLongitude, time_t ttPos);
	
	bool FindPos(double dPosLatitude, double dPosLongitude, double dDistance, vector<_Pos_Info*>& vecPosList);
	
	bool DelPos(const char* pMsisdn);
	
	size_t GetSize(int nMaxCount);
	
private:
	void prepareCryptTable();	
	
	void GetNeighbors(double dPosLatitude, double dPosLongitude, double dDistance, vector<string>& objNeighborsList);
	
private:	
	char*         m_pCryptTable;     //hash�ʵ��㷨��
	CHashTable    m_objHashArea;     //Hash�����ͼ 
	CHashTable    m_objHashCurrPos;  //Hash�����ͼ
	int           m_AreaCount;       //���е�������� 
	CPosInfoPool  m_objPosInfoList;  //��ǰ������
	CAreaInfoPool m_objAreaInfoList; //��ǰ�������� 
	CPosLinkPool  m_objPosLinkList;  //��ǰ���������� 
};

#endif
