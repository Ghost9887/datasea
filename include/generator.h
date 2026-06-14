#ifndef GENERATOR_H
#define GENERATOR_H

#include <common.h>
#include <column.h>
#include <variant>
#include <vector>

using Column = std::variant<VarcharColumn, IntColumn, BooleanColumn>;

class Generator {
public:
	Generator() = default;
	void generate(const std::string output_file_name);
	~Generator() = default;
private:
    std::string column_data();
    std::string column_names();
    std::string varchar_data(VarcharColumn &column);
    std::string int_data(IntColumn &column);
    std::string boolean_data(BooleanColumn &column);
    int generate_random_int(int start, int end);
public:
    std::string m_locale {};
	std::string m_table_name {};
	std::vector<Column> m_columns {};
	int m_count {};
};

#endif
