#include "mapinfo.h"

CMapInfo::CMapInfo()
{
	m_AreaCount   = 0;
	m_pCryptTable = NULL;
}

CMapInfo::~CMapInfo()
{
}

//hash�㷨��Ӧ�ļ����ַ����ʵ�
void CMapInfo::prepareCryptTable()
{
  unsigned long seed = 0x00100001, index1 = 0, index2 = 0, i;
  for(index1 = 0; index1 < 0x100; index1++)
  { 
    for(index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
    { 
      unsigned long temp1, temp2;
      seed = (seed * 125 + 3) % 0x2AAAAB;
      temp1 = (seed & 0xFFFF) << 0x10;
      seed = (seed * 125 + 3) % 0x2AAAAB;
      temp2 = (seed & 0xFFFF);
      if(index2 > 1280)
      {
      	printf("[prepareCryptTable]index2=%u.\n", (unsigned int)index2);
      }
      m_pCryptTable[index2] = (char)(temp1 | temp2); 
    } 
  } 		
}

size_t CMapInfo::GetSize(int nMaxCount)
{
	m_AreaCount = nMaxCount;
	
	//printf("[CMapInfo::GetSize]nCount=%d.\n", nMaxCount);
	size_t stSize = 1280 + sizeof(_Area_Info) * m_AreaCount
								  + m_AreaCount * sizeof(_Pos_Info)
								  + m_AreaCount * sizeof(_Hash_Table_Cell)* 2;
	return stSize;
}

void CMapInfo::Init(char* pData)
{
	size_t nPos = 0;
	m_pCryptTable = pData;
	memset(m_pCryptTable, 0, 1280);
	prepareCryptTable();
	nPos += 1280;	
	
	//��ʼ����ͼ
	m_objAreaInfoList.Init(m_AreaCount, (char* )(pData + nPos));
	nPos += m_AreaCount * sizeof(_Area_Info);
	
	//��ʼ���û��ڴ��
	m_objPosInfoList.Init(m_AreaCount, (char* )(pData + nPos));
	nPos += m_AreaCount * sizeof(_Pos_Info);
	
	//��ʼ������Hash��
	m_objHashArea.Init((char* )(pData + nPos), m_AreaCount, m_pCryptTable);
	nPos += m_AreaCount * sizeof(_Hash_Table_Cell);	
	
	//��ʼ����ǰ��Hash��
	m_objHashCurrPos.Init((char* )(pData + nPos), m_AreaCount, m_pCryptTable);	
	nPos += m_AreaCount * sizeof(_Hash_Table_Cell);	
	
	printf("[CMapInfo::Init]nPos=%d.\n", nPos);
}

void CMapInfo::Load(char* pData)
{
	size_t nPos = 0;
	m_pCryptTable = pData;
	nPos += 1280;		
	
	//���������ڴ��
	m_objAreaInfoList.Load(m_AreaCount, (char* )(pData + nPos));
	nPos += m_AreaCount * sizeof(_Area_Info);	
	
	//���ض�λ���ڴ��
	m_objPosInfoList.Load(m_AreaCount, (char* )(pData + nPos));
	nPos += m_AreaCount * sizeof(_Pos_Info);	
	
	//��������Hash��
	m_objHashArea.Load((char* )(pData + nPos), m_AreaCount, m_pCryptTable);
	nPos += m_AreaCount * sizeof(_Hash_Table_Cell);		
	
	//���ض�λ��Hash��
	m_objHashCurrPos.Load((char* )(pData + nPos), m_AreaCount, m_pCryptTable);
	nPos += m_AreaCount * sizeof(_Hash_Table_Cell);			
}

bool CMapInfo::AddPos(const char* pMsisdn, double dPosLatitude, double dPosLongitude, time_t ttPos)
{
	CGeoHash objGeoHash;
	char pBeforeGeo[10] = {'\0'};
	char pCurrGeo[10]   = {'\0'};
	
	//����ȥHashmap����ȥ���Ƿ��Ѿ�����
	_Pos_Info*  pBeforePosInfo  = NULL;
	_Area_Info* pBeforeAreaInfo = NULL;
	_Area_Info* pCurrAreaInfo   = NULL;
	
	int nPosBefore  = m_objHashCurrPos.Get_Hash_Box_Data(pMsisdn);
	if(nPosBefore >= 0)
	{
		pBeforePosInfo = m_objPosInfoList.Get_NodeOffset_Ptr(nPosBefore);
		sprintf(pBeforeGeo, "%s", objGeoHash.Encode(pBeforePosInfo->m_dPosLatitude, pBeforePosInfo->m_dPosLongitude, GEO_PERSITION));
	}
	
	sprintf(pCurrGeo, "%s", objGeoHash.Encode(dPosLatitude, dPosLongitude, GEO_PERSITION));
	
	if(pBeforePosInfo != NULL)
	{
		//���֮ǰ���Ѵ��ھ�γ�ȣ��Ƚ�֮ǰ�����ڵľ�γ���Ƿ���һ������
		printf("[CMapInfo::AddPos]pCurrGeo=%s, pBeforeGeo=%s.\n", pCurrGeo, pBeforeGeo);
		if(strcmp(pCurrGeo, pBeforeGeo) != 0)
		{
			//��ǰ��γ�Ⱥ�֮ǰ�ľ�γ���Ѿ�����һ����������ɾ��֮ǰ����Ĵ��ڵ�
			int nAreaBefore = m_objHashArea.Get_Hash_Box_Data(pBeforeGeo);
			pBeforeAreaInfo = m_objAreaInfoList.Get_NodeOffset_Ptr(nAreaBefore);
			pBeforeAreaInfo->Delete(nPosBefore);
			
			pBeforePosInfo->m_dPosLatitude  = dPosLatitude;
			pBeforePosInfo->m_dPosLongitude = dPosLongitude;
			pBeforePosInfo->m_ttCurrTime    = ttPos;
			
			//�����������Ƿ��Ѵ���
			int nAreaCurr = m_objHashArea.Get_Hash_Box_Data(pCurrGeo);
			if(nAreaCurr >= 0)
			{
				//����Ѿ�����
				pCurrAreaInfo = m_objAreaInfoList.Get_NodeOffset_Ptr(nAreaCurr);
				bool blState = pCurrAreaInfo->Add(nPosBefore);
				if(false == blState)
				{
					printf("[CMapInfo::AddPos]pCurrAreaInfo is full.\n");
					return false;				
				}
			}
			else
			{
				//���������
				pCurrAreaInfo = m_objAreaInfoList.Create();
				if(NULL == pCurrAreaInfo)
				{
					printf("[CMapInfo::AddPos]pCurrAreaInfo is full.\n");
					return false;							
				}
				
				bool blState = pCurrAreaInfo->Add(nPosBefore);
				if(false == blState)
				{
					printf("[CMapInfo::AddPos]pCurrAreaInfo is full.\n");
					return false;				
				}
				
				//���������
				if(-1 == m_objHashArea.Add_Hash_Data(pCurrGeo, m_objAreaInfoList.Get_Node_Offset(pCurrAreaInfo)))
				{
					printf("[CMapInfo::AddPos]m_objHashArea is full.\n");
					return false;						
				}			
			}	
		}
		else
		{
			printf("[CMapInfo::AddPos]Area is the same.\n");
			
			//��ͬһ�����ڣ���ֻ���µ�ǰ����Ϣ
			pBeforePosInfo->m_dPosLatitude  = dPosLatitude;
			pBeforePosInfo->m_dPosLongitude = dPosLongitude;
			pBeforePosInfo->m_ttCurrTime    = ttPos;		
		}		
	}
	else
	{
		printf("[CMapInfo::AddPos]pCurrGeo=%s.\n", pCurrGeo);
		//֮ǰ�㲢������
		pBeforePosInfo = m_objPosInfoList.Create();
		if(NULL == pBeforePosInfo)
		{
			printf("[CMapInfo::AddPos]m_objPosInfoList is empty.\n");
			return false;
		}
		
		sprintf(pBeforePosInfo->m_szMsisdn, "%s", pMsisdn);
		pBeforePosInfo->m_dPosLatitude  = dPosLatitude;
		pBeforePosInfo->m_dPosLongitude = dPosLongitude;
		pBeforePosInfo->m_ttCurrTime    = ttPos;	
		
		nPosBefore = m_objPosInfoList.Get_Node_Offset(pBeforePosInfo);
		if(-1 == m_objHashCurrPos.Add_Hash_Data(pMsisdn, nPosBefore))
		{
				printf("[CMapInfo::AddPos]m_objHashCurrPos is full.\n");
				return false;				
		}
		
		//�����������Ƿ��Ѵ���
		int nAreaCurr = m_objHashArea.Get_Hash_Box_Data(pCurrGeo);
		if(nAreaCurr >= 0)
		{
			//����Ѿ�����
			pCurrAreaInfo = m_objAreaInfoList.Get_NodeOffset_Ptr(nAreaCurr);
			bool blState = pCurrAreaInfo->Add(nPosBefore);
			if(false == blState)
			{
				printf("[CMapInfo::AddPos]pCurrAreaInfo is full.\n");
				return false;				
			}
		}
		else
		{
			//���������
			pCurrAreaInfo = m_objAreaInfoList.Create();
			if(NULL == pCurrAreaInfo)
			{
				printf("[CMapInfo::AddPos]pCurrAreaInfo is full.\n");
				return false;							
			}
			
			bool blState = pCurrAreaInfo->Add(nPosBefore);
			if(false == blState)
			{
				printf("[CMapInfo::AddPos]pCurrAreaInfo is full.\n");
				return false;				
			}
			
			//���������
			if(-1 == m_objHashArea.Add_Hash_Data(pCurrGeo, m_objAreaInfoList.Get_Node_Offset(pCurrAreaInfo)))
			{
				printf("[CMapInfo::AddPos]m_objHashArea is full.\n");
				return false;						
			}
		}	
	}
	
	return true;
}

void CMapInfo::GetNeighbors(double dPosLatitude, double dPosLongitude, double dDistance, vector<string>& objNeighborsList)
{
	CGeoHash objGeoHash;
	//��γ��Ϊ12���ȵ�ʱ��ı߳�
	double dRowSetp = 0.043945f;
	double dColSetp = 0.043945f;
	
	objNeighborsList.clear();
	
	//���Ȼ��ָ��������ʲô��Χ��
	int nPersition    = GEO_PERSITION;
	if(dDistance >= 626172.419555f)
	{
		nPersition    = 5;
	}
	else if(dDistance >= 121008.232567f)
	{
		nPersition    = 7;	
	}
	else if(dDistance >= 19567.888111f)
	{
		nPersition    = 10;			
	}
	else
	{
		nPersition    = GEO_PERSITION;		
	}	
	
	if(nPersition != GEO_PERSITION)
	{
		//��õ�ǰ���εķ�Χ
		//_Geo_Rect obj_Geo_Rect = objGeoHash.GetGeoRect(dPosLatitude, dPosLongitude, nPersition);
		_Geo_Rect obj_Geo_Rect = objGeoHash.GetBoundingBox(dPosLatitude, dPosLongitude, dDistance);
		//�õ����εĳ��Ϳ�
		double dRowSize = obj_Geo_Rect.m_dMaxLongitude - obj_Geo_Rect.m_dMinLongitude;
														    						 
		double dColSize = obj_Geo_Rect.m_dMaxLatitude - obj_Geo_Rect.m_dMinLatitude;	
		
		printf("[CMapInfo::GetNeighbors]nPersition=%d,dRowSize=%f,dColSize=%f.\n", nPersition, dRowSize, dColSize);
																					 
		//���ݳ����õ�ǰ���а�������Ϊ12������
		for(double dRow = obj_Geo_Rect.m_dMinLongitude; dRow < obj_Geo_Rect.m_dMinLongitude + dRowSize; dRow = dRow + dRowSetp)
		{
			for(double dCol = obj_Geo_Rect.m_dMinLatitude; dCol < obj_Geo_Rect.m_dMinLatitude + dColSize; dCol = dCol + dColSetp)
			{
				//printf("[CMapInfo::GetNeighbors]dCol=%f,dRow=%f.\n", dCol, dRow);
				string strArea = (string)objGeoHash.Encode(dCol, dRow, GEO_PERSITION);
				//printf("[CMapInfo::GetNeighbors]strArea=%s.\n", strArea.c_str());
				objNeighborsList.push_back(strArea);
			}
		}	
		
		printf("[CMapInfo::GetNeighbors]objNeighborsList size=%d.\n", objNeighborsList.size());											    						 
	}
	else
	{
		_Geo_Neighbors objNeighbors = objGeoHash.GetNeighbors(dPosLatitude, dPosLongitude, GEO_PERSITION);
		objNeighborsList.push_back((string)objNeighbors.m_szNerghbors[0]);
		objNeighborsList.push_back((string)objNeighbors.m_szNerghbors[1]);
		objNeighborsList.push_back((string)objNeighbors.m_szNerghbors[2]);
		objNeighborsList.push_back((string)objNeighbors.m_szNerghbors[3]);
		objNeighborsList.push_back((string)objNeighbors.m_szNerghbors[4]);
		objNeighborsList.push_back((string)objNeighbors.m_szNerghbors[5]);
		objNeighborsList.push_back((string)objNeighbors.m_szNerghbors[6]);
		objNeighborsList.push_back((string)objNeighbors.m_szNerghbors[7]);
		objNeighborsList.push_back((string)objNeighbors.m_szNerghbors[8]);
		printf("[CMapInfo::GetNeighbors]objNeighborsList size=%d.\n", objNeighborsList.size());	
	}
}

bool CMapInfo::FindPos(double dPosLatitude, double dPosLongitude, double dDistance, vector<_Pos_Info*>& vecPosList)
{
	CGeoHash objGeoHash;
	_Area_Info* pCurrArea = NULL;
	
	vecPosList.clear();
	
	vector<string> objNeighborsList;
	GetNeighbors(dPosLatitude, dPosLongitude, dDistance, objNeighborsList);
	
	//������������ľŹ���
	for(int i = 0; i < (int)objNeighborsList.size(); i++)
	{
		//printf("[CMapInfo::FindPos](%d)m_szNerghbors=%s.\n", i, objNeighbors.m_szNerghbors[i]);
		int nAreaCurr = m_objHashArea.Get_Hash_Box_Data(objNeighborsList[i].c_str());
		if(nAreaCurr != -1)
		{
			//printf("[CMapInfo::FindPos]Find m_szNerghbors=%s.\n", objNeighbors.m_szNerghbors[i]);
			pCurrArea = m_objAreaInfoList.Get_NodeOffset_Ptr(nAreaCurr);
			if(NULL != pCurrArea)
			{
				for(int j = 0; j < MAX_AREA_OBJECT_COUNT; j++)
				{
					if(pCurrArea->m_objPosList[j] >= 0)
					{
						//ȡ����ǰ��
						_Pos_Info* pPosInfo = m_objPosInfoList.Get_NodeOffset_Ptr(pCurrArea->m_objPosList[j]);
						if(NULL != pPosInfo)
						{
							//���������������ĵ�
							double dCurrDistance = objGeoHash.GetDistance(dPosLatitude, dPosLongitude, 
																												    pPosInfo->m_dPosLatitude, pPosInfo->m_dPosLongitude);
							//printf("[CMapInfo::FindPos](%s)dCurrDistance=%f.\n", pPosInfo->m_szMsisdn, dCurrDistance);
							if(dDistance >= dCurrDistance)
							{
								vecPosList.push_back(pPosInfo);
							}
						}
					} 
				}
			}
		}
	}
	
	return true;
}
