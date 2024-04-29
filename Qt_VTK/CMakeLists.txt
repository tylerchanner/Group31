cmake_minimum_required(VERSION 3.5)

project(Qt_VTK VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

#********************************************************************************************
################################### This needs adding #######################################
#********************************************************************************************
find_package( VTK REQUIRED )
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        ModelPart.cpp
        ModelPart.h
        ModelPartList.cpp
        ModelPartList.h
        mainwindow.ui
        icons.qrc
        optiondialog.h
	optiondialog.ui
        optiondialog.cpp
	newgroupdialog.h
	newgroupdialog.cpp
	newgroupdialog.ui


)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Qt_VTK
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Qt_VTK APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Qt_VTK SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Qt_VTK
            ${PROJECT_SOURCES}
        )
    endif()
endif()

#********************************************************************************************
################################# This needs modifying ######################################
#********************************************************************************************
target_link_libraries(Qt_VTK PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${VTK_LIBRARIES} )
#------------------------------------------------------------------------^^^^^^^^^^^^^^^^----

set_target_properties(Qt_VTK PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS Qt_VTK
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Qt_VTK)
endif()


#********************************************************************************************
################################### This needs adding #######################################
#********************************************************************************************
# Copy across Open VR bindings that map controllers
# The program will expect to find these in the build dir when it runs
add_custom_target(	VRBindings )
add_custom_command(	TARGET VRBindings PRE_BUILD
COMMAND ${CMAKE_COMMAND} -E
              copy_directory ${CMAKE_SOURCE_DIR}/vrbindings ${CMAKE_BINARY_DIR}/ )
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
