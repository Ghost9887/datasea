#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <common.h>
#include <stmnt.h>
#include <generator.h>
#include <interpret_error.h>

class Interpreter : public StmntVisitor, public ExprVisitor {
public:
	Interpreter(std::vector<std::unique_ptr<Stmnt>> statements, Generator &generator);
	void interpret();
	~Interpreter() = default;
private:
	void visitTableStmnt(TableStmnt &stmnt) override;
	void visitBlockStmnt(BlockStmnt &stmnt) override;
    void visitLocaleStmnt(LocaleStmnt &stmnt) override;
	void visitColumnExpr(ColumnExpr &expr) override;
	void visitCountExpr(CountExpr &expr) override;
	void execute(Stmnt &stmnt);
	void evaluate(Expr &expr);
    std::vector<std::vector<std::string>> parse_pattern(std::string &pattern);
    std::vector<std::string> read_data(const std::string &key);
private:
	std::vector<std::unique_ptr<Stmnt>> m_statements;
	Generator &m_generator;
};

#endif
