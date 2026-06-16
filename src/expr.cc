#include <expr.h>

IncrementExpr::IncrementExpr(int start, std::optional<int> end) :
    m_start(start), m_end(end), m_counter(start) {}
void IncrementExpr::accept(ExprVisitor &visitor) {
    visitor.visitIncrementExpr(*this);
}
std::string IncrementExpr::to_string() const {
    if (m_end.has_value()) {
        return std::format("IncrementExpr[{}..{}]", std::to_string(m_start), std::to_string(m_end.value()));
    }
    return std::format("IncrementExpr[{}..]", std::to_string(m_start));
}

RandomExpr::RandomExpr(int start, std::optional<int> end) :
    m_start(start), m_end(end) {}
void RandomExpr::accept(ExprVisitor &visitor) {
    visitor.visitRandomExpr(*this);
}
std::string RandomExpr::to_string() const {
    if (m_end.has_value()) {
        return std::format("RandomExpr[{}..{}]", std::to_string(m_start), std::to_string(m_end.value()));
    }
    return std::format("RandomExpr[{}..]", std::to_string(m_start));
}

GenExpr::GenExpr(TokenType type) :
    m_type(type) {}
void GenExpr::accept(ExprVisitor &visitor) {
    visitor.visitGenExpr(*this);
}
std::string GenExpr::to_string() const {
    return std::format("GenExpr[{}]", tokens_map.at(m_type)); 
}

FormatExpr::FormatExpr(std::string pattern, std::vector<std::unique_ptr<Expr>> variables) :
    m_pattern(pattern), m_variables(std::move(variables)) {}
void FormatExpr::accept(ExprVisitor &visitor) {
    visitor.visitFormatExpr(*this);
}
std::string FormatExpr::to_string() const {
    std::string res { std::format("FormatExpr['{}', ", m_pattern) };
    for (size_t i {0}; i < m_variables.size(); i++) {
        res += m_variables.at(i)->to_string(); 
    }
    res += "]";
    return res;
}

ValueExpr::ValueExpr(Value value) :
    m_value(std::move(value)) {}
void ValueExpr::accept(ExprVisitor &visitor) {
    visitor.visitValueExpr(*this);
}
std::string ValueExpr::to_string() const {
    return std::format("ValueExpr[{}]", value_to_string(m_value));
}

VariableExpr::VariableExpr(std::string name, std::vector<std::unique_ptr<Expr>> functions) :
    m_name(name), m_functions(std::move(functions)) {}
void VariableExpr::accept(ExprVisitor &visitor) {
    visitor.visitVariableExpr(*this);
}
std::string VariableExpr::to_string() const {
    std::string res { std::format("VariableExpr[{}, ", m_name) };
    for (size_t i {0}; i < m_functions.size(); i++) {
        res += m_functions.at(i)->to_string();
        res += ", ";
    }
    res += "]";
    return res;
}

ListExpr::ListExpr(std::vector<std::unique_ptr<Expr>> expressions) :
    m_expressions(std::move(expressions)) {}
void ListExpr::accept(ExprVisitor &visitor) {
    visitor.visitListExpr(*this);
}
std::string ListExpr::to_string() const {
    std::string res { "ListExpr[" };
    for (size_t i {0}; i < m_expressions.size(); i++) {
        res += m_expressions.at(i)->to_string();
        res += ", ";
    }
    res += "]";
    return res;
}

FuncExpr::FuncExpr(std::unique_ptr<Expr> function) :
    m_function(std::move(function)) {}
void FuncExpr::accept(ExprVisitor &visitor) {
    visitor.visitFuncExpr(*this);
}
std::string FuncExpr::to_string() const {
    return std::format("FuncExpr[{}]", m_function->to_string());
}

AtFuncExpr::AtFuncExpr(int index) :
    m_index(index) {}
void AtFuncExpr::accept(ExprVisitor &visitor) {
    visitor.visitAtFuncExpr(*this);
}
std::string AtFuncExpr::to_string() const {
    return std::format("AtFuncExpr[{}]", std::to_string(m_index));
}

SubstrFuncExpr::SubstrFuncExpr(int start, int end) :
    m_start(start), m_end(end) {}
void SubstrFuncExpr::accept(ExprVisitor &visitor) {
    visitor.visitSubstrFuncExpr(*this);
}
std::string SubstrFuncExpr::to_string() const {
    return std::format("SubstrFuncExpr[{}, {}]", std::to_string(m_start), std::to_string(m_end));
}
