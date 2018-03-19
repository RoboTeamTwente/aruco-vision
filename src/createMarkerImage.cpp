#include <opencv2/aruco/charuco.hpp>
#include <opencv2/opencv.hpp>
#include <string>

void printUsage() {
    std::cout << "Usage: CreateMarkerImage marker_id" << std::endl;
}


int main(int argc, char **argv) {
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::Dictionary::create(20,3);
    cv::Mat markerImage;

    cv::aruco::drawMarker(dictionary, std::stoi(argv[1]), 1000, markerImage, 1);

    std::string fileName(argv[1]);
    fileName = "marker" + fileName + ".png";

    cv::imwrite(fileName, markerImage);
}
