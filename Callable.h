#include <vector>
#include <any>
#include "Environment.h"
#include "Interpreter.h"

class Callable
{
public:
  Callable(Lambda* laDeclaration, Environment* closure)
    : laDeclaration(laDeclaration), closure(closure)
  {
    params = laDeclaration->getParams();
  }

  Callable(Function* declaration, Environment *closure)
    : declaration(declaration), closure(closure)
  {
    params = declaration->getParams();
  }

  std::any call(Interpreter& interpreter, const std::vector<std::any>& args)
  {
    Environment closureClone = *closure;
    Environment environment = Environment(&closureClone);



    for (int i = 0; i < params.size(); i++)
      environment.define(params.at(i).lexeme,
                         args.at(i));
 
    try
    {
      interpreter.executeBlock((laDeclaration != nullptr) ? laDeclaration->getBody()
                                                          :   declaration->getBody(),
                                environment);
    }
    catch (std::any returnValue)
    {
      return returnValue;
    }
    return std::any();
  }

  int getArity()
  {
    return params.size();
  }
private:
  Lambda* laDeclaration = nullptr;
  Function* declaration = nullptr;
  std::vector<Token> params;
  Environment *closure = nullptr;
};