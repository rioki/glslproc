
#include "ObjParser.h"

#include <iterator> 
#include "Mesh.h"
#include <map>

namespace std
{
    inline
    std::ostream& operator << (std::ostream& os, const std::vector<std::string>& values)
    {
        for (unsigned int i = 0; i < values.size(); i++)
        {
            os << values[i];
            if (i == values.size() - 2)
            {
                os << " or ";
            }
            else if (i != values.size() - 1)
            {
                os << ", ";
            }
        }
        return os;
    }
}

#include "compose.h"

namespace pkzo
{
    ObjParser::ObjParser(Mesh& m)
    : mesh(m), line(0), token(NO_TOKEN), next_token(NO_TOKEN), homogene(true)
    {
    }

    ObjParser::~ObjParser()
    {
    }

    void ObjParser::parse(const std::string& f)
    {
        file = f;
        input.open(file);
        if (! input.good())
        {
            throw std::runtime_error(compose("Failed to open '%0' for reading.", file));
        }


        // prime the next_token
        get_next_token();

        while (next_token != END_OF_FILE)
        {
            parse_line();
        }

        if (homogene)
        {
            for (unsigned int i = 0; i < vertices.size(); i++)
            {
                rgm::vec3 normal(0);
                if (! normals.empty())
                {
                    normal = normals[i];
                }
                
                rgm::vec2 tc(0);
                if (! texcoords.empty())
                {
                    tc = texcoords[i];
                    // TC adjustment may just be a glitch in L3DT
                    tc[1] = 1.0f - tc[1];
                }
                
                
                mesh.add_vertex(vertices[i], normal, tc);
            }

            for (unsigned int i = 0; i < faces.size(); i++)
            {
                std::vector<rgm::ivec3>& points = faces[i];
                for (unsigned int j = 2; j < points.size(); j++)
                {
                    mesh.add_face(points[0][0] - 1, points[j-1][0] - 1, points[j][0] - 1);
                }
            }
        }
        else
        {
            // here we need to duplicate the vertextes and maybe reduce perfect duplicates.
            throw std::logic_error("WRITE ME");
        }
    }

    void ObjParser::get_next_token()
    {
        // skip whitepsaces

        std::string v;
        TokenType t = lex_token(v);
        while (t == WHITESPACE || t == NEWLINE || t == COMMENT)
        {
            if (t == NEWLINE)
            {
                line++;
            }
            v = "";
            t = lex_token(v);
        }
                
        token = next_token;
        value = next_value;
        next_token = t;
        next_value = v;
    }

    ObjParser::TokenType ObjParser::lex_token(std::string& value)
    {
        int c = input.get();
        switch (c)
        {
            case ' ': case '\t': case '\v':
                value.push_back(c);
                return lex_whitespace(value);            
            case '\n': case '\r':
                value.push_back(c);
                return lex_newline(value);
            case '#':
                value.push_back(c);
                return lex_comment(value);
            case '/':
                value = "/";
                return SLASH;
            case '\\':
                value = "\\";
                return BSLASH;
            case '.':
                value = ".";
                return DOT;
            case '+': case '-': case '0': case '1': case '2': case '3':
            case '4': case '5': case '6': case '7': case '8': case '9':
                value.push_back(c);
                return lex_number(value);
            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': 
            case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
            case 's': case 't': case 'u': case 'v': case 'w': case 'x':
            case 'y': case 'z': 
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': 
            case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
            case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
            case 'Y': case 'Z': 
            case '_':
                value.push_back(c);
                return lex_identifier(value);
            case EOF:
                return END_OF_FILE;
            default:
                value.push_back(c);
                throw std::runtime_error(compose("Unexpected character %0!", value));
        }
    }

    ObjParser::TokenType ObjParser::lex_whitespace(std::string& value)
    {
        int c = input.get();
        while (true)        
        {
            switch (c)
            {
                case ' ': case '\t': case '\v':
                    value.push_back(c);
                    break;
                default:
                    input.unget();
                    return WHITESPACE;
            }
            c = input.get();
        }
    }

