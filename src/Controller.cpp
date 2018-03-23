//
// Created by wouter on 3/19/18.
//

#include "Controller.h"

std::atomic<bool> done;
std::atomic<int> counter;
std::atomic<int> thread_ctr;
std::atomic<int> latest_id;

#define MAX_MULTI_FRAMES 1
#define DEBUG true

struct imgthread_data {
    cv::Mat img;
    unsigned int id;
    pthread_t *pid;
};

struct grabthread_data {
};


std::vector<std::vector<PosRotId>> results;
std::mutex controller_mutex;
std::mutex img_mutex;
unsigned int Controller::id_ctr;
Detector *Controller::detector;
CaptureBasler *Controller::captor;
std::unordered_map<std::thread::id, std::thread &> Controller::threads;
pthread_t Controller::grabber_pid;
pthread_attr_t Controller::attr;
Controller Controller::controller;

Controller::Controller() {
    done = false;
    counter = 0;
    id_ctr = 1;
    latest_id = 0;
    thread_ctr = 0;
    detector = new Detector(88, 84);
    captor = new CaptureBasler();
    grabber_pid = 0;

    // Initialize and set thread joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
}

void Controller::process_image(cv::Mat *img, unsigned int id) {
    if (id > latest_id) {
        std::vector<PosRotId> res = detector->performTrackingOnImage(*img, false);
        controller_mutex.lock();
        if (id > latest_id) {
            latest_id = id;
            counter++;
            results.push_back(res);
        }
        controller_mutex.unlock();
    }
    controller_mutex.lock();
    int ctr = thread_ctr;
    //fprintf(stdout, "Exiting image processor. pid: %lu, thread_ctr: %d\n", std::this_thread::get_id(), ctr);
    thread_ctr--;
    if (!threads.erase(std::this_thread::get_id())) {
        fprintf(stderr, "Could not erase pid from set!\n");
        fflush(stderr);
    }
    controller_mutex.unlock();
}


void *Controller::grabber_thread(void *ptr) {
    captor->startCapture();
    RawImage img;
    while (!done) {
        if (thread_ctr >= MAX_MULTI_FRAMES) {
            usleep(1000);
        }
        img_mutex.lock();
        img.clear();
        img = captor->getFrame();
        cv::Mat cv_img(img.getHeight(), img.getWidth(), CV_8UC3, img.getData());
        cv::cvtColor(cv_img, cv_img, CV_BGR2RGB);
        img_mutex.unlock();
#if(DEBUG)
        cv::imshow("out", cv_img);
        cv::waitKey(10);
#endif

        controller_mutex.lock();
        if (thread_ctr < MAX_MULTI_FRAMES) {

            thread_ctr++;
            //std::cout << "Trying to create thread. thread_ctr: " << thread_ctr << ", threads.size(): " << threads.size() << endl;
            std::thread *trd = (new std::thread(process_image, &cv_img, id_ctr++));
            threads.insert(std::pair<std::thread::id, std::thread &>(trd->get_id(), *trd));
            //printf("created thread: %lu\n", (unsigned long)0);
            //fflush(stdout);
        }

        controller_mutex.unlock();
        usleep(1000);

    }
    pthread_exit(NULL);


}

void Controller::start_run() {
    if (grabber_pid) return;
    grabthread_data data;
    pthread_create(&grabber_pid, NULL, grabber_thread, (void *) &data);
}

void Controller::stop_run() {
    controller_mutex.lock();
    fprintf(stdout, "Trying to stop running threads...\n");
    fflush(stdout);
    void *status;
    done = true;
    controller_mutex.unlock();
    pthread_join(grabber_pid, &status);
    std::cout << "grabber exited with: " << status << std::endl;
    captor->stopCapture();

    controller_mutex.lock();
    std::cout << "There are " << threads.size() << " threads remaining." << std::endl;
    controller_mutex.unlock();
    for (auto &entry : threads) {
        entry.second.join();
    }
    std::unordered_map<int, int> counts;
    for (std::vector<PosRotId> vector1 : results) {
        for (PosRotId posRotId : vector1) {
            counts[posRotId.getID()]++;
        }
    }

    for (pair<int, int> pair : counts) {
        cout << "Saw id " << pair.first << " times: " << pair.second << endl;
    }

    std::cout << "Frames succesfully handled: " << counter << std::endl;

}

Controller Controller::get_controller() {
    return controller;
}
