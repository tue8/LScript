#include "Interpreter.h"
#include <iostream>
#include <utility>

#define checkNumbers(oprtor, l, r, oprand) \
checkNumberOperands(oprtor, l, r); \
return std::any_cast<double>(l) oprand std::any_cast<double>(r) \

static void error(Token token, std::string msg)
{
  if (token.type == _EOF_)
    std::cerr << "[" << token.line << "] at end: " << msg << std::endl;
  else
    std::cerr << "[" << token.line << "] at '" << token.lexeme << "': " << msg << std::endl;
}

bool isEqual(std::any a, std::any b)
{
  if (a.type() == typeid(void) &&
      b.type() == typeid(void))
    return true;
  if (a.type() == typeid(void))
    return false;

  if (a.type() == typeid(bool))
    return (std::any_cast<bool>(a) == std::any_cast<bool>(b));
  if (a.type() == typeid(double))
    return (std::any_cast<double>(a) == std::any_cast<double>(b));
  if (a.type() == typeid(std::string))
    return (std::any_cast<std::string>(a) == std::any_cast<std::string>(b));

  return false;
}

static bool isTruthy(std::any anythang)
{
  if (anythang.type() == typeid(void))
    return false;
  if (anythang.type() == typeid(bool))
    return std::any_cast<bool>(anythang);
  return true;
}

static void checkNumberOperand(const Token& op, std::any operand)
{
  if (operand.type() == typeid(double))
    return;
  throw std::make_pair(op, std::string("Operand must be a number."));
}

static void checkNumberOperands(const Token& op, std::any left, std::any right)
{
  if (left.type() == typeid(double) &&
      right.type() == typeid(double))
    return;
  throw std::make_pair(op, std::string("Operands must be numbers."));
}

static std::string stringify(std::any value)
{
  if (value.type() == typeid(bool))
    return std::any_cast<bool>(value) ? "true" : "false";
  if (value.type() == typeid(void))
    return "nil";
  if (value.type() == typeid(double))
    return std::to_string(std::any_cast<double>(value));
  return std::any_cast<std::string>(value);
}

void Interpreter::interpret(std::list<Stmt*> statements)
{
  try
  {
    while (!statements.empty())
    {
      if (statements.front() != nullptr)
        execute(*(statements.front()));
      statements.pop_front();
    }
  }
  catch (std::pair<Token, std::string>& tokStr)
  {
    error(tokStr.first, tokStr.second);
  }
}

/* I fucking hate myself */
std::any Interpreter::visitWhileStmt(While& stmt)
{
  Expr& condition = (Expr&)stmt.getCondition();
  std::any val = condition.accept(*this);
  while (isTruthy(val))
  {
    Stmt& body = (Stmt&)stmt.getBody();
    body.accept(*this);
    condition = (Expr&)stmt.getCondition();
    val = condition.accept(*this);
  }
  return std::any();
}

std::any Interpreter::visitIfStmt(If& stmt)
{
  /*
   * we cant call stmt.getEle() 2 times since after we call it the first time
   * the elseBranch in the class becomes null from ), we "moved" it somewhere else.
   */
  Stmt& elseBranch = stmt.getElse();
  if (isTruthy(evaluate(stmt.getCondition())))
    execute(stmt.getThen());
  else if (&elseBranch != nullptr)
    execute(elseBranch);
  return std::any();
}

std::any Interpreter::visitExpressionStmt(Expression& stmt)
{
  evaluate(stmt.getExpr());
  return std::any();
}

std::any Interpreter::visitPrintStmt(Print& stmt)
{
  std::cout << stringify(evaluate(stmt.getExpr())) << std::endl;
  return std::any();
}

std::any Interpreter::visitVarStmt(Var& stmt)
{
  std::any value;
  Expr& initializer = stmt.getInitializer();
  if (&initializer != nullptr)
    value = evaluate(initializer);
  env.define(stmt.getName().lexeme, value);
  return std::any();
}

