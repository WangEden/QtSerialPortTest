cmake_minimum_required(VERSION 3.14)

project(QtSerialPortTest VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick LinguistTools)
find_package(Qt5 COMPONENTS Core SerialPort REQUIRED)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(TS_FILES Font_zh_CN.ts)

# Set OpenCV dirctory
execute_process(COMMAND hostname OUTPUT_VARIABLE HOSTNAME OUTPUT_STRIP_TRAILING_WHITESPACE)
if(${HOSTNAME} STREQUAL "eden-y9000p")
    set(OpenCV_DIR /home/eden/Package/opencv-4.6.0/build)
else()
    set(OpenCV_DIR /home/user/Package/opencv-4.6.0/build)
endif()

include_directories(.)
include_directories(include/)

set(PROJECT_SOURCES
        src/main.cpp
        include/d435i.h src/Device/d435i.cpp
        include/UR5.h src/Device/UR5.cpp
        include/RS485.h src/Device/RS485.cpp

        include/camerathread.h src/Thread/camerathread.cpp
        include/serialthread.h src/Thread/serialthread.cpp

        include/imagetransmission.h src/Components/imagetransmission.cpp
        include/serialadapter.h src/Components/serialadapter.cpp
        include/QWidgetRenderer.h src/Components/QWidgetRenderer.cpp
        qml.qrc
        ${TS_FILES}
)

find_package(OpenCV 4.6.0 REQUIRED)
find_package(realsense2 REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost 1.65 REQUIRED COMPONENTS system)

include_directories(${OpenCV_DIR}/include)
link_directories(${OpenCV_DIR}/lib)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET DustbinWithQT5 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
          ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    ${OpenCV_LIBS} 
    Qt5::SerialPort
    ${Boost_LIBRARIES} 
    Threads::Threads 
    ${OpenCV_LIBS} 
    realsense2 
    -lstdc++fs
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(${PROJECT_NAME})
    qt_finalize_executable(${PROJECT_NAME})
endif()

