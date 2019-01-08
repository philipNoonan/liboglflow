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

layout(binding = 0, rg32i) uniform iimage2D im_dtnn_0;
layout(binding = 1, rg32i) uniform iimage2D im_dtnn_1;

#define LENGTH_SQ(dir) ((dir).x*(dir).x + (dir).y*(dir).y)
ivec2 dtnn = ivec2(32767); // some large number
ivec2 pix;
int dmin = LENGTH_SQ(dtnn);
uniform int jump;

subroutine void launchSubroutine();
subroutine uniform launchSubroutine jumpFloodSubroutine;


subroutine(launchSubroutine)
void jumpFloodAlgorithmInit()
{
    // http://rykap.com/graphics/skew/2016/02/25/voronoi-diagrams/
    // https://github.com/diwi/PixelFlow/blob/master/examples/Miscellaneous/DistanceTransform_Demo/DistanceTransform_Demo.java
    // https://github.com/diwi/PixelFlow/blob/master/src/com/thomasdiewald/pixelflow/glsl/Filter/distancetransform.frag
    // http://www.comp.nus.edu.sg/~tants/jfa/i3d06.pdf

    ivec2 pix = ivec2(gl_GlobalInvocationID.xy);

    if (pix.x == 200 && pix.y == 200)
    {
        imageStore(im_dtnn_0, pix, ivec4(pix.x, pix.y, 0, 0));
    }

    if (pix.x == 200 && pix.y == 400)
    {
        imageStore(im_dtnn_0, pix, ivec4(pix.x, pix.y, 0, 0));

    }

    if (pix.x == 120 && pix.y == 360)
    {
        imageStore(im_dtnn_0, pix, ivec4(pix.x, pix.y, 0, 0));

    }


    if (pix.x == 23 && pix.y == 586)
    {
        imageStore(im_dtnn_0, pix, ivec4(pix.x, pix.y, 0, 0));

    }

    if (pix.x == 400 && pix.y == 200)
    {
        imageStore(im_dtnn_0, pix, ivec4(pix.x, pix.y, 0, 0));

    }

    if (pix.x == 75 && pix.y == 200)
    {
        imageStore(im_dtnn_0, pix, ivec4(pix.x, pix.y, 0, 0));

    }

    if (pix.x == 523 && pix.y == 320)
    {
        imageStore(im_dtnn_0, pix, ivec4(pix.x, pix.y, 0, 0));

    }

    if (pix.x == 400 && pix.y == 400)
    {
        imageStore(im_dtnn_0, pix, ivec4(pix.x, pix.y, 0, 0));

    }

    if (pix.x == 400 && pix.y == 600)
    {
        imageStore(im_dtnn_0, pix, ivec4(pix.x, pix.y, 0, 0));

    }

    if (pix.x == 600 && pix.y == 400)
    {
        imageStore(im_dtnn_0, pix, ivec4(pix.x, pix.y, 0, 0));

    }
    //imageStore(im_dtnn_0, pix * 30, ivec4(pix.x * 30, pix.y * 30, 0, 0));


}

void DTNN(const in ivec2 off)
{
    ivec2 dtnn_cur = imageLoad(im_dtnn_0, off).xy;
    ivec2 ddxy = dtnn_cur - pix;
    int dcur = LENGTH_SQ(ddxy);
    if (dcur < dmin)
    {
        dmin = dcur;
        dtnn = dtnn_cur;
    }
}

subroutine(launchSubroutine)
void jumpFloodAlgorithmUpdate()
{
    pix = ivec2(gl_GlobalInvocationID.xy);

    dtnn = imageLoad(im_dtnn_0, pix).xy;

    ivec2 ddxy = dtnn - pix;
    dmin = LENGTH_SQ(ddxy);

    DTNN(pix + ivec2(-jump, jump));     DTNN(pix + ivec2(0, jump));     DTNN(pix + ivec2(jump, jump));
    DTNN(pix + ivec2(-jump, 0));                                        DTNN(pix + ivec2(jump, 0));
    DTNN(pix + ivec2(-jump, -jump));    DTNN(pix + ivec2(0, -jump));    DTNN(pix + ivec2(jump, -jump));

    imageStore(im_dtnn_1, pix, ivec4(dtnn.x, dtnn.y, 0, 0));


    // for pixel pix, get the 8 points at +- k distance away and see if the distance they store is less than the distance pixel pix has

    // store the lowest distance as the new distance for pixel pix in output buffer

    // next iteration flip the input and output buffer

    // after the last iteration the distance transform can be determined for each pixel by getting the length of the vec between pixel pix and distance



}

void main()
{
    jumpFloodSubroutine();

}