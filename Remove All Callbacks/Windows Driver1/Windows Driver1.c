#include "BaseInfo.h"
#include "SsdtInfo.h"

MODULE_BASE_INFO TencentMgr_Info;

ULONG_PTR DbgkDebugObjectType_Address;

PSPTERMINATETHREADBYPOINTER PspTerminateThreadByPointer;
PSSUSPENDTHREAD PsSuspendThread;

//�жϵ�ǰ�������ַ�Ƿ��ڵ��Թܼҵ�������
BOOLEAN IsInTencentDriver(ULONG_PTR FuncAddress)
{
	if (FuncAddress >= TencentMgr_Info.QMUdisk_Base && FuncAddress <= TencentMgr_Info.QMUdisk_EndAddress)
		return TRUE;

	//TencentMgr_Info.QQPCHW_Base��������Ҳ�����ʱ����Ϊ0��
	if (FuncAddress >= TencentMgr_Info.QQPCHW_Base && FuncAddress <= TencentMgr_Info.QQPCHW_EndAddress && TencentMgr_Info.QQPCHW_Base != 0)
		return TRUE;

	if (FuncAddress >= TencentMgr_Info.QQSysMon_Base && FuncAddress <= TencentMgr_Info.QQSysMon_EndAddress)
		return TRUE;

	if (FuncAddress >= TencentMgr_Info.softaal_Base && FuncAddress <= TencentMgr_Info.softaal_EndAddress)
		return TRUE;

	if (FuncAddress >= TencentMgr_Info.TAOAccelerator_Base && FuncAddress <= TencentMgr_Info.TAOAccelerator_EndAddress)
		return TRUE;

	if (FuncAddress >= TencentMgr_Info.TAOKernel_Base && FuncAddress <= TencentMgr_Info.TAOKernel_EndAddress)
		return TRUE;

	if (FuncAddress >= TencentMgr_Info.TFsFltX_Base && FuncAddress <= TencentMgr_Info.TFsFltX_EndAddress)
		return TRUE;

	if (FuncAddress >= TencentMgr_Info.TS888_Base && FuncAddress <= TencentMgr_Info.TS888_EndAddress)
		return TRUE;

	if (FuncAddress >= TencentMgr_Info.TsDefenseBT_Base && FuncAddress <= TencentMgr_Info.TsDefenseBT_EndAddress)
		return TRUE;

	if (FuncAddress >= TencentMgr_Info.TSSysKit_Base && FuncAddress <= TencentMgr_Info.TSSysKit_EndAddress)
		return TRUE;

	if (FuncAddress >= TencentMgr_Info.TsNetHlp_Base && FuncAddress <= TencentMgr_Info.TsNetHlp_EndAddress)
		return TRUE;

	return FALSE;
}

//��ȡSSDT�Ļ�ַ���س���
ULONG_PTR GetSsdtBase()
{
	ULONG_PTR SystemCall64;								//��msr�ж�ȡ����SystemCall64�ĵ�ַ
	ULONG_PTR StartAddress;								//��Ѱ����ʼ��ַ����SystemCall64����ʼ��ַ
	ULONG_PTR EndAddress;								//��Ѱ���ս��ַ
	UCHAR *p;											//�����жϵ�������
	ULONG_PTR SsdtBast;									//SSDT��ַ

	SystemCall64 = __readmsr(MSR_LSTAR);
	StartAddress = SystemCall64;
	EndAddress = StartAddress + 0x500;
	while (StartAddress < EndAddress)
	{
		p = (UCHAR*)StartAddress;
		if (MmIsAddressValid(p) && MmIsAddressValid(p + 1) && MmIsAddressValid(p + 2))
		{
			if (*p == 0x4c && *(p + 1) == 0x8d && *(p + 2) == 0x15)
			{
				SsdtBast = (ULONG_PTR)(*(ULONG*)(p + 3)) + (ULONG_PTR)(p + 7);
				break;
			}
		}
		++StartAddress;
	}

	if (StartAddress == EndAddress)
		SsdtBast = 0;

	return SsdtBast;
}

//�����±��SSDT�л�ȡ��Ӧ�±�ĺ����ĵ�ַ
ULONG_PTR GetSsdtFuncAddressById(ULONG FunctionId)
{
	PSYSTEM_SERVICE_TABLE SsdtBase = (PSYSTEM_SERVICE_TABLE)GetSsdtBase();
	ULONG_PTR FunctionAddress;
	ULONG Offset;

	if (SsdtBase == NULL)
	{
		KdPrint(("Get SsdtBase Fail!\n"));
		return 0;
	}

	Offset = SsdtBase->ServiceTableBase[FunctionId];
	Offset = Offset >> 4;
	FunctionAddress = (ULONG_PTR)SsdtBase->ServiceTableBase + Offset;
	FunctionAddress &= 0xFFFFFFFF0FFFFFFF;						//�����¼ӷ��Ľ�λ����
	return FunctionAddress;
}

