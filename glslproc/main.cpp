
/*
    Note:
    Just in case you are wondering why no windowless context is used here...
    The windowless context can be created through the ARB extentions; but they 
    need to be loaded through a context. So to get the functions for a windowless
    context a context with a window needs to be created...
    If there is already a hidden window, why not just use that context and 
    render everything to an FBO and be done with it.
*/

#include <pkzo/pkzo.h>

int main(int argc, char* argv[])
{
    // options
    try
    {
        std::string input;
        std::string vcode;
        std::string fcode;
        std::string output;
        if (argc == 5)
        {
            input  = argv[1];
            vcode  = argv[2];
            fcode  = argv[3];
            output = argv[4];
        }
        else
        {
            std::cerr << "Usage: " << std::endl
                      << "glslproc <image> <vertex code> <fragment code> <output>" << std::endl;

        }

        pkzo::Texture input_texture;
        input_texture.load(input);
        
        pkzo::Window window("dummy", rgm::ivec2(0, 0), input_texture.get_size());
        window.show();

        pkzo::Shader shader;
        shader.load(vcode, fcode);

        rgm::uvec2 is = input_texture.get_size();

        pkzo::Mesh mesh;
        mesh.add_vertex(rgm::vec3(-1, -1, 0), rgm::vec3(1, 0, 0), rgm::vec2(0, 0));
        mesh.add_vertex(rgm::vec3(-1, 1, 0), rgm::vec3(0, 1, 0), rgm::vec2(0, is[1]));
        mesh.add_vertex(rgm::vec3(1, 1, 0), rgm::vec3(0, 0, 1), rgm::vec2(is[0], is[1]));
        mesh.add_vertex(rgm::vec3(1, -1, 0), rgm::vec3(1, 1, 1), rgm::vec2(is[0], 0));
        mesh.add_face(0, 1, 2);
        mesh.add_face(2, 3, 0);
        
        window.on_draw([&] () {
            shader.bind();        

            input_texture.bind(0);
            shader.set_uniform("uTexture", 0);
            shader.set_uniform("uTextureSize", input_texture.get_size());

            mesh.draw(shader);

            pkzo::Texture& output_texture = window.read_color();
            output_texture.save(output);

            window.close();
        });            
        
        window.run();              
        

        return 0;
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return -1;
    }
}
