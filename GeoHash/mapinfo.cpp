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
	/*
	size_t stSize = 1280 + sizeof(_Area_Info) * m_AreaCount
									+ m_AreaCount * sizeof(_PosLink_Info)
								  + m_AreaCount * sizeof(_Pos_Info)
								  + m_AreaCount * sizeof(_Hash_Table_Cell)* 2;
	*/
	size_t stSize = 1280 + m_objPosInfoList.GetSize(nMaxCount) +
									m_objAreaInfoList.GetSize(nMaxCount) + 
									m_objPosLinkList.GetSize(nMaxCount) + 
									m_AreaCount * sizeof(_Hash_Table_Cell)* 2;
	
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
	nPos += m_objAreaInfoList.GetSize(m_AreaCount);
	
	//��ʼ���û��ڴ��
	m_objPosInfoList.Init(m_AreaCount, (char* )(pData + nPos));
	nPos += m_objPosInfoList.GetSize(m_AreaCount);
	
	//��ʼ����ǰ�������
	m_objPosLinkList.Init(m_AreaCount, (char* )(pData + nPos));	
	nPos += m_objPosLinkList.GetSize(m_AreaCount);		
	
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
	nPos +=  m_objAreaInfoList.GetSize(m_AreaCount);	
	
	//���ض�λ���ڴ��
	m_objPosInfoList.Load(m_AreaCount, (char* )(pData + nPos));
	nPos += m_objPosInfoList.GetSize(m_AreaCount);	
	
	//��ʼ����ǰ�������
	m_objPosLinkList.Load(m_AreaCount, (char* )(pData + nPos));	
	nPos += m_objPosLinkList.GetSize(m_AreaCount);			
	
	//��������Hash��
	m_objHashArea.Load((char* )(pData + nPos), m_AreaCount, m_pCryptTable);
	nPos += m_AreaCount * sizeof(_Hash_Table_Cell);		
	
	//���ض�λ��Hash��
	m_objHashCurrPos.Load((char* )(pData + nPos), m_AreaCount, m_pCryptTable);
	nPos += m_AreaCount * sizeof(_Hash_Table_Cell);			
	
	//��ԭ��������������ӳ���ϵ
	for(int i = 0; i < m_AreaCount; i++)
	{
		_PosLink_Info* pPosLinkInfo = m_objPosLinkList.Get(i);
		if(NULL != pPosLinkInfo && pPosLinkInfo->m_cUsed == 1)
		{
			_Area_Info* pAreaInfo = m_objAreaInfoList.Get(pPosLinkInfo->m_nAreaIndex);
			if(NULL != pAreaInfo)
			{
				//printf("[CMapInfo::Load]pPosLinkInfo->nIndex=%d,m_nAreaIndex=%d.\n", pPosLinkInfo->Get_Index(),pPosLinkInfo->m_nAreaIndex);
				//printf("[CMapInfo::Load]pAreaInfo->m_szHashGeo=%s.\n", pAreaInfo->m_szHashGeo);
				pAreaInfo->Add(pPosLinkInfo, pAreaInfo->m_szHashGeo);
				//printf("[CMapInfo::Load]pAreaInfo->m_pPosList=0x%08x.\n", pAreaInfo->m_pPosList);
			}
		}
	}
}

