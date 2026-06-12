#include <generator.h>
#include <fstream>
#include <random>
#include <climits>

void Generator::generate(const std::string output_file_name) {
    std::ofstream output_file("../" + output_file_name);

    for (size_t i = 0; i < m_count; i++) {
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
    return "Not implemented";
}


std::string Generator::int_data(IntColumn &column) {
    if (!column.m_increment) {
        static std::mt19937 rng(std::random_device{}());
        int max_value = column.m_end.has_value()
            ? column.m_end.value()
            : std::numeric_limits<int>::max();

        std::uniform_int_distribution<int> dist(column.m_start, max_value);
        return std::to_string(dist(rng));
    }else {
        static int increment { 0 };
        return std::to_string(increment++);
    }
}

std::string Generator::boolean_data(BooleanColumn &column) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 1);
    return dist(rng) == 1 ? "TRUE" : "FALSE";
}
