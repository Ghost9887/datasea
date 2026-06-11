#include <stmnt.h>

TableStmnt::TableStmnt(std::string name, std::unique_ptr<Stmnt> body) :
	m_name(name), m_body(std::move(body)) {}
void TableStmnt::accept(StmntVisitor &visitor) {
	visitor.visitTableStmnt(*this);
}
std::string TableStmnt::to_string() const {
	return std::format("TableStmnt[{}, {}]", m_name, m_body->to_string());
}

BlockStmnt::BlockStmnt(std::vector<std::unique_ptr<Expr>> expressions) :
	m_expressions(std::move(expressions)) {}
void BlockStmnt::accept(StmntVisitor &visitor) {
	visitor.visitBlockStmnt(*this);
}
std::string BlockStmnt::to_string() const {
	std::string res {};
	res += "BlockStmnt[ ";
	for (size_t i = 0; i < m_expressions.size(); i++) {
		res += m_expressions.at(i)->to_string();
	}
	res += " ]";
	return res;
}
