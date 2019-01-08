#ifndef FRAME_GRABBER_H
#define FRAME_GRABBER_H

#include "opencv2/core/utility.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>

class FrameGrabber
{
	enum Status
	{
		CAPTURING,
		STOPPED
	};

public:
	FrameGrabber();
	~FrameGrabber()
	{
		if (m_status == CAPTURING)
		{
			stop();
		}
	}

	void start();
	void startVideo();

	void stop();

	void setVideoCaptureTarget(cv::VideoCapture cap)
	{
		/*if (!m_cap.isOpened())
		{
			m_cap.open(filename);
		}*/
		m_cap = cap;
	}

	void setImageDimensions(int width, int height)
	{
		m_colorWidth = width;
		m_colorHeight = height;

		mBuffer = cv::Mat(m_colorHeight, m_colorWidth, CV_8UC3);
		outbuf = cv::Mat(m_colorHeight, m_colorWidth, CV_8UC3);
	}

	void resetImageDimensions()
	{
		stop();

		m_cap.release();

		mBuffer = cv::Mat(m_colorHeight, m_colorWidth, CV_8UC3);
		outbuf = cv::Mat(m_colorHeight, m_colorWidth, CV_8UC3);

		start();
	}

	bool ready()
	{
		return m_frames_ready;
	}

	cv::Mat frames(bool &newFrame);
	cv::Mat framesVideo(bool &newFrame);

	/*std::string getFrameName();

	double get(const int capProperty);

	void set(const int capProperty, const double value);*/

private:
	//double mFps;
	//long long mFrameNameCounter;
	//bool mThreadOpened;
	cv::VideoCapture m_cap;
	cv::Mat mBuffer = cv::Mat(960, 540, CV_8UC3);
	cv::Mat outbuf = cv::Mat(960, 540, CV_8UC3);;
	//std::mutex mBufferMutex;
	//std::atomic<bool> mCloseThread;
	//std::thread mThread;
	int m_colorWidth = 960;
	int m_colorHeight = 540;
	void captureLoop();
	void captureVideoFrameLoop();
	Status m_status = STOPPED;
	bool m_frames_ready;
	std::thread *m_thread;
	std::mutex m_mtx;
	std::condition_variable m_convar;
	std::mutex m_mtx_cv;
	int m_videoFrameNumber = 0;
};


#endif // OPENPOSE_PRODUCER_WEBCAM_READER_HPP




