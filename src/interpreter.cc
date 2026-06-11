#include <interpreter.h>

Interpreter::Interpreter(std::vector<std::unique_ptr<Stmnt>> statements, Generator &generator) :
	m_statements(std::move(statements)), m_generator(generator) {}

void Interpreter::interpret() {
	for (size_t i {0}; i < m_statements.size(); i++) {
		execute(*m_statements.at(i));
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

void Interpreter::visitColumnExpr(ColumnExpr &expr) {
	if (std::holds_alternative<VarcharType>(expr.m_column_type)) {
		m_generator.m_columns.emplace_back (
			VarcharColumn (
				expr.m_name,
				std::get<VarcharType>(expr.m_column_type).m_count,
				std::get<VarcharType>(expr.m_column_type).m_pattern
			)
		);
	}else if (std::holds_alternative<IntType>(expr.m_column_type)) {
		m_generator.m_columns.emplace_back (
			IntColumn (
				expr.m_name,
				std::get<IntType>(expr.m_column_type).m_start,
				std::get<IntType>(expr.m_column_type).m_end
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
	m_generator.count = expr.m_count;
}

void Interpreter::execute(Stmnt &stmnt) {
	stmnt.accept(*this);
}

void Interpreter::evaluate(Expr &expr) {
	expr.accept(*this);
}
