#include "Tracker.h"
#include "PosRotId.h"
#include "capture_basler.h"
#include "opencv2/opencv.hpp"
#include "mutex"

void printUsage() {
    std::cout << "Usage: SurfBotTracking ip ros_master_uri opencv_camera_id" << std::endl;
}

int main(int argc, char **argv) {

    int nt;
    if (argc < 0) {
        printUsage();
    } else {
        int ctr = 0;
        //int camId = std::stoi(argv[3]);
        Tracker *tracker = new Tracker(17, 15);
        CaptureBasler *grabber = new CaptureBasler();
        grabber->startCapture();
        while (ctr++ < 1000) {
            printf("trying to grab image\n");
            RawImage img = grabber->getFrame();
            cv::Mat cv_img(img.getHeight(), img.getWidth(), CV_8UC3, img.getData());
            //cv::cvtColor(cv_img, cv_img, CV_BGR2RGB);
            tracker->performTrackingOnImage(cv_img, true);

//            //if (!imageCopy) continue;
//
// //                for (int i = 0; i < markerIds.size(); i++) {
// //                    cv::aruco::drawDetectedMarkers(imageCopy, markerCorners, markerIds);
// //                    cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rotationVectors[i], translationVectors[i],
// //                                        0.1);
// //                }
//
//            cv::imshow("out", cv_img);
//            cv::waitKey(10);



            //std::vector<PosRotId> posrots;
            //posrots = tracker->performTrackingOnImage(grabber->grabImage(), true);
        }
        grabber->stopCapture();
    }
}

