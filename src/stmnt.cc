#include <stmnt.h>

RowStmnt::RowStmnt(int count, std::unique_ptr<Stmnt> body) :
    m_count(count), m_body(std::move(body)) {}
void RowStmnt::accept(StmntVisitor &visitor) {
    visitor.visitRowStmnt(*this);
}
std::string RowStmnt::to_string() const {
    return std::format("RowStmnt[{}, {}]", std::to_string(m_count), m_body->to_string());
}

TableStmnt::TableStmnt(std::string name, std::unique_ptr<Stmnt> body) :
	m_name(std::move(name)), m_body(std::move(body)) {}
void TableStmnt::accept(StmntVisitor &visitor) {
	visitor.visitTableStmnt(*this);
}
std::string TableStmnt::to_string() const {
	return std::format("TableStmnt[{}, {}]", m_name, m_body->to_string());
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

ColumnStmnt::ColumnStmnt(std::string name, ColumnType column_type, std::optional<std::unique_ptr<Expr>> parameter) :
	m_name(std::move(name)), m_column_type(column_type), m_parameter(std::move(parameter)) {}
void ColumnStmnt::accept(StmntVisitor &visitor) {
	visitor.visitColumnStmnt(*this);
}
std::string ColumnStmnt::to_string() const {
    if (m_parameter.has_value()) {
        return std::format("ColumnExpr[{}, {}, {}]", m_name, column_map.at(m_column_type), m_parameter.value()->to_string());
    }
    return std::format("ColumnExpr[{}, {}]", m_name, column_map.at(m_column_type));
}
