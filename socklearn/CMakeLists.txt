# Add the path to the vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "C:/Users/blaik/OneDrive/Desktop/cpp/network/socklearn/vcpkg/scripts/buildsystems/vcpkg.cmake")


cmake_minimum_required(VERSION 3.10)
project(cpp)
 
set(CMAKE_CXX_STANDARD 14)


# Find the cpp_redis and tacopie packages
find_package(hiredis CONFIG REQUIRED)

add_executable(uredis uredis.cpp)

# # Link the cpp_redis and tacopie libraries
target_link_libraries(uredis 
                    PRIVATE 
                    hiredis::hiredis)  