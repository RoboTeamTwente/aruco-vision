#include "Tracker.h"
#include "utils/PosRotId.h"
#include "capture/capture_basler.h"
#include "opencv2/opencv.hpp"
#include "mutex"

void printUsage() {
    std::cout << "Usage: SurfBotTracking ip ros_master_uri opencv_camera_id" << std::endl;
}

int main(int argc, char **argv) {

    int nt;

    int ctr = 0;
    Tracker *tracker = new Tracker(17, 15);
    CaptureBasler *grabber = new CaptureBasler();
    grabber->startCapture();
    while (ctr++ < 1000) {
        printf("trying to grab image\n");
        RawImage img = grabber->getFrame();
        cv::Mat cv_img(img.getHeight(), img.getWidth(), CV_8UC3, img.getData());
        cv::cvtColor(cv_img, cv_img, CV_BGR2RGB);
        tracker->performTrackingOnImage(cv_img, true);

    }
    grabber->stopCapture();
}


