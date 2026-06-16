#include <interpreter.h>
#include <random>
#include <climits>
#include <unordered_set>

Interpreter::Interpreter(std::string output_name) :
    m_output_file("../" + output_name) 
{
    m_envs.emplace_back(std::make_unique<Environment>(nullptr));
}

void Interpreter::interpret(std::vector<std::unique_ptr<Stmnt>> statements) {
    try {
        for (size_t i {0}; i < statements.size(); i++) {
            execute(*statements.at(i));
        }
    }catch(const InterpretError &e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }
}

void Interpreter::visitTableStmnt(TableStmnt &stmnt) {
    if (stmnt.m_row_count < 0) error("Row count cannot be lower than 0");
    for (int i {0}; i < stmnt.m_row_count; i++) {
        m_column_names.clear();
        m_column_data.clear();
        execute(*stmnt.m_body);
        write(build_query(stmnt.m_name));
    }
}

void Interpreter::visitBlockStmnt(BlockStmnt &stmnt) {
    m_envs.emplace_back(std::make_unique<Environment>(m_envs.at(m_current_env).get()));
    m_current_env++;
    for (size_t i {0}; i < stmnt.m_statements.size(); i++) {
        execute(*stmnt.m_statements.at(i));
    }
    if (m_envs.at(m_current_env)->has_parent()) {
        m_envs.erase(m_envs.begin() + m_current_env);
        m_current_env--;
    }
}

void Interpreter::visitLocaleStmnt(LocaleStmnt &stmnt) {
    static std::unordered_set<std::string> locales_set = {
        "us"
    };

    if (locales_set.find(stmnt.m_locale) != locales_set.end()) {
        m_locale = std::format("../data/{}/", stmnt.m_locale);
    }else {
        error(std::format("Locale '{}' does not exist", stmnt.m_locale));
    }
}

void Interpreter::visitColumnStmnt(ColumnStmnt &stmnt) {
    m_column_names.push_back(stmnt.m_name);
    evaluate(*stmnt.m_parameter);
}

void Interpreter::visitDeclStmnt(DeclStmnt &stmnt) {
    evaluate(*stmnt.m_expr);
    if (m_envs.at(m_current_env)->insert_var(stmnt.m_name, pop())) {
        return;
    }
    error(std::format("Variable '{}' already exists.", stmnt.m_name)); 
}

void Interpreter::visitPrintStmnt(PrintStmnt &stmnt) {
    evaluate(*stmnt.m_expr);
    std::cout << value_to_string(pop()) << std::endl;
}

void Interpreter::visitAssignStmnt(AssignStmnt &stmnt) {
    evaluate(*stmnt.m_expr);
    if (m_envs.at(m_current_env)->assign_var(stmnt.m_name, pop())) {
        return;
    }

    error(std::format("Variable '{}' does not exist", stmnt.m_name));
}

void Interpreter::visitIncrementExpr(IncrementExpr &expr) {
    //reset the counter if we hit the limit
    if (expr.m_end.has_value()) {
        if (expr.m_counter >= expr.m_end.value()) {
            expr.m_counter = expr.m_start;
        }
    }

    m_column_data.push_back(Value{ expr.m_counter++ });
}

void Interpreter::visitRandomExpr(RandomExpr &expr) {
    int max = expr.m_end.has_value() ? expr.m_end.value() : std::numeric_limits<int>::max();

    
    m_column_data.push_back(Value{ generate_random_int(expr.m_start, max) }); 
}

void Interpreter::visitGenExpr(GenExpr &expr) {
    if (m_locale.size() < 1) {
        error("No locale found before executing patterns.");
    }
    
    static CachedData cached_data {};
    
    if (cached_data.find(expr.m_type) != cached_data.end()) {
        std::vector<std::string> &data { cached_data.at(expr.m_type) }; 
        m_column_data.push_back(Value{ data.at(generate_random_int(0, data.size() - 1)) });
    }else {
        switch(expr.m_type) {
            case TokenType::FIRST_NAME: {
                m_column_data.push_back(Value{ cache_data(cached_data, "first_name", TokenType::FIRST_NAME) });
            };break;
            case TokenType::LAST_NAME: { 
                m_column_data.push_back(Value{ cache_data(cached_data, "last_name", TokenType::LAST_NAME) });
            };break;
            case TokenType::SEX: {
                m_column_data.push_back(Value{ cache_data(cached_data, "sex", TokenType::SEX) });
            };break;
            case TokenType::BOOL: {
                m_column_data.push_back(Value{ generate_random_int(0, 1) == 0 ? "FALSE" : "TRUE" }); 
            }; break;
            default:  
                //should be unreachable
                error("Failed to find a valid gen type.");
        }
    }
}

