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

//��������̣߳���������Խ��ж�ȡ��Ϣ���в�������Ϣ��
//������羭γ�ȸ��µȽӿڵȲ�����
//�粻����Ҫ�����������ʲô����д��
//��ҪΪC++�ṩ������ڡ�
int Runtime_Thread(IMapInfo* pMapInfo)
{
	while(true)
	{
		printf("[Runtime_Thread]Do.\n");
		sleep(SLEEP_TIME);
	}
	
	return 0;
}

