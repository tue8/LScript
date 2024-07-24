#include <vector>
#include <any>
#include <iostream>
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
    std::any returnValue;
    Environment closureClone = *closure;
    Environment funcEnvironment = Environment(&closureClone);

    for (int i = 0; i < params.size(); i++)
      funcEnvironment.define(params.at(i).lexeme,
                         args.at(i));
 
    try
    {
      interpreter.executeBlock((laDeclaration != nullptr) ? laDeclaration->getBody()
                                                          :   declaration->getBody(),
                                                          funcEnvironment);
    }
    catch (std::any ret)
    {
      /*
       * took me just a couple fucking hours to realize i had
       * to set the environment back to normal after returning
       */
      returnValue = ret;
    }
    interpreter.setEnv(closureClone);
    return returnValue;
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