#include "flood.h"


#include "opencv2/core/utility.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

void gFlood::compileAndLinkShader()
{
	try { 

	jumpFloodProg.compileShader("shaders/jumpFlood.cs");
	jumpFloodProg.link();

	edgeDetectProg.compileShader("shaders/edgeDetect.cs");
	edgeDetectProg.link();
	 
	}            
	catch (GLSLProgramException &e) {        
		std::cerr << e.what() << std::endl;           
		exit(EXIT_FAILURE);          
	}                                                 
}                                                                          
void gFlood::setLocations()
{                        
	// jump flood algorithm
	m_subroutine_jumpFloodID = glGetSubroutineUniformLocation(jumpFloodProg.getHandle(), GL_COMPUTE_SHADER, "jumpFloodSubroutine");
	m_jfaInitID = glGetSubroutineIndex(jumpFloodProg.getHandle(), GL_COMPUTE_SHADER, "jumpFloodAlgorithmInit");
	m_jfaUpdateID = glGetSubroutineIndex(jumpFloodProg.getHandle(), GL_COMPUTE_SHADER, "jumpFloodAlgorithmUpdate");
	m_getColorID = glGetSubroutineIndex(jumpFloodProg.getHandle(), GL_COMPUTE_SHADER, "getColorFromRGB");

	m_jumpID = glGetUniformLocation(jumpFloodProg.getHandle(), "jump");


	m_subroutine_edgeDetectID = glGetSubroutineUniformLocation(edgeDetectProg.getHandle(), GL_COMPUTE_SHADER, "edgeDetectSubroutine");
	m_applyFilterID = glGetSubroutineIndex(edgeDetectProg.getHandle(), GL_COMPUTE_SHADER, "applyFilter");
	m_edgeThresholdID = glGetUniformLocation(edgeDetectProg.getHandle(), "edgeThreshold");
	
	glGenQueries(1, timeQuery);

} 
 
   
GLuint gFlood::createTexture(GLuint ID, GLenum target, int levels, int w, int h, int d, GLuint internalformat)
{
	GLuint texid;

	if (ID == 0)
	{
		glGenTextures(1, &texid);
	}
	else
	{
		glDeleteTextures(1, &ID);
		texid = ID;
		glGenTextures(1, &texid);
	}
	glBindTexture(target, texid);


	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// https://stackoverflow.com/questions/15405869/is-gltexstorage2d-imperative-when-auto-generating-mipmaps
	//glTexImage2D(target, 0, internalformat, w, h, 0, format, type, 0); // cretes mutable storage that requires glTexImage2D

	if (target == GL_TEXTURE_1D) 
	{   
		glTexStorage1D(target, levels, internalformat, w); 
	}  
	else if (target == GL_TEXTURE_2D)  
	{
		glTexStorage2D(target, levels, internalformat, w, h); // creates immutable storage and requires glTexSubImage2D

	}  
	else if (target == GL_TEXTURE_3D || d > 0) 
	{
		glTexStorage3D(target, levels, internalformat, w, h, d);  
	} 
	return texid;      
}            
     
void gFlood::allocateBuffers()
{ 
	//m_trackedPoints.resize(2, 0.0f);
	/*glGenBuffers(1, &m_bufferClickedPoints);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bufferClickedPoints);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_bufferClickedPoints);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_trackedPoints.size() * sizeof(float), m_trackedPoints.data(), GL_DYNAMIC_DRAW);*/
}  
    
          
void gFlood::allocateTextures()
{
	
	// JFA stuff
	std::vector<float> zeroVals(m_texture_width * m_texture_height * 2, 0);
	m_texture_initial = createTexture(m_texture_initial, GL_TEXTURE_2D, 1, m_texture_width, m_texture_height, 0, GL_R8);

	m_texture_jfa_0 = createTexture(m_texture_jfa_0, GL_TEXTURE_2D, 1, m_texture_width, m_texture_height, 0, GL_RG32F);
	m_texture_jfa_1 = createTexture(m_texture_jfa_1, GL_TEXTURE_2D, 1, m_texture_width, m_texture_height, 0, GL_RG32F);

	//glBindTexture(GL_TEXTURE_2D, m_texture_initial);
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_width, m_texture_height, GL_RG, GL_FLOAT, zeroVals.data());
	m_texture_initial_RGB = createTexture(m_texture_initial_RGB, GL_TEXTURE_2D, 1, m_texture_width, m_texture_height, 0, GL_RGB8);
	m_texture_output_RGBA = createTexture(m_texture_output_RGBA, GL_TEXTURE_2D, 1, m_texture_width, m_texture_height, 0, GL_RGBA8);
}   

void gFlood::pushBackTP(float x, float y)
{
	//m_trackedPoints.push_back(x);
	//m_trackedPoints.push_back(y);

	//for (int u = -50; u < 50; u++)
	//{
	//	m_trackedPoints.push_back(x + u);
	//	m_trackedPoints.push_back(y);
	//}

	//for (int v = -50; v < 50; v++)
	//{
	//	m_trackedPoints.push_back(x);
	//	m_trackedPoints.push_back(y + v);
	//}

	//for (auto i : m_trackedPoints)
	//	std::cout << i << " " << std::endl;
}
void gFlood::uploadTP()
{
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bufferClickedPoints);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, m_trackedPoints.size() * sizeof(float), m_trackedPoints.data(), GL_DYNAMIC_DRAW);

	//wipeFlood();
	//std::vector<uint32_t> points(2);
	//for (size_t i = 0; i < m_trackedPoints.size(); i += 2)
	//{
	//	points[0] = m_trackedPoints[i];
	//	points[1] = m_trackedPoints[i + 1];
	//	glBindTexture(GL_TEXTURE_2D, m_texture_initial);
	//	glTexSubImage2D(GL_TEXTURE_2D, 0, points[0], points[1], 1, 1, GL_RG, GL_FLOAT, points.data());
	//}

	//glCopyImageSubData(m_texture_initial, GL_TEXTURE_2D, 0, 0, 0, 0,
	//	m_texture_jfa_0, GL_TEXTURE_2D, 0, 0, 0, 0,
	//	m_texture_width, m_texture_height, 1);


	//cv::Mat prejfaMat = cv::Mat(m_texture_height, m_texture_width, CV_32SC2);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_texture_jfa_0);
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RG_INTEGER, GL_INT, prejfaMat.data);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTexture(0);

	////cv::imshow("dens1", sxx3);   

	//cv::Mat image1[2];
	//cv::split(prejfaMat, image1);
	//cv::imshow("prejfa0", image1[0] * 255);
	//cv::imshow("prejfa1", image1[1] * 255);

}

