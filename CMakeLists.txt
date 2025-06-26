cmake_minimum_required(VERSION 3.16)
project(LazyFooSDL3)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

# This assumes the SDL_image source is available in vendored/SDL_image
add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)

# Create your game executable target as usual
add_executable(spriteClippingAndStretching WIN32 spriteClippingAndStretching.cpp)

# Link to the actual SDL3 library.
target_link_libraries(spriteClippingAndStretching PRIVATE SDL3_image::SDL3_image SDL3::SDL3)
