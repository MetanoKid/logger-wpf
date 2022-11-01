#ifndef _LOG_LIBRARY_LOG_LEVEL_H_
#define _LOG_LIBRARY_LOG_LEVEL_H_

#define LogLevelDefinitions       \
	/* use your own here       */ \
	/* these are just examples */ \
	X( INFO,    i, "#0000FF" )    \
	X( DEBUG,   d, "#000000" )    \
	X( WARNING, w, "#B8860B" )    \
	X( ERROR,   e, "#FF0000" )    \

enum class LogLevel
{
	INVALID,

#define X(NAME, SHORTHAND, COLOR) NAME,
	LogLevelDefinitions
#undef X
};

#endif
