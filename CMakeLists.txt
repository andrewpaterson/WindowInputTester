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
# The targets are read from WindowInputTester.sln (codaphela_solution): the libraries its GCC-x64 rows build, each
# one's sources from its .vcxproj, so Visual Studio stays the source of truth.  Building re-runs the configure step
# when the solution or a project changes.  Of the program's two mains, WindowInputTester_Windows.cpp (WinMain)
# compiles to nothing here and WindowInputTester_Linux.cpp is the main.

cmake_minimum_required(VERSION 3.24)
project(WindowInputTester CXX)


# This repository is beside CommonLibraries.
get_filename_component(CODAPHELA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
set(COMMON_DIR "${CODAPHELA_ROOT}/CommonLibraries/Libraries")

include("${COMMON_DIR}/../CMake/Codaphela.cmake")

find_path(VULKAN_INCLUDE_DIR vulkan/vulkan.h HINTS "$ENV{VULKAN_SDK}/include" REQUIRED)
codaphela_options(ONLY_64 "${COMMON_DIR}" "${VULKAN_INCLUDE_DIR}")

codaphela_solution("${CMAKE_CURRENT_SOURCE_DIR}/WindowInputTester.sln" PROGRAMS WindowInputTester)
