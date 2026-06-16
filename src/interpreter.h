#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <common.h>
#include <stmnt.h>
#include <interpret_error.h>
#include <environment.h>
#include <unordered_map>
#include <fstream>

using CachedData = std::unordered_map<TokenType, std::vector<std::string>>;

class Interpreter : public StmntVisitor, public ExprVisitor {
public:
    Interpreter(std::string output);
    void interpret(std::vector<std::unique_ptr<Stmnt>> statements);
    ~Interpreter() = default;
private:
	void visitTableStmnt(TableStmnt &stmnt) override;
	void visitBlockStmnt(BlockStmnt &stmnt) override;
    void visitLocaleStmnt(LocaleStmnt &stmnt) override;
    void visitColumnStmnt(ColumnStmnt &stmnt) override;
    void visitDeclStmnt(DeclStmnt &stmnt) override;
    void visitPrintStmnt(PrintStmnt &stmnt) override;
    void visitAssignStmnt(AssignStmnt &stmnt) override;

    void visitIncrementExpr(IncrementExpr &expr) override;
    void visitRandintExpr(RandintExpr &expr) override;
    void visitRandboolExpr(RandboolExpr &expr) override;
    void visitRanddoubleExpr(RanddoubleExpr &expr) override;
    void visitGenExpr(GenExpr &expr) override;
    void visitFormatExpr(FormatExpr &expr) override;
    void visitValueExpr(ValueExpr &expr) override;
    void visitVariableExpr(VariableExpr &expr) override;
    void visitListExpr(ListExpr &expr) override;
    void visitFuncExpr(FuncExpr &expr) override;
    void visitAtFuncExpr(AtFuncExpr &expr) override;
    void visitSubstrFuncExpr(SubstrFuncExpr &expr) override;
    void visitLowerFuncExpr(LowerFuncExpr &expr) override;
    void visitUpperFuncExpr(UpperFuncExpr &expr) override;

    void execute(Stmnt &stmnt);
    void evaluate(Expr &expr);
    const Value pop();
    const std::string pop_as_str();
    int generate_random_int(int start, int end);
    bool generate_random_bool(double weight);
    double generate_random_double(double start, double end);
    std::string cache_data(CachedData &cached_data, std::string &&file_name, TokenType type);
    std::string build_query(const std::string &table_name);
    void write(const std::string &content);
    bool is_string(const Value &value);
    bool is_int(const Value &value); 
    bool is_bool(const Value &value);
    bool is_list(const Value &value);
    [[noreturn]]
    void error(const std::string &message);
private:
    std::ofstream m_output_file;
    std::string m_locale {};
    std::vector<std::string> m_column_names {};
    std::vector<Value> m_column_data {};
    std::vector<std::unique_ptr<Environment>> m_envs {};
    std::size_t m_current_env { 0 };
};

#endif
