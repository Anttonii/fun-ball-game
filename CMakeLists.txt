cmake_minimum_required(VERSION 3.12)

project(SuikaGame)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/include)
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")

if(WIN32)
    set(SDL2_DIR ${PROJECT_SOURCE_DIR}/dependencies/SDL2)
    set(BOX2D_DIR ${PROJECT_SOURCE_DIR}/dependencies/box2d)

    set(BOX2D_INCLUDE_DIR ${BOX2D_DIR}/include/box2d/)
    set(BOX2D_LIBRARY ${BOX2D_DIR}/lib/libbox2d.a)
endif(WIN32)

if(APPLE)
    set(BOX2D_INCLUDE_DIR /opt/homebrew/include/box2d/)
    set(BOX2D_LIBRARY /opt/homebrew/lib/libbox2d.a)
endif(APPLE)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(
    ${SDL2_INCLUDE_DIRS} 
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${BOX2D_INCLUDE_DIR}
)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "src/*.cpp")
file (GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.h")

set (P_INCLUDE_DIRS "")
foreach (_headerFile ${HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND P_INCLUDE_DIRS ${_dir})
endforeach()
list (REMOVE_DUPLICATES P_INCLUDE_DIRS)

add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${P_INCLUDE_DIRS})

if(WIN32)
target_link_libraries(
    ${PROJECT_NAME}
        mingw32
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${BOX2D_LIBRARY}
)
endif(WIN32)

if(APPLE)
target_link_libraries(
    ${PROJECT_NAME}
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${BOX2D_LIBRARY}
)
endif(APPLE)

message(STATUS "Copying all assets to destination folder...")
file(
    COPY assets
    DESTINATION ${OUTPUT_DIR}
)

message(STATUS "Copying all dependencies to destination folder...")
file(GLOB PROJECT_DEPENDENCIES ${SDL2_LIBRARIES}/*.dll)
file(GLOB PROJECT_DEPENDENCIES ${SDL2_IMAGE_LIBRARIES}/*.dll)
file(GLOB PROJECT_DEPENDENCIES ${SDL2_TTF_LIBRARIES}/*.dll)

foreach(Dependency IN LISTS PROJECT_DEPENDENCIES)
    file(
        COPY ${Dependency}
        DESTINATION ${OUTPUT_DIR}
    )
endforeach()