#version 400

uniform sampler2D uTexture;
uniform uvec2 uTextureSize;

in vec2 vTexCoord;

out vec4 oFragColor;

void main(void)
{	
	oFragColor = vec4(texelFetch(uTexture, ivec2(vTexCoord), 0).rgb, 1.0);
}

