cmake_minimum_required (VERSION 3.5)
project(OgrePoleProject)

# required for Ogre 1.11+
set(CMAKE_CXX_STANDARD 11)

## [discover_ogre]
# specify which version and components you need
find_package(OGRE 1.11 REQUIRED COMPONENTS Bites RTShaderSystem)
find_package(ur_rtde REQUIRED)
#find_package(boost REQUIRED)

# copy resource.cfg next to our binaries where OGRE looks for it
file(COPY ${OGRE_CONFIG_DIR}/resources.cfg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#link the lib boost
#link_directories(${boost_LIBRARIES})  #Not added because specific to a computer
# add the source files as usual
add_executable(OgrePoleProject main.cpp AugmentedWindow.cpp)

# this also sets the includes and pulls third party dependencies
target_link_libraries(OgrePoleProject PRIVATE ${OGRE_LIBRARIES} PRIVATE ur_rtde::rtde)
link_directories(C:/local/boost_1_71_0/lib64-msvc-14.2)
# /libboost_date_time-vc142-mt-gd-x64-1_71.lib)
#target_link_libraries(OgrePoleProject PRIVATE ur_rtde::rtde)