#pragma once

#ifdef MIXED_MODE_DLL_EXPORT
#define MIXED_BRIDGE_API __declspec(dllexport)
#else
#define MIXED_BRIDGE_API __declspec(dllimport)
#endif

#include <vector>

class MIXED_BRIDGE_API Bridge
{
public:
	Bridge(int x, int y, int w, int h);
	~Bridge();

	void configureSystems(std::vector<const char *> systems);
	void setSystemEnabled(const char *system, bool enabled);
	void configureLevels(std::vector<std::pair<const char *, const char *>> levels);

	void log(const char *level, float timestamp, const char *system, const char *message) const;
};