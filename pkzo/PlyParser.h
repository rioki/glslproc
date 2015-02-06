
#ifndef _ICE_PLY_PARSER_H_
#define _ICE_PLY_PARSER_H_

#include <string>
#include <fstream>
#include <vector>
#include <tuple>

namespace pkzo
{
    class Mesh;

    class PlyParser
    {
    public:
        PlyParser(Mesh& mesh);

        ~PlyParser();

        void parse(const std::string& file);

    private:
        enum TokenType
        {
            WHITESPACE,
            NEWLINE,
            IDENTIFIER,
            NUMBER, 
            END_OF_FILE
        };

        Mesh& mesh;

        std::ifstream input;
        std::string file;
        unsigned int line;

        // - type: vertex or face
        // - number of entires
        // - properties
        typedef std::tuple<unsigned int, unsigned int, std::vector<std::string>> Element;  
        std::vector<Element> elements;

        TokenType get_next_token(std::string& value);
        TokenType lex_token(std::string& value);
        TokenType lex_whitespace(std::string& value);
        TokenType lex_newline(std::string& value);
        TokenType lex_identifier(std::string& value);
        TokenType lex_number(std::string& value);
        void lex_discard_line();

        void parse_keyword(const std::string& keyword);
        unsigned int parse_keyword(const std::vector<std::string>& keywords);
        std::string parse_identifier();
        double parse_float();
        unsigned long parse_integer();
        

        void parse_header();
        void parse_element();
        void parse_property();
        void parse_body();
        void parse_vertex(const std::vector<std::string>& properties);
        void parse_face();        
    };
}

#endif
