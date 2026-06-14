#ifndef STMNT_H
#define STMNT_H

#include <common.h>
#include <expr.h>
#include <vector>
#include <memory>

class TableStmnt;
class BlockStmnt;
class LocaleStmnt;

class StmntVisitor {
public:
	virtual void visitTableStmnt(TableStmnt &stmnt) = 0;
	virtual void visitBlockStmnt(BlockStmnt &stmnt) = 0;
    virtual void visitLocaleStmnt(LocaleStmnt &stmnt) = 0;
};

class Stmnt {
public:
	virtual void accept(StmntVisitor &visitory) = 0;
	virtual std::string to_string() const = 0;
	virtual ~Stmnt() = default;
};

class TableStmnt : public Stmnt {
public:
	TableStmnt(std::string name, std::unique_ptr<Stmnt> body);
	void accept(StmntVisitor &visitor) override;
	std::string to_string() const override;
public:
	std::string m_name;
	std::unique_ptr<Stmnt> m_body;
};

class BlockStmnt : public Stmnt {
public:
	BlockStmnt(std::vector<std::unique_ptr<Expr>> expressions);
	void accept(StmntVisitor &visitor) override;
	std::string to_string() const override;
public:
	std::vector<std::unique_ptr<Expr>> m_expressions;
};

class LocaleStmnt : public Stmnt {
public:
    LocaleStmnt(std::string locale);
    void accept(StmntVisitor &visitor) override;
    std::string to_string() const override;
public:
    std::string m_locale;
};

#endif
