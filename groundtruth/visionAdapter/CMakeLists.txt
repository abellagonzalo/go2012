##------------------------------------------------------------
## Author:		Gonzalo Abella (abellagonzalo@gmail.com)
## Update:		25/01/2012 by Gonzalo Abella
##------------------------------------------------------------

cmake_minimum_required (VERSION 2.6)
project(SSLVISIONADAPTER)

## Paths
set (SSLVISION_PATH ${PROJECT_SOURCE_DIR}/../ssl-vision)
set (PLAYER_PATH ${PROJECT_SOURCE_DIR}/../../player)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


## QT Libraries
set (QT_USE_QTNETWORK true)
set (QT_USE_QTOPENGL true)
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )


## Include directories
include_directories(${PROJECT_BINARY_DIR})
include_directories(${SSLVISION_PATH}/src/shared/net)
include_directories(${SSLVISION_PATH}/src/shared/util)
include_directories(../../player/src/interfaces/cpp/bica)


## Shared sources
set (SRCS
	${SSLVISION_PATH}/src/shared/net/netraw.cpp
	${SSLVISION_PATH}/src/shared/net/robocup_ssl_client.cpp
	src/Adapter.cpp
	src/UpdateLocalizationTimerTask.cpp
)

## PROTOBUF generation code
include("GenerateProtobuf.cmake")

## BICA generation code 
include("GenerateBica.cmake")

## ICE generation code
include("GenerateIce.cmake")

## Compile protobuf, ice sources & shared sources
add_library( common ${CC_PROTO} ${SRCS} ${CPP_BICA} ${CPP_ICE} )

## Libs
set(libs ${QT_LIBRARIES} Ice IceUtil protobuf common)

## Compile visionAdapter
set(visionAdapter visionAdapter)
add_executable(${visionAdapter} src/VisionAdapterI.cpp)
target_link_libraries(${visionAdapter} ${libs})



