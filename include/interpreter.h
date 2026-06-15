#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <common.h>
#include <stmnt.h>
#include <interpret_error.h>
#include <unordered_map>
#include <fstream>

using CachedData = std::unordered_map<TokenType, std::vector<std::string>>;

class Interpreter : public StmntVisitor, public ExprVisitor {
public:
    Interpreter(std::string output, std::vector<std::unique_ptr<Stmnt>> statements);
    void interpret();
    ~Interpreter() = default;
private:
    void visitRowStmnt(RowStmnt &stmnt) override;
	void visitTableStmnt(TableStmnt &stmnt) override;
	void visitBlockStmnt(BlockStmnt &stmnt) override;
    void visitLocaleStmnt(LocaleStmnt &stmnt) override;
    void visitColumnStmnt(ColumnStmnt &stmnt) override;
    void visitDeclStmnt(DeclStmnt &stmnt) override;
    void visitPrintStmnt(PrintStmnt &stmnt) override;
    void visitIncrementExpr(IncrementExpr &expr) override;
    void visitRandomExpr(RandomExpr &expr) override;
    void visitGenExpr(GenExpr &expr) override;
    void visitFormatExpr(FormatExpr &expr) override;
    void visitValueExpr(ValueExpr &expr) override;
    void visitVariableExpr(VariableExpr &expr) override;
    void execute(Stmnt &stmnt);
    void evaluate(Expr &expr);
    const std::string pop();
    int generate_random_int(int start, int end);
    std::string cache_data(CachedData &cached_data, std::string &&file_name, TokenType type);
    std::string build_query(const std::string &table_name);
    void write(const std::string &content);
    [[noreturn]]
    void error(const std::string &message);
private:
    std::ofstream m_output_file;
    std::vector<std::unique_ptr<Stmnt>> m_statements;
    std::string m_locale {};
    std::vector<std::string> m_column_names {};
    std::vector<Value> m_column_data {};
    std::unordered_map<std::string, Value> m_variables {};
};

#endif
