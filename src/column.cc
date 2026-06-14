#include <column.h>

VarcharColumn::VarcharColumn(std::string name, std::vector<std::vector<std::string>> data, std::string pattern) :
	m_name(std::move(name)), m_data(std::move(data)), m_pattern(std::move(pattern)) {}

IntColumn::IntColumn(std::string name, int start, std::optional<int> end, bool increment) :
	m_name(std::move(name)), m_start(start), m_end(end), m_increment(increment) {}

BooleanColumn::BooleanColumn(std::string name) :
	m_name(std::move(name)) {}