bool CMapInfo::AddPos(const char* pMsisdn, double dPosLatitude, double dPosLongitude, time_t ttPos)
{
	CGeoHash objGeoHash;
	char pBeforeGeo[10]         = {'\0'};
	char pCurrGeo[10]           = {'\0'};
	_PosLink_Info* pPosLinkInfo = NULL;
	
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
		//printf("[CMapInfo::AddPos]pCurrGeo=%s, pBeforeGeo=%s.\n", pCurrGeo, pBeforeGeo);
		if(strcmp(pCurrGeo, pBeforeGeo) != 0)
		{
			//��ǰ��γ�Ⱥ�֮ǰ�ľ�γ���Ѿ�����һ����������ɾ��֮ǰ����Ĵ��ڵ�
			int nAreaBefore = m_objHashArea.Get_Hash_Box_Data(pBeforeGeo);
			pBeforeAreaInfo = m_objAreaInfoList.Get_NodeOffset_Ptr(nAreaBefore);
			
			//�������д��
			pthread_rwlock_wrlock(&pBeforeAreaInfo->m_rwLock);
			
			pPosLinkInfo = pBeforeAreaInfo->Get(nPosBefore);
			if(NULL != pPosLinkInfo)
			{
				//�������ӽڵ�
				//printf("[CMapInfo::AddPos]Delete pPosLinkInfo->nIndex=%d,nPosBefore=%d,pPosLinkInfo=0x%08x.\n", pPosLinkInfo->Get_Index(), nPosBefore,pPosLinkInfo);
				pBeforeAreaInfo->Delete(pPosLinkInfo);
				if(NULL == pBeforeAreaInfo->m_pPosList)
				{
					//����������Ѿ�û�б�ĳ������������
					m_objAreaInfoList.Delete(pBeforeAreaInfo);
					m_objHashArea.Del_Hash_Data(pBeforeGeo);
				}
				
				m_objPosLinkList.Delete(pPosLinkInfo);
			}
			else
			{
				printf("[CMapInfo::AddPos](%d)before pPosLinkInfo = NULL.\n", nPosBefore);
			}
			
			pBeforePosInfo->m_dPosLatitude  = dPosLatitude;
			pBeforePosInfo->m_dPosLongitude = dPosLongitude;
			pBeforePosInfo->m_ttCurrTime    = ttPos;
			
			//�����������Ƿ��Ѵ���
			int nAreaCurr = m_objHashArea.Get_Hash_Box_Data(pCurrGeo);
			if(nAreaCurr >= 0)
			{
				//����Ѿ�����
				pCurrAreaInfo = m_objAreaInfoList.Get_NodeOffset_Ptr(nAreaCurr);
				pPosLinkInfo = m_objPosLinkList.Create();
				if(NULL != pPosLinkInfo)
				{
					pPosLinkInfo->m_nPosOffset = nPosBefore;
					pPosLinkInfo->m_nAreaIndex = pCurrAreaInfo->m_nIndex;
					pCurrAreaInfo->Add(pPosLinkInfo, pCurrGeo);
					//printf("[CMapInfo::AddPos]pPosLinkInfo->nIndex=%d,nPosBefore=%d,pPosLinkInfo=0x%08x.\n", pPosLinkInfo->Get_Index(), nPosBefore,pPosLinkInfo);
				}
				else
				{
					printf("[CMapInfo::AddPos](%d)new pPosLinkInfo = NULL.\n", nPosBefore);
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
				
				pPosLinkInfo = m_objPosLinkList.Create();
				if(NULL != pPosLinkInfo)
				{
					pPosLinkInfo->m_nPosOffset = nPosBefore;
					pPosLinkInfo->m_nAreaIndex = pCurrAreaInfo->m_nIndex;
					pCurrAreaInfo->Add(pPosLinkInfo, pCurrGeo);
					//printf("[CMapInfo::AddPos]pPosLinkInfo->nIndex=%d,nPosBefore=%d,pPosLinkInfo=0x%08x.\n", pPosLinkInfo->Get_Index(), nPosBefore,pPosLinkInfo);
				}
				else
				{
					printf("[CMapInfo::AddPos](%d)new pPosLinkInfo = NULL.\n", nPosBefore);
				}				
				
				//���������
				if(-1 == m_objHashArea.Add_Hash_Data(pCurrGeo, m_objAreaInfoList.Get_Node_Offset(pCurrAreaInfo)))
				{
					printf("[CMapInfo::AddPos]m_objHashArea is full.\n");
					return false;						
				}			
			}
			
			//�ͷ�д��
			pthread_rwlock_unlock(&pBeforeAreaInfo->m_rwLock);				
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
		//printf("[CMapInfo::AddPos]pCurrGeo=%s.\n", pCurrGeo);
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
			
			//�������д��
			pthread_rwlock_wrlock(&pCurrAreaInfo->m_rwLock);
			
			pPosLinkInfo = m_objPosLinkList.Create();
			if(NULL != pPosLinkInfo)
			{
				pPosLinkInfo->m_nPosOffset = nPosBefore;
				pPosLinkInfo->m_nAreaIndex = pCurrAreaInfo->m_nIndex;
				pCurrAreaInfo->Add(pPosLinkInfo, pCurrGeo);
				//printf("[CMapInfo::AddPos]pPosLinkInfo->nIndex=%d,nPosBefore=%d,pPosLinkInfo=0x%08x.\n", pPosLinkInfo->Get_Index(), nPosBefore,pPosLinkInfo);
			}
			else
			{
				printf("[CMapInfo::AddPos](%d)new pPosLinkInfo = NULL.\n", nPosBefore);
			}
			
			//�ͷ�д��
			pthread_rwlock_unlock(&pCurrAreaInfo->m_rwLock);				
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
			
			//�������д��
			pthread_rwlock_wrlock(&pCurrAreaInfo->m_rwLock);			
			
			pPosLinkInfo = m_objPosLinkList.Create();
			if(NULL != pPosLinkInfo)
			{
				pPosLinkInfo->m_nPosOffset = nPosBefore;
				pPosLinkInfo->m_nAreaIndex = pCurrAreaInfo->m_nIndex;
				pCurrAreaInfo->Add(pPosLinkInfo, pCurrGeo);
				//printf("[CMapInfo::AddPos]pPosLinkInfo->nIndex=%d,nPosBefore=%d,pPosLinkInfo=0x%08x.\n", pPosLinkInfo->Get_Index(), nPosBefore,pPosLinkInfo);
			}
			else
			{
				printf("[CMapInfo::AddPos](%d)new pPosLinkInfo = NULL.\n", nPosBefore);
			}
			
			//���������
			if(-1 == m_objHashArea.Add_Hash_Data(pCurrGeo, m_objAreaInfoList.Get_Node_Offset(pCurrAreaInfo)))
			{
				printf("[CMapInfo::AddPos]m_objHashArea is full.\n");
				return false;						
			}
			
			//�ͷ�д��
			pthread_rwlock_unlock(&pCurrAreaInfo->m_rwLock);						
		}	
	}
	
	return true;
}

