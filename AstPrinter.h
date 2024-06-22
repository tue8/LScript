#include "Expr.h"

class AstPrinter : public ExprVisitor<std::any>
{
public:
	std::any print(Expr& expr);
	std::any visitBinaryExpr(Binary &expr) override;
	std::any visitGroupingExpr(Grouping &expr) override;
	std::any visitLiteralExpr(Literal &expr) override;
	std::any visitUnaryExpr(Unary &expr) override;
};