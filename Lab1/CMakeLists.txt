cmake_minimum_required(VERSION 3.10)
project(Lab1_BuzzyDefender LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Sources (all .cpp files inside Lab1/code) ---
file(GLOB LAB1_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/code/*.cpp"
)

add_executable(Lab1 ${LAB1_SOURCES})

# Headers live alongside sources
target_include_directories(Lab1 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/code
)

# Link SFML targets that come from the top-level add_subdirectory(SFML)
target_link_libraries(Lab1 PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

# Run from Lab1/ so relative paths like "graphics/..." work
set_target_properties(Lab1 PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Copy the graphics folder next to the built executable (handy when running outside the IDE)
add_custom_command(TARGET Lab1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/graphics"
            "$<TARGET_FILE_DIR:Lab1>/graphics"
)
