#include <opencv2/aruco/charuco.hpp>
#include <opencv2/opencv.hpp>

void printUsage() {
    std::cout << "Usage: CreateMarkerImage AMOUNT BITS" << std::endl;
}


int main(int argc, char **argv) {
    int amount, bits;
    if (argc != 3) {
        printUsage();
        exit(1);
    }
    try {
        amount = std::stoi(argv[1]);
        bits = std::stoi(argv[2]);
    } catch (std::invalid_argument& e) {
        fprintf(stderr, e.what());
        printUsage();
        exit(1);
    }
    printf("Creating %d markers with %d bits...\n", amount, bits);
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::Dictionary::create(amount, bits);
    cv::Mat markerImage;
    for (int i = 0; i < amount; i++) {
        cv::aruco::drawMarker(dictionary, i, 1000, markerImage, 1);

        std::string fileName(std::to_string(i));
        fileName = "marker" + fileName + ".png";

        cv::imwrite(fileName, markerImage);
    }
}
