#pragma once

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
	static BOOL WriteLog(LogType::en type, const TCHAR* data, ...)
	{
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
		_sntprintf_s(CurrentDate, 32, _T("%d-%d-%d %d:%d:%d"), SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);
		_sntprintf_s(CurrentFileName, MAX_PATH, _T("LOG_%d-%d-%d %d.log"), SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour);

		_tfopen_s(&FilePtr, CurrentFileName, _T("a"));
		if (!FilePtr)
			return FALSE;

		_ftprintf(FilePtr, _T("[%s] [%s] %s\n"), CurrentDate, LogType::GetString(type), Log);
		_sntprintf_s(DebugLog, MAX_LOG_LENGTH, _T("[%s] [%s] %s\n"), CurrentDate, LogType::GetString(type), Log);

		fflush(FilePtr);
		fclose(FilePtr);

		OutputDebugString(DebugLog);
		_tprintf(_T("%s"), DebugLog);
		return TRUE;
	}
};