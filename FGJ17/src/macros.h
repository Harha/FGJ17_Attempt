#ifndef MACROS_H
#define MACROS_H

#include <iostream>
#include <cstring>

// MSVC Crap
#ifdef _MSC_VER 
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

// VALUE FOR FLOATING POINT COMPARISON
#define EPSILON 1e-5

// Fix __FILE__ to use relative path instead
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

// Logging (deprecated)
#define LOG(msg)  std::cout << "LOG | " << msg << std::endl;
#define ERR(msg)  std::cerr << "ERR | " << msg << std::endl;

// Logging
#define LOG_LEVEL 3

#if LOG_LEVEL > 3
#define LOG_DEBUG(msg_, ...)	printf("DBG | " msg_ "\n", ##__VA_ARGS__)
#else
#define LOG_DEBUG(msg, ...)		do {} while(0)
#endif

#if LOG_LEVEL > 2
#define LOG_INFO(msg_, ...)		printf("INF | " msg_ "\n", ##__VA_ARGS__)
#else
#define LOG_INFO(msg, ...)		do {} while(0)
#endif

#if LOG_LEVEL > 1
#define LOG_WARNING(msg_, ...)	printf("WRN | " msg_ "\n", ##__VA_ARGS__)
#else
#define LOG_WARNING(msg, ...)	do {} while(0)
#endif

#if LOG_LEVEL > 0
#define LOG_ERROR(msg_, ...)	printf("ERR | " msg_ "\n", ##__VA_ARGS__)
#else
#define LOG_ERROR(msg, ...)		do {} while(0)
#endif

// Utility stuff
#define DELETE_SP(a) if((a) != NULL) delete (a); (a) = NULL;
#define DELETE_AP(a) if((a) != NULL) delete[] (a); (a) = NULL;

#endif // MACROS_H
