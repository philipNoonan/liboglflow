#include "render.h"

gRender::~gRender()
{
}


GLFWwindow * gRender::loadGLFWWindow()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_REFRESH_RATE, 5);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	m_window = glfwCreateWindow(m_screen_width, m_screen_height, "oglflow", nullptr, nullptr);

	if (m_window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		//return -1;
	}

	glfwMakeContextCurrent(m_window);
	//glfwSwapInterval(1); // Enable vsync

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		//return -1;
	}

	return m_window;
}


void gRender::requestShaderInfo()
{
	renderProg.printActiveUniforms();
}

void gRender::compileAndLinkShader()
{
	try {
		renderProg.compileShader("shaders/vertShader.vs");
		renderProg.compileShader("shaders/fragShader.fs");
		renderProg.link();

		renderFlowLinesProg.compileShader("shaders/vertShaderFL.vs");
		renderFlowLinesProg.compileShader("shaders/geomShaderFL.gs");
		renderFlowLinesProg.compileShader("shaders/fragShaderFL.fs");
		renderFlowLinesProg.link();

	}
	catch (GLSLProgramException &e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

void gRender::setLocations()
{
	m_ProjectionID = glGetUniformLocation(renderProg.getHandle(), "projection");
	m_MvpID = glGetUniformLocation(renderProg.getHandle(), "MVP");
	m_ModelID = glGetUniformLocation(renderProg.getHandle(), "model");
	m_ViewProjectionID = glGetUniformLocation(renderProg.getHandle(), "ViewProjection");
	m_sliceID = glGetUniformLocation(renderProg.getHandle(), "slice");
	m_imSizeID = glGetUniformLocation(renderProg.getHandle(), "imSize");
	m_texLevelID = glGetUniformLocation(renderProg.getHandle(), "texLevel");
	m_zOffID = glGetUniformLocation(renderProg.getHandle(), "zOff");

	m_getPositionSubroutineID = glGetSubroutineUniformLocation(renderProg.getHandle(), GL_VERTEX_SHADER, "getPositionSubroutine");
	m_fromTextureID = glGetSubroutineIndex(renderProg.getHandle(), GL_VERTEX_SHADER, "fromTexture");
	m_fromPosition4DID = glGetSubroutineIndex(renderProg.getHandle(), GL_VERTEX_SHADER, "fromPosition4D");
	m_fromPosition2DID = glGetSubroutineIndex(renderProg.getHandle(), GL_VERTEX_SHADER, "fromPosition2D");
	m_fromPosePoints2DID = glGetSubroutineIndex(renderProg.getHandle(), GL_VERTEX_SHADER, "fromPosePoints2D");
	m_fromFacePoints2DID = glGetSubroutineIndex(renderProg.getHandle(), GL_VERTEX_SHADER, "fromFacePoints2D");
	m_fromHandsPoints2DID = glGetSubroutineIndex(renderProg.getHandle(), GL_VERTEX_SHADER, "fromHandsPoints2D");
	m_fromStandardTextureID = glGetSubroutineIndex(renderProg.getHandle(), GL_VERTEX_SHADER, "fromStandardTexture");
	m_fromQuadlistID = glGetSubroutineIndex(renderProg.getHandle(), GL_VERTEX_SHADER, "fromQuadlist");


	m_colorSelectionRoutineID = glGetSubroutineUniformLocation(renderProg.getHandle(), GL_FRAGMENT_SHADER, "getColorSelection");
	m_fromDepthID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromDepth");
	m_fromColorID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromColor");
	m_fromRayNormID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromRayNorm");
	m_fromRayVertID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromRayVert");
	m_fromPointsID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromPoints");
	m_fromVolumeID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromVolume");
	m_fromTrackID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromTrack");
	m_fromFlowID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromFlow");
	m_fromEdgesID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromEdges");
	m_fromDistanceID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromDistance");
	m_fromQuadtreeID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromQuadtree");
	//m_fromQuadlistID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromQuadlist");

	m_MvpFlowID = glGetUniformLocation(renderFlowLinesProg.getHandle(), "MVP");


	//m_ambientID = glGetUniformLocation(renderProg.getHandle(), "ambient");
	//m_lightID = glGetUniformLocation(renderProg.getHandle(), "light");

	//m_irLowID = glGetUniformLocation(renderProg.getHandle(), "irLow");
	//m_irHighID = glGetUniformLocation(renderProg.getHandle(), "irHigh");

}

void gRender::updateVerts(float w, float h)
{
	std::vector<float> vertices = {
		// Positions		// Texture coords
		w / 2.0f, h / 2.0f, 0.0f, 1.0f, 1.0f, // top right
		w / 2.0f, -h / 2.0f, 0.0f, 1.0f, 0.0f, // bottom right
		-w / 2.0f, -h / 2.0f, 0.0f, 0.0f, 0.0f, // bottom left
		-w / 2.0f, h / 2.0f, 0.0f, 0.0f, 1.0f  // Top left
	};

	m_standard_verts = vertices;

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_Standard);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_standard_verts.size() * sizeof(float), m_standard_verts.data());

}

