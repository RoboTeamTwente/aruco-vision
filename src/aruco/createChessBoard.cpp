#include <opencv2/aruco/charuco.hpp>
#include <opencv2/opencv.hpp>


int main(int argc, char **argv) {
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

    cv::Ptr<cv::aruco::CharucoBoard> board = cv::aruco::CharucoBoard::create(4, 5, 0.05, 0.035, dictionary);
    cv::Mat boardImage;
    board -> draw( cv::Size(1200, 1000), boardImage, 10, 1 );

    cv::imwrite("charucoboard.png", boardImage);
}