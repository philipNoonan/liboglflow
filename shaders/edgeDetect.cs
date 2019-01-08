#version 430

layout(local_size_x = 32, local_size_y = 32) in;

uniform float edgeThreshold = 0.1;

uniform int imageType = 0;
uniform int level;

subroutine void launchSubroutine();
subroutine uniform launchSubroutine edgeDetectSubroutine;

// PLEASE REMEMBER THE DOUBLE LINE PROBLEM https://docs.opencv.org/3.2.0/d5/d0f/tutorial_py_gradients.html

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

float luminance(vec3 color)
{

    return rgb2hsv(color).z;
    //return 0.2126 * float(color.x) / 255.0f + 0.7152 * float(color.y) / 255.0f + 0.0722 * float(color.z) / 255.0f;
    //return 0.299 * float(color.x) + 0.587 * float(color.y) + 0.114 * float(color.z);
   // return float(color.x) + float(color.y) + float(color.z);

}

shared float localData[gl_WorkGroupSize.x + 2][gl_WorkGroupSize.y + 2];

layout(binding = 0) uniform sampler2D inputTexture;



layout(binding = 0, r8) uniform image2D OutputImg;


//layout(binding= 3, r16i) uniform iimage2D outputGradientY;

subroutine(launchSubroutine)
void applyFilter()
{
    uvec2 p = gl_LocalInvocationID.xy + uvec2(1, 1);

    float lesser = 3.0f;
    float upper = 10.0f;

    float sx = -(lesser * localData[p.x - 1][p.y - 1] + upper * localData[p.x - 1][p.y] + lesser * localData[p.x - 1][p.y + 1]) + (lesser * localData[p.x + 1][p.y - 1] + upper * localData[p.x + 1][p.y] + lesser * localData[p.x + 1][p.y + 1]);
    float sy = (lesser * localData[p.x - 1][p.y + 1] + upper * localData[p.x][p.y + 1] + lesser * localData[p.x + 1][p.y + 1]) - (lesser * localData[p.x - 1][p.y - 1] + upper * localData[p.x][p.y - 1] + lesser * localData[p.x + 1][p.y - 1]);

    float dist = sx * sx + sy * sy;

    if (dist > edgeThreshold)
        imageStore(OutputImg, ivec2(gl_GlobalInvocationID.xy), vec4(1));
    else
        imageStore(OutputImg, ivec2(gl_GlobalInvocationID.xy), vec4(0));
}



void loadImage(ivec2 pos, ivec2 localDataLoc)
{
    //if (imageType == 0) // rgba8ui
    //{
    ivec2 texSize = textureSize(inputTexture, 0).xy;
    localData[localDataLoc.x][localDataLoc.y] = luminance(texture(inputTexture, vec2(vec2(pos) / vec2(texSize))).xyz);
        // gauss smoothed
        //localData[localDataLoc.x][localDataLoc.y] = luminance(imageLoad(InputImgUI, pos).xyz);
        //localData[localDataLoc.x][localDataLoc.y] = luminance(imageLoad(InputImgUI, ivec2(pos.x - 1, pos.y - 1)).xyz) * 0.077847f + luminance(imageLoad(InputImgUI, ivec2(pos.x, pos.y - 1)).xyz) * 0.123317f + luminance(imageLoad(InputImgUI, ivec2(pos.x + 1, pos.y - 1)).xyz) * 0.077847f +
        //                                     luminance(imageLoad(InputImgUI, ivec2(pos.x - 1, pos.y)).xyz) * 0.123317f + luminance(imageLoad(InputImgUI, ivec2(pos.x, pos.y)).xyz) * 0.195346f + luminance(imageLoad(InputImgUI, ivec2(pos.x + 1, pos.y)).xyz) * 0.123317f +
        //                                     luminance(imageLoad(InputImgUI, ivec2(pos.x - 1, pos.y + 1)).xyz) * 0.077847f + luminance(imageLoad(InputImgUI, ivec2(pos.x, pos.y + 1)).xyz) * 0.123317f + luminance(imageLoad(InputImgUI, ivec2(pos.x + 1, pos.y + 1)).xyz) * 0.077847f;

    // }

}

