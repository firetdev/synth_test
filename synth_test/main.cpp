#include <SFML/Graphics.hpp>
#include <map>
#include <cmath>
#include "synth.hpp"
#include "get_base_frequency.hpp"
#include "ui/button.hpp"
#include "ui/slider.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Synth", sf::Style::Titlebar | sf::Style::Close);

    Synth synth;
    synth.setADSR(2.0, 3.0, 0.0, 2.0);
    synth.play();
    
    float blend = 0.8;
    synth.setBlend(blend);

    // UI stuff
    sf::Font font;
    if (!font.openFromFile("arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }
    
    sf::Text osc_text(font);
    osc_text.setPosition({82, 26});
    osc_text.setString("Oscillator blend");
    osc_text.setFillColor(sf::Color::White);
    osc_text.setCharacterSize(26);
    
    sf::Text sine_text(font);
    sf::Text saw_text(font);
    sine_text.setString("Sine");
    saw_text.setString("Saw");
    sine_text.setFillColor(sf::Color::White);
    sine_text.setCharacterSize(20);
    saw_text.setFillColor(sf::Color::White);
    saw_text.setCharacterSize(20);
    sine_text.setPosition({30, 70});
    saw_text.setPosition({274, 70});
    Slider blend_slider(83, 70, 180, 27, 1.0, blend);
    
    int octaveShift = 0;
    std::map<sf::Keyboard::Key, double> activeNotes;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
                auto code = keyPress->code;
                
                // Handle Octave Shifting
                if (code == sf::Keyboard::Key::Up) {
                    if (octaveShift < 3) octaveShift++;
                } else if (code == sf::Keyboard::Key::Down) {
                    if (octaveShift > -3) octaveShift--;
                }
                // Handle Note Playing
                else {
                    double baseFreq = getBaseFrequency(code);
                    
                    // If it's a mapped key and not already being held down
                    if (baseFreq > 0.0 && activeNotes.find(code) == activeNotes.end()) {
                        double freq = baseFreq * std::pow(2.0, octaveShift);
                        
                        synth.noteOn(freq);
                        activeNotes[code] = freq;
                    }
                }
            } else if (const auto* keyRelease = event->getIf<sf::Event::KeyReleased>()) {
                auto code = keyRelease->code;
                
                // If we release a key that is currently playing, turn it off
                if (activeNotes.find(code) != activeNotes.end()) {
                    synth.noteOff(activeNotes[code]);
                    activeNotes.erase(code);
                }
            }
            
            // Handle clicks
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2i mousePos = mousePressed->position;
                if (blend_slider.handleClick(mousePos)) {
                    blend = blend_slider.getValue();
                    synth.setBlend(blend);
                }
            }
        }
        
        window.clear();
        window.draw(osc_text);
        window.draw(sine_text);
        window.draw(saw_text);
        blend_slider.render(window);
        window.display();
    }

    return 0;
}
