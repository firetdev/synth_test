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
        }
        
        window.clear();
        window.display();
    }

    return 0;
}
