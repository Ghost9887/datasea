#include <interpreter.h>
#include <random>
#include <climits>
#include <unordered_set>
#include <algorithm>
#include <config.h>

Interpreter::Interpreter(std::string output_name) :
    m_output_file(output_name) 
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
        m_locale = std::format("{}/{}/", DATA_DIR, stmnt.m_locale);
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

void Interpreter::visitRandintExpr(RandintExpr &expr) {
    evaluate(*expr.m_start);
    Value start_val { pop() };
    Value end_val { std::numeric_limits<int>::max() };

    if (expr.m_end.has_value()) {
        evaluate(*expr.m_end.value());
        end_val = pop();
    }

    if (!is_int(start_val) || !is_int(end_val)) error("Randint parameters must be integers"); 

    m_column_data.push_back(Value{ 
        generate_random_int(std::get<int>(start_val.m_data), std::get<int>(end_val.m_data)) 
    }); 
}

void Interpreter::visitRandboolExpr(RandboolExpr &expr) {
    evaluate(*expr.m_weight);
    Value weight_val { pop() };
    if (!is_double(weight_val)) error("Randbool parameter must be a double");
    
    m_column_data.push_back(Value{ generate_random_bool(std::get<double>(weight_val.m_data)) });
}

void Interpreter::visitRanddoubleExpr(RanddoubleExpr &expr) {
    evaluate(*expr.m_start);
    Value start_val { pop() };
    Value end_val { std::numeric_limits<double>::max() };

    if (expr.m_end.has_value()) {
        evaluate(*expr.m_end.value());
        end_val = pop();
    }
    if (!is_double(start_val) || !is_double(end_val)) error("Randdouble parameters must be doubles");   

    m_column_data.push_back(Value{ generate_random_double
        (std::get<double>(start_val.m_data), std::get<double>(end_val.m_data)) 
    });

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
            case TokenType::CITY: {
                m_column_data.push_back(Value{ cache_data(cached_data, "city", TokenType::CITY) });
            }; break;
            case TokenType::COMPANY: {
                m_column_data.push_back(Value{ cache_data(cached_data, "company", TokenType::COMPANY) });
            }; break;
            case TokenType::DEPARTMENT: {
                m_column_data.push_back(Value{ cache_data(cached_data, "department", TokenType::DEPARTMENT) });
            }; break;
            case TokenType::JOB_TITLE: {
                m_column_data.push_back(Value{ cache_data(cached_data, "job_title", TokenType::JOB_TITLE) });
            }; break;
            case TokenType::STATE: {
                m_column_data.push_back(Value{ cache_data(cached_data, "state", TokenType::STATE) });
            }; break;
            case TokenType::STREET_NAME: {
                m_column_data.push_back(Value{ cache_data(cached_data, "street_name", TokenType::STREET_NAME) });
            }; break;
            default:  
                error("Failed to find a valid gen type.");
        }
    }
}

void Interpreter::visitFormatExpr(FormatExpr &expr) {
    if (expr.m_variables.empty()) {
        m_column_data.push_back(Value{ expr.m_pattern });
        return;
    }
    size_t index {0};
    size_t arg_counter {0};
    std::string res { "'" };
    while(index < expr.m_pattern.size()) {
        char c { expr.m_pattern.at(index++) };
        if (c == '{') {
            std::string padding {};
            while (index < expr.m_pattern.size() && expr.m_pattern.at(index) != '}') {
                padding += expr.m_pattern.at(index++);
            }
            if (arg_counter >= expr.m_variables.size()) error("Too many arguments in format.");
            evaluate(*expr.m_variables.at(arg_counter++));
            std::string str { pop_as_str() };
            if (!padding.empty() && padding.size() != str.size()) {
                if (padding.size() > str.size()) {
                    res += padding.replace(padding.size() - str.size(), str.size(), str);
                }else error("Generated string is bigger than the formated padding");
            }else res += str;
            index++;
        }else res += c;
    }
    res += '\'';
    m_column_data.push_back(Value {std::move(res)});
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
    evaluate(*expr.m_index);
    Value index_val { pop() };
    if (!is_int(index_val)) error("Index must be a int");
    int index { std::get<int>(index_val.m_data) };

    if (is_string(value)) {
        std::string str { std::get<std::string>(value.m_data) };
        if (index < static_cast<int>(str.size())) {
            if (index > -1) {
                str = str.at(index);
                m_column_data.push_back(Value {std::move(str)});
                return;
            }else error("Index cannot be smaller than 0");
        }else error(std::format("Index out of bounds: {} > {}", std::to_string(index), std::to_string(str.size() - 1)));
    }else if (is_list(value)) {
        std::vector<Value> array { std::get<std::vector<Value>>(value.m_data) };
        if (index < static_cast<int>(array.size())) {
            if (index > -1) {
                m_column_data.push_back(Value {std::move(array.at(index))});
                return;
            }else error("Index cannot be smaller than 0");
        }else error(std::format("Index out of bounds: {} > {}", std::to_string(index), std::to_string(array.size() - 1)));
    }


    error("Cannot perfom 'At' function on a non string or non list type");
}