//���ݺ������ֻ�ȡ�����ĵ�ַ��ntos�����ĺ�����
ULONG_PTR GetFuncAddress(PWSTR FuncName)
{
	UNICODE_STRING uFunctionName;
	RtlInitUnicodeString(&uFunctionName, FuncName);
	return (ULONG_PTR)MmGetSystemRoutineAddress(&uFunctionName);
}

//��ȡ���Զ���Ļ�ַ
ULONG_PTR GetDbgkDebugobjectAddr()
{
	ULONG_PTR NtCreateDebugObject_Address = GetSsdtFuncAddressById(Index_NtCreateDebugObject);
	ULONG_PTR SearchEndAddress;
	UCHAR *p;

	if (NtCreateDebugObject_Address == 0)
	{
		KdPrint(("Get NtCreateDebugObject_Address Fail!\n"));
		return 0;
	}

	SearchEndAddress = NtCreateDebugObject_Address + 0x500;
	p = (UCHAR*)NtCreateDebugObject_Address;
	while (NtCreateDebugObject_Address < SearchEndAddress)
	{
		if (MmIsAddressValid(p) && MmIsAddressValid(p + 1) && MmIsAddressValid(p + 2))
		{
			if (*p == 0x48 && *(p + 1) == 0x8b && *(p + 2) == 0x15)
				return *(ULONG_PTR*)(((NtCreateDebugObject_Address + 0x7) + *(ULONG*)(NtCreateDebugObject_Address + 0x3) & 0xFFFFFFF0FFFFFFFF));
		}
		++NtCreateDebugObject_Address;
		++p;
	}

	return 0;
}

//��ȡPspTerminateThreadByPointer�����ĵ�ַ����PsTerminateSystemThread��
ULONG_PTR GetPspTerminateThreadByPointer()
{
	ULONG_PTR PsTerminateSystemThread = 0;
	ULONG_PTR EndAddress = 0;
	UCHAR *p;

	PsTerminateSystemThread = GetFuncAddress(L"PsTerminateSystemThread");

	if (PsTerminateSystemThread == 0)
	{
		KdPrint(("Get PsTerminateSystemThread Fail!\n"));
		return 0;
	}

	//windbg���������û���ٸ��ֽڣ�ֱ��+0x100�ͺ���
	EndAddress = PsTerminateSystemThread + 0x100;
	p = (UCHAR *)PsTerminateSystemThread;
	while (PsTerminateSystemThread < EndAddress)
	{
		if (MmIsAddressValid(p) && MmIsAddressValid(p + 1) && MmIsAddressValid(p + 2))
		{
			if (*p == 0xb0 && *(p + 1) == 0x01 && *(p + 2) == 0xe8)
				return ((PsTerminateSystemThread + 0x7) + *(ULONG*)(PsTerminateSystemThread + 0x3) & 0xFFFFFFF0FFFFFFFF);
		}
		++PsTerminateSystemThread;
		++p;
	}

	return 0;
}

//��ȡPsSuspendThread�����ĵ�ַ����NtSuspendThread��
ULONG_PTR GetPsSuspendThread()
{
	ULONG_PTR NtSuspendThread = 0;
	ULONG_PTR EndAddress;
	UCHAR *p;

	NtSuspendThread = GetSsdtFuncAddressById(Index_NtSuspendThread);
	if (NtSuspendThread == 0)
	{
		KdPrint(("Get NtSuspendThread Fail!\n"));
		return 0;
	}

	EndAddress = NtSuspendThread + 0x100;
	p = (UCHAR *)NtSuspendThread;
	while (NtSuspendThread < EndAddress)
	{
		if (MmIsAddressValid(p) && MmIsAddressValid(p + 1) && MmIsAddressValid(p + 2))
		{
			if (*p == 0x24 && *(p + 1) == 0x68 && *(p + 2) == 0xe8)
				return ((NtSuspendThread + 0x7) + *(ULONG*)(NtSuspendThread + 0x3) & 0xFFFFFFF0FFFFFFFF);
		}
		++NtSuspendThread;
		++p;
	}

	return 0;
}

