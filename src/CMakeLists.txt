cmake_minimum_required(VERSION 3.12) # FindSFML.cmake only present in >=3.12.

set(PROJECT_SOURCES
  main.cpp

  video.hpp
  video.cpp

  screen.hpp
  )

add_custom_target(run
    COMMAND ${CMAKE_PROJECT_NAME}
    DEPENDS ${CMAKE_PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

find_package(SFML COMPONENTS system window graphics audio REQUIRED)
add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} sfml-system sfml-window sfml-graphics sfml-audio)
