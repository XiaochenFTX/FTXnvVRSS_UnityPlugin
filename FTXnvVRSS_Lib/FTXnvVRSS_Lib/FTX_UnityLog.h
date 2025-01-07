#pragma once

#include <string>

void SetUnityLog(IUnityLog* pLog);

void DebugLog(const char* msg);
void DebugLog(std::string msg);

void UnityLog_Warning(const char* msg);
void UnityLog_Warning(std::string msg);

void UnityLog_Error(const char* msg);
void UnityLog_Error(std::string msg);