//��ȡPspCreateProcessNotifyRoutine������̻ص����ĵ�ַ�����²�����PsSetCreateProcessNotifyRoutine -> PspSetCreateProcessNotifyRoutine -> PspCreateProcessNotifyRoutine
ULONG_PTR GetPspCreateProcessNotifyRoutine()
{
	ULONG_PTR PsSetCreateProcessNotifyRoutine = 0;
	ULONG_PTR PspSetCreateProcessNotifyRoutine = 0;
	ULONG_PTR PspCreateProcessNotifyRoutine = 0;

	ULONG_PTR EndAddress;
	UCHAR *p;
	
	PsSetCreateProcessNotifyRoutine = GetFuncAddress(L"PsSetCreateProcessNotifyRoutine");
	if (PsSetCreateProcessNotifyRoutine == 0)
	{
		KdPrint(("Get PsSetCreateProcessNotifyRoutine Fail!\n"));
		return 0;
	}

	EndAddress = PsSetCreateProcessNotifyRoutine + 0x100;
	p = (UCHAR *)PsSetCreateProcessNotifyRoutine;
	while (PsSetCreateProcessNotifyRoutine < EndAddress)
	{
		if (MmIsAddressValid(p) && MmIsAddressValid(p + 1) && MmIsAddressValid(p + 2))
		{
			if (*p == 0x33 && *(p + 1) == 0xc0 && *(p + 2) == 0xe9)
			{
				PspSetCreateProcessNotifyRoutine = ((PsSetCreateProcessNotifyRoutine + 0x7) + *(ULONG*)(PsSetCreateProcessNotifyRoutine + 0x3) & 0xFFFFFFF0FFFFFFFF);
				break;
			}
		}
		++PsSetCreateProcessNotifyRoutine;
		++p;
	}

	if (PspSetCreateProcessNotifyRoutine == 0)
	{
		KdPrint(("Get PspSetCreateProcessNotifyRoutine Fail!\n"));
		return 0;
	}

	EndAddress = PspSetCreateProcessNotifyRoutine + 0x100;
	p = (UCHAR *)PspSetCreateProcessNotifyRoutine;
	while (PspSetCreateProcessNotifyRoutine < EndAddress)
	{
		if (MmIsAddressValid(p) && MmIsAddressValid(p + 1) && MmIsAddressValid(p + 2))
		{
			if (*p == 0x4c && *(p + 1) == 0x8d && *(p + 2) == 0x35)
			{
				PspCreateProcessNotifyRoutine = ((PspSetCreateProcessNotifyRoutine + 0x7) + *(ULONG*)(PspSetCreateProcessNotifyRoutine + 0x3) & 0xFFFFFFF0FFFFFFFF);
				break;
			}
		}
		++PspSetCreateProcessNotifyRoutine;
		++p;
	}

	if (PspCreateProcessNotifyRoutine == 0)
		KdPrint(("Get PspCreateProcessNotifyRoutine Fail!\n"));

	return PspCreateProcessNotifyRoutine;
}

//��ȡPspCreateThreadNotifyRoutine����̻߳ص����ĵ�ַ�����²�����PsSetCreateThreadNotifyRoutine -> PspCreateThreadNotifyRoutine
ULONG_PTR GetPspCreateThreadNotifyRoutine()
{
	ULONG_PTR PsSetCreateThreadNotifyRoutine = 0;
	ULONG_PTR PspCreateThreadNotifyRoutine = 0;
	ULONG_PTR EndAddress;
	UCHAR *p;

	PsSetCreateThreadNotifyRoutine = GetFuncAddress(L"PsSetCreateThreadNotifyRoutine");
	if (PsSetCreateThreadNotifyRoutine == 0)
	{
		KdPrint(("Get PsSetCreateThreadNotifyRoutine Fail!\n"));
		return 0;
	}

	EndAddress = PsSetCreateThreadNotifyRoutine + 0x100;
	p = (UCHAR *)PsSetCreateThreadNotifyRoutine;
	while (PsSetCreateThreadNotifyRoutine < EndAddress)
	{
		if (MmIsAddressValid(p) && MmIsAddressValid(p + 1) && MmIsAddressValid(p + 2))
		{
			if (*p == 0x48 && *(p + 1) == 0x8d && *(p + 2) == 0x0d)
			{
				PspCreateThreadNotifyRoutine = ((PsSetCreateThreadNotifyRoutine + 0x7) + *(ULONG*)(PsSetCreateThreadNotifyRoutine + 0x3) & 0xFFFFFFF0FFFFFFFF);
				break;
			}
		}
		++PsSetCreateThreadNotifyRoutine;
		++p;
	}

	if (PspCreateThreadNotifyRoutine == 0)
		KdPrint(("Get PspCreateThreadNotifyRoutine Fail!\n"));

	return PspCreateThreadNotifyRoutine;
}

