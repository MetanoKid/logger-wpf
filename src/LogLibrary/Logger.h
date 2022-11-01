#ifndef _LOG_LIBRARY_LOGGER_H_
#define _LOG_LIBRARY_LOGGER_H_

#include <chrono>

#include "LogLevel.h"
#include "LogSystem.h"
#include "Macros.h"

class Bridge;

class Logger
{
private:
	static Logger *s_instance;

	Logger(int x, int y, int w, int h);
	~Logger();

public:
	static void initialize(int x, int y, int w, int h);
	static void destroy();
	static Logger *getInstance();
	static LogLevel asLogLevel(const char *logLevelName);

	// define as many log methods as log levels
#define X(NAME, SHORTHAND, COLOR) \
	static void SHORTHAND(const char *system, const char *format, ...);
	LogLevelDefinitions
#undef X

	// define as many shorthand log methods as combinations between log levels and systems
#define PER_PAIR(SYSTEM_SHORTHAND, SYSTEM_NAME, LEVEL) \
	static void LEVEL##SYSTEM_SHORTHAND(const char *format, ...);
#define X(LEVEL_NAME, LEVEL_SHORTHAND, LEVEL_COLOR) \
	LEVEL_SHORTHAND,
#define PER_SYSTEM(SHORTHAND, NAME, ...) \
	CALL_FOR_EACH_TRAILING_COMMA(PER_PAIR, SHORTHAND, NAME, ##__VA_ARGS__)
#define S(SYSTEM_NAME, SYSTEM_SHORTHAND) \
	PER_SYSTEM(SYSTEM_SHORTHAND, SYSTEM_NAME, LogLevelDefinitions)
	LogSystemDefinitions
#undef S
#undef PER_SYSTEM
#undef X
#undef PER_PAIR

	void setEnabled(bool enabled);
	void setSystemEnabled(const char *system, bool enabled);
	void setAllSystemsEnabled(bool enabled);
	void setMinimumLogLevel(const LogLevel &logLevel);
private:
	std::chrono::time_point<std::chrono::steady_clock> m_initializationDate;
	bool m_enabled;
	LogLevel m_minimumLogLevel;
	Bridge *m_bridge;

	// define as many log methods as log levels with the va_list alternative
#define X(NAME, SHORTHAND, COLOR) \
	static void SHORTHAND(const char *system, const char *format, va_list args);
	LogLevelDefinitions
#undef X

	void log(LogLevel level, const char *system, const char *message) const;
	const char *logLevelToString(LogLevel level) const;
};

#endif
