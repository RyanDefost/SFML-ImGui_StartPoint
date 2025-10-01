#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Conways.hpp"
#include "Profiler.hpp"

Profiler profiler;

int main() {
    sf::RenderWindow window;
    window.create(sf::VideoMode({ 1500, 1000 }), "My window");
    //window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    if (!ImGui::SFML::Init(window))
        return -1;

    Conways conways;

    sf::Clock deltaClock;
    bool toggleTest = false;
    while (window.isOpen())
    {
        // Event Polling ----------
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Update -----------------
        ImGui::SFML::Update(window, deltaClock.restart());

        {
            PROFILE(profiler, "Update Conways");
            conways.UpdateCells();
        }

        // Render -----------------
        window.clear();
        
        profiler.renderImGui();
        {
            PROFILE(profiler, "Render Conways");
            conways.DisplayCells(window);
        }
        ImGui::SFML::Render(window);

        window.display();
    }

    return 0;
};