bool CMapInfo::DelPos(const char* pMsisdn)
{
	//���ҵ�ǰ�ֻ����Ƿ����
	CGeoHash objGeoHash;
	char pCurrGeo[10]           = {'\0'};
	_Pos_Info* pCurrPosInfo     = NULL;
	
	//����ȥHashmap����ȥ���Ƿ��Ѿ�����
	_Area_Info* pCurrAreaInfo   = NULL;
	
	int nPosCurr = m_objHashCurrPos.Get_Hash_Box_Data(pMsisdn);
	if(nPosCurr < 0)
	{
		//û���ҵ����˳�
		printf("[CMapInfo::DelPos](%s)pos is no find.\n", pMsisdn);
		return false;
	}
	
	//���ҵ�ǰ�ڵ���������
	pCurrPosInfo = m_objPosInfoList.Get_NodeOffset_Ptr(nPosCurr);
	if(NULL == pCurrPosInfo)
	{
		//û���ҵ����˳�
		printf("[CMapInfo::DelPos](%s)pool pos is no find.\n", pMsisdn);
		return false;		
	}
	
	sprintf(pCurrGeo, "%s", objGeoHash.Encode(pCurrPosInfo->m_dPosLatitude, pCurrPosInfo->m_dPosLongitude, GEO_PERSITION));	
	
	int nAreaCurr = m_objHashArea.Get_Hash_Box_Data(pCurrGeo);
	if(nAreaCurr < 0)
	{
		m_objHashCurrPos.Del_Hash_Data(pMsisdn);
		m_objPosInfoList.Delete(pCurrPosInfo);
		printf("[CMapInfo::DelPos](%s)area is no find.\n", pMsisdn);
		return false;		
	}
	
	pCurrAreaInfo = m_objAreaInfoList.Get_NodeOffset_Ptr(nAreaCurr);	
	if(pCurrAreaInfo == NULL)
	{
		//û���ҵ����˳�
		printf("[CMapInfo::DelPos](%s)pool area is no find.\n", pMsisdn);
		return false;
	}
	
	//ɾ�������еĳ�����Ϣ
	//��д��
	pthread_rwlock_wrlock(&pCurrAreaInfo->m_rwLock);
	
	_PosLink_Info* pAreaPosLinkInfo = pCurrAreaInfo->Get(nPosCurr);
	if(NULL != pAreaPosLinkInfo)
	{
		//�������ӽڵ�
		//printf("[CMapInfo::AddPos]Delete pPosLinkInfo->nIndex=%d,nPosBefore=%d,pPosLinkInfo=0x%08x.\n", pPosLinkInfo->Get_Index(), nPosBefore,pPosLinkInfo);
		pCurrAreaInfo->Delete(pAreaPosLinkInfo);
		if(NULL == pCurrAreaInfo->m_pPosList)
		{
			//����������Ѿ�û�б�ĳ������������
			m_objAreaInfoList.Delete(pCurrAreaInfo);
			m_objHashArea.Del_Hash_Data(pCurrGeo);
		}
		
		m_objPosLinkList.Delete(pAreaPosLinkInfo);	
	}	
	
	//���յ�ǰ�����
	m_objHashCurrPos.Del_Hash_Data(pMsisdn);
	m_objPosInfoList.Delete(pCurrPosInfo);	
	
	//�ͷ�д��
	pthread_rwlock_unlock(&pCurrAreaInfo->m_rwLock);		
	
	return true;
}

