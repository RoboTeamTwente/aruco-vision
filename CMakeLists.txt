cmake_minimum_required(VERSION 3.9)
project(marker_tracking)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 11)
find_package( OpenCV REQUIRED )
find_package( Pylon5 REQUIRED)



include_directories(
        # include
        ${catkin_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIRS}
        ${PYLON5_INCLUDE_DIRS}
)


set(SOURCE_FILES src/main.cpp src/Tracker.cpp src/Tracker.h src/PosRotId.cpp src/PosRotId.h src/capture_basler.h src/capture_basler.cpp src/rawimage.h src/rawimage.cpp src/colors.h)
add_executable(SurfBotTracking ${SOURCE_FILES})
target_link_libraries(SurfBotTracking ${OpenCV_LIBS} ${catkin_LIBRARIES} ${PYLON5_LIBRARIES})


set(CAL_SOURCE_FILES src/calibration.cpp)
add_executable(SurfBotCalibration ${CAL_SOURCE_FILES})
target_link_libraries(SurfBotCalibration ${OpenCV_LIBS})

set(CB_SOURCE_FILES src/createChessBoard.cpp)
add_executable(GenerateChessboard ${CB_SOURCE_FILES})
target_link_libraries(GenerateChessboard ${OpenCV_LIBS})

set(MG_SOURCE_FILES src/createMarkerImage.cpp)
add_executable(CreateMarkerImage ${MG_SOURCE_FILES})
target_link_libraries(CreateMarkerImage ${OpenCV_LIBS})


add_executable(marker_tracking src/main.cpp)