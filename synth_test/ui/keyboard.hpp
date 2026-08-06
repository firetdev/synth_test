#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Keyboard {
private:
    std::vector<sf::RectangleShape> whiteKeys;
    std::vector<sf::RectangleShape> blackKeys;
    
public:
    Keyboard() {
        for (int i = 0; i < 8; ++i) {
            whiteKeys.emplace_back(sf::RectangleShape({100, 150}));
            whiteKeys[i].setPosition({100 * static_cast<float>(i), 450});
            whiteKeys[i].setOutlineColor(sf::Color(210, 210, 210));
            whiteKeys[i].setOutlineThickness(-1);
        }
        for (int i = 0; i < 5; ++i) {
            blackKeys.emplace_back(sf::RectangleShape({50, 90}));
        }
        
        blackKeys[0].setPosition({75, 450});
        blackKeys[1].setPosition({175, 450});
        blackKeys[2].setPosition({375, 450});
        blackKeys[3].setPosition({475, 450});
        blackKeys[4].setPosition({575, 450});
        
        for(auto& key : whiteKeys) {
            key.setFillColor(sf::Color::White);
        }
        for(auto& key : blackKeys) {
            key.setFillColor(sf::Color(50, 50, 50));
        }
    }
    
    void render(sf::RenderWindow& win) {
        for (auto& key : whiteKeys) {
            win.draw(key);
        }
        for (auto& key : blackKeys) {
            win.draw(key);
        }
    }
    
    void keyPress(sf::Keyboard::Key key, bool value) {
        if (value == true) {
            switch (key) {
                case sf::Keyboard::Key::A: whiteKeys[0].setFillColor(sf::Color(210, 210, 210)); return;
                case sf::Keyboard::Key::W: blackKeys[0].setFillColor(sf::Color(110, 110, 110)); return;
                case sf::Keyboard::Key::S: whiteKeys[1].setFillColor(sf::Color(210, 210, 210)); return;
                case sf::Keyboard::Key::E: blackKeys[1].setFillColor(sf::Color(110, 110, 110)); return;
                case sf::Keyboard::Key::D: whiteKeys[2].setFillColor(sf::Color(210, 210, 210)); return;
                case sf::Keyboard::Key::F: whiteKeys[3].setFillColor(sf::Color(210, 210, 210)); return;
                case sf::Keyboard::Key::T: blackKeys[2].setFillColor(sf::Color(110, 110, 110)); return;
                case sf::Keyboard::Key::G: whiteKeys[4].setFillColor(sf::Color(210, 210, 210)); return;
                case sf::Keyboard::Key::Y: blackKeys[3].setFillColor(sf::Color(110, 110, 110)); return;
                case sf::Keyboard::Key::H: whiteKeys[5].setFillColor(sf::Color(210, 210, 210)); return;
                case sf::Keyboard::Key::U: blackKeys[4].setFillColor(sf::Color(110, 110, 110)); return;
                case sf::Keyboard::Key::J: whiteKeys[6].setFillColor(sf::Color(210, 210, 210)); return;
                case sf::Keyboard::Key::K: whiteKeys[7].setFillColor(sf::Color(210, 210, 210)); return;
                default: return;
            }
        } else {
            switch (key) {
                case sf::Keyboard::Key::A: whiteKeys[0].setFillColor(sf::Color::White); return;
                case sf::Keyboard::Key::W: blackKeys[0].setFillColor(sf::Color(50, 50, 50)); return;
                case sf::Keyboard::Key::S: whiteKeys[1].setFillColor(sf::Color::White); return;
                case sf::Keyboard::Key::E: blackKeys[1].setFillColor(sf::Color(50, 50, 50)); return;
                case sf::Keyboard::Key::D: whiteKeys[2].setFillColor(sf::Color::White); return;
                case sf::Keyboard::Key::F: whiteKeys[3].setFillColor(sf::Color::White); return;
                case sf::Keyboard::Key::T: blackKeys[2].setFillColor(sf::Color(50, 50, 50)); return;
                case sf::Keyboard::Key::G: whiteKeys[4].setFillColor(sf::Color::White); return;
                case sf::Keyboard::Key::Y: blackKeys[3].setFillColor(sf::Color(50, 50, 50)); return;
                case sf::Keyboard::Key::H: whiteKeys[5].setFillColor(sf::Color::White); return;
                case sf::Keyboard::Key::U: blackKeys[4].setFillColor(sf::Color(50, 50, 50)); return;
                case sf::Keyboard::Key::J: whiteKeys[6].setFillColor(sf::Color::White); return;
                case sf::Keyboard::Key::K: whiteKeys[7].setFillColor(sf::Color::White); return;
                default: return;
            }
        }
    }
};
