#ifndef STMNT_H
#define STMNT_H

#include <common.h>
#include <expr.h>
#include <vector>
#include <memory>
#include <unordered_map>

class TableStmnt;
class BlockStmnt;
class LocaleStmnt;
class ColumnStmnt;
class DeclStmnt;
class PrintStmnt;
class AssignStmnt;

class StmntVisitor {
public:
	virtual void visitTableStmnt(TableStmnt &stmnt) = 0;
	virtual void visitBlockStmnt(BlockStmnt &stmnt) = 0;
    virtual void visitLocaleStmnt(LocaleStmnt &stmnt) = 0;
    virtual void visitColumnStmnt(ColumnStmnt &stmnt) = 0;
    virtual void visitDeclStmnt(DeclStmnt &stmnt) = 0;
    virtual void visitPrintStmnt(PrintStmnt &stmnt) = 0;
    virtual void visitAssignStmnt(AssignStmnt &stmnt) = 0;
    virtual ~StmntVisitor() = default;
};

class Stmnt {
public:
	virtual void accept(StmntVisitor &visitory) = 0;
	virtual std::string to_string() const = 0;
	virtual ~Stmnt() = default;
};

class TableStmnt : public Stmnt {
public:
	TableStmnt(std::string name, int row_count, std::unique_ptr<Stmnt> body);
	void accept(StmntVisitor &visitor) override;
	std::string to_string() const override;
public:
	std::string m_name;
    int m_row_count;
	std::unique_ptr<Stmnt> m_body;
};

class BlockStmnt : public Stmnt {
public:
	BlockStmnt(std::vector<std::unique_ptr<Stmnt>> statemenets);
	void accept(StmntVisitor &visitor) override;
	std::string to_string() const override;
public:
	std::vector<std::unique_ptr<Stmnt>> m_statements;
};

class LocaleStmnt : public Stmnt {
public:
    LocaleStmnt(std::string locale);
    void accept(StmntVisitor &visitor) override;
    std::string to_string() const override;
public:
    std::string m_locale;
};

class ColumnStmnt : public Stmnt {
public:
	ColumnStmnt(std::string name, std::unique_ptr<Expr> parameter);
	void accept(StmntVisitor &visitor) override;
	std::string to_string() const override;
public:
	std::string m_name;
    std::unique_ptr<Expr> m_parameter;
};

class DeclStmnt : public Stmnt {
public:
    DeclStmnt(std::string name, std::unique_ptr<Expr> expr);
    void accept(StmntVisitor &visitor) override;
    std::string to_string() const override;
public:
    std::string m_name;
    std::unique_ptr<Expr> m_expr;
};

class PrintStmnt : public Stmnt {
public:
    PrintStmnt(std::unique_ptr<Expr> expr);
    void accept(StmntVisitor &visitor) override;
    std::string to_string() const override;
public:
    std::unique_ptr<Expr> m_expr;
};

class AssignStmnt : public Stmnt {
public:
    AssignStmnt(std::string name, std::unique_ptr<Expr> expr);
    void accept(StmntVisitor &visitor) override;
    std::string to_string() const override;
public:
    std::string m_name;
    std::unique_ptr<Expr> m_expr;
};

#endif
