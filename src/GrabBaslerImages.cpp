//
// Created by wouter on 3/7/18.
//

#include "GrabBaslerImages.h"
#include <thread>
#include <chrono>
// Grab.cpp
/*
    Note: Before getting started, Basler recommends reading the Programmer's Guide topic
    in the pylon C++ API documentation that gets installed with pylon.
    If you are upgrading to a higher major version of pylon, Basler also
    strongly recommends reading the Migration topic in the pylon C++ API documentation.
    This sample illustrates how to grab and process images using the CInstantCamera class.
    The images are grabbed and processed asynchronously, i.e.,
    while the application is processing a buffer, the acquisition of the next buffer is done
    in parallel.
    The CInstantCamera class uses a pool of buffers to retrieve image data
    from the camera device. Once a buffer is filled and ready,
    the buffer can be retrieved from the camera object for processing. The buffer
    and additional image data are collected in a grab result. The grab result is
    held by a smart pointer after retrieval. The buffer is automatically reused
    when explicitly released or when the smart pointer object is destroyed.
*/
// Include files to use the PYLON API.



// Namespace for using cout.
using namespace std;


GrabBaslerImages::GrabBaslerImages() {
    PylonInitialize();
    try {

        camera = new CInstantCamera(CTlFactory::GetInstance().CreateFirstDevice());
        camera->Open();

        cout << "Using device " << camera->GetDeviceInfo().GetModelName() << endl;


    } catch (const GenericException &e)
    {
        // Error handling.
        cerr << "An exception occurred." << endl
             << e.GetDescription() << endl;
    }
    formatConverter.OutputPixelFormat = PixelType_BGR8packed;

}

GrabBaslerImages::~GrabBaslerImages() {
    if (camera->IsGrabbing()) {
        camera->StopGrabbing();
    }
    if (camera->IsOpen()) {
        camera->Close();
    }
    PylonTerminate();
}

void GrabBaslerImages::startCapture() {
    camera->StartGrabbing(GrabStrategy_LatestImageOnly);
}

void GrabBaslerImages::stopCapture() {
    if (camera->IsGrabbing()) {
        camera->StopGrabbing();
    }
}


cv::Mat* GrabBaslerImages::grabLatest() {
    CGrabResultPtr grabResultPtr;
    CPylonImage pylonImage;
    try {
        camera->RetrieveResult(5000, grabResultPtr,
                               Pylon::TimeoutHandling_ThrowException);
    }catch (Pylon::TimeoutException& e) {
        fprintf(stderr,
                "Timeout expired in CaptureBasler::getFrame: %s\n",
                e.what());
    }

    if (grabResultPtr && grabResultPtr->GrabSucceeded()) {
        formatConverter.Convert(pylonImage, grabResultPtr);
        cv::Mat result = cv::Mat(grabResultPtr->GetHeight(), grabResultPtr->GetWidth(), CV_8UC3, (uint8_t *) grabResultPtr->GetBuffer());
        return &result;
    } else {
        fprintf(stderr,"Grab unsuccessful\n");
    }
    return NULL;
}

cv::Mat GrabBaslerImages::grabImage() {
    CGrabResultPtr grabResultPtr;
    CPylonImage pylonImage;

    camera->GrabOne(5000, grabResultPtr);
    while (camera->IsGrabbing()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    formatConverter.Convert(pylonImage, grabResultPtr);
    cv::Mat result = cv::Mat(grabResultPtr->GetHeight(), grabResultPtr->GetWidth(), CV_8UC3, (uint8_t *) grabResultPtr->GetBuffer());
    return result;
};
