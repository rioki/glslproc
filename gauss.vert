
#version 400

const int   kernelSize = 25;
float sigma      = 0.3;
float PI         = 3.14159265358979323846;
float mean       = kernelSize / 2.0;
        
in vec3 aVertex;
in vec3 aNormal;
in vec2 aTexCoord;
in vec3 aTangent;

out vec2 vTexCoord;
out float kernel[kernelSize];

void main()
{	
    float sum = 0;
    for (int x = 0; x < kernelSize; ++x) 
    {
        kernel[x] = sqrt( exp( -0.5 * (pow((x-mean)/sigma, 2.0) + pow((mean)/sigma,2.0)) )
            / (2 * PI * sigma * sigma) );
        sum += kernel[x];
    }
    for (int x = 0; x < kernelSize; ++x) 
    {
        kernel[x] /= sum;
    }
    

    vTexCoord   = aTexCoord;
    gl_Position = vec4(aVertex, 1);
}
