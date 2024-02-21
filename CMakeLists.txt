cmake_minimum_required(VERSION 3.16)

# Set the AWS service components used by this project.
set(SERVICE_COMPONENTS s3 transfer)

project("s3_ckusm_test")

set(CMAKE_CXX_STANDARD 20)

set(AWS_SDK_TOP "/opt/aws-sdk-cpp/")

set(AWSSDK_DIR "/opt/aws-sdk-cpp/lib64/cmake/AWSSDK/")
set(AWSSDK_DIR "/opt/aws-sdk-cpp/lib64/cmake/AWSSDK/")
set(aws-cpp-sdk-core_DIR "/opt/aws-sdk-cpp/lib64/cmake/aws-cpp-sdk-core/")
set(aws-crt-cpp_DIR "/opt/aws-sdk-cpp/lib64/aws-crt-cpp/cmake/")
set(aws-c-http_DIR "/opt/aws-sdk-cpp/lib64/aws-c-http/cmake/")
set(aws-c-io_DIR "/opt/aws-sdk-cpp/lib64/aws-c-io/cmake/")
set(s2n_DIR "/opt/aws-sdk-cpp/lib64/s2n/cmake/")
set(aws-c-common_DIR "/opt/aws-sdk-cpp/lib64/aws-c-common/cmake/")
set(aws-c-cal_DIR "/opt/aws-sdk-cpp/lib64/aws-c-cal/cmake/")
set(aws-c-compression_DIR
"/opt/aws-sdk-cpp/lib64/aws-c-compression/cmake/")
set(aws-c-mqtt_DIR "/opt/aws-sdk-cpp/lib64/aws-c-mqtt/cmake/")
set(aws-c-auth_DIR "/opt/aws-sdk-cpp/lib64/aws-c-auth/cmake/")
set(aws-c-sdkutils_DIR "/opt/aws-sdk-cpp/lib64/aws-c-sdkutils/cmake/")
set(aws-c-event-stream_DIR "/opt/aws-sdk-cpp/lib64/aws-c-event-stream/cmake/")
set(aws-checksums_DIR "/opt/aws-sdk-cpp/lib64/aws-checksums/cmake/")
set(aws-c-s3_DIR "/opt/aws-sdk-cpp/lib64/aws-c-s3/cmake/")
set(aws-cpp-sdk-s3_DIR "/opt/aws-sdk-cpp/lib64/cmake/aws-cpp-sdk-s3/")
set(aws-cpp-sdk-transfer_DIR "/opt/aws-sdk-cpp/lib64/cmake/aws-cpp-sdk-transfer/")

find_package(AWSSDK REQUIRED COMPONENTS ${SERVICE_COMPONENTS})

add_library(s3_common STATIC
	s3_common.cpp)

target_include_directories(s3_common
	PUBLIC ${AWSSDK_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}
        s3_cksum.cpp)

target_link_libraries(${PROJECT_NAME}
	s3_common
        ${AWSSDK_LINK_LIBRARIES})

add_executable(s3_keyreserved
        s3_keyreserved.cpp)

target_include_directories(s3_keyreserved
	PUBLIC ${AWSSDK_INCLUDE_DIRS})

target_link_libraries(s3_keyreserved
	s3_common
        ${AWSSDK_LINK_LIBRARIES})
