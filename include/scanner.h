#ifndef SCANNER_h
#define SCANNER_h

#include <common.h>

class Scanner {
public:
	Scanner(std::string source);
	~Scanner() = default;
private:
	std::string m_source;
};

#endif
