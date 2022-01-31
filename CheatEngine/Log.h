#pragma once

#define LOG_PRINT(fmt, ...) Log::Message("[Info]", fmt, ##__VA_ARGS__);
#define LOG_ERROR(fmt, ...) Log::Message("[Fail]", fmt, ##__VA_ARGS__);

class Log {
public:
    static HRESULT Initialize();

    static const char* GetTimeAsString();
    static void Message(const char* type, const char* fmt, ...);
    static void ClearLog();
    static void LogToFile(const char* message);
};
