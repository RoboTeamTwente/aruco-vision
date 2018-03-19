cmake_minimum_required(VERSION 3.9)
project(aruco-vision)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 11)
find_package( OpenCV REQUIRED )
find_package( Pylon5 REQUIRED)



include_directories(
        # include
        ${OpenCV_INCLUDE_DIRS}
        ${PYLON5_INCLUDE_DIRS}
)


set(SOURCE_FILES src/main.cpp src/Tracker.cpp src/Tracker.h src/utils/PosRotId.cpp src/utils/PosRotId.h src/capture/capture_basler.h src/capture/capture_basler.cpp src/utils/rawimage.h src/utils/rawimage.cpp src/utils/colors.h)
add_executable(SurfBotTracking ${SOURCE_FILES})
target_link_libraries(SurfBotTracking ${OpenCV_LIBS} ${PYLON5_LIBRARIES})

set(MG_SOURCE_FILES src/aruco/createMarkerImage.cpp)
add_executable(CreateMarkerImage ${MG_SOURCE_FILES})
target_link_libraries(CreateMarkerImage ${OpenCV_LIBS})