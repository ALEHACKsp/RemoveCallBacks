#ifndef SSDTINFO_H
#define SSDTINFO_H

#include <ntddk.h>

//����KiSystemCall64��msr�Ĵ���������
#define MSR_LSTAR 0xC0000082

//win7-*64 NtCreateDebugObject��SSDT�е��±�
#define Index_NtCreateDebugObject 144

extern UCHAR *PsGetProcessImageFileName(PEPROCESS Process);

extern unsigned __int64 __readmsr(int register);				//��ȡmsr�Ĵ���

extern unsigned __int64 __readcr0(void);			//��ȡcr0��ֵ

extern void __writecr0(unsigned __int64 Data);		//д��cr0

extern void __debugbreak();							//�ϵ㣬����int 3

extern void __disable(void);						//�����ж�

extern void __enable(void);							//�����ж�

//SSDT��Ľṹ
typedef struct _SYSTEM_SERVICE_TABLE
{
	PUINT32 ServiceTableBase;
	PUINT32 ServiceCounterTableBase;
	UINT64 NumberOfServices;
	PUCHAR ParamTableBase;
}SYSTEM_SERVICE_TABLE, *PSYSTEM_SERVICE_TABLE;
#endif