//��ȡPspLoadImageNotifyRoutine���ģ��ص����ĵ�ַ�����²�����PsSetLoadImageNotifyRoutine -> PspLoadImageNotifyRoutine
ULONG_PTR GetPspLoadImageNotifyRoutine()
{
	ULONG_PTR PsSetLoadImageNotifyRoutine = 0;
	ULONG_PTR PspLoadImageNotifyRoutine = 0;
	ULONG_PTR EndAddress;
	UCHAR *p;

	PsSetLoadImageNotifyRoutine = GetFuncAddress(L"PsSetLoadImageNotifyRoutine");
	if (PsSetLoadImageNotifyRoutine == 0)
	{
		KdPrint(("Get PsSetLoadImageNotifyRoutine Fail!\n"));
		return 0;
	}

	EndAddress = PsSetLoadImageNotifyRoutine + 0x100;
	p = (UCHAR*)PsSetLoadImageNotifyRoutine;
	while (PsSetLoadImageNotifyRoutine < EndAddress)
	{
		if (MmIsAddressValid(p) && MmIsAddressValid(p + 1) && MmIsAddressValid(p + 2))
		{
			if (*p == 0x48 && *(p + 1) == 0x8d && *(p + 2) == 0x0d)
			{
				PspLoadImageNotifyRoutine = ((PsSetLoadImageNotifyRoutine + 0x7) + *(ULONG*)(PsSetLoadImageNotifyRoutine + 0x3) & 0xFFFFFFF0FFFFFFFF);
				break;
			}
		}
		++PsSetLoadImageNotifyRoutine;
		++p;
	}

	if (PspLoadImageNotifyRoutine == 0)
		KdPrint(("Get PspLoadImageNotifyRoutine Fail!\n"));

	return PspLoadImageNotifyRoutine;
}

//��ȡCallbackListHead���ע���ص����ĵ�ַ�����²�����CmUnRegisterCallBack -> CallbackListHead
ULONG_PTR GetCallbackListHead()
{
	ULONG_PTR CmUnRegisterCallBack = 0;
	ULONG_PTR CallbackListHead = 0;
	ULONG_PTR EndAddress;
	UCHAR *p;

	CmUnRegisterCallBack = GetFuncAddress(L"CmUnRegisterCallback");
	if (CmUnRegisterCallBack == 0)
	{
		KdPrint(("Get CmUnRegisterCallback Fail!\n"));
		return 0;
	}

	//���ͦ����ģ���0x200����
	EndAddress = CmUnRegisterCallBack + 0x200;
	p = (UCHAR*)CmUnRegisterCallBack;
	while (CmUnRegisterCallBack < EndAddress)
	{
		//�����и���Ȥ��������Mm�ж�������ڴ棬�����ǲ����Զ�ȡ�ģ�����ʵ�����ڴ��ǿ��Զ�ȡ��
		if (*(p - 1) == 0x20 && *p == 0x48 && *(p + 1) == 0x8d && *(p + 2) == 0x0d)
		{
			CallbackListHead = ((CmUnRegisterCallBack + 0x7) + *(ULONG*)(CmUnRegisterCallBack + 0x3) & 0xFFFFFFF0FFFFFFFF);
			break;
		}

		++CmUnRegisterCallBack;
		++p;
	}

	if (CallbackListHead == 0)
		KdPrint(("Get CallbackListHead Fail!\n"));

	return CallbackListHead;
}

//��ȡIopNotifyShutdownQueueHead����ػ��ص����ĵ�ַ�����²�����IoRegisterShutdownNotification -> IopNotifyShutdownQueueHead
ULONG_PTR GetIopNotifyShutdownQueueHead()
{
	ULONG_PTR IoRegisterShutdownNotification = 0;
	ULONG_PTR IopNotifyShutdownQueueHead = 0;
	ULONG_PTR EndAddress;
	UCHAR *p;

	IoRegisterShutdownNotification = GetFuncAddress(L"IoRegisterShutdownNotification");
	if (IoRegisterShutdownNotification == 0)
	{
		KdPrint(("Get IoRegisterShutdownNotification Fail!\n"));
		return 0;
	}

	EndAddress = IoRegisterShutdownNotification + 0x100;
	p = (UCHAR*)IoRegisterShutdownNotification;
	while (IoRegisterShutdownNotification < EndAddress)
	{
		if (MmIsAddressValid(p) && MmIsAddressValid(p + 1) && MmIsAddressValid(p + 2))
		{
			if (*p == 0x48 && *(p + 1) == 0x8d && *(p + 2) == 0x0d)
			{
				IopNotifyShutdownQueueHead = ((IoRegisterShutdownNotification + 0x7) + *(ULONG*)(IoRegisterShutdownNotification + 0x3) & 0xFFFFFFF0FFFFFFFF);
				break;
			}
		}

		++IoRegisterShutdownNotification;
		++p;
	}

	if (IopNotifyShutdownQueueHead == 0)
		KdPrint(("Get IopNotifyShutdownQueueHead Fail!\n"));

	return IopNotifyShutdownQueueHead;
}

