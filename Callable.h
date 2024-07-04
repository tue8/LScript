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
    std::unique_ptr<Environment> closureClone(new Environment(*closure));
    Environment funcEnvironment = Environment(closureClone.get());

    for (int i = 0; i < params.size(); i++)
      funcEnvironment.define(params.at(i).lexeme,
                         args.at(i));
 
    Environment currEnvironment = interpreter.getEnv();
    try
    {
      interpreter.executeBlock((laDeclaration != nullptr) ? laDeclaration->getBody()
                                                          :   declaration->getBody(),
                                                          funcEnvironment);
    }
    catch (std::any returnValue)
    {
      /*
       * took me just a couple fucking hours to realize i had
       * to set the environment back to normal after returning
       */
      interpreter.setEnv(currEnvironment);
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