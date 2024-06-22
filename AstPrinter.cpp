#include "AstPrinter.h"
#include <sstream>

std::any AstPrinter::print(Expr& expr)
{
  return expr.accept(*this);
}

std::any AstPrinter::visitBinaryExpr(Binary &expr)
{
  std::stringstream ss;
  ss << "(" << expr.getOp().lexeme << " " << std::any_cast<std::string>(expr.getLeft()->accept(*this));
  ss << " " << std::any_cast<std::string>(expr.getRight()->accept(*this)) << ")";
  return ss.str();
}

std::any AstPrinter::visitGroupingExpr(Grouping &expr)
{
  std::stringstream ss;
  ss << "(group " << std::any_cast<std::string>(expr.getExpr()->accept(*this)) << ")";
  return ss.str();
}

std::any AstPrinter::visitLiteralExpr(Literal &expr)
{
  std::any lit = expr.get_lit();
  if (lit.type() == typeid(double))
    return std::to_string(std::any_cast<double>(lit));
  if (lit.type() == typeid(std::string))
    return std::any_cast<std::string>(lit);
  return std::string();
}

std::any AstPrinter::visitUnaryExpr(Unary &expr)
{
  std::stringstream ss;
  ss << "(" << expr.getOp().lexeme << " " << std::any_cast<std::string>(expr.getRight()->accept(*this)) << ")";
  return ss.str();
}