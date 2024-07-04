#pragma once

#include <memory>
#include <any>
#include <string>
#include "Expr.h"
#include "Environment.h"
#include <vector>

class Return;
class Function;
class Break;
class Continue;
class If;
class Block;
class Expression;
class Print;
class Var;
class While;

template <typename T>
class StmtVisitor
{
public:
	virtual T visitReturnStmt(Return& stmt) = 0;
	virtual T visitFunctionStmt(Function& stmt) = 0;
	virtual T visitBreakStmt(Break& stmt) = 0;
	virtual T visitContinueStmt(Continue& stmt) = 0;
	virtual T visitIfStmt(If& stmt) = 0;
	virtual T visitBlockStmt(Block& stmt) = 0;
	virtual T visitExpressionStmt(Expression& stmt) = 0;
	virtual T visitPrintStmt(Print& stmt) = 0;
	virtual T visitVarStmt(Var& stmt) = 0;
	virtual T visitWhileStmt(While& stmt) = 0;
};

class Stmt
{
public:
	virtual std::any accept(StmtVisitor<std::any>& visitor) = 0;
};

class Return : public Stmt
{
public:
	Return(const Token& token, std::unique_ptr<Expr> value)
		: token(token),
			value(std::move(value))
	{}

	std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitReturnStmt(*this);
	}

	const Token& getToken()
	{
		return token;
	}

	Expr& getValue()
	{
		return *value;
	}
private:
	Token token;
	std::unique_ptr<Expr> value;
};


class Function : public Stmt
{
public:
	Function(const Token& name, std::vector<Token> params, std::vector<std::unique_ptr<Stmt>> body)
  : name(name), params(params), body(std::move(body))
  {}

  std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitFunctionStmt(*this);
	}

  const Token& getName()
	{
		return name;
	}

	const std::vector<Token>& getParams()
	{
		return params;
	}

	const std::vector<std::unique_ptr<Stmt>>& getBody()
	{
		return body;
	}
private:
  Token name;
  std::vector<Token> params;
  std::vector<std::unique_ptr<Stmt>> body;
};

class Break : public Stmt
{
public:
  std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitBreakStmt(*this);
	}
};

class Continue : public Stmt
{
public:
  std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitContinueStmt(*this);
	}
};

class If : public Stmt
{
public:
	If(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
		: condition(std::move(condition)),
			thenBranch(std::move(thenBranch)),
			elseBranch(std::move(elseBranch))
	{}

	std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitIfStmt(*this);
	}

	Expr& getCondition()
	{
		return *condition;
	}

	Stmt& getThen()
	{
		return *thenBranch;
	}

	Stmt& getElse()
	{
		return *elseBranch;
	}

  bool hasElse()
  {
    return (elseBranch != nullptr);
  }
private:
	std::unique_ptr<Expr> condition;
	std::unique_ptr<Stmt> thenBranch;
	std::unique_ptr<Stmt> elseBranch;
};


class Block : public Stmt
{
public:
	Block(std::vector<std::unique_ptr<Stmt>> statements)
		: statements(std::move(statements))
	{}

	std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitBlockStmt(*this);
	}

	const std::vector<std::unique_ptr<Stmt>>& getStatements()
	{
		return statements;
	}
private:
	std::vector<std::unique_ptr<Stmt>> statements;
};


class Expression : public Stmt
{
public:
	Expression(std::unique_ptr<Expr> expression)
		: expression(std::move(expression))
	{}

	std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitExpressionStmt(*this);
	}

	Expr& getExpr()
	{
		return *expression;
	}

private:
	std::unique_ptr<Expr> expression;
};

class Print : public Stmt
{
public:
	Print(std::unique_ptr<Expr> expression)
		: expression(std::move(expression))
	{}

	std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitPrintStmt(*this);
	}

	Expr& getExpr()
	{
		return *expression;
	}

private:
	std::unique_ptr<Expr> expression;
};

class Var : public Stmt
{
public:
	Var(const Token& name, std::unique_ptr<Expr> initializer)
		: name(name), initializer(std::move(initializer))
	{}

	std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitVarStmt(*this);
	}

	Token getName()
	{
		return name;
	}

	Expr& getInitializer()
	{
		return *initializer;
	}
  
  bool hasInitializer()
  {
    return (initializer != nullptr);
  }

private:
	Token name;
	std::unique_ptr<Expr> initializer;
};

class While : public Stmt
{
public:
	While(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
		: condition(std::move(condition)),
			body(std::move(body))
	{}

	std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitWhileStmt(*this);
	}

	Expr& getCondition()
	{
		return *condition;
	}

	Stmt& getBody()
	{
		return *body;
	}
private:
	std::unique_ptr<Expr> condition;
	std::unique_ptr<Stmt> body;
};

class Lambda : public Expr
{
public:
	Lambda(std::vector<Token> params, std::vector<std::unique_ptr<Stmt>> body)
		: params(params), body(std::move(body))
	{}

	std::any accept(ExprVisitor<std::any>& visitor) override
	{
		return visitor.visitLambdaExpr(*this);
	}

	const std::vector<Token>& getParams()
	{
		return params;
	}

	const std::vector<std::unique_ptr<Stmt>>& getBody()
	{
		return body;
	}
private:
	std::vector<Token> params;
	std::vector<std::unique_ptr<Stmt>> body;
};