void gRender::setVertPositions()
{
	std::vector<float> vertices = {
		// Positions				// Texture coords
		1.0f,	1.0f,	0.0f,		1.0f, 1.0f, // top right
		1.0f,	-1.0f,	0.0f,		1.0f, 0.0f, // bottom right
		-1.0f,	-1.0f,	0.0f,		0.0f, 0.0f, // bottom left
		-1.0f,	1.0f,	0.0f,		0.0f, 1.0f  // Top left
	};

	m_standard_verts = vertices;

	m_vertices = vertices;

	std::vector<unsigned int>  indices = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};

	m_indices = indices;



}

void gRender::allocateTextures()
{
	m_texturePreviousColour = GLHelper::createTexture(m_texturePreviousColour, GL_TEXTURE_2D, 1, m_color_width, m_color_height, 0, GL_RGBA8);




}

void gRender::allocateBuffers()
{
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO_Standard);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);

	// standard verts
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_Standard);
	glBufferData(GL_ARRAY_BUFFER, m_standard_verts.size() * sizeof(float), &m_standard_verts[0], GL_DYNAMIC_DRAW);
	// EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_DYNAMIC_DRAW);
	// Position attribute for Depth
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(4);
	//// TexCoord attribute for Depth
	glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(5);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);




	// FOR QUADLIST
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferQuadlist);
	glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(11);

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferQuadlistMeanTemp);
	glVertexAttribPointer(12, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(12);

	//glVertexAttribIPointer(11, 1, GL_FLOAT, 4 * sizeof(float), (GLvoid*)0);
	//glVertexAttribDivisor(11, 1); // IMPORTANT https://learnopengl.com/Advanced-OpenGL/Instancing

	glBindVertexArray(0);

	// FOR FLOW DENSIFICATION
	//glGenVertexArrays(1, &m_VAO_FLOW);
	//glGenBuffers(1, &m_VBO_FLOW);
	//glBindVertexArray(m_VAO_FLOW);
	//glBindBuffer(GL_ARRAY_BUFFER, m_VBO_FLOW);
	//glBufferData(GL_ARRAY_BUFFER, m_standard_verts.size() * sizeof(float), &m_standard_verts[0], GL_DYNAMIC_DRAW);
	//glBindVertexArray(0);




}

void gRender::setBuffers(GLuint quadList, GLuint quadlistMeanTemp) 
{
	m_bufferQuadlist = quadList;
	m_bufferQuadlistMeanTemp = quadlistMeanTemp;
}

void gRender::createOffscreenFramebuffer()
{
	glGenFramebuffers(1, &m_FBO_FLOW);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO_FLOW);

	glGenTextures(1, &m_textureDensifiedFlow);
	glBindTexture(GL_TEXTURE_2D, m_textureDensifiedFlow);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, 960, 540, 0, GL_RG, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureDensifiedFlow, 0);

	glGenRenderbuffers(1, &m_RBO_FLOW);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO_FLOW);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 960, 540); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO_FLOW); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


void gRender::setComputeWindowPosition()
{
	glViewport(0, 0, 1920, 1080);
}

void gRender::setRenderingOptions(bool showDepthFlag, bool showBigDepthFlag, bool showInfraFlag, bool showColorFlag, bool showLightFlag, bool showPointFlag, bool showFlowFlag, bool showEdgesFlag, bool showNormalFlag, bool showVolumeSDFFlag, bool showTrackFlag, bool showDistance, bool showQuads)
{
	m_showDepthFlag = showDepthFlag;
	m_showBigDepthFlag = showBigDepthFlag;
	m_showInfraFlag = showInfraFlag;
	m_showColorFlag = showColorFlag;
	m_showLightFlag = showLightFlag;
	m_showPointFlag = showPointFlag;
	m_showFlowFlag = showFlowFlag;
	m_showEdgesFlag = showEdgesFlag;
	m_showNormalFlag = showNormalFlag;
	m_showVolumeSDFFlag = showVolumeSDFFlag;
	m_showTrackFlag = showTrackFlag;
	m_showDistanceFlag = showDistance;
	m_showQuadsFlag = showQuads;
}

void gRender::setTextures(GLuint colorTex, GLuint edgesTex, GLuint fmmfTex)
{

	m_textureColor = colorTex;
	m_textureEdges = edgesTex;
	m_textureFlowMinusMeanFlow = fmmfTex;
	

}
void gRender::setFlowTexture(GLuint flowTex)
{
	m_textureFlow = flowTex;
}

