cmake_minimum_required(VERSION 3.13)
project(cmpa_uc3m)

set(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk)
set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fopenmp")
set(ROOT /Users/tiffanys_mac/Documents/Projects)



include_directories(${ROOT}/googletest/googletest/include/)
link_directories(${ROOT}/googletest/ ${ROOT}/googletest/build/lib/)

add_executable(cmpa_uc3m main.cpp asteroid.h)

target_link_libraries(cmpa_uc3m gtest gtest_main)

#find_package(OpenMP REQUIRED) # Find the package
#target_link_libraries(${PROJECT_NAME} ${OpenMP_CXX_LIBRARIES}) # Link against it for