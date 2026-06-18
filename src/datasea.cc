#include <datasea.h>
#include <scanner.h>
#include <parser.h>
#include <interpreter.h>

std::string Datasea::m_source {};
std::string Datasea::m_output_file_name {};
bool Datasea::m_had_error {false};

void Datasea::init(std::string source, std::string output_file_name) {
    Datasea::m_source = source;
    Datasea::m_output_file_name = output_file_name;
}

void Datasea::run() {
    Scanner scanner { Datasea::m_source };
	std::vector<Token> tokens { scanner.tokenize() };

    /*   
    for (Token &token : tokens) {
        std::cout << token.to_string() << std::endl;
    }
    */

	Parser parser { tokens };
	std::vector<std::unique_ptr<Stmnt>> statements { parser.parse() };

    if (Datasea::m_had_error) return;

    /*   
    for (size_t i { 0 }; i < statements.size(); i++) {
        std::cout << statements.at(i)->to_string() << std::endl;;
    }
    */

    Interpreter interpreter { Datasea::m_output_file_name };
    interpreter.interpret(std::move(statements));      

    std::cout << "Data generated succesfully" << std::endl;
}

void Datasea::error(Token &token, const std::string &message) {
    Datasea::m_had_error = true;
    std::cout << std::endl;
    std::cout << std::format("Error: {} at line: {}", message, std::to_string(token.m_line)) << std::endl;
    std::cout << std::endl;
    std::string line { get_line(token.m_line) };
    std::cout << get_line(token.m_line) << std::endl;
    std::string pointer {}; 
    for (size_t i {0}; i < token.m_column; i++) {
        pointer += '~';
    }
    pointer += '^';

    std::cout << pointer << std::endl;
}

std::string Datasea::get_line(int line) {
    int current_line = 1;
    size_t start_index = 0;

    while (start_index < Datasea::m_source.size() && current_line < line) {
        if (Datasea::m_source.at(start_index) == '\n')
            current_line++;
        start_index++;
    }

    if (start_index >= Datasea::m_source.size())
        return "";

    size_t end_index = start_index;
    while (end_index < Datasea::m_source.size() && Datasea::m_source.at(end_index) != '\n')
        end_index++;

    return Datasea::m_source.substr(start_index, end_index - start_index);
}

