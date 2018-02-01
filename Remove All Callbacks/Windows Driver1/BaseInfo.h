#ifndef BASEINFO_H
#define BASEINFO_H

#include <ntddk.h>

//һ��ö��ֵ�������˻�������������NtQuerySystemInformation���������е�ֵ
typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation,
	SystemProcessorInformation,             // obsolete...delete
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemPathInformation,
	SystemProcessInformation,                //ϵͳ������Ϣ
	SystemCallCountInformation,
	SystemDeviceInformation,
	SystemProcessorPerformanceInformation,
	SystemFlagsInformation,
	SystemCallTimeInformation,
	SystemModuleInformation,
	SystemLocksInformation,
	SystemStackTraceInformation,
	SystemPagedPoolInformation,
	SystemNonPagedPoolInformation,
	SystemHandleInformation,
	SystemObjectInformation,
	SystemPageFileInformation,
	SystemVdmInstemulInformation,
	SystemVdmBopInformation,
	SystemFileCacheInformation,
	SystemPoolTagInformation,
	SystemInterruptInformation,
	SystemDpcBehaviorInformation,
	SystemFullMemoryInformation,
	SystemLoadGdiDriverInformation,
	SystemUnloadGdiDriverInformation,
	SystemTimeAdjustmentInformation,
	SystemSummaryMemoryInformation,
	SystemMirrorMemoryInformation,
	SystemPerformanceTraceInformation,
	SystemObsolete0,
	SystemExceptionInformation,
	SystemCrashDumpStateInformation,
	SystemKernelDebuggerInformation,
	SystemContextSwitchInformation,
	SystemRegistryQuotaInformation,
	SystemExtendServiceTableInformation,
	SystemPrioritySeperation,
	SystemVerifierAddDriverInformation,
	SystemVerifierRemoveDriverInformation,
	SystemProcessorIdleInformation,
	SystemLegacyDriverInformation,
	SystemCurrentTimeZoneInformation,
	SystemLookasideInformation,
	SystemTimeSlipNotification,
	SystemSessionCreate,
	SystemSessionDetach,
	SystemSessionInformation,
	SystemRangeStartInformation,
	SystemVerifierInformation,
	SystemVerifierThunkExtend,
	SystemSessionProcessInformation,
	SystemLoadGdiDriverInSystemSpace,
	SystemNumaProcessorMap,
	SystemPrefetcherInformation,
	SystemExtendedProcessInformation,
	SystemRecommendedSharedDataAlignment,
	SystemComPlusPackage,
	SystemNumaAvailableMemory,
	SystemProcessorPowerInformation,
	SystemEmulationBasicInformation,
	SystemEmulationProcessorInformation,
	SystemExtendedHandleInformation,
	SystemLostDelayedWriteInformation,
	SystemBigPoolInformation,
	SystemSessionPoolTagInformation,
	SystemSessionMappedViewInformation,
	SystemHotpatchInformation,
	SystemObjectSecurityMode,
	SystemWatchdogTimerHandler,
	SystemWatchdogTimerInformation,
	SystemLogicalProcessorInformation,
	SystemWow64SharedInformation,
	SystemRegisterFirmwareTableInformationHandler,
	SystemFirmwareTableInformation,
	SystemModuleInformationEx,
	SystemVerifierTriageInformation,
	SystemSuperfetchInformation,
	SystemMemoryListInformation,
	SystemFileCacheInformationEx,
	MaxSystemInfoClass  // MaxSystemInfoClass should always be the last enum
} SYSTEM_INFORMATION_CLASS;

//ϵͳģ����Ϣ�Ľṹ
typedef struct _SYSTEM_MODULE_INFORMATION_ENTRY {
	HANDLE Section;
	PVOID MappedBase;
	PVOID Base;
	ULONG Size;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT PathLength;
	CHAR ImageName[256];
} SYSTEM_MODULE_INFORMATION_ENTRY, *PSYSTEM_MODULE_INFORMATION_ENTRY;

