#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <common.h>
#include <stmnt.h>
#include <generator.h>

class Interpreter : public StmntVisitor, public ExprVisitor {
public:
	Interpreter(std::vector<std::unique_ptr<Stmnt>> statements, Generator &generator);
	void interpret();
	~Interpreter() = default;
private:
	void visitTableStmnt(TableStmnt &stmnt) override;
	void visitBlockStmnt(BlockStmnt &stmnt) override;
	void visitColumnExpr(ColumnExpr &expr) override;
	void visitCountExpr(CountExpr &expr) override;
	void execute(Stmnt &stmnt);
	void evaluate(Expr &expr);
private:
	std::vector<std::unique_ptr<Stmnt>> m_statements;
	Generator &m_generator;
};

#endif
