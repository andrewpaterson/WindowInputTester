# Linux build of the window input tester (BaseLib + StandardLib + SupportLib + RasterLib + RenderLib + RasterRenderLib + VulkanLib + SceneViewLib + InputLib + WindowLib + X11Lib + the
# tester's sources).
# VulkanLib needs Vulkan's headers to build (the SDK, or libvulkan-dev); it loads libvulkan.so.1 when a scene is shown.
#
# 64-bit only: there is no 32-bit libX11 installed.  Needs libx11-dev and libxext-dev.
#
# From the folder the WindowInputTester repository and CommonLibraries are in:
#
#   cmake -S WindowInputTester -B build/WindowInputTester -G Ninja -DCMAKE_BUILD_TYPE=Debug
#   cmake --build build/WindowInputTester
#   cd WindowInputTester/WindowInputTester && ../../build/WindowInputTester/WindowInputTester
#
# Add -DBASELIB_ASAN=ON for AddressSanitizer and LeakSanitizer.
#
# Source lists are read from the .vcxproj files so Visual Studio stays the source of truth.  Of the program's two
# mains, WindowInputTester_Windows.cpp (WinMain) compiles to nothing here and WindowInputTester_Linux.cpp is the main.

cmake_minimum_required(VERSION 3.19)
project(WindowInputTester CXX)


# This repository is beside CommonLibraries.
get_filename_component(CODAPHELA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
set(COMMON_DIR "${CODAPHELA_ROOT}/CommonLibraries/Libraries")
set(BASELIB_DIR "${COMMON_DIR}/BaseLib")
set(STANDARDLIB_DIR "${COMMON_DIR}/StandardLib")
set(SUPPORTLIB_DIR "${COMMON_DIR}/SupportLib")
set(RASTERLIB_DIR "${COMMON_DIR}/RasterLib")
set(RENDERLIB_DIR "${COMMON_DIR}/RenderLib")
set(RASTERRENDERLIB_DIR "${COMMON_DIR}/RasterRenderLib")
set(VULKANLIB_DIR "${COMMON_DIR}/VulkanLib")
set(INPUTLIB_DIR "${COMMON_DIR}/InputLib")
set(WINDOWLIB_DIR "${COMMON_DIR}/WindowLib")
set(X11LIB_DIR "${COMMON_DIR}/X11Lib")
set(SCENEVIEWLIB_DIR "${COMMON_DIR}/SceneViewLib")
set(TESTER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/WindowInputTester")

include("${COMMON_DIR}/../CMake/Codaphela.cmake")


vcxproj_sources(BASELIB_SOURCES "${BASELIB_DIR}/BaseLib.vcxproj")
vcxproj_sources(STANDARDLIB_SOURCES "${STANDARDLIB_DIR}/StandardLib.vcxproj")
vcxproj_sources(SUPPORTLIB_SOURCES "${SUPPORTLIB_DIR}/SupportLib.vcxproj")
vcxproj_sources(RASTERLIB_SOURCES "${RASTERLIB_DIR}/RasterLib.vcxproj")
vcxproj_sources(RENDERLIB_SOURCES "${RENDERLIB_DIR}/RenderLib.vcxproj")
vcxproj_sources(RASTERRENDERLIB_SOURCES "${RASTERRENDERLIB_DIR}/RasterRenderLib.vcxproj")
vcxproj_sources(VULKANLIB_SOURCES "${VULKANLIB_DIR}/VulkanLib.vcxproj")
vcxproj_sources(INPUTLIB_SOURCES "${INPUTLIB_DIR}/InputLib.vcxproj")
vcxproj_sources(WINDOWLIB_SOURCES "${WINDOWLIB_DIR}/WindowLib.vcxproj")
vcxproj_sources(X11LIB_SOURCES "${X11LIB_DIR}/X11Lib.vcxproj")
vcxproj_sources(SCENEVIEWLIB_SOURCES "${SCENEVIEWLIB_DIR}/SceneViewLib.vcxproj")
vcxproj_sources(TESTER_SOURCES "${TESTER_DIR}/WindowInputTester.vcxproj")


codaphela_options(ONLY_64 "${COMMON_DIR}")


# CMutex uses pthreads on Linux.
find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

add_library(BaseLib STATIC ${BASELIB_SOURCES})
target_link_libraries(BaseLib PUBLIC baselib_options Threads::Threads)

add_library(StandardLib STATIC ${STANDARDLIB_SOURCES})
target_link_libraries(StandardLib PUBLIC BaseLib)

add_library(SupportLib STATIC ${SUPPORTLIB_SOURCES})
target_link_libraries(SupportLib PUBLIC StandardLib BaseLib)

add_library(RasterLib STATIC ${RASTERLIB_SOURCES})
target_link_libraries(RasterLib PUBLIC StandardLib BaseLib)

add_library(RenderLib STATIC ${RENDERLIB_SOURCES})
target_link_libraries(RenderLib PUBLIC SupportLib StandardLib BaseLib)

add_library(RasterRenderLib STATIC ${RASTERRENDERLIB_SOURCES})
target_link_libraries(RasterRenderLib PUBLIC RenderLib RasterLib SupportLib StandardLib BaseLib)

# Only Vulkan's headers (the SDK or libvulkan-dev): VulkanLib finds its functions in libvulkan.so.1 while it runs.
find_path(VULKAN_INCLUDE_DIR vulkan/vulkan.h HINTS "$ENV{VULKAN_SDK}/include" REQUIRED)
add_library(VulkanLib STATIC ${VULKANLIB_SOURCES})
target_include_directories(VulkanLib PUBLIC "${VULKAN_INCLUDE_DIR}")
target_link_libraries(VulkanLib PUBLIC RenderLib SupportLib StandardLib BaseLib)

add_library(InputLib STATIC ${INPUTLIB_SOURCES})
target_link_libraries(InputLib PUBLIC StandardLib BaseLib)

add_library(WindowLib STATIC ${WINDOWLIB_SOURCES})
target_link_libraries(WindowLib PUBLIC InputLib SupportLib StandardLib BaseLib)

add_library(X11Lib STATIC ${X11LIB_SOURCES})
target_link_libraries(X11Lib PUBLIC WindowLib InputLib SupportLib StandardLib BaseLib X11::X11 X11::Xext)

add_library(SceneViewLib STATIC ${SCENEVIEWLIB_SOURCES})
target_link_libraries(SceneViewLib PUBLIC WindowLib InputLib VulkanLib RasterRenderLib RenderLib RasterLib SupportLib StandardLib BaseLib)

add_executable(WindowInputTester ${TESTER_SOURCES})
target_include_directories(WindowInputTester PRIVATE "${TESTER_DIR}")
target_link_libraries(WindowInputTester PRIVATE X11Lib SceneViewLib WindowLib InputLib VulkanLib RasterRenderLib RenderLib RasterLib SupportLib StandardLib BaseLib)
