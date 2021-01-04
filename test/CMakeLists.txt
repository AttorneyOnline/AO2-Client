find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Catch2 REQUIRED)

add_executable(test test_aopacket.cpp ../include/aopacket.h ../src/aopacket.cpp)
target_include_directories(test PRIVATE ../include)
target_link_libraries(test PRIVATE Qt5::Core Catch2::Catch2)
