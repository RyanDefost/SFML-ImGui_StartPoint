#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Profiler.hpp"
#include "Particles.hpp"

Profiler profiler;

int main() {
    sf::RenderWindow window;
    window.create(sf::VideoMode({ 1280, 720 }), "My window");
    //window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    if (!ImGui::SFML::Init(window))
        return -1;

    auto system = ParticleSystem{&window};
    system.spawnParticles(100, sf::Vector2f(250,250));

    sf::Clock deltaClock;
    bool toggleTest = false;
    while (window.isOpen())
    {
        // Event Polling
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        system.update(deltaClock.getElapsedTime().asSeconds());

        profiler.renderImGui();

        // Render
        window.clear();

        ImGui::SFML::Render(window);
        system.render();

        window.display();
    }

    return 0;
};