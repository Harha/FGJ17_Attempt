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

// Logging
#define LOG(msg)  std::cout << "LOG | " << msg << std::endl;
#define ERR(msg)  std::cerr << "ERR | " << msg << std::endl;

// Utility stuff
#define DELETE_SP(a) if((a) != NULL) delete (a); (a) = NULL;
#define DELETE_AP(a) if((a) != NULL) delete[] (a); (a) = NULL;

#endif // MACROS_H
