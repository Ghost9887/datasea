#include <generator.h>
#include <random>
#include <climits>
#include <unordered_map>
#include <fstream>
#include <vector>

void Generator::generate(const std::string output_file_name) {
    std::ofstream output_file("../" + output_file_name);

    for (int i = 0; i < m_count; i++) {
        output_file << std::format("INSERT INTO {}({}) VALUES ({});\n", m_table_name, column_names(),  column_data());
    }

    output_file.close();
}

std::string Generator::column_names() {
    std::string names {};
    for (size_t i = 0; i < m_columns.size(); i++) {
        if (std::holds_alternative<VarcharColumn>(m_columns.at(i))) {
            names += std::get<VarcharColumn>(m_columns.at(i)).m_name;
        }else if (std::holds_alternative<IntColumn>(m_columns.at(i))) {
            names += std::get<IntColumn>(m_columns.at(i)).m_name;
        }else if (std::holds_alternative<BooleanColumn>(m_columns.at(i))) {
            names += std::get<BooleanColumn>(m_columns.at(i)).m_name;
        }
        if (i + 1 < m_columns.size()) {
            names += ", ";
        }
    }

    return names;
}

std::string Generator::column_data() {
    std::string data {};
    
    for (size_t i = 0; i < m_columns.size(); i++) {
        if (std::holds_alternative<VarcharColumn>(m_columns.at(i))) {
            data += varchar_data(std::get<VarcharColumn>(m_columns.at(i)));
        }else if (std::holds_alternative<IntColumn>(m_columns.at(i))) {
            data += int_data(std::get<IntColumn>(m_columns.at(i)));
        }else if (std::holds_alternative<BooleanColumn>(m_columns.at(i))) {
            data += boolean_data(std::get<BooleanColumn>(m_columns.at(i)));
        }
        if (i + 1 < m_columns.size()) {
            data += ", ";
        }
    }

    return data;
}

std::string Generator::varchar_data(VarcharColumn &column) {
    switch (column.m_data.size()) {
        case 1: 
            return std::vformat(
                    column.m_pattern, 
                        std::make_format_args(
                            column.m_data[0][generate_random_int(0, column.m_data[0].size() - 1)]
                        )  
                    );
        case 2:
            return std::vformat(
                    column.m_pattern,
                        std::make_format_args(
                            column.m_data[0][generate_random_int(0, column.m_data[0].size() - 1)],
                            column.m_data[1][generate_random_int(0, column.m_data[1].size() - 1)]
                        )
                    );
        case 3:
            return  std::vformat(
                        column.m_pattern,
                        std::make_format_args(
                            column.m_data[0][generate_random_int(0, column.m_data[0].size() - 1)],
                            column.m_data[1][generate_random_int(0, column.m_data[1].size() - 1)],
                            column.m_data[2][generate_random_int(0, column.m_data[2].size() - 1)]
                        )
                    );
        case 4:
            return  std::vformat(
                        column.m_pattern,
                        std::make_format_args(
                            column.m_data[0][generate_random_int(0, column.m_data[0].size() - 1)],
                            column.m_data[1][generate_random_int(0, column.m_data[1].size() - 1)],
                            column.m_data[2][generate_random_int(0, column.m_data[2].size() - 1)],
                            column.m_data[3][generate_random_int(0, column.m_data[3].size() - 1)]   
                        )
                    );
        case 5:
            return  std::vformat(
                        column.m_pattern,
                        std::make_format_args(
                            column.m_data[0][generate_random_int(0, column.m_data[0].size() - 1)],
                            column.m_data[1][generate_random_int(0, column.m_data[1].size() - 1)],
                            column.m_data[2][generate_random_int(0, column.m_data[2].size() - 1)],
                            column.m_data[3][generate_random_int(0, column.m_data[3].size() - 1)],   
                            column.m_data[4][generate_random_int(0, column.m_data[4].size() - 1)]
                        )
                    );
        default: return "";
    }
}

std::string Generator::int_data(IntColumn &column) {
    if (!column.m_increment) {
        int max_value = column.m_end.has_value() ? column.m_end.value() : std::numeric_limits<int>::max();

        return std::to_string(generate_random_int(column.m_start, max_value));
    }else {
        static std::unordered_map<std::string, int> counters;
        int &counter = counters[column.m_name];
        if (counter == 0) counter = column.m_start;

        return std::to_string(counter++);
    }
}

std::string Generator::boolean_data([[maybe_unused]]BooleanColumn &column) {
    return generate_random_int(0, 1) == 1 ? "TRUE" : "FALSE";
}

int Generator::generate_random_int(int start, int end) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(start, end);

    return dist(rng);
}
