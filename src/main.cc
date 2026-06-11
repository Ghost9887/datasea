#include <common.h>
#include <scanner.h>
#include <parser.h>
#include <interpreter.h>
#include <fstream>
#include <sstream>

void print_help() {
	std::cout << "Usage: ./datasea <file_path> [output_file_name]" << std::endl;
}

std::string read_file(const char *file_path) {
    std::string source {};
    std::fstream file { file_path };

    std::stringstream buffer {};
    buffer << file.rdbuf();
    source = buffer.str();
	return source;
}

int main(int argc, char **argv) {

	if (argc != 2 && argc != 3) {
		print_help();
		return 0;
	}

	std::string output_file_name { argc == 3 ? argv[2] : "output.sql" };

	if (!output_file_name.ends_with(".sql")) {
		std::cout << "Output file name must end with .sql" << std::endl;
		return 0;
	}

	std::string source { read_file(argv[1]) };

	Scanner scanner { std::move(source) };
	std::vector<Token> tokens { scanner.tokenize() };

	for (Token token : tokens) {
		std::cout << token.to_string() << std::endl;
	}

	Parser parser { tokens };
	std::vector<std::unique_ptr<Stmnt>> statements { parser.parse() };

	for (size_t i {0}; i < statements.size(); i++) {
		std::cout << statements.at(i)->to_string() << std::endl;
	}

	Generator generator {};
	Interpreter interpreter(std::move(statements), generator);
	interpreter.interpret();

	generator.generate(std::move(output_file_name));

	return 0;
}