void gRender::bindTexturesForRendering()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, m_textureColor);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_textureFlow);
	


		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, m_textureColor);
	


		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, m_textureEdges);
	

	
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, m_textureDistance);
	



}




void gRender::Render(bool useInfrared)
{
	int display_w, display_h;

	glfwGetFramebufferSize(m_window, &display_w, &display_h);

	//// Rendering
	glViewport(0, 0, display_w, display_h);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// set positions
	// set uniforms
	// render textures
	bindTexturesForRendering();
	//setDepthImageRenderPosition();
	//setNormalImageRenderPosition();
	//setViewport(0, 0, 1920, 1080);

	renderLiveVideoWindow(useInfrared);



}



void gRender::setColorImageRenderPosition(float vertFov)
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	//// if setting z dist
	//float zDist = 8000.0f;
	//float halfHeightAtDist = zDist * tan(22.5f * M_PI / 180.0f);
	//float halfWidthAtDistance = halfHeightAtDist * (float)w / (float)h; // notsure why this ratio is used here...
	//// else if setting size on window
	float zDist;
	zDist = ((float)m_color_height * 1) / tan(vertFov * M_PI / 180.0f);
	//float halfHeightAtDist = (float)h * 4;
	//float halfWidthAtDistance = (float)w * 4;
	//m_model_color = glm::translate(glm::mat4(1.0f), glm::vec3(-m_color_width / 2.0f, -halfHeightAtDist, -zDist));
	//glm::vec3 scaleVec = glm::vec3(6.f, 6.f, 1.0f);

	//m_model_color = glm::scale(glm::mat4(1.0f), scaleVec);
	m_model_color = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5, -0.5, -1.0));

	//std::cout << "zDis" << zDist << "w " << w << " h" << h << " ad " << halfWidthAtDistance << std::endl;
	//m_model_color = glm::translate(glm::mat4(1.0f), glm::vec3(-m_color_width / 2.0f, 0.0f, -2000.0f));
}


void gRender::setFlowImageRenderPosition(int height, int width, float vertFov) 
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	//// if setting z dist
	//float zDist = 8000.0f;
	//float halfHeightAtDist = zDist * tan(22.5f * M_PI / 180.0f);
	//float halfWidthAtDistance = halfHeightAtDist * (float)w / (float)h; // notsure why this ratio is used here...
	//// else if setting size on window
	float zDist;
	zDist = ((float)height * 1) / tan(vertFov * M_PI / 180.0f);
	//float halfHeightAtDist = (float)h * 4;
	//float halfWidthAtDistance = (float)w * 4;
	//m_model_color = glm::translate(glm::mat4(1.0f), glm::vec3(-m_color_width / 2.0f, -halfHeightAtDist, -zDist));
	glm::vec3 scaleVec = glm::vec3(1.f,1.f, 1.0f);

	//m_model_flow = glm::scale(glm::mat4(1.0f), scaleVec);
	m_model_flow = glm::translate(glm::mat4(1.0f), glm::vec3(-width / 2.0f, -height / 2.0f, -zDist));

	}


void gRender::setViewMatrix(float xRot, float yRot, float zRot, float xTran, float yTran, float zTran)
{
	glm::mat4 t0, t1, r0;
	m_view = glm::mat4(1.0f);

	t0 = glm::translate(glm::mat4(1.0), glm::vec3(xTran, yTran, zTran));
	t1 = glm::translate(glm::mat4(1.0), glm::vec3(-xTran, -yTran, -zTran));

	r0 = glm::eulerAngleXYZ(glm::radians(xRot), glm::radians(yRot), glm::radians(zRot));


	m_view = t1 * r0 * t0;
	//m_view = glm::translate(m_view, glm::vec3(0.0f, 0.0f, 0.0f));

}

void gRender::setProjectionMatrix()
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	m_projection = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 100.0f); // scaling the texture to the current window size seems to work
	glViewport(0, 0, w, h);


}



