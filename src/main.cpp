#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Profiler.hpp"
#include "Balls.hpp"

Profiler profiler;

int main() {
    sf::RenderWindow window;
    window.create(sf::VideoMode({ 1280, 720 }), "My window");
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    if (!ImGui::SFML::Init(window))
        return -1;

    BallGame ballgame;
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

        // Update
        //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
        {
            PROFILE(profiler, "Update Balls");
            ballgame.UpdateBalls({ 500,720 }, deltaClock.getElapsedTime().asSeconds());
        }

        ImGui::SFML::Update(window, deltaClock.restart());


        profiler.renderImGui();

        //ImGui::ShowDemoWindow();

        // Render
        window.clear();

        ImGui::SFML::Render(window);
        ballgame.DrawBalls(window);

        window.display();
    }

    return 0;
};