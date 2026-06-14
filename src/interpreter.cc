#include <interpreter.h>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <fstream>

Interpreter::Interpreter(std::vector<std::unique_ptr<Stmnt>> statements, Generator &generator) :
	m_statements(std::move(statements)), m_generator(generator) {}

void Interpreter::interpret() {
    try {
        for (size_t i {0}; i < m_statements.size(); i++) {
            execute(*m_statements.at(i));
        }
    }catch (const InterpretError &e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }
}

void Interpreter::visitTableStmnt(TableStmnt &stmnt) {
	m_generator.m_table_name = stmnt.m_name;
	execute(*stmnt.m_body);
}

void Interpreter::visitBlockStmnt(BlockStmnt &stmnt) {
	for (size_t i {0}; i < stmnt.m_expressions.size(); i++) {
		evaluate(*stmnt.m_expressions.at(i));
	}
}

void Interpreter::visitLocaleStmnt(LocaleStmnt &stmnt) {
    std::unordered_map<std::string, std::string> locales = {
        {"en", "../data/en/"},
    };
    
    if (locales.find(stmnt.m_locale) != 0) {
        m_generator.m_locale = locales.at(stmnt.m_locale);
        return;
    }

    throw InterpretError("No locale found!");
}

void Interpreter::visitColumnExpr(ColumnExpr &expr) {
	if (std::holds_alternative<VarcharType>(expr.m_column_type)) {
        VarcharType &varchar_type { std::get<VarcharType>(expr.m_column_type) };
        std::vector<std::vector<std::string>> data = parse_pattern(varchar_type.m_pattern);
		m_generator.m_columns.emplace_back (
			VarcharColumn (
				expr.m_name,
                data,
                varchar_type.m_pattern
			)
		);
	}else if (std::holds_alternative<IntType>(expr.m_column_type)) {
        IntType &int_type { std::get<IntType>(expr.m_column_type) };
		m_generator.m_columns.emplace_back (
			IntColumn (
				expr.m_name,
				int_type.m_start,
				int_type.m_end,
                int_type.m_increment
			)
		);
	}else if (std::holds_alternative<BooleanType>(expr.m_column_type)) {
		m_generator.m_columns.emplace_back (
			BooleanColumn (
				expr.m_name
			)
		);
	}
}

void Interpreter::visitCountExpr(CountExpr &expr) {
	m_generator.m_count = expr.m_count;
}

void Interpreter::execute(Stmnt &stmnt) {
	stmnt.accept(*this);
}

void Interpreter::evaluate(Expr &expr) {
	expr.accept(*this);
}

std::vector<std::vector<std::string>> Interpreter::parse_pattern(std::string &pattern) {
    static std::unordered_set<std::string> key_set = {
        "first_name", "last_name", "sex" 
    };

    std::vector<std::vector<std::string>> data {};
    std::string new_pattern {};
    new_pattern += '\'';
    size_t index {0};
    constexpr size_t MAX_KEYS { 5 };
    size_t key_count {0};
    while(index < pattern.size()) {
        char c = pattern.at(index);
        if (c == '{') {
            if (key_count >= MAX_KEYS) throw InterpretError("To many keys in pattern: max is 5.");
            std::string key {};
            index++;
            while (true) {
                if (index >= pattern.size()) throw InterpretError("Missing closing '}' in pattern.");
                if (pattern.at(index) == '}') break;
                key += pattern.at(index++);
            }

            if (key_set.find(key) != key_set.end()) {
                data.push_back(read_data(key));
            }else {
                 throw InterpretError("Invalid key in pattern.");
            }
            new_pattern += "{}";
            key_count++;
        }else new_pattern += c;
        index++;
    }
    new_pattern += '\'';

    pattern = new_pattern;
    
    return data;
}

std::vector<std::string> Interpreter::read_data(const std::string &key) {
    if (m_generator.m_locale.size() < 1) throw InterpretError("No locale found before executing patterns");

    std::vector<std::string> data {};
    std::string full_path { std::format("{}{}.txt", m_generator.m_locale, key) };
    std::ifstream data_file (full_path);
    if (!data_file) 
        throw InterpretError("Failed to open data file: " + full_path);
    
    std::string line {};
    while(std::getline(data_file, line)) {
        data.push_back(line);
    }
    data_file.close();
    return data;
}