std::any Interpreter::visitBlockStmt(Block& stmt)
{
  Environment prev = this->env;
  this->env = Environment(&prev);

  // First reference (&) to the std::unique_ptr avoids the copying
  // Second reference so you can use the std::unique_ptr without dereferencing
  for (const auto& statement : stmt.getStatements())
    if (statement != nullptr)
      execute(*statement);
  this->env = prev;
  return std::any();
}

std::any Interpreter::evaluate(Expr& expr)
{
  return expr.accept(*this);
}

std::any Interpreter::execute(Stmt& stmt)
{
  return stmt.accept(*this);
}

std::any Interpreter::visitLogicalExpr(Logical& expr)
{
  std::any left = evaluate(expr.getLeft());
  if (expr.getOp().type == OR)
  {
    if (isTruthy(left))
    {
      return left;
    }
  }
  else
  {
    if (!isTruthy(left))
    {
      return left;
    }
  }

  return evaluate(expr.getRight());
}

std::any Interpreter::visitBinaryExpr(Binary& expr)
{
  Expr& leftExpr = (Expr&)expr.getLeft();
  Expr& rightExpr = (Expr&)expr.getRight();
  std::any left = leftExpr.accept(*this);
  std::any right = rightExpr.accept(*this);

  switch (expr.getOp().type)
  {
  case BANG_EQUAL:    return !isEqual(left, right);
  case EQUAL_EQUAL:   return isEqual(left, right);
    /* evil shit */
  case GREATER:       checkNumbers(expr.getOp(), left, right, >);
  case GREATER_EQUAL: checkNumbers(expr.getOp(), left, right, >=);
  case LESS:          checkNumbers(expr.getOp(), left, right, <);
  case LESS_EQUAL:    checkNumbers(expr.getOp(), left, right, <=);
  case MINUS:         checkNumbers(expr.getOp(), left, right, -);
  case STAR:          checkNumbers(expr.getOp(), left, right, *);
  case SLASH:
    checkNumberOperands(expr.getOp(), left, right);
    if (std::any_cast<double>(right) == 0)
      throw std::make_pair(expr.getOp(), std::string("Check your math big man!! you cant divide a number by 0"));
    return std::any_cast<double>(left) / std::any_cast<double>(right);
  case PLUS:
    if (left.type() == typeid(double) && right.type() == typeid(double))
      return std::any_cast<double>(left) + std::any_cast<double>(right);
    if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
      return std::any_cast<std::string>(left).append(std::any_cast<std::string>(right));
    if (left.type() == typeid(std::string) && right.type() == typeid(double))
      return std::any_cast<std::string>(left).append(std::to_string(std::any_cast<double>(right)));
    if (left.type() == typeid(double) && right.type() == typeid(std::string))
      return std::to_string(std::any_cast<double>(left)).append(std::any_cast<std::string>(right));
    throw std::make_pair(expr.getOp(), std::string("Operands must be FUCKING NUMBERS or FUCKING STRINGS"));
  }

  return std::any();
}

std::any Interpreter::visitGroupingExpr(Grouping& expr)
{
  return evaluate(expr.getExpr());
}

std::any Interpreter::visitLiteralExpr(Literal& expr)
{
  return expr.get_lit();
}

std::any Interpreter::visitUnaryExpr(Unary& expr)
{
  std::any right = evaluate(expr.getRight());

  switch (expr.getOp().type)
  {
  case BANG:
    return !isTruthy(right);
  case MINUS:
    return -std::any_cast<double>(right);
  }

  return std::any(nullptr);
}

std::any Interpreter::visitVariableExpr(Variable& expr)
{
  return env.get(expr.getName());
}

std::any Interpreter::visitAssignExpr(Assign& expr)
{
  std::any lit = evaluate(expr.getValue());
  env.assign(expr.getName(), lit);
  return lit;
}
