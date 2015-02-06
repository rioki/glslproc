
#ifndef _ICE_OBJ_PARSER_H_
#define _ICE_OBJ_PARSER_H_

#include <string>
#include <fstream>
#include <vector>
#include <tuple>

#include <rgm/rgm.h>

namespace pkzo
{
    class Mesh;

    class ObjParser
    {
    public:

        ObjParser(Mesh& mesh);

        ~ObjParser();

        void parse(const std::string& file);

    private:
        enum TokenType
        {
            NO_TOKEN,
            WHITESPACE,
            NEWLINE,
            COMMENT,
            IDENTIFIER,
            NUMBER, 
            SLASH,
            DOT,
            BSLASH,
            END_OF_FILE
        };


        Mesh& mesh;

        std::ifstream input;
        std::string file;
        unsigned int line;
        
        TokenType   token;
        TokenType   next_token;
        std::string value;
        std::string next_value;

        std::vector<rgm::vec3> vertices;
        std::vector<rgm::vec3> normals;
        std::vector<rgm::vec2> texcoords;
        std::vector<std::vector<rgm::ivec3>> faces;
        bool homogene;

        void get_next_token();
        TokenType lex_token(std::string& value);
        TokenType lex_whitespace(std::string& value);
        TokenType lex_newline(std::string& value);
        TokenType lex_comment(std::string& value);
        TokenType lex_identifier(std::string& value);
        TokenType lex_number(std::string& value);

        void parse_keyword(const std::string& keyword);
        unsigned int parse_keyword(const std::vector<std::string>& keywords);
        std::string parse_identifier();
        std::string parse_identifier_or_number();
        float parse_float();
        unsigned long parse_integer();
        std::string parse_filename();

        void parse_line();
        void parse_vertex();
        void parse_texcoord();
        void parse_normal();
        void parse_parmeter();
        void parse_face();
        void parse_mtllib();
        void parse_usemtl();
        void parse_object();
        void parse_group();
        void parse_smothing();
        rgm::ivec3 parse_face_point();
    };
}

#endif
