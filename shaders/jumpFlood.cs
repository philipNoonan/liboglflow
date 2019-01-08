// adapted from 
/**
 * 
 * PixelFlow | Copyright (C) 2017 Thomas Diewald - http://thomasdiewald.com
 * 
 * A Processing/Java library for high performance GPU-Computing (GLSL).
 * MIT License: https://opensource.org/licenses/MIT
 * 
 */


//
// resouces
//
// Jumpflood Algorithm (JFA)
//
// Jump Flooding in GPU with Applications to Voronoi Diagram and Distance Transform
// www.comp.nus.edu.sg/~tants/jfa/i3d06.pdf
//


#version 430

layout(local_size_x = 32, local_size_y = 32) in; // 

layout(binding = 0) uniform sampler2D textureInitialRGB;

layout(binding = 0, rg32f) uniform image2D im_dtnn_0;
layout(binding = 1, rg32f) uniform image2D im_dtnn_1;
layout(binding = 2, rgba8) uniform image2D outImage;


#define LENGTH_SQ(dir) ((dir).x*(dir).x + (dir).y*(dir).y)
#define POS_MAX 0x7FFF // == 32767 == ((1<<15) - 1)
vec2 dtnn = vec2(POS_MAX); // some large number
vec2 pix;
float dmin = LENGTH_SQ(dtnn);
uniform float jump;





subroutine void launchSubroutine();
subroutine uniform launchSubroutine jumpFloodSubroutine;

subroutine(launchSubroutine)
void jumpFloodAlgorithmInit()
{
    // http://rykap.com/graphics/skew/2016/02/25/voronoi-diagrams/
    // https://github.com/diwi/PixelFlow/blob/master/examples/Miscellaneous/DistanceTransform_Demo/DistanceTransform_Demo.java
    // https://github.com/diwi/PixelFlow/blob/master/src/com/thomasdiewald/pixelflow/glsl/Filter/distancetransform.frag
    // http://www.comp.nus.edu.sg/~tants/jfa/i3d06.pdf
    // https://www.shadertoy.com/view/4syGWK
    ivec2 pix = ivec2(gl_GlobalInvocationID.xy);

    vec4 tColor = texelFetch(textureInitialRGB, pix, 0);

    vec4 outPos = vec4(POS_MAX);
    imageStore(im_dtnn_1, pix, outPos); // wipe it

    if (tColor.x >= 1.0)
    {
        outPos = vec4(pix.xy, 0, 0);
    }

    imageStore(im_dtnn_0, pix, outPos);


}     


void DTNN(const in vec2 off)
{
    vec2 dtnn_cur = vec2(imageLoad(im_dtnn_0, ivec2(off)).xy);
    // THIS WAS A PAIN AND IS PROBABLY A HACK FIX. DTNN_CURR WOULD RETURN ZERO VALUES AND MESS EVERYTHING UP LEADING TO A 0,0 POINT PERSISTING
    if (dtnn_cur != vec2(0) && off != vec2(0))
    {
        vec2 ddxy = dtnn_cur - pix;
        float dcur = LENGTH_SQ(ddxy);
        if (dcur < dmin)
        {
            dmin = dcur;
            dtnn = dtnn_cur;
        }
    }
}

subroutine(launchSubroutine)
void jumpFloodAlgorithmUpdate()
{
    pix = vec2(gl_GlobalInvocationID.xy);

    dtnn = vec2(imageLoad(im_dtnn_0, ivec2(pix)).xy);

    vec2 ddxy = dtnn - pix;
    dmin = LENGTH_SQ(ddxy);

    DTNN(pix + vec2(-jump, jump));     DTNN(pix + vec2(0, jump));     DTNN(pix + vec2(jump, jump));
    DTNN(pix + vec2(-jump, 0));                                       DTNN(pix + vec2(jump, 0));
    DTNN(pix + vec2(-jump, -jump));    DTNN(pix + vec2(0, -jump));    DTNN(pix + vec2(jump, -jump));

    imageStore(im_dtnn_1, ivec2(pix), vec4(dtnn.x, dtnn.y, 0, 0));


    // for pixel pix, get the 8 points at +- k distance away and see if the distance they store is less than the distance pixel pix has

    // store the lowest distance as the new distance for pixel pix in output buffer

    // next iteration flip the input and output buffer

    // after the last iteration the distance transform can be determined for each pixel by getting the length of the vec between pixel pix and distance

}

subroutine(launchSubroutine)
void getColorFromRGB()
{
    pix = vec2(gl_GlobalInvocationID.xy);
    vec2 jfa = vec2(imageLoad(im_dtnn_1, ivec2(pix)).xy);

    vec4 tColor = texelFetch(textureInitialRGB, ivec2(jfa), 0);

    float distCol = distance(pix, jfa);

    imageStore(outImage, ivec2(pix), vec4(distCol.xxx / 100, 1.0));

}

void main()
{
    jumpFloodSubroutine();

}