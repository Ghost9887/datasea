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
    m_value(value) {}
void ValueExpr::accept(ExprVisitor &visitor) {
    visitor.visitValueExpr(*this);
}
std::string ValueExpr::to_string() const {
    return std::format("ValueExpr[{}]", Token::value_to_string(m_value));
}

VariableExpr::VariableExpr(std::string name) :
    m_name(name) {}
void VariableExpr::accept(ExprVisitor &visitor) {
    visitor.visitVariableExpr(*this);
}
std::string VariableExpr::to_string() const {
    return std::format("VariableExpr[{}]", m_name);
}