void main()
{
    uvec2 gSize = gl_WorkGroupSize.xy * gl_NumWorkGroups.xy;

    // Copy into local memory
    loadImage(ivec2(gl_GlobalInvocationID.xy), ivec2(gl_LocalInvocationID.x + 1, gl_LocalInvocationID.y + 1));

    // Handle the edges
    // Bottom edge
    if (gl_LocalInvocationID.y == 0)
    {
        if (gl_GlobalInvocationID.y > 0)
        {
            loadImage(ivec2(gl_GlobalInvocationID.xy + ivec2(0, -1)), ivec2(gl_LocalInvocationID.x + 1, 0));

            // Lower left corner
            if (gl_LocalInvocationID.x == 0)
            {
                if (gl_GlobalInvocationID.x > 0)
                {
                    loadImage(ivec2(gl_GlobalInvocationID.xy + ivec2(-1, -1)), ivec2(0, 0));
                }
                else
                {
                    localData[0][0] = 0.0;
                }
            }
                

            // Lower right corner
            if (gl_LocalInvocationID.x == gl_WorkGroupSize.x - 1)
            {
                if (gl_GlobalInvocationID.x < gSize.x - 1)
                {
                    loadImage(ivec2(gl_GlobalInvocationID.xy + ivec2(1, -1)), ivec2(gl_WorkGroupSize.x + 1, 0));
                }
                else
                {
                    localData[gl_WorkGroupSize.x + 1][0] = 0.0;
                }
            }

        }
        else
        {
            localData[gl_LocalInvocationID.x + 1][0] = 0.0;
        }

    }
    // Top edge
    if (gl_LocalInvocationID.y == gl_WorkGroupSize.y - 1)
    {
        if (gl_GlobalInvocationID.y < gSize.y - 1)
        {
            loadImage(ivec2(gl_GlobalInvocationID.xy + ivec2(0, 1)), ivec2(gl_LocalInvocationID.x + 1, gl_WorkGroupSize.y + 1));
            // Upper left corner
            if (gl_LocalInvocationID.x == 0)
            {
                if (gl_GlobalInvocationID.x > 0)
                {
                    loadImage(ivec2(gl_GlobalInvocationID.xy) + ivec2(-1, 1), ivec2(0, gl_WorkGroupSize.y + 1));
                }
                else
                {
                    localData[0][gl_WorkGroupSize.y + 1] = 0.0;
                }
            }

            // Lower right corner
            if (gl_LocalInvocationID.x == gl_WorkGroupSize.x - 1)
            {
                if (gl_GlobalInvocationID.x < gSize.x - 1)
                {
                    loadImage(ivec2(gl_GlobalInvocationID.xy) + ivec2(1, 1), ivec2(gl_WorkGroupSize.x + 1, gl_WorkGroupSize.y + 1));
                }
                else
                {
                    localData[gl_WorkGroupSize.x + 1][gl_WorkGroupSize.y + 1] = 0.0;
                }

            }

        }
        else
        {
            localData[gl_LocalInvocationID.x + 1][gl_WorkGroupSize.y + 1] = 0.0;
        }
    }
    // Left edge
    if (gl_LocalInvocationID.x == 0)
    {
        if (gl_GlobalInvocationID.x > 0)
        {
            loadImage(ivec2(gl_GlobalInvocationID.xy) + ivec2(-1, 0), ivec2(0, gl_LocalInvocationID.y + 1));
        }
        else
        {
            localData[0][gl_LocalInvocationID.y + 1] = 0.0;
        }

    }
    // Right edge
    if (gl_LocalInvocationID.x == gl_WorkGroupSize.x - 1)
    {
        if (gl_GlobalInvocationID.x < gSize.x - 1)
        {
            loadImage(ivec2(gl_GlobalInvocationID.xy) + ivec2(1, 0), ivec2(gl_WorkGroupSize.x + 1, gl_LocalInvocationID.y + 1));
        }
        else
        {
            localData[gl_WorkGroupSize.x + 1][gl_LocalInvocationID.y + 1] = 0.0;
        }
        


    }

    barrier();

    edgeDetectSubroutine();
}