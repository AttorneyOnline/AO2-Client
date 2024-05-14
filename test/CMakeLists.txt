find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)
find_package(Catch2 REQUIRED)

target_include_directories(test PRIVATE ../src)
target_link_directories(test PRIVATE ../lib)
target_link_libraries(test PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Catch2::Catch2)

add_executable(test
  test_aopacket.cpp
  test_caseloading.cpp
  test_apng.cpp
  ../src/aopacket.h
  ../src/aopacket.cpp
)

if(AO_ENABLE_BASS)
  target_compile_definitions(Attorney_Online PRIVATE AO_ENABLE_BASS)
  target_link_libraries(Attorney_Online PRIVATE bass bassmidi bassopus)
  target_sources(test PRIVATE test_bass.cpp)
endif()
