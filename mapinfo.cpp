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
	CGeoHash m_GeoHash;
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
		sprintf(pBeforeGeo, "%s", m_GeoHash.Encode(pBeforePosInfo->m_dPosLatitude, pBeforePosInfo->m_dPosLongitude, GEO_PERSITION));
	}
	
	sprintf(pCurrGeo, "%s", m_GeoHash.Encode(dPosLatitude, dPosLongitude, GEO_PERSITION));
	
	if(pBeforePosInfo != NULL)
	{
		//���֮ǰ���Ѵ��ھ�γ�ȣ��Ƚ�֮ǰ�����ڵľ�γ���Ƿ���һ������
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
				m_objHashArea.Add_Hash_Data(pCurrGeo, m_objAreaInfoList.Get_Node_Offset(pCurrAreaInfo));				
			}	
		}
		else
		{
			//��ͬһ�����ڣ���ֻ���µ�ǰ����Ϣ
			pBeforePosInfo->m_dPosLatitude  = dPosLatitude;
			pBeforePosInfo->m_dPosLongitude = dPosLongitude;
			pBeforePosInfo->m_ttCurrTime    = ttPos;		
		}		
	}
	else
	{
		//֮ǰ�㲢������
		pBeforePosInfo = m_objPosInfoList.Create();
		if(NULL == pBeforePosInfo)
		{
			printf("[CMapInfo::AddPos]m_objPosInfoList is empty.\n");
			return false;
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
			m_objHashArea.Add_Hash_Data(pCurrGeo, m_objAreaInfoList.Get_Node_Offset(pCurrAreaInfo));				
		}	
	}
	
	return true;
}
