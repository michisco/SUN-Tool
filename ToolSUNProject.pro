QT += core gui
QT += network
QT += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        AcquisitionSetup.cpp \
        CalibrationCamera.cpp \
        DevicesPose.cpp \
        FileUtilities.cpp \
        arduinodevice.cpp \
        handDevice.cpp \
        ObjectsPose.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    AcquisitionSetup.h \
    DeviceMarkers.h \
    ObjectMarkers.h \
    arduinodevice.h \
    handDevice.h

ROOT_DIR = $${PWD}\..\..\..\..\..\..
OPENCV_DIR = $${ROOT_DIR}\OpenCV

INCLUDEPATH += $${OPENCV_DIR}\Builds\CMake\install\include

Release: {
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_core480.lib
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_aruco480.lib
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_highgui480.lib
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_objdetect480.lib
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_imgcodecs480.lib
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_imgproc480.lib
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_features2d480.lib
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_calib3d480.lib
}

Debug: {
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_core480d.lib
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_aruco480d.lib
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_highgui480d.lib
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_objdetect480d.lib
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_imgcodecs480d.lib
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_imgproc480d.lib
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_features2d480d.lib
LIBS +=  -L$${OPENCV_DIR}\Builds\CMake\install\x64\vc17\lib\opencv_calib3d480d.lib
}