void Interpreter::visitFormatExpr(FormatExpr &expr) {
    if (expr.m_variables.empty()) {
        m_column_data.push_back(Value{ expr.m_pattern });
        return;
    }

    for (size_t i {0}; i < expr.m_variables.size(); i++) {
        evaluate(*expr.m_variables.at(i));
    }

    switch (expr.m_variables.size()) {
        case 1: {
            m_column_data.push_back(
                Value {
                    std::vformat(expr.m_pattern, 
                        std::make_format_args(pop_as_str()))
                });
        };break;
        case 2: {
            m_column_data.push_back(
                Value {
                    std::vformat(expr.m_pattern, 
                        std::make_format_args(pop_as_str(), pop_as_str()))
                });
        };break;
        case 3: {
            m_column_data.push_back(
                Value {
                    std::vformat(expr.m_pattern, 
                        std::make_format_args(pop_as_str(), pop_as_str(), pop_as_str()))
                });
        };break;
        case 4: {
            m_column_data.push_back(
                Value {
                    std::vformat(expr.m_pattern, 
                        std::make_format_args(pop_as_str(), pop_as_str(), pop_as_str(), pop_as_str()))
                });
        };break;
        case 5: {
            m_column_data.push_back(
                Value {
                    std::vformat(expr.m_pattern, 
                        std::make_format_args(pop_as_str(), pop_as_str(), pop_as_str(), pop_as_str(), pop_as_str()))
                    });
        };break;
        //should be unreachable
        default: error("Too many format args: max is 5");
    }
}

void Interpreter::visitValueExpr(ValueExpr &expr) {
    m_column_data.push_back(expr.m_value);
}

void Interpreter::visitVariableExpr(VariableExpr &expr) {
    std::optional<Value> retrieved_value { m_envs.at(m_current_env)->get_var(expr.m_name) };
    if (retrieved_value.has_value()) {
        if (!expr.m_functions.empty()) {
            Value temp { retrieved_value.value() };
            for (size_t i {0}; i < expr.m_functions.size(); i++) {
                m_column_data.push_back(temp);
                evaluate(*expr.m_functions.at(i));
                //only pop when we still have another function to go
                if (i + 1 < expr.m_functions.size()) {
                    temp = pop();
                }
            }
            return;
        }else {
            m_column_data.push_back(std::move(retrieved_value.value()));
            return;
        }
    }

    error(std::format("Variable '{}' does not exist", expr.m_name)); 
}

void Interpreter::visitListExpr(ListExpr &expr) {
    std::vector<Value> values {};
    for (size_t i {0}; i < expr.m_expressions.size(); i++) {
        evaluate(*expr.m_expressions.at(i));
        values.push_back(pop());
    }
    m_column_data.push_back(Value{ std::move(values) });
}

void Interpreter::visitFuncExpr(FuncExpr &expr) {
    evaluate(*expr.m_function);
}

void Interpreter::visitAtFuncExpr(AtFuncExpr &expr) {
    Value value { pop() };
    if (std::holds_alternative<std::string>(value.m_data)) {
        std::string str { std::get<std::string>(value.m_data) };
        if (expr.m_index < static_cast<int>(str.size())) {
            if (expr.m_index > -1) {
                str = str.at(expr.m_index);
                m_column_data.push_back(Value {std::move(str)});
                return;
            }else error("Index cannot be smaller than 0");
        }else error(std::format("Index out of bounds: {} > {}", std::to_string(expr.m_index), std::to_string(str.size() - 1)));
    }else if (std::holds_alternative<std::vector<Value>>(value.m_data)) {
        std::vector<Value> array { std::get<std::vector<Value>>(value.m_data) };
        if (expr.m_index < static_cast<int>(array.size())) {
            if (expr.m_index > -1) {
                m_column_data.push_back(Value {std::move(array.at(expr.m_index))});
                return;
            }else error("Index cannot be smaller than 0");
        }else error(std::format("Index out of bounds: {} > {}", std::to_string(expr.m_index), std::to_string(array.size() - 1)));
    }


    error("Cannot perfom 'At' function on a non string or non list type");
}

void Interpreter::visitSubstrFuncExpr(SubstrFuncExpr &expr) {
    Value value { pop() };
    if (std::holds_alternative<std::string>(value.m_data)) {
        std::string str { std::get<std::string>(value.m_data) };
        if (expr.m_start < static_cast<int>(str.size()) &&
                expr.m_end < static_cast<int>(str.size())) {
            if (expr.m_start > -1 && expr.m_end > -1) {
                str = str.substr(expr.m_start, expr.m_end - expr.m_start);
                m_column_data.push_back(Value {std::move(str)});
                return;
            }else error("Index cannot be smaller than 0");
        }else error(std::format("Index out of bounds: ({}..{}) > {}", 
                    std::to_string(expr.m_start), std::to_string(expr.m_end), std::to_string(str.size() - 1)));
    }

    error("Cannot perfom 'Substr' function on a non string type");
}

void Interpreter::execute(Stmnt &stmnt) {
    stmnt.accept(*this);
}

void Interpreter::evaluate(Expr &expr) {
    expr.accept(*this);
}

const Value Interpreter::pop() {
    Value value { m_column_data.back() };
    m_column_data.pop_back();
    return value;
}

const std::string Interpreter::pop_as_str() {
    return value_to_string(pop());
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
        data.push_back(line);
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
        query += value_to_string(m_column_data.at(i));
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
