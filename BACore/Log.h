#pragma once
#include "BALock.h"
#include <sstream>
#define MAX_LOG_LENGTH 1024

#define ErrorLog(data, ...) CLog::WriteLog(LogType::Error, _T(data), __VA_ARGS__)
#define InfoLog(data, ...) CLog::WriteLog(LogType::Info, _T(data), __VA_ARGS__)

class LogType
{
public:
	enum en{
		Error = 0,
		Warnning = 1,
		Info = 2
	};

	static const TCHAR* GetString(LogType::en type)
	{
		switch (type)
		{
		case Error:
			return _T("Error");
		case Warnning:
			return _T("Warnning");
		case Info:
			return _T("Info");
		}
		return _T("");
	}
};

class CLog
{
public:
	static bool WriteLog(LogType::en type, const TCHAR* data, ...)
	{
		static BACS _cs;
		BASmartCS lock(_cs);
		SYSTEMTIME SystemTime;
		TCHAR CurrentDate[32] = { 0, };
		TCHAR CurrentFileName[MAX_PATH] = { 0, };
		FILE* FilePtr = NULL;
		TCHAR DebugLog[MAX_LOG_LENGTH] = { 0, };

		va_list ap;
		TCHAR Log[MAX_LOG_LENGTH] = { 0, };

		va_start(ap, data);
		_vstprintf_s(Log, data, ap);
		va_end(ap);

		GetLocalTime(&SystemTime);
		_sntprintf_s(CurrentDate, 32, _T("%04d-%02d-%02d %02d:%02d:%02d"), SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);
		_sntprintf_s(CurrentFileName, MAX_PATH, _T("LOG_%02d-%02d-%02d %02d.log"), SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour);

		_tfopen_s(&FilePtr, CurrentFileName, _T("a"));
		if (!FilePtr)
			return false;

#if defined(_UNICODE)
		std::wostringstream ss;
#else
		std::ostringstream ss;
#endif
		ss << std::this_thread::get_id();
		auto thread_id = ss.str();

		_ftprintf(FilePtr, _T("[%s] Thread[%s] [%s] %s\n"), CurrentDate, thread_id.c_str(), LogType::GetString(type), Log);
#if defined(_DEBUG)
		_sntprintf_s(DebugLog, MAX_LOG_LENGTH, _T("[%s] Thread[%s] [%s] %s\n"), CurrentDate, thread_id.c_str(), LogType::GetString(type), Log);
#endif
		fflush(FilePtr);
		fclose(FilePtr);

#if defined(_DEBUG)
		OutputDebugString(DebugLog);
		_tprintf(_T("%s"), DebugLog);
#endif
		return true;
	}
};