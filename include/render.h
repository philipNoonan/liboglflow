#define GLUT_NO_LIB_PRAGMA
//##### OpenGL ######
#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

//#include "shader.hpp"


#include "opencv2/core/utility.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


#include "glhelper.h"


#include <iostream>
#include <fstream>

#include <vector>
#include <list>
#include <numeric>
#include <algorithm>

#include "glutils.h"
#include "glslprogram.h"

#define _USE_MATH_DEFINES
#include <math.h>

class gRender
{
public:
	gRender()
		: m_window()
		, m_show_imgui(true)
		, m_screen_height(540)
		, m_screen_width(960)
		, m_depth_height(424)
		, m_depth_width(512)
		, m_color_height(1080)
		, m_color_width(1920)
		, m_big_depth_height(1082)
		, m_big_depth_width(1920)
		, m_VAO()
		, m_VBO_Color()
		, m_VBO_Depth()
		, m_EBO()
		//, m_graph_points_x()
		//, m_graph_points_y()
		//, m_graph_points_z()
		//, m_graph_points_long_x()
		//, m_graph_points_long_y()
		//, m_graph_points_long_z()
		//, m_graph_vector_x()
	{}
	~gRender();

	GLFWwindow * window()
	{
		return m_window;
	}


	bool showImgui()
	{
		return m_show_imgui;
	}



	GLFWwindow * loadGLFWWindow();

	void compileAndLinkShader();
	void requestShaderInfo();
	void setLocations();
	void setVertPositions();
	void allocateBuffers();
	void allocateTextures();
	void setTextures(GLuint colorTex, GLuint edgesTex, GLuint fmmfTex);
	void setBuffers(GLuint quadlist, GLuint quadlistMeanTemp);
	void setFlowTexture(GLuint flowTex);
	void setDistanceTexture(GLuint distTex)
	{
		m_textureDistance = distTex;
	}

	void bindBuffersForRendering();
	//void setWindowPositions();
	//void setWindowPositions(std::pair<int, int> anchorMW, std::pair<int, int> anchorAS, std::pair<int, int> anchorSG);
	void setWindowLayout();
	//void setupComputeFBO();

	// The correcter way 
	void setRenderingOptions(bool showDepthFlag, bool showBigDepthFlag, bool showInfraFlag, bool showColorFlag, bool showLightFlag, bool showPointFlag, bool showFlowFlag, bool showEdgesFlag, bool showNormalFlag, bool showVolumeSDFFlag, bool showTrackFlag, bool showDistance, bool showQuads);
	void setBuffersForRendering(float * depthArray, float * bigDepthArray, float * colorArray, float * infraArray, unsigned char * flowPtr);
	void setColorImageRenderPosition(float vertFov);
	void setFlowImageRenderPosition(int height, int width, float vertFov);
	void setViewMatrix(float xRot, float yRot, float zRot, float xTran, float yTran, float zTran);
	void setProjectionMatrix();


	void createOffscreenFramebuffer();
	//void setDepthTextureProjectionMatrix();
	//void setColorTextureProjectionMatrix();
	//void setViewport(int x, int y, int w, int h);



	void updateVerts(float w, float h);

	void bindTexturesForRendering();

	//void setVolumeSDFRenderPosition(float slice);

	void Render(bool useInfrared);

	void renderLiveVideoWindow(bool useInfrared);

	void setComputeWindowPosition();



	/*
	void setRVec(cv::Mat rvec)
	{
	rotation_vector = rvec;
	}
	void setTVec(cv::Mat tvec)
	{
	translation_vector = tvec;
	}*/
	void setIrBrightness(float irL, float irH)
	{
		m_ir_low = irL;
		m_ir_high = irH;
	}

	void setFov(float fov)
	{
		m_vertFov = fov;
	}

	void setQuadlistCount(uint32_t cnt)
	{
		m_quadlistCount = cnt;
	}





	void setColorSize(int width, int height)
	{
		m_color_width = width;
		m_color_height = height;
	}

	void setRenderLevel(int lvl)
	{
		m_texLevel = lvl;
	}


private:

	GLSLProgram renderProg;
	GLSLProgram renderFlowLinesProg;

	GLFWwindow * m_window;
	bool m_show_imgui;

	GLuint m_VAO, m_EBO;
	GLuint m_VBO_Standard, m_VBO_Color, m_VBO_Depth;
	std::vector<float> m_standard_verts;
	std::vector<float> m_color_vert;
	std::vector<float> m_depth_vert;
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indices;

	// Offscreen rendering for densifiction
	GLuint m_FBO_FLOW;
	GLuint m_VAO_FLOW;
	GLuint m_VBO_FLOW;
	GLuint m_textureDensifiedFlow;
	GLuint m_RBO_FLOW;

