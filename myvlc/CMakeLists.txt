project(MyVLC)

set(CMAKE_CXX_FLAGS "-std=c++11 -g")
add_executable(myvlc_test main.cpp)

add_executable(video_encode video_encode.cpp)
add_executable(video_decode video_decode.cpp)
