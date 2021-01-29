find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Catch2 REQUIRED)

add_executable(test test_aopacket.cpp test_caseloading.cpp test_apng.cpp test_bass.cpp ../include/aopacket.h ../src/aopacket.cpp)
target_include_directories(test PRIVATE ../include)
target_link_directories(test PRIVATE ../lib)
target_link_libraries(test PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Catch2::Catch2 bass bassopus discord-rpc)
target_compile_definitions(Attorney_Online PRIVATE DISCORD)
