#version 400
 
int radius = 25;
 
uniform sampler2D uTexture;
uniform uvec2 uTextureSize;
 
in vec2 vTexCoord;
 
out vec4 oFragColor;
 
void main(void)
{	
    vec3 result = vec3(0);
    for (int i = -radius; i <= radius; i++)
    {
        for (int j = -radius; j <= radius; j++) 
        {
            float r = radius;
            float f = (radius - length(vec2(0,0) - vec2(i,j))) / r;
            result += texelFetch(uTexture, ivec2(vTexCoord) + ivec2(i, j), 0).rgb * f;
        }
    }
    result = normalize(result);
    
    oFragColor = vec4(result, 1);
}