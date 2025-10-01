add_executable(low-level_setup
 main.cpp
 "Profiler.hpp"  "Conways.cpp" "RenderTexture.cpp" "background.png")

target_link_libraries(low-level_setup
 PUBLIC
  ImGui-SFML::ImGui-SFML
)

# Do we need this?
# include(Install.cmake)