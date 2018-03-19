/*
 * capture_basler.h
 *
 *  Created on: Nov 21, 2016
 *      Author: root
 */

#ifndef CAPTURE_BASLER_H_
#define CAPTURE_BASLER_H_

#ifndef VDATA_NO_QT
#define VDATA_NO_QT
#endif

#include "../utils/rawimage.h"
#include <string>
#include <pylon/PylonIncludes.h>
#include <pylon/PylonBase.h>
#include <pylon/PylonImage.h>
#include <pylon/gige/BaslerGigEInstantCamera.h>
#include <sys/time.h>
#include <opencv2/opencv.hpp>

#ifndef VDATA_NO_QT
#include <QMutex>
#endif

class BaslerInitManager {
public:
	static void register_capture();
	static void unregister_capture();
//private:
	static int count;
};

class CaptureBasler {


public:
	CaptureBasler();

	~CaptureBasler();

	bool startCapture();

	bool stopCapture();

	bool isCapturing() { return is_capturing; };

	RawImage getFrame();

	void releaseFrame();

	std::string getCaptureMethodName() const;

	bool copyAndConvertFrame(const RawImage & src, RawImage & target);

	void readAllParameterValues();


private:
	bool is_capturing;
	bool ignore_capture_failure;
	Pylon::CBaslerGigEInstantCamera* camera;
	Pylon::CGrabResultPtr grab_result;
	Pylon::CImageFormatConverter converter;
	int current_id;
  	unsigned char* last_buf;

  	void resetCamera(unsigned int new_id);
  	bool _stopCapture();
  	bool _buildCamera();

// A slight blur helps to reduce noise and improve color recognition.
#ifdef OPENCV
  	static const double blur_sigma;
  	void gaussianBlur(RawImage& img);
    void contrast(RawImage& img, double factor);
    void sharpen(RawImage& img);
#endif
};

#endif