//����ģ�����ֻ�ȡģ��Ļ�ַ�Լ���ַ
NTSTATUS GetModuleBase(PSTR ModuleName,ULONG_PTR *ModuleBase,ULONG_PTR* ModuleEndAddress)
{
	NTQUERYSYSTEMINFORMATION NtQuerySystemInformation;				//��ȡ����NtQuerySystemInformation����
	PSYSTEM_MODULE_INFORMATION_ENTRY ModuleEntry;					//ÿһ��ģ������
	UNICODE_STRING n_NtQuerySystemInformation;						//NtQuerySystemInformation����������
	ULONG ModuleNumber;											    //ģ�������
	ULONG RetLength;												//��Ҫ������ڴ��С
	NTSTATUS Status;												//״̬����ֵ
	UCHAR *Module;													//��ȡ��ģ������

	if (ModuleBase == NULL || ModuleEndAddress == NULL || !MmIsAddressValid(ModuleBase) || !MmIsAddressValid(ModuleEndAddress))
	{
		KdPrint(("�������ݴ���\n"));
		return STATUS_INVALID_PARAMETER;
	}

	RtlInitUnicodeString(&n_NtQuerySystemInformation, L"NtQuerySystemInformation");

	NtQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)MmGetSystemRoutineAddress(&n_NtQuerySystemInformation);

	Status = NtQuerySystemInformation(SystemModuleInformation, NULL, 0, &RetLength);

	if (Status != STATUS_INFO_LENGTH_MISMATCH)
	{
		KdPrint(("1.NtQuerySystemInformation Fail!Status = %x\n",Status));
		return 0;
	}

	Module = (UCHAR*)ExAllocatePoolWithTag(NonPagedPool, RetLength, 'ytz');

	if (Module == NULL)
	{
		KdPrint(("Allocate Memory Fial!\n"));
		return 0;
	}

	Status = NtQuerySystemInformation(SystemModuleInformation, Module, RetLength, &RetLength);

	if (!NT_SUCCESS(Status))
	{
		KdPrint(("2.NtQuerySystemInformation Fail!Status = %x\n", Status));
		return 0;
	}

	ModuleNumber = *(ULONG*)Module;

	ModuleEntry = (PSYSTEM_MODULE_INFORMATION_ENTRY)(Module + 8);

	while (ModuleNumber > 0)
	{
		if (strstr(ModuleEntry->ImageName, ModuleName))
		{
			*ModuleBase = (ULONG_PTR)ModuleEntry->Base;
			*ModuleEndAddress = ((ULONG_PTR)ModuleEntry->Base + ModuleEntry->Size);
			return STATUS_SUCCESS;
		}
		--ModuleNumber;
		++ModuleEntry;
	}

	*ModuleBase = 0;
	*ModuleEndAddress = 0;
	return STATUS_UNSUCCESSFUL;
}

//��ʼ���������ȡģ����ں˵�ַ�ͱ߽�
NTSTATUS InitKernelBase()
{
	NTSTATUS status;

	status = GetModuleBase("ntoskrnl", &TencentMgr_Info.Ntoskrnl_Base, &TencentMgr_Info.Ntoskrnl_EndAddress);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Get Ntoskrnl Fail!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	status = GetModuleBase("QQSysMonX64", &TencentMgr_Info.QQSysMon_Base, &TencentMgr_Info.QQSysMon_EndAddress);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Get QQSysMonX64 Fail!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	status = GetModuleBase("TSSysKit64", &TencentMgr_Info.TSSysKit_Base, &TencentMgr_Info.TSSysKit_EndAddress);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Get TSSysKit64 Fail!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	status = GetModuleBase("TFsFltX64", &TencentMgr_Info.TFsFltX_Base, &TencentMgr_Info.TFsFltX_EndAddress);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Get TFsFltX64 Fail!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	status = GetModuleBase("TAOKernel64", &TencentMgr_Info.TAOKernel_Base, &TencentMgr_Info.TAOKernel_EndAddress);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Get TAOKernel64 Fail!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	//���TsDefenseBT64�ܹ��죬ʱ����дʱ��Сд
	status = GetModuleBase("DefenseBT64", &TencentMgr_Info.TsDefenseBT_Base, &TencentMgr_Info.TsDefenseBT_EndAddress);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Get TsDefenseBT64 Fail!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	status = GetModuleBase("TS888x64", &TencentMgr_Info.TS888_Base, &TencentMgr_Info.TS888_EndAddress);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Get TS888x64 Fail!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	status = GetModuleBase("TAOAccelerator64", &TencentMgr_Info.TAOAccelerator_Base, &TencentMgr_Info.TAOAccelerator_EndAddress);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Get TAOAccelerator64 Fail!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	status = GetModuleBase("softaal64", &TencentMgr_Info.softaal_Base, &TencentMgr_Info.softaal_EndAddress);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Get softaal64 Fail!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	status = GetModuleBase("QQPCHW_x64", &TencentMgr_Info.QQPCHW_Base, &TencentMgr_Info.QQPCHW_EndAddress);
	if (!NT_SUCCESS(status))
	{
		//����ļ�ʱ�����ڣ�ʱ�������ڣ���̫���ж��Ƿ����
		KdPrint(("QQPCHW_x64 isn't exit!\n"));
		//return STATUS_UNSUCCESSFUL;
	}

	status = GetModuleBase("QMUdisk64", &TencentMgr_Info.QMUdisk_Base, &TencentMgr_Info.QMUdisk_EndAddress);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Get QMUdisk64 Fail!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	status = GetModuleBase("TsNetHlpX64", &TencentMgr_Info.TsNetHlp_Base, &TencentMgr_Info.TsNetHlp_EndAddress);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Get TsNetHlpX64 Fail!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}

