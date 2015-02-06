
#include "Mesh.h"

#include <cassert>
#include <GL/glew.h>

#include "path.h"
#include "compose.h"
#include "PlyParser.h"
#include "ObjParser.h"

namespace pkzo
{
    enum MeshBufferId
    {
        VERTEX_BUFFER   = 0,
        NORMAL_BUFFER   = 1,
        TEXCOORD_BUFFER = 2,
        TANGENT_BUFFER  = 3,
        INDEX_BUFFER    = 4,
        EDGE_BUFFER     = 5
    };

    Mesh::Mesh()
    : vao(0) {}

    Mesh::Mesh(const std::string& file)
    : Mesh()
    {
        load(file);
    }

    Mesh::Mesh(const Mesh& other)
    : Mesh()
    {
        vertexes  = other.vertexes;
        normals   = other.normals;
        texcoords = other.texcoords;
        tangents  = other.tangents;
        faces     = other.faces;
        edges     = other.edges;
    }
                
    Mesh::~Mesh() 
    {
        relase();
    }

    const Mesh& Mesh::operator = (const Mesh& other)
    {
        if (this != &other)
        {
            relase();
            vertexes  = other.vertexes;
            normals   = other.normals;
            texcoords = other.texcoords;
            tangents  = other.tangents;
            faces     = other.faces;
            edges     = other.edges;
        }

        return *this;
    }

    unsigned int Mesh::get_vertex_count() const
    {
        return vertexes.size();
    }

    unsigned int Mesh::get_face_count() const
    {
        return faces.size();
    }

    unsigned int Mesh::add_vertex(const rgm::vec3& vertex, const rgm::vec3& normal, const rgm::vec2& texcoord)
    {
        vertexes.push_back(vertex);
        normals.push_back(normal);
        texcoords.push_back(texcoord);

        return vertexes.size() - 1;
    }

    std::vector<rgm::vec3> Mesh::get_vertexes() const
    {
        return vertexes;
    }

    std::vector<rgm::vec3> Mesh::get_normals() const
    {
        return normals;
    }

    std::vector<rgm::vec2> Mesh::get_texcoords() const
    {
        return texcoords;
    }

    std::vector<rgm::vec3> Mesh::get_tangents() const
    {
        if (tangents.size() != vertexes.size())
        {
            const_cast<Mesh*>(this)->compute_tangents();
        }
        return tangents;
    }

    void Mesh::add_face(unsigned int a, unsigned int b, unsigned int c)
    {
        if ((a >= vertexes.size()) || (b >= vertexes.size()) || (c >= vertexes.size()))
        {
            throw std::invalid_argument("Mesh::add_face: index out of bounds");
        }
        
        IndexTrinagle face = {a, b, c};
        faces.push_back(face); 
    }

    std::vector<IndexTrinagle> Mesh::get_faces() const
    {
        return faces;
    }

    void Mesh::add_edge(unsigned int a, unsigned int b)
    {
        if ((a >= vertexes.size()) || (b >= vertexes.size()))
        {
            throw std::invalid_argument("Mesh::add_edge: index out of bounds");
        }

        IndexLine edge = {a, b};
        edges.push_back(edge);
    }

    std::vector<IndexLine> Mesh::get_edges() const
    {
        return edges;
    }

    void Mesh::load(const std::string& file)
    {       
        std::string ext = path::ext(file); // tolower
        Mesh tmp;

        if (ext == "ply")
        {
            PlyParser parser(tmp); 
            parser.parse(file);
        }
        else if (ext == "obj")
        {
            ObjParser parser(tmp); 
            parser.parse(file);
        }
        else
        {
            throw std::logic_error(compose("Unknown mesh extention %0.", ext));
        }
                
        *this = tmp;
    }   

    void Mesh::compute_tangents()
    {
        std::vector<rgm::vec3> tan1(vertexes.size(), rgm::vec3(0, 0, 0));
        std::vector<rgm::vec3> tan2(vertexes.size(), rgm::vec3(0, 0, 0));
        
        for (IndexTrinagle face : faces)
        {
            rgm::vec3 v1 = rgm::vec3(vertexes[face.a]);
            rgm::vec3 v2 = rgm::vec3(vertexes[face.b]);
            rgm::vec3 v3 = rgm::vec3(vertexes[face.c]);
            
            rgm::vec2 w1 = texcoords[face.a];
            rgm::vec2 w2 = texcoords[face.b];
            rgm::vec2 w3 = texcoords[face.c];
            
            float x1 = v2[0] - v1[0];
            float x2 = v3[0] - v1[0];
            float y1 = v2[1] - v1[1];
            float y2 = v3[1] - v1[1];
            float z1 = v2[2] - v1[2];
            float z2 = v3[2] - v1[2];
            
            float s1 = w2[0] - w1[0];
            float s2 = w3[0] - w1[0];
            float t1 = w2[1] - w1[1];
            float t2 = w3[1] - w1[1];
            
            float r = 1.0F / (s1 * t2 - s2 * t1);
            rgm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
            rgm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);
            
            tan1[face.a] = tan1[face.a] + sdir;
            tan1[face.b] = tan1[face.b] + sdir;
            tan1[face.c] = tan1[face.c] + sdir;
            
            tan2[face.a] = tan2[face.a] + tdir;
            tan2[face.b] = tan2[face.b] + tdir;
            tan2[face.c] = tan2[face.c] + tdir;
        }
        