typedef struct _MODULE_BASE_INFO
{
	//Ntoskrnl.exe
	ULONG_PTR Ntoskrnl_Base;
	ULONG_PTR Ntoskrnl_EndAddress;
	//QQSysMonX64.sys
	ULONG_PTR QQSysMon_Base;
	ULONG_PTR QQSysMon_EndAddress;
	//TSSysKit64.sys
	ULONG_PTR TSSysKit_Base;
	ULONG_PTR TSSysKit_EndAddress;
	//TFsFltX64.sys
	ULONG_PTR TFsFltX_Base;
	ULONG_PTR TFsFltX_EndAddress;
	//TAOKernel64.sys
	ULONG_PTR TAOKernel_Base;
	ULONG_PTR TAOKernel_EndAddress;
	//TSDefenseBT64.sys;
	ULONG_PTR TsDefenseBT_Base;
	ULONG_PTR TsDefenseBT_EndAddress;
	//TS888x64.sys
	ULONG_PTR TS888_Base;
	ULONG_PTR TS888_EndAddress;
	//TAOAccelerator64.sys
	ULONG_PTR TAOAccelerator_Base;
	ULONG_PTR TAOAccelerator_EndAddress;
	//softaal64.sys
	ULONG_PTR softaal_Base;
	ULONG_PTR softaal_EndAddress;
	//QQPCHW_x64.sys
	ULONG_PTR QQPCHW_Base;
	ULONG_PTR QQPCHW_EndAddress;
	//QMUdisk64.sys
	ULONG_PTR QMUdisk_Base;
	ULONG_PTR QMUdisk_EndAddress;
	//TsNetHlpX64.sys
	ULONG_PTR TsNetHlp_Base;
	ULONG_PTR TsNetHlp_EndAddress;
}MODULE_BASE_INFO, *PMODULE_BASE_INFO;

typedef struct _CM_NOTIFY_ENTRY
{
	LIST_ENTRY  ListEntryHead;							//����ͷ
	ULONG_PTR  Unknown;									//��֪�������õģ�����Ϊ0
	LARGE_INTEGER Cookie;								//ע���ʱ�򷵻ص�Cookie
	ULONG_PTR   Context;								//�����Ĳ���
	ULONG_PTR   Function;								//�����ĵ�ַ
}CM_NOTIFY_ENTRY, *PCM_NOTIFY_ENTRY;

typedef struct _SHUTDOWN_NOTIFY
{
	LIST_ENTRY ListEntry;
	PDEVICE_OBJECT DeviceObject;
}SHUTDOWN_NOTIFY, *PSHUTDOWN_NOTIFY;

typedef struct _OB_CALLBACK
{
	LIST_ENTRY  ListEntry;					//���лص�������������
	OB_OPERATION Operations;				//ע��ص�ʱָ���Ĳ���
	ULONG Active;							//�ƺ���Ϊ1��һ��
	ULONG_PTR ObHandle;						//ע���ʱ�򷵻ص�Handle��Ҳ��ж�ص�ʱ��ʹ�õĶ���
	ULONG_PTR ObjTypeAddr;					//��ǰ�������ĵ�ַ	
	ULONG_PTR PreOperation;					//ǰ�ò����ĵ�ַ
	ULONG_PTR PostOperation;				//���ò����ĵ�ַ
	ULONG_PTR RundownProtection;			//�������õĺ���
}OB_CALLBACK, *POB_CALLBACK;

typedef NTSTATUS(__fastcall *NTQUERYSYSTEMINFORMATION)(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID   SystemInformation,
	IN ULONG    SystemInformationLength,
	OUT PULONG   ReturnLength OPTIONAL);

typedef NTSTATUS(__fastcall *PSPTERMINATETHREADBYPOINTER)(
	IN PETHREAD Thread,
	IN NTSTATUS ExitStatus,
	IN BOOLEAN DirectTerminate);

#endif