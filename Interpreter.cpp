#include "Interpreter.h"
#include "Callable.h"
#include <iostream>
#include <utility>

#define checkNumbers(oprtor, l, r, oprand) \
checkNumberOperands(oprtor, l, r); \
return std::any_cast<double>(l) oprand std::any_cast<double>(r) \

static void error(Token token, std::string msg)
{
  if (token.type == _EOF_)
    std::cerr << "INTERPRETER ERROR: [" << token.line << "] at end: " << msg << std::endl;
  else
    std::cerr << "INTERPRETER ERROR: [" << token.line << "] at '" << token.lexeme << "': " << msg << std::endl;
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

void Interpreter::interpret(std::list<std::unique_ptr<Stmt>> statements)
{
  try
  {
    for (auto& statement : statements)
    {
      execute(*(statement));
    }
    
    // statements.pop_front() deletes parts of the trees so yeah pretty fucking terrible...
    // Don't ever do that.
  }
  catch (std::pair<Token, std::string>& tokStr)
  {
    error(tokStr.first, tokStr.second);
  }
}

void Interpreter::setEnv(const Environment& env)
{
  environment = env;
}

Environment Interpreter::getEnv()
{
  return environment;
}

std::any Interpreter::visitReturnStmt(Return& stmt)
{
  std::any returnValue = evaluate(stmt.getValue());
  throw returnValue;
}

std::any Interpreter::visitFunctionStmt(Function& stmt)
{
  environment.define(stmt.getName().lexeme, Callable(&stmt, &this->environment));
  return std::any();
}

std::any Interpreter::visitBreakStmt(Break& stmt)
{
  throw BREAK;
}

std::any Interpreter::visitContinueStmt(Continue& stmt)
{
  throw CONTINUE;
}

std::any Interpreter::visitWhileStmt(While& stmt)
{
  while (isTruthy(evaluate(stmt.getCondition())))
  {
    try
    {
      execute(stmt.getBody());
    }
    catch (TokenType loopSignal)
    {
      if (loopSignal == BREAK)         break;
      else if (loopSignal == CONTINUE) continue;
    }
  }

  return std::any();
}

std::any Interpreter::visitIfStmt(If& stmt)
{
  if (isTruthy(evaluate(stmt.getCondition())))
    execute(stmt.getThen());
  else if (stmt.hasElse())
    execute(stmt.getElse());
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
  if (stmt.hasInitializer())
    value = evaluate(initializer);
  environment.define(stmt.getName().lexeme, value);
  return std::any();
}

std::any Interpreter::visitBlockStmt(Block& stmt)
{
  executeBlock(stmt.getStatements(), Environment(this->environment));
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

 void Interpreter::executeBlock(const std::vector<std::unique_ptr<Stmt>>& statements, Environment env)
{
  Environment prev = this->environment;
  /* creates a new environment that 'inherits' the values of env */
  this->environment = env;

  // Reference (&) to the std::unique_ptr avoids the copying
  for (const auto& statement : statements)
  {
    if (statement != nullptr)
    {
      execute(*statement);
    }
  }
  this->environment = prev;
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
  std::any left = evaluate(expr.getLeft());
  std::any right = evaluate(expr.getRight());

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
  return expr.getLit();
}

std::any Interpreter::visitCallExpr(Call& expr)
{
  /* lookup function from variable */
  std::any callee = evaluate(expr.getCallee());

  std::vector<std::any> args;
  for (const auto& arg : expr.getArgs())
  {
    args.push_back(evaluate(*arg));
  }

  if (callee.type() != typeid(Callable))
    throw std::make_pair(expr.getParen(), std::string("Object called is not a function")); 

  if (args.size() != std::any_cast<Callable>(callee).getArity())
    throw std::make_pair(expr.getParen(), std::string("Invalid number of arguments")); 
  return std::any_cast<Callable>(callee).call(*this, args);
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
  return environment.get(expr.getName());
}

std::any Interpreter::visitAssignExpr(Assign& expr)
{
  std::any lit = evaluate(expr.getValue());
  environment.assign(expr.getName(), lit);
  return lit;
}

std::any Interpreter::visitLambdaExpr(Lambda& expr)
{
  return Callable(&expr, &this->environment);
}