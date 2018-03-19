/*
 * capture_basler.cpp
 *
 *  Created on: Nov 21, 2016
 *      Author: root
 */

#include "capture_basler.h"

#include <vector>
#include <string>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#define MUTEX_LOCK
#define MUTEX_UNLOCK

int BaslerInitManager::count = 0;

void BaslerInitManager::register_capture() {
	if (count++ == 0) {
		Pylon::PylonInitialize();
	}
}

void BaslerInitManager::unregister_capture() {
	if (--count == 0) {
		Pylon::PylonTerminate();
	}
}


CaptureBasler::CaptureBasler() {
	is_capturing = false;
	camera = NULL;
	ignore_capture_failure = false;
	converter.OutputPixelFormat = Pylon::PixelType_RGB8packed;

	last_buf = NULL;

	current_id = 0;
}

CaptureBasler::~CaptureBasler() {
}

bool CaptureBasler::_buildCamera() {
	BaslerInitManager::register_capture();
	Pylon::DeviceInfoList devices;
	int amt = Pylon::CTlFactory::GetInstance().EnumerateDevices(devices);
    printf("Current camera id: %d\n", current_id);
	if (amt > current_id) {
		Pylon::CDeviceInfo info = devices[current_id];

		camera = new Pylon::CBaslerGigEInstantCamera(
				Pylon::CTlFactory::GetInstance().CreateDevice(info));

        printf("Opening camera %d...\n", current_id);
		camera->Open();
		//camera->PixelFormat.SetValue(Basler_GigECamera::PixelFormat_RGB8Packed);
        printf("Setting interpacket delay...\n");
		camera->GevSCPD.SetValue(600);
        printf("Done!\n");
		is_capturing = true;
		return true;
	}
	return false;
}

bool CaptureBasler::startCapture() {
	MUTEX_LOCK;
	try {
		if (camera == NULL) {
			if (!_buildCamera()) {
                // Did not make a camera!
				MUTEX_UNLOCK
                return false;
            }
		}
		camera->StartGrabbing(Pylon::GrabStrategy_LatestImageOnly);
	} catch (Pylon::GenericException& e) {
        printf("Pylon exception: %s", e.what());
        delete camera;
        camera = NULL;
        current_id = -1;
		MUTEX_UNLOCK;
        return false;
	} catch (...) {
        printf("Uncaught exception at line 132\n");
		MUTEX_UNLOCK;
		throw;
	}
	MUTEX_UNLOCK;
	return true;
}

bool CaptureBasler::_stopCapture() {
	if (is_capturing) {
		camera->StopGrabbing();
		camera->Close();
		is_capturing = false;
		return true;
	}
	return false;
}

bool CaptureBasler::stopCapture() {
	MUTEX_LOCK;
	bool stopped;
	try {
		stopped = _stopCapture();
		if (stopped) {
			delete camera;
			camera = 0;
			BaslerInitManager::unregister_capture();
		}
	} catch (...) {
		MUTEX_UNLOCK;
		throw;
	}
	MUTEX_UNLOCK;
	return stopped;
}

void CaptureBasler::releaseFrame() {
	MUTEX_LOCK;
	try {
		if (last_buf) {
			free(last_buf);
			last_buf = NULL;
		}
	} catch (...) {
		MUTEX_UNLOCK;
		throw;
	}
	MUTEX_UNLOCK;
}

void write_img(const RawImage& img, const std::string& name) {
	std::vector<int> params;
	params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	params.push_back(9);
	cv::Mat cv_img = cv::Mat(img.getHeight(), img.getWidth(), CV_8UC3,
			img.getData());
	cv::imwrite(name + ".png", cv_img, params);
}