//ȥ�����л�ȡ��ģ�����µĽ��̻ص�
NTSTATUS RemoveProcessCallBack()
{
	//���ʵ���Ͼ���һ�����飬�����ÿһ�������һ�������ĵ�ַ������������Ǳ���ע�������ĺ���
	ULONG_PTR* PspCreateProcessNotifyRoutine;
	ULONG_PTR TempFuncAddress;
	NTSTATUS status;

	PspCreateProcessNotifyRoutine = (ULONG_PTR*)GetPspCreateProcessNotifyRoutine();
	if (PspCreateProcessNotifyRoutine == NULL)
		return STATUS_UNSUCCESSFUL;

	KdPrint(("PspCreateProcessNotifyRoutine Address is %llx\n", (ULONG_PTR)PspCreateProcessNotifyRoutine));

	while (*PspCreateProcessNotifyRoutine)
	{
		TempFuncAddress = *PspCreateProcessNotifyRoutine;
		TempFuncAddress &= 0xFFFFFFFFFFFFFFF8;					//ĩβ��λӦ���ǡ�Ȩ�ޡ�λ��
		TempFuncAddress = *(ULONG_PTR*)TempFuncAddress;
		if (IsInTencentDriver(TempFuncAddress))
		{
			status = PsSetCreateProcessNotifyRoutine((PCREATE_PROCESS_NOTIFY_ROUTINE)TempFuncAddress, 1);
			KdPrint(("�Ƴ����̻ص���%llx,����ֵ�ǣ�%x\n", TempFuncAddress, status));
		}

		++PspCreateProcessNotifyRoutine;
	}

	return STATUS_SUCCESS;
}

//ȥ�����л�ȡ��ģ�����µ��̻߳ص�
NTSTATUS RemoveThreadCallBack()
{
	ULONG_PTR *PspCreateThreadNotifyRoutine;
	ULONG_PTR TempFuncAddress;
	NTSTATUS status;

	PspCreateThreadNotifyRoutine = (ULONG_PTR*)GetPspCreateThreadNotifyRoutine();
	if (PspCreateThreadNotifyRoutine == NULL)
		return STATUS_UNSUCCESSFUL;

	KdPrint(("PspCreateThreadNotifyRoutine Address is %llx\n", PspCreateThreadNotifyRoutine));

	while (*PspCreateThreadNotifyRoutine)
	{
		TempFuncAddress = *PspCreateThreadNotifyRoutine;
		TempFuncAddress &= 0xFFFFFFFFFFFFFFF8;					//ĩβ��λӦ���ǡ�Ȩ�ޡ�λ��
		TempFuncAddress = *(ULONG_PTR*)TempFuncAddress;
		if (IsInTencentDriver(TempFuncAddress))
		{
			status = PsRemoveCreateThreadNotifyRoutine((PCREATE_THREAD_NOTIFY_ROUTINE)TempFuncAddress);
			KdPrint(("�Ƴ��̻߳ص���%llx,����ֵ�ǣ�%d\n", TempFuncAddress, status));
		}

		++PspCreateThreadNotifyRoutine;
	}

	return STATUS_SUCCESS;
}

