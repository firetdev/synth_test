#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

// Simple button class
class Button {
private:
    float x, y, width, height;
    std::string text_content;
    sf::Font font;
    sf::Text text;
    sf::RectangleShape box;
    
public:
    Button(float x, float y, float w, float h, std::string msg)
    : x(x), y(y), width(w), height(h), text_content(msg), text(sf::Text(font)) {
        if (!font.openFromFile("Arial.ttf")) {
            std::cerr << "Failed to load font!" << std::endl;
            return;
        }
        
        text.setFont(font);
        text.setPosition({x + 11, y});
        text.setString(text_content);
        box.setPosition({x, y});
        box.setOutlineColor(sf::Color::White);
        box.setOutlineThickness(1);
        box.setSize({width, height});
        box.setFillColor(sf::Color::Transparent);
    }
    
    bool is_clicked(sf::Vector2i pos) {
        // Avoids needing live input, just uses event loop
        return pos.x > x && pos.x < (x + width) && pos.y > y && pos.y < (y + height);
    }
    
    void render(sf::RenderWindow& window) {
        window.draw(box);
        window.draw(text);
    }
};
