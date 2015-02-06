
#version 400

const int   kernelSize = 15;

uniform sampler2D uTexture;
uniform uvec2 uTextureSize;

in vec2 vTexCoord;
in float kernel[kernelSize];

out vec4 oFragColor;

void main(void)
{	
    vec3 result = vec3(0);
    for (int i = -kernelSize; i <= kernelSize; i++)
    {
        float f = kernel[abs(i)]; 
        result += texelFetch(uTexture, ivec2(vTexCoord) + ivec2(i, 0), 0).rgb * f;
    }
    
    oFragColor = vec4(result, 1);
}