    ObjParser::TokenType ObjParser::lex_newline(std::string& value)
    {
        int c = input.get();
        switch (c)
        {
            case '\n': case '\r':
                if (c != value[0])
                {
                    // \r\n or \n\r
                    value.push_back(c);
                }
                else
                {
                    // treat \n \n as two newline, obviously
                    input.unget();                            
                }
                return NEWLINE;
            default:
                input.unget();
                return NEWLINE;
        }
    }

    ObjParser::TokenType ObjParser::lex_identifier(std::string& value)
    {
        int c = input.get();
        while (true)        
        {
            switch (c)
            {                            
                case '0': case '1': case '2': case '3': case '4': 
                case '5': case '6': case '7': case '8': case '9':
                case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': 
                case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
                case 's': case 't': case 'u': case 'v': case 'w': case 'x':
                case 'y': case 'z': 
                case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': 
                case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
                case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
                case 'Y': case 'Z': 
                case '_':
                    value.push_back(c);
                    break;
                default:
                    input.unget();
                    return IDENTIFIER;
            }
            c = input.get();
        }        
    }

    ObjParser::TokenType ObjParser::lex_number(std::string& value)
    {
        // NOTE: we don't actually validate if it makes a valid number here
        // NOTE: the e-notation is not implemented, is that actually valid in PLY?
        int c = input.get();
        while (true)        
        {
            switch (c)
            {                            
                case '+': case '-': case '.':
                case '0': case '1': case '2': case '3': case '4': 
                case '5': case '6': case '7': case '8': case '9':
                    value.push_back(c);
                    break;
                default:
                    input.unget();
                    return NUMBER;
            }
            c = input.get();
        }
    }

    ObjParser::TokenType ObjParser::lex_comment(std::string& value)
    {
        int c = input.get();
        while (c != '\n' && c != '\r' && c != EOF) 
        {
            c = input.get();
            value.push_back(c);
        }
        input.unget();
        return COMMENT;
    }

    void ObjParser::parse_keyword(const std::string& keyword)
    {        
        get_next_token();
        if (token != IDENTIFIER || value != keyword)
        {
            throw std::runtime_error(compose("%0(%1): Expected '%2' but got '%3'.", file, line, keyword, value));
        }
    }

    unsigned int ObjParser::parse_keyword(const std::vector<std::string>& keywords)
    {
        get_next_token();
        auto i =  std::find(keywords.begin(), keywords.end(), value);
        if (token != IDENTIFIER || i == keywords.end())
        {
            throw std::runtime_error(compose("%0(%1): Expected %2 but got '%3'.", file, line, keywords, value));
        }
        return std::distance(keywords.begin(), i);
    }

    std::string ObjParser::parse_identifier()
    {
        get_next_token();
        if (token != IDENTIFIER)
        {
            throw std::runtime_error(compose("%0(%1): Expected identifier but got '%2'.", file, line, value));
        }
        return value;
    }

    std::string ObjParser::parse_identifier_or_number()
    {
        get_next_token();
        if (token != IDENTIFIER && token != NUMBER)
        {
            throw std::runtime_error(compose("%0(%1): Expected identifier or number but got '%2'.", file, line, value));
        }
        return value;
    }

    float ObjParser::parse_float()
    {
        get_next_token();
        if (token == NUMBER)
        {
            // TODO use endptr to check if the entiere string was read
            double dv = strtod(&value[0], NULL);
            return (float)dv;
        }
        else
        {
            throw std::runtime_error(compose("%0(%1): Expected number but got '%2'.", file, line, value));
        }
    }

    unsigned long ObjParser::parse_integer()
    {
        get_next_token();
        if (token == NUMBER)
        {
            // TODO use endptr to check if the entiere string was read
            unsigned long ulv = strtoul(&value[0], NULL, 10);
            return ulv;
        }
        else
        {
            throw std::runtime_error(compose("%0(%1): Expected integer but got '%2'.", file, line, value));
        }
    }

