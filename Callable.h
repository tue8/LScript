#include <vector>
#include <any>
#include "Environment.h"
#include "Interpreter.h"

class Callable
{
public:
  Callable(Function* declaration, Environment *closure)
    : declaration(declaration), closure(closure)
  {}

  std::any call(Interpreter& interpreter, const std::vector<std::any>& args)
  {
    Environment closureClone = *closure;
    Environment environment = Environment(&closureClone);
    for (int i = 0; i < declaration->getParams().size(); i++)
      environment.define(declaration->getParams().at(i).lexeme,
                         args.at(i));
 
    try
    {
      interpreter.executeBlock(declaration->getBody(), environment);
    }
    catch (std::any returnValue)
    {
      return returnValue;
    }
    return std::any();
  }

  int getArity()
  {
    return declaration->getParams().size();
  }
private:
  Function *declaration;
  Environment *closure;
};