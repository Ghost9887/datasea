#ifndef DATASEA_H
#define DATASEA_H

#include <common.h>
#include <token.h>

class Datasea {
public:
    Datasea() = delete;
    ~Datasea() = delete;

    static void init(std::string source, std::string output_file_name);
    static void run();
    static void error(Token &token, const std::string &message);
private:
    static std::string get_line(int line);
private:
    static std::string m_source;
    static std::string m_output_file_name;
    static bool m_had_error;
};




#endif