void CMapInfo::GetNeighbors(double dPosLatitude, double dPosLongitude, double dDistance, vector<string>& objNeighborsList)
{
	CGeoHash objGeoHash;
	//��γ��Ϊ12���ȵ�ʱ��ı߳�
	double dRowSetp = 0.043945f;
	double dColSetp = 0.043945f;
	
	objNeighborsList.clear();
	
	//��õ�ǰ���εķ�Χ
	_Geo_Rect obj_Geo_Rect = objGeoHash.GetBoundingBox(dPosLatitude, dPosLongitude, dDistance);
	//�õ����εĳ��Ϳ�
	double dRowSize = obj_Geo_Rect.m_dMaxLongitude - obj_Geo_Rect.m_dMinLongitude;
													    						 
	double dColSize = obj_Geo_Rect.m_dMaxLatitude - obj_Geo_Rect.m_dMinLatitude;	
	
	//printf("[CMapInfo::GetNeighbors]nPersition=%d,dRowSize=%f,dColSize=%f.\n", nPersition, dRowSize, dColSize);
																				 
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
	
	//printf("[CMapInfo::GetNeighbors]objNeighborsList size=%d.\n", objNeighborsList.size());												    						 
}

bool CMapInfo::FindPos(double dPosLatitude, double dPosLongitude, double dDistance, vector<_Pos_Info*>& vecPosList)
{
	CGeoHash objGeoHash;
	_Area_Info* pCurrArea = NULL;
	
	vecPosList.clear();
	
	vector<string> objNeighborsList;
	GetNeighbors(dPosLatitude, dPosLongitude, dDistance, objNeighborsList);
	
	//����������ָ����Χ�ڴ��ڵ�����
	for(int i = 0; i < (int)objNeighborsList.size(); i++)
	{
		printf("[CMapInfo::FindPos](%d)m_szNerghbors=%s.\n", i, objNeighborsList[i].c_str());
		int nAreaCurr = m_objHashArea.Get_Hash_Box_Data(objNeighborsList[i].c_str());
		if(nAreaCurr != -1)
		{
			//printf("[CMapInfo::FindPos]Find m_szNerghbors=%s,nAreaCurr=%d.\n", objNeighborsList[i].c_str(), nAreaCurr);
			pCurrArea = m_objAreaInfoList.Get_NodeOffset_Ptr(nAreaCurr);
			if(NULL != pCurrArea)
			{
				//printf("[CMapInfo::FindPos]pCurrArea->m_rwLock=0x%08x.\n", &pCurrArea->m_rwLock);
				//��Ӷ���
				pthread_rwlock_rdlock(&pCurrArea->m_rwLock);
				
				_PosLink_Info* pTail = pCurrArea->m_pPosList;
				//printf("[CMapInfo::FindPos]pCurrArea pTail=0x%08x.\n", pTail);
				
				while(pTail != NULL)
				{
					//ȡ����ǰ��
					_Pos_Info* pPosInfo = m_objPosInfoList.Get_NodeOffset_Ptr(pTail->m_nPosOffset);
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
					
					pTail = pTail->m_pNext;
				}
				
				//�ͷŶ���
				pthread_rwlock_unlock(&pCurrArea->m_rwLock);					
			}
		}
	}
	
	return true;
}

string CMapInfo::GetPoolState()
{
	string strJson;
	char szTemp[1024] = {'\0'};

	strJson = "{";
	_Pool_Info obj_Pos_Pool = m_objPosInfoList.Get_Pool_State();
	sprintf(szTemp, "\"PoolName\":\"Pos_Pool\",\"Used\":\"%d\",\"Free\":\"%d\",\"All\":\"%d\",", obj_Pos_Pool.m_nUsedCount, 
																																							                 obj_Pos_Pool.m_nPoolCount - obj_Pos_Pool.m_nUsedCount,
																																							                 obj_Pos_Pool.m_nPoolCount);
	strJson += (string)szTemp;																														                	
	_Pool_Info obj_Area_Pool = m_objAreaInfoList.Get_Pool_State();
	sprintf(szTemp, "\"PoolName\":\"Area_Pool\",\"Used\":\"%d\",\"Free\":\"%d\",\"All\":\"%d\",", obj_Area_Pool.m_nUsedCount, 
																																							                  obj_Area_Pool.m_nPoolCount - obj_Area_Pool.m_nUsedCount,
																																							                  obj_Area_Pool.m_nPoolCount);
	strJson += (string)szTemp;																														                	
	_Pool_Info obj_PosLink_Pool = m_objPosLinkList.Get_Pool_State();
	sprintf(szTemp, "\"PoolName\":\"PosLink_Pool\",\"Used\":\"%d\",\"Free\":\"%d\",\"All\":\"%d\"", obj_PosLink_Pool.m_nUsedCount, 
																																							                    obj_PosLink_Pool.m_nPoolCount - obj_PosLink_Pool.m_nUsedCount,
																																							                    obj_PosLink_Pool.m_nPoolCount);																																		                 
	strJson += (string)szTemp;	
	strJson += "}";
	
	return strJson;
}
