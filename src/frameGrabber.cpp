#include "frameGrabber.h"

FrameGrabber::FrameGrabber()
{
}



void FrameGrabber::start()
{
	if (m_status == STOPPED)
	{
		m_status = CAPTURING; 
		m_thread = new std::thread(&FrameGrabber::captureLoop, this);
	} 
}

void FrameGrabber::startVideo()
{
	if (m_status == STOPPED)
	{
		m_status = CAPTURING;
		m_thread = new std::thread(&FrameGrabber::captureVideoFrameLoop, this);
	}
}

void FrameGrabber::stop()
{
	if (m_status == CAPTURING)
	{
		m_status = STOPPED;

		if (m_thread->joinable())
		{
			m_thread->join();
		}

		m_thread = nullptr;
	}
}

cv::Mat FrameGrabber::frames(bool &newFrame)
{
	//outbuf = mBuffer.clone();
	if (m_frames_ready)
	{
		memcpy_s(outbuf.data, m_colorWidth * m_colorHeight * 3, mBuffer.data, m_colorWidth * m_colorHeight * 3);
		newFrame = m_frames_ready;
		m_frames_ready = false;
	}
	else
	{
		newFrame = m_frames_ready;
	}

	return outbuf;
}

cv::Mat FrameGrabber::framesVideo(bool &newFrame)
{
	//outbuf = mBuffer.clone();
	if (m_frames_ready)
	{
		std::unique_lock<std::mutex> lck(m_mtx_cv);
		memcpy_s(outbuf.data, m_colorWidth * m_colorHeight * 3, mBuffer.data, m_colorWidth * m_colorHeight * 3);
		newFrame = m_frames_ready;
		m_frames_ready = false;
		m_convar.notify_one();
		std::cout << " fame ready" << std::endl;
	}
	else
	{
		newFrame = m_frames_ready;
		std::cout << "frame not ready" << std::endl;
	}

	return outbuf;
}

void FrameGrabber::captureVideoFrameLoop()
{
	cv::Mat cvMat;

	m_cap.set(CV_CAP_PROP_POS_FRAMES, m_videoFrameNumber);
	m_videoFrameNumber++;
	//std::memcpy()

	if (m_videoFrameNumber > m_cap.get(CV_CAP_PROP_FRAME_COUNT) - 50)
	{
		m_videoFrameNumber = 0;
	}

	m_cap.read(cvMat);
	m_frames_ready = true;

	std::unique_lock<std::mutex> lck(m_mtx_cv);
	m_convar.wait(lck);
}


// black frames appear due to the same frame being present in the original source video - probably due to poor resampling/saving from raw or post processing/conversion
void FrameGrabber::captureLoop()
{

	if (!m_cap.isOpened())
	{
		m_cap.open(2);
	}
	m_cap.set(CV_CAP_PROP_FRAME_WIDTH, m_colorWidth);
	m_cap.set(CV_CAP_PROP_FRAME_HEIGHT, m_colorHeight);

	cv::Mat cvMat;


	while (m_status == CAPTURING)
	{
		m_cap.read(cvMat);
		//cv::imshow("cols", cvMat);
		//cv::waitKey(1);
		// Move to buffer
		if (!cvMat.empty() && cvMat.cols == m_colorWidth && cvMat.rows == m_colorHeight)
		{
			m_mtx.lock();
			memcpy_s(mBuffer.data, m_colorWidth * m_colorHeight * 3, cvMat.data, m_colorWidth * m_colorHeight * 3);
			m_frames_ready = true;
			m_mtx.unlock();
		}
	}
}