    std::string ObjParser::parse_filename()
    {
        // this is not correctly implemented; it could be a fully qualified path with spaces and all that jazz
        std::string file;
        
        while (next_token == IDENTIFIER || 
               next_token == SLASH || 
               next_token == BSLASH ||
               next_token == DOT)
        {
            get_next_token();
            file += value;

            if (value == "mtl")
            {
                return file;
            }
        }

        return file;
    }

    std::vector<std::string> keywords = [] () -> std::vector<std::string> {
        std::vector<std::string> result;

        result.push_back("v");
        result.push_back("vt");
        result.push_back("vn");
        result.push_back("vp");        
        result.push_back("f");
        result.push_back("o");
        result.push_back("g");
        result.push_back("s");
        result.push_back("mtllib");
        result.push_back("usemtl");

        return result;
    }();

    enum Keyword
    {
        V,
        VT,
        VN,
        VP,
        F,
        O,
        G,
        S,
        MTLLIB,
        USEMTL
    };

    void ObjParser::parse_line()
    {
        unsigned int keyword = parse_keyword(keywords);
        switch (keyword)
        {
            case V:
                parse_vertex();
                break;
            case VT:
                parse_texcoord();
                break;
            case VN:
                parse_normal();
                break;
            case VP:
                parse_parmeter();
                break;
            case F:
                parse_face();
                break;
            case O:
                parse_object();
                break;
            case G:
                parse_group();
                break;
            case S:
                parse_smothing();
                break;
            case MTLLIB:
                parse_mtllib();
                break;
            case USEMTL:            
                parse_usemtl();
                break;
            default:
                throw std::logic_error("Unknown line keyword!");
        }
    }

    void ObjParser::parse_vertex() 
    {
        float x = parse_float();
        float y = parse_float();
        float z = parse_float();
        float w = 0.0f;
        if (next_token == NUMBER)
        {
            w = parse_float();
        }

        vertices.push_back(rgm::vec3(x, y, z));
    }
    
    void ObjParser::parse_texcoord() 
    {
        float u = parse_float();
        float v = parse_float();
        float w = 0.0f;
        if (next_token == NUMBER)
        {
            w = parse_float();
        }

        texcoords.push_back(rgm::vec2(u, v));
    }
    
    void ObjParser::parse_normal() 
    {
        float x = parse_float();
        float y = parse_float();
        float z = parse_float();

        normals.push_back(rgm::vec3(x, y, z));
    }

    void ObjParser::parse_parmeter()
    {
        float u = parse_float();
        float v = 0.0f;
        if (next_token == NUMBER)
        {
            v = parse_float();
        }
        float w = 0.0f;
        if (next_token == NUMBER)
        {
            w = parse_float();
        }
        // discard value
    }
    
    void ObjParser::parse_face() 
    {
        std::vector<rgm::ivec3> points;

        while (next_token == NUMBER)
        {
            rgm::ivec3 p = parse_face_point();
            points.push_back(p);
        }

        faces.push_back(points);
    }

    rgm::ivec3 ObjParser::parse_face_point()
    {
        int v = -1;
        int t = -1;
        int n = -1;

        v = parse_integer();

        if (next_token == SLASH)
        {
            get_next_token();
        }

        if (next_token == NUMBER)
        {
            t = parse_integer();
        }

        if (next_token == SLASH)
        {
            get_next_token();

            if (next_token == NUMBER)
            {
                n = parse_integer();
            }
        }

        if (v != t && v != n)
        {
            homogene = false;
        }

        return rgm::ivec3(v, t, n);
    }

    void ObjParser::parse_mtllib()
    {
        std::string file = parse_filename();
    }

    void ObjParser::parse_usemtl()
    {
        std::string id = parse_identifier();
    }

    void ObjParser::parse_object()
    {
        std::string id = parse_identifier();
    }

    void ObjParser::parse_group()
    {
        std::string id = parse_identifier();
    }

    void ObjParser::parse_smothing()
    {
        // s 1
        // s on
        // s off
        std::string id = parse_identifier_or_number();
    }
}
