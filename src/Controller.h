//
// Created by wouter on 3/19/18.
//

#ifndef ARUCO_VISION_CONTROLLER_H
#define ARUCO_VISION_CONTROLLER_H

#include <unordered_set>
#include <atomic>
#include <thread>
#include "mutex"
#include "opencv2/opencv.hpp"
#include "pthread.h"
#include "Detector.h"
#include "capture/capture_basler.h"
#include "unistd.h"


class Controller {
public:
    static Controller get_controller();

    void stop_run();

    void start_run();

private:

    static void process_image(cv::Mat *img, unsigned int id);

    static void *grabber_thread(void *ptr);

    static Controller controller;

    Controller();

    static Detector *detector;

    static CaptureBasler *captor;

    static std::unordered_map<std::thread::id, std::thread &> threads;

    //static std::mutex mutex;

    static unsigned int id_ctr;

    static pthread_t grabber_pid;

    static pthread_attr_t attr;
};

#endif //ARUCO_VISION_CONTROLLER_H
