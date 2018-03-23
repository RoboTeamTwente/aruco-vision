cmake_minimum_required(VERSION 3.9)
project(aruco-vision)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 11)
find_package( OpenCV 3 REQUIRED )
find_package( Pylon5 REQUIRED)
find_package (Threads REQUIRED)
add_definitions(-D OPENCV)

include_directories(
        # include
        ${OpenCV_INCLUDE_DIRS}
        ${PYLON5_INCLUDE_DIRS}
        ${CMAKE_THREAD_LIBS_INIT}
)


set(MAIN_SOURCE_FILES
        src/main.cpp
        src/Detector.cpp
        src/Detector.h
        src/utils/PosRotId.cpp
        src/utils/PosRotId.h
        src/capture/capture_basler.h
        src/capture/capture_basler.cpp
        src/utils/rawimage.h
        src/utils/rawimage.cpp
        src/utils/colors.h 
        src/Controller.cpp 
        src/Controller.h
        )

add_executable(SurfBotTracking ${MAIN_SOURCE_FILES})
target_link_libraries(SurfBotTracking ${OpenCV_LIBS} ${PYLON5_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})


set(MARKER_SOURCE_FILES src/aruco/createMarkerImage.cpp)
add_executable(CreateMarkerImage ${MARKER_SOURCE_FILES})
target_link_libraries(CreateMarkerImage ${OpenCV_LIBS})