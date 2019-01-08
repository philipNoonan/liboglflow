#ifndef OGLFLOW_H
#define OGLFLOW_H

#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

#include <stdio.h>
#include <iostream>
#define GLUT_NO_LIB_PRAGMA
//##### OpenGL ######
#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <deque>

//#include "openCVStuff.h"
#include "flow.h"
#include "flood.h"
#include "render.h"

#include "opencv2/core/utility.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/optflow.hpp"

#include "frameGrabber.h"

//#include "OpenPoseWrapper.h"

#include <thread>
#include <mutex>

namespace OglFlow {

	GLFWwindow *window;

	gRender grender;

	gFlow gflow;

	gFlood gflood;

	FrameGrabber fGrabber;

	cv::Mat newcol;

	/////////////////////////
	// CAMERA STUFF

	const int screenWidth = 1920;
	const int screenHeight = 1080;

	int colorWidth;
	int colorHeight;

	bool useWebcamFlag = 1;
	bool useImagesFlag = 0;
	bool useVideosFlag = 0;


	// FLOW STUFF
	//cv::VideoCapture cap;

	std::vector<cv::Mat> imagesFromFile;
	std::vector<cv::VideoCapture> videosFromFile;
	int videoNumber = 0;
	int videoFrameNumber = 0;

	std::vector<std::pair<int, int> > resoPresetPair;
	int resoPreset = 1;
	int imageNumber = 0;

	bool changedSource = false;

	cv::Mat col;

	std::mutex mtx;
	std::thread *m_thread;

	int m_status = 0;

	cv::VideoWriter outWriter;

	void setColorSize(int width, int height)
	{
		colorWidth = width;
		colorHeight = height;
	}


	void gRenderInit()
	{
		grender.compileAndLinkShader();
		grender.setColorSize(colorWidth, colorHeight);

		grender.setBuffers(gflow.getQuadlist(), gflow.getQuadlistMeanTemp());

		grender.setLocations();
		grender.setVertPositions();
		grender.allocateBuffers();
		grender.allocateTextures();
		grender.setTextures(gflow.getColorTexture(), gflow.getEdgesTexture(), gflow.getFlowMinusMeanFlowTexture()); //needs texture uints from gfusion init
		grender.setFlowTexture(gflow.getFlowTexture());

	}

	void resetFlowSize()
	{
		gflow.firstFrame = true;
		//gflow.clearTexturesAndBuffers();
		gflow.setNumLevels(colorWidth);
		gflow.setTextureParameters(colorWidth, colorHeight);
		gflow.allocateTextures(false);
		gflow.allocateBuffers();
		gflow.allocateOffscreenRendering();

		grender.setBuffers(gflow.getQuadlist(), gflow.getQuadlistMeanTemp());

		grender.setColorSize(colorWidth, colorHeight);
		grender.allocateBuffers();

		fGrabber.setImageDimensions(colorWidth, colorHeight);
		fGrabber.resetImageDimensions();



		changedSource = false;
	}

	void gFlowInit()
	{
		gflow.compileAndLinkShader();
		gflow.setLocations();

		gflow.setNumLevels(colorWidth);
		gflow.setTextureParameters(colorWidth, colorHeight);
		gflow.allocateTextures(false);

		gflow.allocateBuffers();
		gflow.allocateOffscreenRendering();
	}

	void gFloodInit()
	{
		gflood.compileAndLinkShader();
		gflood.setLocations();
	}
}



#endif

