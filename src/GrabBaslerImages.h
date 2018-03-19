//
// Created by wouter on 3/7/18.
//

#ifndef MARKER_TRACKING_GRABBASLERIMAGES_H
#define MARKER_TRACKING_GRABBASLERIMAGES_H

#include <pylon/PylonIncludes.h>
#include <pylon/gige/BaslerGigEInstantCamera.h>
#include <opencv2/opencv.hpp>

// Namespace for using pylon objects.
using namespace Pylon;

class GrabBaslerImages {
public:
    GrabBaslerImages();
    ~GrabBaslerImages();
    cv::Mat grabImage();
    cv::Mat* grabLatest();

protected:
    Pylon::CInstantCamera* camera;
    CImageFormatConverter formatConverter;

    void startCapture();

    void stopCapture();


};

#endif //MARKER_TRACKING_GRABBASLERIMAGES_H
