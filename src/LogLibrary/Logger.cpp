#include "Logger.h"
#include <LogWindowBridge\Bridge.h>

#include <cstdarg>
#include <cassert>

Logger *Logger::s_instance = nullptr;

Logger::Logger(int x, int y, int w, int h) :
	m_initializationDate(std::chrono::high_resolution_clock::now()),
	m_bridge(new Bridge(x, y, w, h)),
	m_enabled(true),
	m_minimumLogLevel(LogLevel::INVALID)
{
}

Logger::~Logger()
{
	delete m_bridge;
	m_bridge = nullptr;
}

void Logger::initialize(int x, int y, int w, int h)
{
	assert(s_instance == nullptr);

	// create instance
	s_instance = new Logger(x, y, w, h);

	// configure log levels
	std::vector<std::pair<const char *, const char *>> logLevels;
#define X(NAME, SHORTHAND, COLOR) \
	logLevels.push_back(std::pair<const char *, const char *>(#NAME, COLOR));
	LogLevelDefinitions
#undef X
	s_instance->m_bridge->configureLevels(logLevels);

	// configure log systems
	std::vector<const char *> logSystems;
#define S(NAME, SHORTHAND) \
	logSystems.push_back(#NAME);
	LogSystemDefinitions
#undef S
	s_instance->m_bridge->configureSystems(logSystems);
}

void Logger::destroy()
{
	assert(s_instance != nullptr);

	delete s_instance;
	s_instance = nullptr;
}

Logger *Logger::getInstance()
{
	return s_instance;
}

void Logger::log(LogLevel level, const char *system, const char *message) const
{
	auto now = std::chrono::high_resolution_clock::now();
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_initializationDate).count();
	m_bridge->log(logLevelToString(level), milliseconds / 1000.0f, system, message);
}

// define as many log methods as log levels
#define X(NAME, SHORTHAND, COLOR)                                                \
	void Logger::SHORTHAND(const char *system, const char *format, va_list args) \
	{                                                                            \
		const Logger *instance = getInstance();                                  \
		if(instance == nullptr)                                                  \
		{                                                                        \
			return;                                                              \
		}                                                                        \
		                                                                         \
		if(!instance->m_enabled)                                                 \
		{                                                                        \
			return;                                                              \
		}                                                                        \
		                                                                         \
		assert(LogLevel::NAME != LogLevel::INVALID);                             \
		if (LogLevel::NAME < instance->m_minimumLogLevel)                        \
		{                                                                        \
			return;                                                              \
		}                                                                        \
		                                                                         \
		char message[1024];                                                      \
		vsnprintf_s(message, sizeof(message) - 1, format, args);                 \
		instance->log(LogLevel::NAME, system, message);                          \
	}                                                                            \
	void Logger::SHORTHAND(const char *system, const char *format, ...)          \
	{                                                                            \
		va_list args;                                                            \
		va_start(args, format);                                                  \
		SHORTHAND(system, format, args);                                         \
		va_end(args);                                                            \
	}
	LogLevelDefinitions
#undef X

// define as many shorthand log methods as combinations between log levels and systems
#define PER_PAIR(SYSTEM_SHORTHAND, SYSTEM_NAME, LEVEL)            \
	void Logger::LEVEL##SYSTEM_SHORTHAND(const char *format, ...) \
	{                                                             \
		va_list args;                                             \
		va_start(args, format);                                   \
		LEVEL(#SYSTEM_NAME, format, args);                        \
		va_end(args);                                             \
	}
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

const char *Logger::logLevelToString(LogLevel level) const
{
	switch (level)
	{
#define X(NAME, SHORTHAND, COLOR) \
	case LogLevel::NAME:          \
		return #NAME;
	LogLevelDefinitions
#undef X
	}

	return nullptr;
}

void Logger::setEnabled(bool enabled)
{
	m_enabled = enabled;
}

void Logger::setSystemEnabled(const char *system, bool enabled)
{
	m_bridge->setSystemEnabled(system, enabled);
}

void Logger::setAllSystemsEnabled(bool enabled)
{
#define S(NAME, SHORTHAND) \
	setSystemEnabled(#NAME, enabled);
	LogSystemDefinitions
#undef S
}

void Logger::setMinimumLogLevel(const LogLevel &logLevel)
{
	assert(logLevel > LogLevel::INVALID);

	m_minimumLogLevel = logLevel;
}

LogLevel Logger::asLogLevel(const char *logLevelName)
{
#define X(NAME, SHORTHAND, COLOR) if(strcmp(#NAME, logLevelName) == 0) return LogLevel::NAME;
	LogLevelDefinitions
#undef X

	return LogLevel::INVALID;
}
