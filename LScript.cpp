#include <iostream>
#include <fstream>
#include <sstream>

#include "AstPrinter.h"
#include "Parser.h"
#include "Lexer.h"
#include "Interpreter.h"

Interpreter interpreter;

static void run(std::string str)
{
	Lexer lexer = Lexer(str);
	std::vector<Token> tokens = lexer.lexAll();
	Parser parser = Parser(tokens);
	std::list<Stmt*> stmt_list = parser.parse();
	if (!stmt_list.empty())
		interpreter.interpret(stmt_list);
}

static int runFile(char *script_name)
{
	std::ifstream fileStream(script_name);
	std::ostringstream oss;
	oss << fileStream.rdbuf();
	std::string fileStr = oss.str();
	if (fileStr.empty())
		return 1;
	run(fileStr);
	return 0;
}

static int runPrompt()
{
	for (;;)
	{
		std::string line;
		std::cout << "> ";
		std::getline(std::cin, line);
		if (!line.empty())
			run(line);
	}
	return 0;
}

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cerr << "Usage: LScript [script]" << std::endl;
		return 1;
	}

#ifdef LDEBUG
	return runFile((char *)"../scripts/script.ls");
#else
	return (argc == 2) ? runFile(argv[1]) : runPrompt();
#endif
}
