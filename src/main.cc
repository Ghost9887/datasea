#include <common.h>
#include <scanner.h>
#include <fstream>
#include <sstream>

void print_help() {
	std::cout << "Usage: ./datasea <file_path>" << std::endl;
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

	if (argc != 2) {
		print_help();
		return 0;
	}
	
	std::string source { read_file(argv[1]) };

	Scanner scanner { std::move(source) };
	std::vector<Token> tokens { scanner.tokenize() };

	for (Token token : tokens) {
		std::cout << token.to_string() << std::endl;
	}
	return 0;
}