void gFlood::setFloodInitialRGBTexture(unsigned char * data, int width, int height, int nrChan)
{
	//GLenum err;
	//err = glGetError();

	//err = glGetError();

	glBindTexture(GL_TEXTURE_2D, m_texture_initial_RGB);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

	////err = glGetError();

	//cv::Mat im0 = cv::Mat(height, width, CV_8UC3);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_texture_initial_RGB);
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, im0.data);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTexture(0);

	////cv::Mat colSplit[2];
	////cv::split(col, colSplit);
	////cv::imshow("col0", colSplit[0] > 0);
	////cv::imshow("col1", colSplit[1] > 0);

	//cv::imshow("mat", im0);
	//cv::waitKey(1);




}



void gFlood::wipeFlood()
{
	std::vector<float> zeroValues(m_texture_width * m_texture_height * 2, 32767);

	glBindTexture(GL_TEXTURE_2D, m_texture_jfa_0);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_width, m_texture_height, GL_RG, GL_FLOAT, zeroValues.data());
	//glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, m_texture_jfa_1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_width, m_texture_height, GL_RG, GL_FLOAT, zeroValues.data());
	//glGenerateMipmap(GL_TEXTURE_2D);


}    

void gFlood::clearPoints()
{
	//glBindBuffer(GL_ARRAY_BUFFER, m_trackedPointsBuffer);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, m_trackedPoints.size() * sizeof(float), m_trackedPoints.data());
}

     
void gFlood::jumpFloodCalc()
{

	int compWidth = divup(m_texture_width, 32);
	int compHeight = divup(m_texture_height, 32);


	edgeDetectProg.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture_initial_RGB);
	glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_applyFilterID);
	glBindImageTexture(0, m_texture_initial, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8);
	glUniform1f(m_edgeThresholdID, m_edgeThreshold);

	glDispatchCompute(compWidth, compWidth, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


	//std::vector<int8_t> edgeData(m_texture_width * m_texture_height, 3);
	//cv::Mat colEdge = cv::Mat(m_texture_height, m_texture_width, CV_8UC1, edgeData.data());


	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_texture_initial);
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_BYTE, colEdge.data);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTexture(0);

	//cv::imshow("edges", colEdge);
	//cv::waitKey(1);


	jumpFloodProg.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture_initial);
	glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_jfaInitID);
	glBindImageTexture(0, m_texture_jfa_0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	glBindImageTexture(1, m_texture_jfa_1, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);

	glDispatchCompute(compWidth, compHeight, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);











	//std::vector<float>::iterator it;

	//it = std::find(vecdata.begin(), vecdata.end(), 0);
	//if (it != vecdata.end())
	//	std::cout << "Element found in myvector: " << *it << '\n';
	//else
	//	std::cout << "Element not found in myvector\n";




	//int compWidth = divup(m_texture_width, 32);
	//int compHeight = divup(m_texture_height, 32);

	int iterCount = 0;
	float maxDim = std::max(m_texture_width, m_texture_height);
	//int log2ceil = std::ceil(std::log(maxDim) / std::log(2));
	//int passes = log2ceil - 1;
	// using 11 levels that will enable an image up to 4096x4096
	for (int level = 0; level < 11; level++)
	{
		int stepWidth = int(std::pow(11.0 - level, 2) + 0.5);

		glUniform1f(m_jumpID, (float)stepWidth);
		glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_jfaUpdateID);

		if (level % 2 == 0)
		{
			glBindImageTexture(0, m_texture_jfa_0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
			glBindImageTexture(1, m_texture_jfa_1, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
		}
		else
		{
			glBindImageTexture(0, m_texture_jfa_1, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
			glBindImageTexture(1, m_texture_jfa_0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
		}



		glDispatchCompute(compWidth, compHeight, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}



	//std::vector<float> vecdata(m_texture_width * m_texture_height * 2);
	//cv::Mat col = cv::Mat(m_texture_height, m_texture_width, CV_32FC2, vecdata.data());


	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_texture_jfa_1);
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_FLOAT, col.data);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTexture(0);

	//cv::Mat colSplit[2];
	//cv::split(col, colSplit);
	//cv::imshow("col0", colSplit[0] * 0.001f);
	//cv::imshow("col1", colSplit[1] * 0.001f);



	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture_initial);
	glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_getColorID);
	glBindImageTexture(1, m_texture_jfa_1, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(2, m_texture_output_RGBA, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glDispatchCompute(compWidth, compHeight, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);






	//std::vector<uint8_t> colMatVon(m_texture_width * m_texture_height * 4, 4);
	//cv::Mat colVon = cv::Mat(m_texture_height, m_texture_width, CV_8UC4, colMatVon.data());


	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_texture_output_RGBA);
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, colVon.data);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTexture(0);

	//cv::imshow("cols", colVon);
	//cv::waitKey(1);

}
