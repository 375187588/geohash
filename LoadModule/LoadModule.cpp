#include "../GeoHash/imapinfo.h"

//add by freeeyes
//�������������ǣ�����һ�ν��������ڴ��ʱ��
//�����������������Դ�е����ݣ���Ϊ����Դ���������ںܶ෽��(���ݿ�,����,�ļ���)��
//���Խ��ⲿ�ֶ����������ɲ�������������е�ʱ����Զ��������������̣�����ʼ�����ݡ�
//�ɿ�����Ա�Լ���дָ���Ĵ���

extern "C"
{
	int LoadModuleData(IMapInfo* pMapInfo);
}

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
