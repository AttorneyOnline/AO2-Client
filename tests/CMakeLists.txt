project(tests LANGUAGES CXX C)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

enable_testing(true)

set(SKIP_AUTOMOC ON)

function(declare_client_test partial_test_id)
  message(STATUS "Declaring client test ${partial_test_id}")
  set(test_id "test_client_${partial_test_id}")
  add_executable(${test_id} ${ARGN})
  target_include_directories(${test_id} PRIVATE ../src)
  target_link_directories(${test_id} PRIVATE ../lib)
  target_link_libraries(${test_id} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Test)
  target_compile_definitions(${test_id} PRIVATE KAL_CLIENT_TEST_DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/data")
  add_test(NAME ${test_id} COMMAND ${test_id})
endfunction()

declare_client_test(aopacket test_aopacket.cpp ../src/aopacket.cpp)
