cmake_minimum_required(VERSION 3.0.0)
project(elib_static VERSION 0.1.0)


include(cmake/install_overrides.cmake)
include(cmake/find_third_party.cmake)


file(GLOB ebase_src "src/ebase/*.cpp" "src/ebase/c/*.c")
file(GLOB eio_src "src/eio/*.cpp" "src/eio/base/*.cpp" "src/eio/posix/*.cpp")
file(GLOB ehttp_src "src/ehttp/*.cpp" "src/ehttp/c/*.c")

file(GLOB ebase_header "src/ebase/*.hpp")
file(GLOB eio_header "src/eio/*.hpp")

#https://blog.csdn.net/amuseme_lu/article/details/5915859
include (${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)


IF (CMAKE_SYSTEM_NAME MATCHES "Windows")

	file(GLOB ebase_src ${ebase_src} "src/ebase/platform/*_win32.cpp")
	file(GLOB eio_src ${eio_src} "src/eio/iocp/*.cpp" "src/eio/platform/*_win32.cpp" )
    set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} ws2_32.lib Winmm.lib")
    add_compile_options(/D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
    add_compile_options(/EHsc)

ELSE ()
    check_function_exists (setcontext HAVE_SETCONTEXT)
    check_function_exists (setjmp HAVE_SETJMP)
	file(GLOB ebase_src ${ebase_src} "src/ebase/platform/*_linux.cpp")
	file(GLOB eio_src ${eio_src} "src/eio/epoll/*.cpp" "src/eio/platform/*_linux.cpp" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
 
CONFIGURE_FILE("src/config.h.in" "${PROJECT_SOURCE_DIR}/src/config.h")

add_library(${PROJECT_NAME} STATIC ${ebase_src} ${eio_src} ${ehttp_src})

add_library("elib" SHARED ${ebase_src} ${eio_src} ${ehttp_src})

add_executable(echo_server "src/examples/echo_server_main.cpp" "src/examples/echo_server.cpp")
target_link_libraries(echo_server ${PROJECT_NAME})

file(GLOB unittest_src "src/ebase/unittest/*.cpp")
file(GLOB unittest_src ${unittest_src} "src/eio/unittest/*.cpp")

add_executable(elib_unittest ${unittest_src} )
add_dependencies(elib_unittest elib_static)
target_link_libraries(elib_unittest elib_static gtest gtest_main )

target_include_directories(elib_unittest PRIVATE "src")


install(TARGETS elib LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS ${PROJECT_NAME}  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES ${ebase_header} DESTINATION ${CMAKE_INSTALL_PREFIX}/inlude/ebase)
install(FILES ${eio_header} DESTINATION ${CMAKE_INSTALL_PREFIX}/inlude/eio)

enable_testing()
add_test(elib_test elib_unittest)