void Interpreter::visitSubstrFuncExpr(SubstrFuncExpr &expr) {
    Value value { pop() };

    evaluate(*expr.m_start);
    Value start_val { pop() };

    evaluate(*expr.m_end);
    Value end_val { pop() };

    if (!is_int(start_val) || !is_int(end_val)) error("Start and end must be ints");
    int start { std::get<int>(start_val.m_data) };
    int end { std::get<int>(end_val.m_data) };

    if (is_string(value)) {
        std::string str { std::get<std::string>(value.m_data) };
        if (start < static_cast<int>(str.size()) &&
                end < static_cast<int>(str.size())) {
            if (start > -1 && end > -1) {
                str = str.substr(start, end - start);
                m_column_data.push_back(Value {std::move(str)});
                return;
            }else error("Index cannot be smaller than 0");
        }else error(std::format("Index out of bounds: ({}..{}) > {}", 
                    std::to_string(start), std::to_string(end), std::to_string(str.size() - 1)));
    }

    error("Cannot perform 'Substr' function on a non string type");
}

void Interpreter::visitLowerFuncExpr([[maybe_unused]]LowerFuncExpr &expr) {
    Value value { pop() };
    if (is_string(value)) {
        std::string str { std::get<std::string>(value.m_data) };
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        m_column_data.push_back(Value {std::move(str)});
        return;
    }

    error ("Cannot perform 'Lower' function on a non string type");
}

void Interpreter::visitUpperFuncExpr([[maybe_unused]]UpperFuncExpr &expr) {
    Value value { pop() };
    if (is_string(value)) {
        std::string str { std::get<std::string>(value.m_data) };
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        m_column_data.push_back(Value {std::move(str)});
        return;
    }

    error ("Cannot perform 'Upper' function on a non string type");
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
    std::uniform_int_distribution<int> dist(start, end);    
    return dist(rng);
}

bool Interpreter::generate_random_bool(double weight) {
    static thread_local std::mt19937 rng(std::random_device{}());
    std::bernoulli_distribution dist(weight);
    return dist(rng);
}

double Interpreter::generate_random_double(double start, double end) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(start, end);
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
        if (is_string(m_column_data.at(i))) {
            std::string &str { std::get<std::string>(m_column_data.at(i).m_data) };
            if (!str.starts_with("'")) {
                query += std::format("'{}'", str);
                goto comma_check;
            }
        }
        query += value_to_string(m_column_data.at(i));
        comma_check:
        if (i + 1 < m_column_data.size()) query += ", ";
    }
    query += ");\n";
    return query;
}

void Interpreter::write(const std::string &content) {
    m_output_file << content;
}

bool Interpreter::is_string(const Value &value) {
    return std::holds_alternative<std::string>(value.m_data);
}

bool Interpreter::is_int(const Value &value) {  
    return std::holds_alternative<int>(value.m_data);
}

bool Interpreter::is_bool(const Value &value) {
    return std::holds_alternative<bool>(value.m_data);
}

bool Interpreter::is_list(const Value &value) {
    return std::holds_alternative<std::vector<Value>>(value.m_data);
}

bool Interpreter::is_double(const Value &value) {
    return std::holds_alternative<double>(value.m_data);
}

[[noreturn]]
void Interpreter::error(const std::string &message) {
    throw InterpretError(message);
}
