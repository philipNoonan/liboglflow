#version 430 core
layout (location = 0) in vec3 positionColor;
layout (location = 1) in vec2 texCoordColor;
layout (location = 2) in vec3 positionDepth;
layout (location = 3) in vec2 texCoordDepth;

layout (location = 4) in vec3 position;
layout (location = 5) in vec2 texCoord;

layout (location = 6) in vec4 position4D;

layout (location = 7) in vec2 trackedPoints; 

layout (location = 8) in vec3 facePoints; 
layout (location = 9) in vec3 posePoints; 
layout (location = 10) in vec3 handsPoints; 

layout (location = 11) in vec4 quadlist; 
layout (location = 12) in vec4 quadlistMeanTemp; 

//layout (std430, binding = 7) buffer trackedPoints; 

uniform mat4 model;
uniform mat4 ViewProjection;
uniform mat4 projection;
uniform mat4 MVP;
uniform vec2 imSize;
uniform float zOff;

out vec2 TexCoord;
out float zDepth;
out vec2 stdDev;

subroutine vec4 getPosition();
subroutine uniform getPosition getPositionSubroutine;

subroutine(getPosition)
vec4 fromTexture()
{
	TexCoord = vec2(texCoordColor.x, 1 - texCoordColor.y);
	return vec4(MVP * vec4(positionDepth, 1.0f));
}

subroutine(getPosition)
vec4 fromStandardTexture()
{
 // an approach that doesnt require CPU passed vertices, very nice
 // taken from demanze answer at https://stackoverflow.com/questions/2588875/whats-the-best-way-to-draw-a-fullscreen-quad-in-opengl-3-2
	float x = float(((uint(gl_VertexID) + 2u) / 3u)%2u);
    float y = float(((uint(gl_VertexID) + 1u) / 3u)%2u); 

	TexCoord = vec2(x, 1.0 - y);

	return vec4(-1.0f + x*2.0f, -1.0f+y*2.0f, zOff, 1.0f);
}

subroutine(getPosition)
vec4 fromPosition4D()
{
	return vec4(MVP * vec4(position4D.xyz, 1.0f));
	zDepth = position4D.z;
}

subroutine(getPosition)
vec4 fromPosition2D()
{
    //gl_PointSize = 0.5f;
	return vec4(MVP * vec4(trackedPoints.x, imSize.y - trackedPoints.y, 0.0f, 1.0f));
	//	return vec4(trackedPoints.xy, 1000.0f, 1.0f);

}

subroutine(getPosition)
vec4 fromPosePoints2D()
{
    gl_PointSize = 3;//
	zDepth = posePoints.z;
	return vec4(MVP * vec4(posePoints.x, imSize.y - posePoints.y, 0.0f, 1.0f));
}

subroutine(getPosition)
vec4 fromFacePoints2D()
{
    gl_PointSize = 3;//
	zDepth = facePoints.z * 10.0f;
	return vec4(MVP * vec4(facePoints.x, imSize.y - facePoints.y, 0.0f, 1.0f));
}

subroutine(getPosition)
vec4 fromHandsPoints2D()
{
    gl_PointSize = 20;//
	zDepth = handsPoints.z * 10.0f;
	return vec4(MVP * vec4(handsPoints.x, imSize.y - handsPoints.y, 0.0f, 1.0f));
}

subroutine(getPosition)
vec4 fromQuadlist()
{
    // to render on screen we have to map the quad dims to -1 -> 1 NDC, 
	// as they are currently from 0 -> nextPowerofTwoUp(imageSize)^2
	
	// uint xPos = (octlist & 4286578688) >> 23;
	// uint yPos = (octlist & 8372224) >> 14;
	// uint zPos = (octlist & 16352) >> 5;
	// uint lod = (octlist & 31);
	/*
	uint xPos = uint(quadlist.x);
	uint yPos = uint(quadlist.y);
	uint lod = uint(quadlist.z);

	stdDev = quadlistMeanTemp.xy;

	float quadSideLength = float(pow(2, lod)); //

	vec2 origin = ((vec2(xPos, yPos) * quadSideLength) + (quadSideLength * 0.5f)); // 

	//gl_PointSize = (quadSideLength);

	 
	vec4 pos = vec4((origin.x / (imSize.x / 2.0f))-1.0f, (origin.y / (imSize.y / 2.0f)) - 1.0f, 0.0f, 1.0f);


	// 1920.0f here is the current window size
	//gl_PointSize = max(int(quadSideLength * (1920.0/imSize.x)) - 1, 1);
	gl_PointSize = quadSideLength * (1280.0/imSize.x);

	return vec4(pos.x, -pos.y, pos.z, pos.w);
	*/

	float x = float(((uint(gl_VertexID) + 2u) / 3u)%2u); // u is just the type qualifer, like f, i think
    float y = float(((uint(gl_VertexID) + 1u) / 3u)%2u); 

	TexCoord = vec2(x, 1.0 - y);

	return vec4(-1.0f + x*2.0f, -1.0f+y*2.0f, 0.f, 1.0f);

}

void main()
{
	gl_Position = getPositionSubroutine();
}