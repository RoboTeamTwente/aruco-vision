#include "Controller.h"

void printUsage() {
    std::cout << "Usage: SurfBotTracking" << std::endl;
}

int main(int argc, char **argv) {
    unsigned int seconds;
    std::cin >> seconds;
    std::cout <<"running for " << seconds << " seconds" << std::endl;
    Controller cont = Controller::get_controller();
    cont.start_run();
    std::cout << "run started!" << std::endl;
    sleep(seconds);
    void* status;
    cont.stop_run();
}


