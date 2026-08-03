#include "slider.hpp"
#include <iostream>
#include <string>

Slider::Slider(float x, float y, float w, float h, float max_v, float current_v)
: text(sf::Text(font)) {
    x = x;
    y = y;
    width = w;
    height = h;
    max_value = max_v;
    value = current_v;
    percentage_full = current_v / max_v;
    
    if (!font.openFromFile("Arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return;
    }
    
    text.setFont(font);
    text.setPosition({x + 11, y});
    text.setString(std::to_string(value));
    text.setFillColor(sf::Color::Black);
    text.setOutlineColor(sf::Color::White);
    text.setOutlineThickness(1);
    
    outline.setPosition({x, y});
    outline.setOutlineColor(sf::Color::White);
    outline.setOutlineThickness(1);
    outline.setSize({width, height});
    outline.setFillColor(sf::Color::Transparent);
    
    bar.setPosition({x, y});
    bar.setOutlineThickness(0);
    bar.setSize({width * percentage_full, height});
    bar.setFillColor(sf::Color::White);
}

void Slider::render(sf::RenderWindow& win) {
    win.draw(outline);
    win.draw(bar);
    win.draw(text);
}

void Slider::handleClick(sf::Vector2i pos) {
    if (!(pos.x > x && pos.x < (x + width) && pos.y > y && pos.y < (y + height))) return;  // Click was not within slider
    
    float relative_pos = pos.x - x;
    float pos_percentage = relative_pos / width;
    
    percentage_full = pos_percentage;
    value = pos_percentage * max_value;
    
    bar.setSize({width * percentage_full, height});
    text.setString(std::to_string(value));
}