//ȥ�����л�ȡ��ģ�����µ�ģ��ص�
NTSTATUS RemoveImageCallBack()
{
	ULONG_PTR *PspLoadImageNotifyRoutine;
	ULONG_PTR TempFuncAddress;
	NTSTATUS status;

	PspLoadImageNotifyRoutine = (ULONG_PTR *)GetPspLoadImageNotifyRoutine();
	if (PspLoadImageNotifyRoutine == NULL)
		return STATUS_UNSUCCESSFUL;

	while (*PspLoadImageNotifyRoutine)
	{
		TempFuncAddress = *PspLoadImageNotifyRoutine;
		TempFuncAddress &= 0xFFFFFFFFFFFFFFF8;					//ĩβ��λӦ���ǡ�Ȩ�ޡ�λ��
		TempFuncAddress = *(ULONG_PTR*)TempFuncAddress;
		if (IsInTencentDriver(TempFuncAddress))
		{
			status = PsRemoveLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)TempFuncAddress);
			KdPrint(("�Ƴ�ģ��ص���%llx,����ֵ�ǣ�%d\n", TempFuncAddress, status));
		}
		++PspLoadImageNotifyRoutine;
	}

	return STATUS_SUCCESS;
}

//ȥ�����л�ȡ��ģ�����µ�ע���ص�
NTSTATUS RemoveCallBack()
{
	PCM_NOTIFY_ENTRY CmNotifyEntry;
	PCM_NOTIFY_ENTRY Temp;
	LARGE_INTEGER Cookie[20];								//�������20��ע���ص�������20��ע���ص���������������
	NTSTATUS status;
	LONG Index = 0;

	CmNotifyEntry = (PCM_NOTIFY_ENTRY)(*(ULONG_PTR *)GetCallbackListHead());
	if (CmNotifyEntry == NULL)
		return STATUS_UNSUCCESSFUL;

	Temp = CmNotifyEntry;
	do
	{
		if (IsInTencentDriver(Temp->Function))
		{
			Cookie[Index].QuadPart = Temp->Cookie.QuadPart;
			++Index;
		}

		Temp = (PCM_NOTIFY_ENTRY)Temp->ListEntryHead.Flink;
	} while (Temp != CmNotifyEntry);

	while (--Index >= 0)
	{
		status = CmUnRegisterCallback(Cookie[Index]);
		KdPrint(("ժ��CookieΪ��%llx������ֵ�ǣ�%x\n", Cookie[Index].QuadPart, status));
	}

	return STATUS_SUCCESS;
}

//ȥ�����л�ȡ��ģ�����µĹػ��ص�
NTSTATUS RemoveShutdownCallBack()
{
	PSHUTDOWN_NOTIFY ShutdownNotify;
	PSHUTDOWN_NOTIFY TempNotify;
	PDEVICE_OBJECT TencentDevice[20];					//��Ҳ��������ע�ᳬ��20���ػ��ص�
	LONG Index = 0;

	ShutdownNotify = (PSHUTDOWN_NOTIFY)(*(ULONG_PTR *)GetIopNotifyShutdownQueueHead());
	if (ShutdownNotify == NULL)
		return STATUS_UNSUCCESSFUL;

	TempNotify = ShutdownNotify;
	do
	{
		if (IsInTencentDriver((ULONG_PTR)TempNotify->DeviceObject->DriverObject->DriverInit))
		{
			TencentDevice[Index] = TempNotify->DeviceObject;
			Index++;
		}
		TempNotify = (PSHUTDOWN_NOTIFY)TempNotify->ListEntry.Flink;
	} while (TempNotify != ShutdownNotify);

	while (--Index >= 0)
	{
		IoUnregisterShutdownNotification(TencentDevice[Index]);
		KdPrint(("�ɹ��Ƴ�һ���ػ��ص���\n"));
	}
	
	return STATUS_SUCCESS;
}

//�Ƴ���Խ��̶�����̶߳���ʹ�õ�ObRegisterCallBack
NTSTATUS RemoveObRegisterCallBack(ULONG_PTR ObjectType)
{
	POB_CALLBACK ObCallBackItem;
	POB_CALLBACK TempItem;
	ULONG_PTR ObHandle[20];									//ͬ������20����ע�ᳬ��20��Ob����ص����Ǿ���������
	LONG Index = 0;

	if (ObjectType == 0)
	{
		KdPrint(("���������\n"));
		return STATUS_UNSUCCESSFUL;
	}

	ObCallBackItem = (POB_CALLBACK)(ObjectType + 0xc0);
	TempItem = ObCallBackItem;
	do
	{
		if (IsInTencentDriver(TempItem->PreOperation) || IsInTencentDriver(TempItem->PostOperation))
		{
			ObHandle[Index] = TempItem->ObHandle;
			++Index;
		}
		TempItem = (POB_CALLBACK)TempItem->ListEntry.Flink;
	} while (TempItem != ObCallBackItem);

	while (--Index >= 0)
	{
		ObUnRegisterCallbacks((PVOID)ObHandle[Index]);
		KdPrint(("�Ƴ���һ������ص���\n"));
	}

	return STATUS_SUCCESS;
}

