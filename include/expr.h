#ifndef EXPR_H
#define EXPR_H

#include <common.h>
#include <token.h>
#include <vector>
#include <memory>

class IncrementExpr;
class RandomExpr;
class GenExpr;
class FormatExpr;

class ExprVisitor {
public:
    virtual void visitIncrementExpr(IncrementExpr &expr) = 0;
    virtual void visitRandomExpr(RandomExpr &expr) = 0;
    virtual void visitGenExpr(GenExpr &expr) = 0;
    virtual void visitFormatExpr(FormatExpr &expr) = 0;
	virtual ~ExprVisitor() = default;
};

class Expr {
public:
	virtual void accept(ExprVisitor &visitor) = 0;
	virtual std::string to_string() const = 0;
	virtual ~Expr() = default;
};

class IncrementExpr : public Expr {
public:
    IncrementExpr(int start, std::optional<int> end);
    void accept(ExprVisitor &visitor) override;
    std::string to_string() const override;
public:
    int m_start;
    std::optional<int> m_end;
    int m_counter;
};

class RandomExpr : public Expr {
public:
    RandomExpr(int start, std::optional<int> end);
    void accept(ExprVisitor &visitor) override;
    std::string to_string() const override;
public:
    int m_start;
    std::optional<int> m_end;
};

class GenExpr : public Expr {
public:
    GenExpr(TokenType type);
    void accept(ExprVisitor &visitor) override;
    std::string to_string() const override;
public:
    TokenType m_type;
};

class FormatExpr : public Expr {
public:
    FormatExpr(std::string pattern, std::vector<std::unique_ptr<Expr>> variables);
    void accept(ExprVisitor &visitor) override;
    std::string to_string() const override;
public:
    std::string m_pattern;
    std::vector<std::unique_ptr<Expr>> m_variables;
};

#endif
