#ifndef COLUMN_H
#define COLUMN_H

#include <common.h>
#include <optional>
#include <vector>

class VarcharColumn {
public:
	VarcharColumn(std::string name, std::vector<std::vector<std::string>> data, std::string pattern);
	~VarcharColumn() = default;
public:
	std::string m_name;
    std::vector<std::vector<std::string>> m_data;
    std::string m_pattern;
};

class IntColumn {
public:
	IntColumn(std::string name, int start, std::optional<int> end, bool increment); 
	~IntColumn() = default;
public:
	std::string m_name;
	int m_start;
	std::optional<int> m_end;
    bool m_increment;
};

class BooleanColumn {
public:
	BooleanColumn(std::string name);
	~BooleanColumn() = default;
public:
	std::string m_name;
};

#endif
