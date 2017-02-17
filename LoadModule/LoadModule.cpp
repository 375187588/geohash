#include <unistd.h>
#include "../GeoHash/imapinfo.h"

#define SLEEP_TIME 5 //��λ����

//add by freeeyes
//�������������ǣ�����һ�ν��������ڴ��ʱ��
//�����������������Դ�е����ݣ���Ϊ����Դ���������ںܶ෽��(���ݿ�,����,�ļ���)��
//���Խ��ⲿ�ֶ����������ɲ�������������е�ʱ����Զ��������������̣�����ʼ�����ݡ�
//�ɿ�����Ա�Լ���дָ���Ĵ���

extern "C"
{
	int LoadModuleData(IMapInfo* pMapInfo);
	
	int Runtime_Thread(IMapInfo* pMapInfo);
}

//�ڴ��һ�δ�����ʼ���ӿڣ�������ʵ�ִ�����Դ���ڴ����䡣
int LoadModuleData(IMapInfo* pMapInfo)
{
	//�����ĳ�ʼ������������
	printf("[LoadModuleData]Hello.\n");
	
	//�����������ʵʱ������
	time_t ttNow = time(NULL);
	pMapInfo->AddPos("13661201024", 39.928167, 116.389550, ttNow);
	
	pMapInfo->AddPos("13661201025", 39.928167, 116.389550, ttNow);
	
	return 0;
}

//������ҽ��̣���������Խ��ж�ȡ��Ϣ���в�������Ϣ��
//������羭γ�ȸ��µȽӿڵȲ�����
//�粻����Ҫ�����������ʲô����д��
//��ҪΪC++�ṩ�������,Ҳ��������ء�
int Runtime_Thread(IMapInfo* pMapInfo)
{
	char szTestMsisdn[15] = {'\0'};
	sprintf(szTestMsisdn, "13661201024");
	
	while(true)
	{
		printf("[Runtime_Thread]Do.\n");
		vector<_Pos_Info*> vecPosList;
		
		//����ɾ��һ����ǰ��
		bool blState = pMapInfo->DelPos(szTestMsisdn);
		if(true == blState)
		{
			printf("[Runtime_Thread](%s)Delete OK.\n", szTestMsisdn);
		}
		
		//���һ����ǰ��
		time_t ttNow = time(NULL);
		pMapInfo->AddPos(szTestMsisdn, 39.928187, 116.389550, ttNow);
		
		//��ѯһ����ǰ��
		pMapInfo->FindPos(39.928167, 116.389550,10000.0, vecPosList);
		for(int i = 0; i < (int)vecPosList.size(); i++)
		{
			printf("[Runtime_Thread]msisdn=%s.\n", vecPosList[i]->m_szMsisdn);
		}
		printf("[Runtime_Thread]Done.\n");
		sleep(SLEEP_TIME);
	}
	
	return 0;
}

