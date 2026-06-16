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
class ValueExpr;
class VariableExpr;
class ListExpr;
class FuncExpr;
class AtFuncExpr;
class SubstrFuncExpr;
class LowerFuncExpr;
class UpperFuncExpr;

class ExprVisitor {
public:
    virtual void visitIncrementExpr(IncrementExpr &expr) = 0;
    virtual void visitRandomExpr(RandomExpr &expr) = 0;
    virtual void visitGenExpr(GenExpr &expr) = 0;
    virtual void visitFormatExpr(FormatExpr &expr) = 0;
    virtual void visitValueExpr(ValueExpr &expr) = 0;
    virtual void visitVariableExpr(VariableExpr &expr) = 0;
    virtual void visitListExpr(ListExpr &expr) = 0;
    virtual void visitFuncExpr(FuncExpr &expr) = 0;
    virtual void visitAtFuncExpr(AtFuncExpr &expr) = 0;
    virtual void visitSubstrFuncExpr(SubstrFuncExpr &expr) = 0;
    virtual void visitLowerFuncExpr(LowerFuncExpr &expr) = 0;
    virtual void visitUpperFuncExpr(UpperFuncExpr &expr) = 0;
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

class ValueExpr : public Expr {
public:
    ValueExpr(Value value);
    void accept(ExprVisitor &visitor) override;
    std::string to_string() const override;
public:
    Value m_value;
};

class VariableExpr : public Expr {
public:
    VariableExpr(std::string name, std::vector<std::unique_ptr<Expr>> m_functions);
    void accept(ExprVisitor &visitor) override;
    std::string to_string() const override;
public:
    std::string m_name;
    std::vector<std::unique_ptr<Expr>> m_functions;
};

class ListExpr : public Expr {
public:
    ListExpr(std::vector<std::unique_ptr<Expr>> expressions);
    void accept(ExprVisitor &visitor) override;
    std::string to_string() const override;
public:
    std::vector<std::unique_ptr<Expr>> m_expressions;
};

//built in functions
class FuncExpr : public Expr {
public:
    FuncExpr(std::unique_ptr<Expr> function);
    void accept(ExprVisitor &visitor) override;
    std::string to_string() const override;
public:
    std::unique_ptr<Expr> m_function;
};

class AtFuncExpr : public Expr {
public:
    AtFuncExpr(int index);
    void accept(ExprVisitor &visitor) override;
    std::string to_string() const override;
public:
    int m_index;
};

class SubstrFuncExpr : public Expr {
public:
    SubstrFuncExpr(int start, int end);
    void accept(ExprVisitor &visitor) override;
    std::string to_string() const override;
public:
    int m_start;
    int m_end;
};

class LowerFuncExpr : public Expr {
public:
    LowerFuncExpr() = default;
    void accept(ExprVisitor &visitor) override;
    std::string to_string() const override;
};

class UpperFuncExpr : public Expr {
public:
    UpperFuncExpr() = default;
    void accept(ExprVisitor &visitor) override;
    std::string to_string() const override;
};

#endif
