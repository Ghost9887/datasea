#include <common.h>
#include <datasea.h>
#include <fstream>
#include <sstream>

void print_help() {
    std::cout << "Script files must end with .ds" << std::endl;
    std::cout << "Custom output files must end with .sql" << std::endl;
	std::cout << "Usage: ./datasea <script_file> [output_file_name]" << std::endl;
    std::cout << "CURRENTLY ONLY 'us' LOCALE IS SUPPORTED" << std::endl;
}

std::string read_file(const std::string file_path) {
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
    std::string input_file { argv[1] };
    
    if (input_file == "-h") {
        print_help();
        return 0;
    }

    if (!input_file.ends_with(".ds")) {
        std::cout << "Script file must end with .ds" << std::endl;
        return 0;
    }
	std::string output_file_name { argc == 3 ? argv[2] : "output.sql" };

	if (!output_file_name.ends_with(".sql")) {
		std::cout << "Output file name must end with .sql" << std::endl;
		return 0;
	}

	std::string source { read_file(std::move(input_file)) };

    Datasea::init(source, output_file_name);
    Datasea::run();

	return 0;
}
