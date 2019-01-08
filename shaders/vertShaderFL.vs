#version 430 core

layout (binding=0) uniform sampler2D currentTextureFlow;


out VS_OUT {
    vec2 flow;
} vs_out;


void main()
{
	ivec2 imSize = textureSize(currentTextureFlow, 0).xy;
    // get 2D pixel locations from 1D gl_VertexID
	ivec2 aPos = ivec2(gl_InstanceID / (imSize.x / 4), (gl_InstanceID % (imSize.y / 4))) * 4;

    vs_out.flow = texelFetch(currentTextureFlow, ivec2(aPos.x, imSize.y - aPos.y), 0).xy / vec2(imSize);
    gl_Position = vec4(((float(aPos.x) / float(imSize.x) ) * 2.0f - 1.0f), ((float(aPos.y) / float(imSize.y)) * 2.0f - 1.0), 0, 1.0); 
	//gl_Position = vec4(0,0,0,0); 

}