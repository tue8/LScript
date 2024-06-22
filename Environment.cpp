#include "Environment.h"

std::any Environment::get(const Token& name)
{
  if (values.find(name.lexeme) != values.end())
    return values.find(name.lexeme)->second;

  if (enclosing != nullptr)
    return enclosing->get(name);

  throw (std::make_pair(name, std::string("Undefined variable: ").append(name.lexeme)));
}

void Environment::define(std::string name, std::any value)
{
  values[name] = value;
}

void Environment::assign(const Token& name, std::any value)
{
  if (values.find(name.lexeme) != values.end())
  {
    values[name.lexeme] = value;
    return;
  }

  if (enclosing != nullptr)
  {
    enclosing->assign(name, value);
    return;
  }

  throw (std::make_pair(name, std::string("Undefined variable: ").append(name.lexeme)));
}