	GLuint m_VAO_MC;
	GLuint m_VBO_Vert_MC;

	GLuint m_programID;
	GLuint m_ProjectionID;
	GLuint m_MvpID;
	GLuint m_zOffID;
	GLuint m_ModelID;
	GLuint m_ViewProjectionID;
	GLuint m_sliceID;
	GLuint m_imSizeID;
	GLuint m_texLevelID;

	GLuint m_MvpFlowID;

	GLuint m_getPositionSubroutineID;
	GLuint m_fromTextureID;
	GLuint m_fromPosition4DID;
	GLuint m_fromPosition2DID;
	GLuint m_fromPosePoints2DID;
	GLuint m_fromFacePoints2DID;
	GLuint m_fromHandsPoints2DID;
	GLuint m_fromStandardTextureID;
	GLuint m_colorSelectionRoutineID;
	GLuint m_fromDepthID;
	GLuint m_fromColorID;
	GLuint m_fromRayNormID;
	GLuint m_fromRayVertID;
	GLuint m_fromPointsID;
	GLuint m_fromVolumeID;
	GLuint m_fromTrackID;
	GLuint m_fromFlowID;
	GLuint m_fromEdgesID;
	GLuint m_fromDistanceID;
	GLuint m_fromQuadlistID;
	GLuint m_fromQuadtreeID;

	GLuint m_ambientID;
	GLuint m_lightID;

	//GLuint m_irLowID;
	//GLuint m_irHighID;


	//textures
	GLuint m_textureDepth;
	//GLuint m_textureInfra;
	GLuint m_textureColor;


	GLuint m_textureVertex;
	GLuint m_textureNormal;
	GLuint m_textureVolume;
	GLuint m_textureTrack;
	GLuint m_textureFlow;
	GLuint m_textureEdges;
	GLuint m_textureDistance;
	GLuint m_textureFlowMinusMeanFlow;

	GLuint m_bufferQuadlist;
	GLuint m_bufferQuadlistMeanTemp;

	GLuint m_texturePreviousColour;
	int frameCount = 0;


	int m_screen_height;
	int m_screen_width;
	int m_depth_height;
	int m_depth_width;
	int m_color_height;
	int m_color_width;
	int m_big_depth_height;
	int m_big_depth_width;





	//glm::mat4 ColorView = glm::translate(glm::mat4(1.0f), glm::vec3(-0.f, -0.f, -0.0f));







	inline int divup(int a, int b) { return (a % b != 0) ? (a / b + 1) : (a / b); }









	//cv::Mat rotation_vector; // Rotation in axis-angle form
	//cv::Mat translation_vector;


	float m_ir_low = 0.0f;
	float m_ir_high = 65536.0f;
	float m_vertFov = 35.0f;
	//std::vector<cv::Point2f> m_detected_points_color;
	//std::vector<cv::Point2f> m_detected_points_infra;

	glm::mat4 m_model_depth = glm::mat4(1.0);
	glm::mat4 m_model_normal = glm::mat4(1.0f);
	glm::mat4 m_model_color = glm::mat4(1.0);
	glm::mat4 m_model_infra = glm::mat4(1.0);
	glm::mat4 m_model_track = glm::mat4(1.0);
	glm::mat4 m_model_flow = glm::mat4(1.0f);
	glm::mat4 m_model_pointcloud = glm::mat4(1.0f);
	glm::mat4 m_model_lightmodel = glm::mat4(1.0f);
	glm::mat4 m_model_MC = glm::mat4(1.0f);
	glm::mat4 m_model_volume = glm::mat4(1.0f);
	glm::mat4 m_model_raynorm = glm::mat4(1.0f);
	glm::mat4 m_view = glm::mat4(1.0f);
	glm::mat4 m_projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 3000.0f); // some default matrix



	bool m_showDepthFlag = false;
	bool m_showNormalFlag = false;
	bool m_showInfraFlag = false;
	bool m_showColorFlag = false;
	bool m_showBigDepthFlag = false;
	bool m_showLightFlag = false;
	bool m_showPointFlag = false;
	bool m_showFlowFlag = false;
	bool m_showEdgesFlag = false;
	bool m_showVolumeSDFFlag = false;
	bool m_showTrackFlag = false;
	bool m_showDistanceFlag = false;
	bool m_showQuadsFlag = false;


	const GLint tcOffsetColumns = 5;
	const GLint tcOffsetRows = 5;

	GLint filterNumberCode = 0;
	GLint texCoordOffsets[5 * 5 * 2];

	GLuint m_tcOffsetID;
	GLuint m_contrastID;

	uint32_t m_quadlistCount;



	int m_texLevel = 0;

};