//��ͣ������֪֮�ڵ�tencent��ϵͳ�̣߳�ԭ����������ģ�������һ��ģ����߳̽���������...��
NTSTATUS SuspendAllTencentThread()
{
	//��Ѱ����ϵͳ�̣߳������ø��������ˣ�PsLookup��Ѱ��
	ULONG i;
	PETHREAD CurrentThread;
	NTSTATUS status;
	PEPROCESS CurrentProcess;
	ULONG_PTR FuncAddress;
	ULONG RetValue;

	for (i = 8; i < 1024 * 1024; i += 4)
	{
		status = PsLookupThreadByThreadId((HANDLE)i, &CurrentThread);
		if (!NT_SUCCESS(status))
			continue;

		CurrentProcess = IoThreadToProcess(CurrentThread);
		if (strstr(PsGetProcessImageFileName(CurrentProcess),"System"))
		{
			FuncAddress = *(ULONG_PTR *)((UCHAR *)CurrentThread + 0x418);
			if (IsInTencentDriver(FuncAddress))
			{
				status = PsSuspendThread(CurrentThread, &RetValue);
				if (!NT_SUCCESS(status))
				{
					ObDereferenceObject(CurrentThread);
					return status;
				}
			}
		}
		ObDereferenceObject(CurrentThread);
	}

	return STATUS_SUCCESS;
}

//�������������к����������Ƴ����еĺ���
NTSTATUS RemoveAllCallBacks()
{
	NTSTATUS status;

	status = RemoveProcessCallBack();
	if (!NT_SUCCESS(status))
	{
		KdPrint(("RemoveProcessCallBack Fail!\n"));
		return status;
	}

	status = RemoveThreadCallBack();
	if (!NT_SUCCESS(status))
	{
		KdPrint(("RemoveThreadCallBack Fail!\n"));
		return status;
	}

	status = RemoveImageCallBack();
	if (!NT_SUCCESS(status))
	{
		KdPrint(("RemoveImageCallBack Fail!\n"));
		return status;
	}

	status = RemoveCallBack();
	if (!NT_SUCCESS(status))
	{
		KdPrint(("RemoveCallBack Fail!\n"));
		return status;
	}

	status = RemoveShutdownCallBack();
	if (!NT_SUCCESS(status))
	{
		KdPrint(("RemoveShutdownCallBack Fail!\n"));
		return status;
	}

	status = RemoveObRegisterCallBack((ULONG_PTR)*PsProcessType);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Remove PsProcessType CallBack Fail!\n"));
		return status;
	}

	status = RemoveObRegisterCallBack((ULONG_PTR)*PsThreadType);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Remove PsThreadType CallBack Fail!\n"));
		return status;
	}

	return status;
}

//һ�в����Ŀ�ʼ����������ϲ���Ѵ��붼д��DriverEntry����
NTSTATUS StartFunction(PDRIVER_OBJECT DriverObject)
{
	NTSTATUS status;

	status = InitKernelBase();

	if (!NT_SUCCESS(status))
	{
		KdPrint(("InitKernelBase Fail!\n"));
		return status;
	}

	/*DbgkDebugObjectType_Address = GetDbgkDebugobjectAddr();
	if (DbgkDebugObjectType_Address == 0)
	{
		KdPrint(("Get DbgkDebugObject Fail!\n"));
		return STATUS_UNSUCCESSFUL;
	}
	KdPrint(("DbgkDebugObjectType Address is %llx\n", DbgkDebugObjectType_Address));*/

	PspTerminateThreadByPointer = (PSPTERMINATETHREADBYPOINTER)GetPspTerminateThreadByPointer();
	if (PspTerminateThreadByPointer == NULL)
	{
		KdPrint(("Get PspTerminateThreadByPointer Fail!\n"));
		return STATUS_UNSUCCESSFUL;
	}
	KdPrint(("PspTerminateThreadByPointer Address is %llx\n", (ULONG_PTR)PspTerminateThreadByPointer));

	PsSuspendThread = (PSSUSPENDTHREAD)GetPsSuspendThread();
	if (PsSuspendThread == NULL)
	{
		KdPrint(("Get PsSuspendThread Fail!\n"));
		return STATUS_UNSUCCESSFUL;
	}
	KdPrint(("PsSuspendThread Address is %llx\n", (ULONG_PTR)PsSuspendThread));

	status = RemoveAllCallBacks();
	if (!NT_SUCCESS(status))
		return status;

	status = SuspendAllTencentThread();
	if (!NT_SUCCESS(status))
	{
		KdPrint(("SuspendAllTencentThread Fail! Status is %x\n",status));
		return status;
	}

	return STATUS_SUCCESS;
}

VOID Unload(PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Unload Success!\n"));
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegString)
{
	KdPrint(("Entry Driver!\n"));
	DriverObject->DriverUnload = Unload;
	return StartFunction(DriverObject);
}