#include "pch.h"
#include "FTX_UnityLog.h"



static IUnityLog* s_UnityLog = nullptr;

void SetUnityLog(IUnityLog* pLog)
{
	s_UnityLog = pLog;
}

void DebugLog(const char* msg)
{
	if (s_UnityLog == nullptr)
	{
		return;
	}

	UNITY_LOG(s_UnityLog, msg);
}

void DebugLog(std::string msg)
{
	DebugLog(msg.c_str());
}

void UnityLog_Warning(const char* msg)
{
	if (s_UnityLog == nullptr)
	{
		return;
	}

	UNITY_LOG_WARNING(s_UnityLog, msg);
}

void UnityLog_Warning(std::string msg)
{
	UnityLog_Warning(msg.c_str());
}

void UnityLog_Error(const char* msg)
{
	if (s_UnityLog == nullptr)
	{
		return;
	}

	UNITY_LOG_ERROR(s_UnityLog, msg);
}

void UnityLog_Error(std::string msg)
{
	UnityLog_Error(msg.c_str());
}