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
	printf("[LoadModuleData]Hello.\n");
	
	return 0;
}