void gRender::renderLiveVideoWindow(bool useInfrared)
{
	//if (m_showFlowFlag)
	//{

	//if (frameCount == 99)
	//{

	//	glCopyImageSubData(m_textureColor, GL_TEXTURE_2D, 0, 0, 0, 0,
	//		m_texturePreviousColour, GL_TEXTURE_2D, 0, 0, 0, 0,
	//		m_color_width, m_color_height, 1);

	//	cv::Mat prevCol = cv::Mat(m_color_height, m_color_width, CV_8UC4);

	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, m_texturePreviousColour);
	//	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, prevCol.data);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//	glActiveTexture(0);

	//	cv::imshow("prevCol", prevCol);
	//	cv::waitKey(1);

	//}
	//if (frameCount == 100)
	//{
	//	frameCount = 0;




	//}
	//frameCount++;


	//}

	//else
	//{

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		renderProg.use();
		glm::mat4 MVP;
		glm::vec2 imageSize = glm::vec2(m_color_width, m_color_height);

		if (m_showColorFlag)
		{
			glm::vec2 imageSize;

			imageSize = glm::vec2(m_color_width, m_color_height);
			MVP = m_projection * m_view * m_model_color;

			//glTexParameteri(m_textureColor, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			//glTexParameteri(m_textureColor, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glBindVertexArray(m_VAO);

			glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_fromStandardTextureID);
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_fromColorID);
			//glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(m_projection));
			glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));
			glUniform2fv(m_imSizeID, 1, glm::value_ptr(imageSize));
			glUniform1i(m_texLevelID, m_texLevel);
			glUniform1f(m_zOffID, 0.0f);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			//glTexParameteri(m_textureColor, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			//glTexParameteri(m_textureColor, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}

		if (m_showEdgesFlag)
		{
			glm::vec2 imageSize;

			imageSize = glm::vec2(m_color_width, m_color_height);
			MVP = m_projection * m_view * m_model_color;

			glBindVertexArray(m_VAO);
			glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_fromStandardTextureID);
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_fromEdgesID);
			glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));
			glUniform2fv(m_imSizeID, 1, glm::value_ptr(imageSize));
			glUniform1f(m_zOffID, 0.0f);

			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);

		}

		if (1)
		{
			glm::vec2 imageSize;

			imageSize = glm::vec2(m_color_width, m_color_height);
			MVP = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

			glBindVertexArray(m_VAO);
			//MVP = glm::translate(MVP, glm::vec3(0.0f, 0.0f, 0.25f));
			glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_fromStandardTextureID);
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_fromFlowID);
			//glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(m_projection));
			glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));
			glUniform1f(m_zOffID, -0.1f);
			glUniform2fv(m_imSizeID, 1, glm::value_ptr(imageSize));
			glUniform1i(m_texLevelID, m_texLevel);
			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);

		}

		if (m_showDistanceFlag)
		{
			glm::vec2 imageSize;

			imageSize = glm::vec2(m_color_width, m_color_height);
			MVP = m_projection * m_view * m_model_color;

			glBindVertexArray(m_VAO);
			MVP = glm::translate(MVP, glm::vec3(0.0f, 0.0f, 5.0f));
			glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_fromStandardTextureID);
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_fromDistanceID);
			//glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(m_projection));
			glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));
			glUniform2fv(m_imSizeID, 1, glm::value_ptr(imageSize));
			glUniform1i(m_texLevelID, m_texLevel);
			glUniform1f(m_zOffID, 0.0f);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		}

		if (m_showQuadsFlag)
		{
			// here we will pass the quadlist buffer
			// the vertex shader will create pixels quads centered at the center of the quad and of the correct size
			// in the fragment shader we will get the std dev from the quad and 
			// if the std dev is below a thresh, the output gl fragment color is the pass through quad colour
			// if std dev is high, then it is unreliable flow, and output flow is written as zero
			
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			MVP = m_projection * m_view * glm::translate(m_model_color, glm::vec3(0.0f, 0.0f, 0.5f));

			glBindVertexArray(m_VAO);
			//MVP = glm::translate(MVP, glm::vec3(0.0f, 0.0f, 0.5f));

			glEnableVertexAttribArray(11);
			glBindBuffer(GL_ARRAY_BUFFER, m_bufferQuadlist);
			glEnableVertexAttribArray(12);
			glBindBuffer(GL_ARRAY_BUFFER, m_bufferQuadlistMeanTemp);

			//MVP = glm::translate(MVP, glm::vec3(0.0f, 0.0f, 0.5f));
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, m_textureFlowMinusMeanFlow);

			//glUniformMatrix4fv(m_MvpFlowID, 1, GL_FALSE, glm::value_ptr(MVP));
			glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));
			glUniform2fv(m_imSizeID, 1, glm::value_ptr(imageSize));

			glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_fromQuadlistID);
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_fromQuadtreeID);
			//glDrawArrays(GL_POINTS, 0, 2);
			glUniform1f(m_zOffID, -0.2f);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			//glDrawArrays(GL_POINTS, 0, m_quadlistCount);


		}

		if (m_showPointFlag)
		{
			renderFlowLinesProg.use();
			glBindVertexArray(m_VAO_FLOW);
			//MVP = m_view * m_model_color;

			//MVP = glm::translate(MVP, glm::vec3(0.0f, 0.0f, 0.5f));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_textureFlow);
			//glUniformMatrix4fv(m_MvpFlowID, 1, GL_FALSE, glm::value_ptr(MVP));

			glDrawArraysInstanced(GL_POINTS, 0, 1, 1920 * 1080 / 16);
		}



	//} // the attempt at geometry rendering lines






}