        tangents.resize(vertexes.size());
        for (unsigned int i = 0; i < vertexes.size(); i++)
        {
            rgm::vec3& n = normals[i];
            rgm::vec3& t1 = tan1[i];
            rgm::vec3& t2 = tan2[i];
            
            if (dot(cross(n, t1), t2) < 0.0f) 
            {
                tangents[i] = -normalize(t1 - n * dot(n, t1));
            }
            else
            {
                tangents[i] = normalize(t1 - n * dot(n, t1));
            }            
        }
    }

    void Mesh::upload()
    {
        if (vao != 0)
        {
            return;
        }

        compute_tangents();

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(6, vbo);

        if (!vertexes.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[VERTEX_BUFFER]);    
            glBufferData(GL_ARRAY_BUFFER, vertexes.size() * 3 * sizeof(float), &vertexes[0], GL_STATIC_DRAW);
        }
        if (!normals.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL_BUFFER]);    
            glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * sizeof(float), &normals[0], GL_STATIC_DRAW);
        }
        if (!texcoords.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD_BUFFER]);    
            glBufferData(GL_ARRAY_BUFFER, texcoords.size() * 2 * sizeof(float), &texcoords[0], GL_STATIC_DRAW);
        }
        if (!tangents.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[TANGENT_BUFFER]);    
            glBufferData(GL_ARRAY_BUFFER, tangents.size() * 3 * sizeof(float), &tangents[0], GL_STATIC_DRAW);
        }
        if (!faces.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[INDEX_BUFFER]);    
            glBufferData(GL_ARRAY_BUFFER, faces.size() * 3 * sizeof(unsigned int), &faces[0], GL_STATIC_DRAW);
        }
        if (!edges.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[EDGE_BUFFER]);    
            glBufferData(GL_ARRAY_BUFFER, edges.size() * 2 * sizeof(unsigned int), &edges[0], GL_STATIC_DRAW);
        }

        glBindVertexArray(0);
    }

    void Mesh::relase()
    {
        if (vao != 0)
        {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(6, vbo);
        }
        vao = 0;
    }

    void Mesh::draw(Shader& shader)
    {
        upload();

        glBindVertexArray(vao);

        int vertex_location   = shader.get_attribute_location("aVertex");
        int normal_location   = shader.get_attribute_location("aNormal");
        int texcoord_location = shader.get_attribute_location("aTexCoord");
        int tangent_location  = shader.get_attribute_location("aTangent");
        
        if (vertex_location != -1)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[VERTEX_BUFFER]);
            glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(vertex_location);
        }
        
        if (normal_location != -1)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL_BUFFER]);
            glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(normal_location);
        }
        
        if (texcoord_location != -1)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD_BUFFER]);
            glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(texcoord_location);
        }
        
        if (tangent_location != -1)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[TANGENT_BUFFER]);
            glVertexAttribPointer(tangent_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(tangent_location);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[INDEX_BUFFER]);        
        glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  

        if (vertex_location != -1)
        {
            glDisableVertexAttribArray(vertex_location);
        }

        if (normal_location != -1)
        {
            glDisableVertexAttribArray(normal_location);
        }

        if (texcoord_location != -1)
        {
            glDisableVertexAttribArray(texcoord_location);
        }
        
        if (texcoord_location != -1)
        {
            glDisableVertexAttribArray(tangent_location);
        }

        glBindVertexArray(0);
    }

    void Mesh::draw_edges(Shader& shader, float width)
    {
        upload();

        glBindVertexArray(vao);

        int vertex_location   = shader.get_attribute_location("aVertex");
        int normal_location   = shader.get_attribute_location("aNormal");
        int texcoord_location = shader.get_attribute_location("aTexCoord");
        int tangent_location  = shader.get_attribute_location("aTangent");
        
        if (vertex_location != -1)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[VERTEX_BUFFER]);
            glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(vertex_location);
        }
        
        if (normal_location != -1)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL_BUFFER]);
            glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(normal_location);
        }
        
        if (texcoord_location != -1)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD_BUFFER]);
            glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(texcoord_location);
        }
        
        if (tangent_location != -1)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[TANGENT_BUFFER]);
            glVertexAttribPointer(tangent_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(tangent_location);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);  

        glLineWidth(width);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[EDGE_BUFFER]);        
        glDrawElements(GL_LINES, edges.size() * 2, GL_UNSIGNED_INT, 0);        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);   

        if (vertex_location != -1)
        {
            glDisableVertexAttribArray(vertex_location);
        }

        if (normal_location != -1)
        {
            glDisableVertexAttribArray(normal_location);
        }

        if (texcoord_location != -1)
        {
            glDisableVertexAttribArray(texcoord_location);
        }
        
        if (texcoord_location != -1)
        {
            glDisableVertexAttribArray(tangent_location);
        }

        glBindVertexArray(0);
    }
}
