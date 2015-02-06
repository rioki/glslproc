
#ifndef _PKZO_MESH_H_
#define _PKZO_MESH_H_

#include <vector>
#include <rgm/rgm.h>

#include "config.h"
#include "Shader.h"

namespace pkzo
{
    struct IndexTrinagle
    {
        unsigned int a;
        unsigned int b;
        unsigned int c;
    };

    struct IndexLine
    {
        unsigned int a;
        unsigned int b;
    };

    class Shader;

    class PKZO_EXPORT Mesh
    {
    public:      

        Mesh();

        Mesh(const std::string& file);

        Mesh(const Mesh&);
        
        ~Mesh();

        const Mesh& operator = (const Mesh&);

        unsigned int get_vertex_count() const;

        unsigned int get_face_count() const;

        unsigned int add_vertex(const rgm::vec3& vertex, const rgm::vec3& normal, const rgm::vec2& texcoord);

        std::vector<rgm::vec3> get_vertexes() const;

        std::vector<rgm::vec3> get_normals() const;

        std::vector<rgm::vec2> get_texcoords() const;

        std::vector<rgm::vec3> get_tangents() const;

        void add_face(unsigned int a, unsigned int b, unsigned int c);

        std::vector<IndexTrinagle> get_faces() const;

        void add_edge(unsigned int a, unsigned int b);

        std::vector<IndexLine> get_edges() const;

        void load(const std::string& file);

        void upload();

        void relase();

        void draw(Shader& shader);

        void draw_edges(Shader& shader, float width);

    private:
        unsigned int vao;
        unsigned int vbo[6];

        std::vector<rgm::vec3>     vertexes;
        std::vector<rgm::vec3>     normals;
        std::vector<rgm::vec2>     texcoords;
        std::vector<rgm::vec3>     tangents;
        std::vector<IndexTrinagle> faces;
        std::vector<IndexLine>     edges;
        
        void compute_tangents();
    };    
}

#endif
