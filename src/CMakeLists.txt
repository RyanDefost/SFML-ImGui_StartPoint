add_executable(low-level_setup
 main.cpp
 "Profiler.hpp" 
 "Particles.hpp"
)

target_link_libraries(low-level_setup
 PUBLIC
  ImGui-SFML::ImGui-SFML
)

# Do we need this?
# include(Install.cmake)