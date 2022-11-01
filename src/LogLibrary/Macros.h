#ifndef _LOGGER_MACROS_H_
#define _LOGGER_MACROS_H_

// helper macro to expand the contents of another macro
#define EXPAND(x) x

// gets the Nth element of the variable arguments
#define _GET_NTH_ARG(_1, _2, _3, _4, _5, N, ...) N

// expansion for no elements (check CALL_FOR_EACH)
#define _fe_0(EXPR, PARAM1, PARAM2, ...)

// expansion for 1 element (check CALL_FOR_EACH)
#define _fe_1(EXPR, PARAM1, PARAM2, ELEM, ...) \
	EXPR(PARAM1, PARAM2, ELEM)

// expansion for 2 elements (check CALL_FOR_EACH)
#define _fe_2(EXPR, PARAM1, PARAM2, ELEM, ...)         \
	EXPR(PARAM1, PARAM2, ELEM)                         \
	EXPAND(_fe_1(EXPR, PARAM1, PARAM2, ##__VA_ARGS__)) \

// expansion for 3 elements (check CALL_FOR_EACH)
#define _fe_3(EXPR, PARAM1, PARAM2, ELEM, ...)         \
	EXPR(PARAM1, PARAM2, ELEM)                         \
	EXPAND(_fe_2(EXPR, PARAM1, PARAM2, ##__VA_ARGS__)) \

// expansion for 4 elements (check CALL_FOR_EACH)
#define _fe_4(EXPR, PARAM1, PARAM2, ELEM, ...)         \
	EXPR(PARAM1, PARAM2, ELEM)                         \
	EXPAND(_fe_3(EXPR, PARAM1, PARAM2, ##__VA_ARGS__)) \

// expand expression EXPR with two parameters PARAM1 and PARAM2 with the list of variable arguments
// expects a trailing comma or else the last element will be ignored
#define CALL_FOR_EACH_TRAILING_COMMA(EXPR, PARAM1, PARAM2, ...) \
	EXPAND(_GET_NTH_ARG(##__VA_ARGS__, _fe_4, _fe_3, _fe_2, _fe_1, _fe_0)(EXPR, PARAM1, PARAM2, ##__VA_ARGS__))

#endif
