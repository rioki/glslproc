#version 400

in vec3 aVertex;
in vec3 aNormal;
in vec2 aTexCoord;
in vec3 aTangent;

out vec2 vTexCoord;
out vec3 vNormal;

void main()
{	
    vTexCoord   = aTexCoord;
    vNormal     = aNormal;
    gl_Position = vec4(aVertex, 1);
}
