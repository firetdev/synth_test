#pragma once

#include <SFML/Graphics.hpp>

// Basic slider class
class Slider {
private:
    float max_value = 100.f;
    float value = 0.f;
    float x, y, width, height;
    float percentage_full = 0.f;
    
    sf::RectangleShape outline;
    sf::RectangleShape bar;
    sf::Font font;
    sf::Text text;

public:
    Slider(float, float, float, float, float, float);
    
    void render(sf::RenderWindow&);
    
    float getValue() { return value; }
    
    void handleClick(sf::Vector2i);
};
