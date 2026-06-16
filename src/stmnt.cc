#include <stmnt.h>

TableStmnt::TableStmnt(std::string name, int row_count, std::unique_ptr<Stmnt> body) :
	m_name(std::move(name)), m_row_count(row_count), m_body(std::move(body)) {}
void TableStmnt::accept(StmntVisitor &visitor) {
	visitor.visitTableStmnt(*this);
}
std::string TableStmnt::to_string() const {
	return std::format("TableStmnt[{}, {}, {}]", m_name, std::to_string(m_row_count), m_body->to_string());
}

BlockStmnt::BlockStmnt(std::vector<std::unique_ptr<Stmnt>> statements) :
	m_statements(std::move(statements)) {}
void BlockStmnt::accept(StmntVisitor &visitor) {
	visitor.visitBlockStmnt(*this);
}
std::string BlockStmnt::to_string() const {
	std::string res {};
	res += "BlockStmnt[ ";
	for (size_t i = 0; i < m_statements.size(); i++) {
		res += m_statements.at(i)->to_string();
	}
	res += " ]";
	return res;
}

LocaleStmnt::LocaleStmnt(std::string locale) :
    m_locale(locale) {}
void LocaleStmnt::accept(StmntVisitor &visitor) {
    visitor.visitLocaleStmnt(*this);
}
std::string LocaleStmnt::to_string() const {
    return std::format("LocaleStmnt[{}]", m_locale);
}

ColumnStmnt::ColumnStmnt(std::string name, std::unique_ptr<Expr> parameter) :
	m_name(std::move(name)), m_parameter(std::move(parameter)) {}
void ColumnStmnt::accept(StmntVisitor &visitor) {
	visitor.visitColumnStmnt(*this);
}
std::string ColumnStmnt::to_string() const {
    return std::format("ColumnStmnt[{}, {}]", m_name, m_parameter->to_string());
}

DeclStmnt::DeclStmnt(std::string name, std::unique_ptr<Expr> expr) :
    m_name(name), m_expr(std::move(expr)) {}
void DeclStmnt::accept(StmntVisitor &visitor) {
    visitor.visitDeclStmnt(*this);
}
std::string DeclStmnt::to_string() const {
    return std::format("DeclStmnt[{}, {}]", m_name, m_expr->to_string());
}

PrintStmnt::PrintStmnt(std::unique_ptr<Expr> expr) :
    m_expr(std::move(expr)) {}
void PrintStmnt::accept(StmntVisitor &visitor) {
    visitor.visitPrintStmnt(*this);
}
std::string PrintStmnt::to_string() const {
    return std::format("PrintStmnt[{}]", m_expr->to_string());
}

AssignStmnt::AssignStmnt(std::string name, std::unique_ptr<Expr> expr) :
    m_name(std::move(name)), m_expr(std::move(expr)) {}
void AssignStmnt::accept(StmntVisitor &visitor) {
    visitor.visitAssignStmnt(*this);
}
std::string AssignStmnt::to_string() const {
    return std::format("AssignStmnt[{}, {}]", m_name, m_expr->to_string());
}
