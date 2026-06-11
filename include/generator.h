#ifndef GENERATOR_H
#define GENERATOR_H

#include <common.h>
#include <column.h>
#include <variant>

using Column = std::variant<VarcharColumn, IntColumn, BooleanColumn>;

class Generator {
public:
	Generator() = default;
	~Generator() = default;
public:
	std::string m_table_name {};
	std::vector<Column> m_columns {};
	int count {};
};

#endif
