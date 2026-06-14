#include <interpreter.h>
#include <random>
#include <climits>

Interpreter::Interpreter(std::string output_name, std::vector<std::unique_ptr<Stmnt>> statements) :
    m_output_file("../" + output_name), m_statements(std::move(statements)) {}

void Interpreter::interpret() {
    try {
        for (size_t i {0}; i < m_statements.size(); i++) {
            execute(*m_statements.at(i));
        }
    }catch(const InterpretError &e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }
}

void Interpreter::visitRowStmnt(RowStmnt &stmnt) {
    if (stmnt.m_count < 1) error("Rows cannot be less than 1");

    for (int i {0}; i < stmnt.m_count; i++) {
        execute(*stmnt.m_body);
    }
}

void Interpreter::visitTableStmnt(TableStmnt &stmnt) {
    m_column_names.clear();
    m_column_data.clear();
    execute(*stmnt.m_body);
    write(build_query(stmnt.m_name));
}

void Interpreter::visitBlockStmnt(BlockStmnt &stmnt) {
    for (size_t i {0}; i < stmnt.m_statements.size(); i++) {
        execute(*stmnt.m_statements.at(i));
    }
}

void Interpreter::visitLocaleStmnt(LocaleStmnt &stmnt) {
    m_locale = std::format("../data/{}/", stmnt.m_locale);
}

void Interpreter::visitColumnStmnt(ColumnStmnt &stmnt) {
    m_column_names.push_back(stmnt.m_name);
    if (stmnt.m_parameter.has_value()) {
        evaluate(*stmnt.m_parameter.value());
    }
}

void Interpreter::visitIncrementExpr(IncrementExpr &expr) {
    //reset the counter if we hit the limit
    if (expr.m_end.has_value()) {
        if (expr.m_counter >= expr.m_end.value()) {
            expr.m_counter = expr.m_start;
        }
    }

    m_column_data.push_back(std::to_string(expr.m_counter++));
}

void Interpreter::visitRandomExpr(RandomExpr &expr) {
    int max = expr.m_end.has_value() ? expr.m_end.value() : std::numeric_limits<int>::max();

    m_column_data.push_back(std::to_string(generate_random_int(expr.m_start, max))); 
}

void Interpreter::visitGenExpr(GenExpr &expr) {
    if (m_locale.size() < 1) {
        error("No locale found before executing patterns.");
    }
    
    static CachedData cached_data {};
    
    if (cached_data.find(expr.m_type) != cached_data.end()) {
        std::vector<std::string> &data { cached_data.at(expr.m_type) }; 
        m_column_data.push_back(data.at(generate_random_int(0, data.size() - 1)));
    }else {
        switch(expr.m_type) {
            case TokenType::FIRST_NAME: {
                m_column_data.push_back(cache_data(cached_data, "first_name", TokenType::FIRST_NAME));
            };break;
            case TokenType::LAST_NAME: { 
                m_column_data.push_back(cache_data(cached_data, "last_name", TokenType::LAST_NAME));
            };break;
            case TokenType::SEX: {
                m_column_data.push_back(cache_data(cached_data, "sex", TokenType::SEX));
            };break;
            case TokenType::BOOLEAN: {
                m_column_data.push_back(generate_random_int(0, 1) == 0 ? "FALSE" : "TRUE"); 
            }; break;
            default: 
                error("Failed to find a valid gen type.");
        }
    }
}

void Interpreter::visitFormatExpr([[maybe_unused]]FormatExpr &expr) {
}

void Interpreter::execute(Stmnt &stmnt) {
    stmnt.accept(*this);
}

void Interpreter::evaluate(Expr &expr) {
    expr.accept(*this);
}

int Interpreter::generate_random_int(int start, int end) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<std::mt19937::result_type> dist(start, end);    

    return dist(rng);
}

std::string Interpreter::cache_data(CachedData &cached_data, std::string &&file_name, TokenType type) {
    std::vector<std::string> data {};
    std::string full_path { std::format("{}{}.txt", m_locale, file_name) };
    std::ifstream data_file(full_path);

    if (!data_file) {
        error(std::format("Failed to open data file: {}", full_path)); 
    }

    std::string line {};
    while(std::getline(data_file, line)) {
        data.push_back(std::format("'{}'", line));
    }

    data_file.close();
    cached_data.insert({type, data});
    return data.at(generate_random_int(0, data.size() - 1));
}

std::string Interpreter::build_query(const std::string &table_name) {
    std::string query { std::format("INSERT INTO {} (" , table_name) };
    for (size_t i {0}; i < m_column_names.size(); i++) {
        query += m_column_names.at(i);
        if (i + 1 < m_column_names.size()) query += ", ";
    }
    query += ") VALUES (";
    for (size_t i {0}; i < m_column_data.size(); i++) {
        query += m_column_data.at(i);
        if (i + 1 < m_column_data.size()) query += ", ";
    }
    query += ");\n";
    return query;
}

void Interpreter::write(const std::string &content) {
    m_output_file << content;
}

[[noreturn]]
void Interpreter::error(const std::string &message) {
    throw InterpretError(message);
}
