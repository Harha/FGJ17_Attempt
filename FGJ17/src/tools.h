#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <sstream>

// Because MinGW32 is retarded
template <typename T>
std::string to_string(T value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

#endif // TOOLS_H
