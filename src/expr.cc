#include <expr.h>

VarcharType::VarcharType(int count, std::string pattern) :
	m_count(count), m_pattern(std::move(pattern)) {}
std::string VarcharType::to_string() const {
	return std::format("VarcharType[{}, {}]", std::to_string(m_count), m_pattern);
}

IntType::IntType(int start, std::optional<int> end) :
	m_start(start), m_end(end) {}
std::string IntType::to_string() const {
	if (m_end.has_value()) {
		return std::format("IntType[{}..{}]", m_start, m_end.value());
	}
	return std::format("IntType[{}]", m_start);
}

std::string BooleanType::to_string() const {
	return "BooleanType[]";
}

ColumnExpr::ColumnExpr(std::string name, ColumnType column_type) :
	m_name(std::move(name)), m_column_type(column_type) {}
void ColumnExpr::accept(ExprVisitor &visitor) {
	visitor.visitColumnExpr(*this);
}
std::string ColumnExpr::to_string() const {
	std::string column_type_string {};
	if (std::holds_alternative<VarcharType>(m_column_type)) 
		column_type_string = std::get<VarcharType>(m_column_type).to_string();
	else if (std::holds_alternative<IntType>(m_column_type)) 
		column_type_string = std::get<IntType>(m_column_type).to_string();
	else if (std::holds_alternative<BooleanType>(m_column_type)) 
		column_type_string = std::get<BooleanType>(m_column_type).to_string();

	return std::format("ColumnExpr[{}, {}]", m_name, column_type_string);
}

CountExpr::CountExpr(int count) :
	m_count(count) {}
void CountExpr::accept(ExprVisitor &visitor) {
	visitor.visitCountExpr(*this);
}
std::string CountExpr::to_string() const {
	return std::format("CountExpr[{}]", std::to_string(m_count));
}
