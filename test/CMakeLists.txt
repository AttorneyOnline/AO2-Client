project(tests LANGUAGES CXX C)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

enable_testing(true)

set(SKIP_AUTOMOC ON)

function(ao_declare_test test_id)
  message(STATUS "FOOBAR Adding test: ${test_id}")
  message(STATUS "FOOBAR  Source files: ${ARGN}")
  add_executable(${test_id} ${ARGN})
  target_include_directories(${test_id} PRIVATE ../src src)
  target_link_directories(${test_id} PRIVATE ../lib)
  target_link_libraries(${test_id} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Test)
  add_test(NAME ${test_id} COMMAND ${test_id})
endfunction()

ao_declare_test(test_aopacket test_aopacket.cpp ../src/aopacket.cpp)