RawImage CaptureBasler::getFrame() {
	MUTEX_LOCK;
	RawImage img;
	img.setWidth(0);
	img.setHeight(0);
	img.setColorFormat(COLOR_RGB8);
	try {
		timeval tv;
		gettimeofday(&tv, 0);
		img.setTime((double) tv.tv_sec + (tv.tv_usec / 1000000));

		// Keep grabbing in case of partial grabs
		int fail_count = 0;
		while (fail_count < 10
				&& (!grab_result || !grab_result->GrabSucceeded())) {
			try {
				camera->RetrieveResult(1000, grab_result,
						Pylon::TimeoutHandling_ThrowException);
			} catch (Pylon::TimeoutException& e) {
				fprintf(stderr,
						"Timeout expired in CaptureBasler::getFrame: %s\n",
						e.what());
				MUTEX_UNLOCK;
				return img;
			}
			if (!grab_result) {
				fail_count++;
				continue;
			}
			if (!grab_result->GrabSucceeded()) {
				fail_count++;
				fprintf(stderr,
						"Image grab failed in CaptureBasler::getFrame: %s\n",
						grab_result->GetErrorDescription().c_str());
			}
		}
		if (fail_count == 10) {
			fprintf(stderr,
					"Maximum retry count for image grabbing (%d) exceeded in capture_basler",
					fail_count);
			MUTEX_UNLOCK;
			return img;
		}
		Pylon::CPylonImage capture;

		// Convert to RGB8 format
		converter.Convert(capture, grab_result);

		img.setWidth(capture.GetWidth());
		img.setHeight(capture.GetHeight());
		unsigned char* buf = (unsigned char*) malloc(capture.GetImageSize());
		memcpy(buf, capture.GetBuffer(), capture.GetImageSize());
		img.setData(buf);

#ifdef OPENCV
		// gaussianBlur(img);
        // contrast(img, 1.6);
        // sharpen(img);
#endif

		last_buf = buf;

		// Original buffer is not needed anymore, it has been copied to img
		grab_result.Release();
	} catch (Pylon::GenericException& e) {
		fprintf(stderr, "Exception while grabbing a frame: %s\n", e.what());
		MUTEX_UNLOCK;
		throw;
	} catch (...) {
		// Make sure the mutex is unlocked before propagating
        printf("Uncaught exception!\n");
		MUTEX_UNLOCK;
		throw;
	}
	MUTEX_UNLOCK;
	return img;
}

string CaptureBasler::getCaptureMethodName() const {
	return "Basler";
}

bool CaptureBasler::copyAndConvertFrame(const RawImage & src,
		RawImage & target) {
	MUTEX_LOCK;
	try {
		target.ensure_allocation(COLOR_RGB8, src.getWidth(), src.getHeight());
		target.setTime(src.getTime());
		memcpy(target.getData(), src.getData(), src.getNumBytes());
	} catch (...) {
		MUTEX_UNLOCK;
		throw;
	}
	MUTEX_UNLOCK;
	return true;
}


void CaptureBasler::resetCamera(unsigned int new_id) {
	bool restart = is_capturing;
	if (restart) {
		stopCapture();
	}
	current_id = new_id;
	if (restart) {
		startCapture();
	}
}


#ifdef OPENCV
inline void CaptureBasler::gaussianBlur(RawImage& img) {
	cv::Mat cv_img(img.getHeight(), img.getWidth(), CV_8UC3, img.getData());
	cv::GaussianBlur(cv_img, cv_img, cv::Size(), blur_sigma);
}

void CaptureBasler::contrast(RawImage& img, double factor) {
	cv::Mat cv_img(img.getHeight(), img.getWidth(), CV_8UC3, img.getData());
    for (int y = 0; y < cv_img.rows; ++y) {
        for (int x = 0; x < cv_img.cols; ++x) {
            for (int i = 0; i < 3; ++i) {
                uint8_t channel = cv_img.at<cv::Vec3b>(y, x)[i];
                int newChannel = channel * factor;
                if (newChannel > 255) newChannel = 255;
                cv_img.at<cv::Vec3b>(y, x)[i] = newChannel;
            }
        }
    }
}

void CaptureBasler::sharpen(RawImage& img) {
	cv::Mat cv_img(img.getHeight(), img.getWidth(), CV_8UC3, img.getData());
	cv::Mat cv_img_copy = cv_img.clone();
	cv::GaussianBlur(cv_img_copy, cv_img_copy, cv::Size(), 3);
    cv::addWeighted(cv_img, 2.5, cv_img_copy, -1.5, 0, cv_img);
}
#endif

#ifndef VDATA_NO_QT
void CaptureBasler::mvc_connect(VarList * group) {
	vector<VarType *> v = group->getChildren();
	for (unsigned int i = 0; i < v.size(); i++) {
	connect(v[i],SIGNAL(wasEdited(VarType *)),group,SLOT(mvcEditCompleted()));
}
connect(group,SIGNAL(wasEdited(VarType *)),this,SLOT(changed(VarType *)));
}

void CaptureBasler::changed(VarType * group) {
if (group->getType() == VARTYPE_ID_LIST) {
writeParameterValues(dynamic_cast<VarList*>(group));
}
}
#endif
