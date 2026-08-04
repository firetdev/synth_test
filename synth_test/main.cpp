#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include <cmath>
#include "synth.hpp"
#include "get_base_frequency.hpp"
#include "ui/slider.hpp"
#include "ui/keyboard.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Synth", sf::Style::Titlebar | sf::Style::Close);

    Synth synth;
    synth.setADSR(2.0, 3.0, 0.0, 2.0);
    synth.play();
    
    float blend = 0.8;
    synth.setBlend(blend);
    
    float a = 0.5;
    float d = 3.0;
    float s = 0.5;
    float r = 0.5;
    synth.setADSR(a, d, s, r);

    // UI stuff
    Keyboard keyboard;
    
    sf::Font font;
    if (!font.openFromFile("arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }
    
    sf::Text osc_text(font);
    osc_text.setPosition({132, 60});
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
    sine_text.setPosition({80, 110});
    saw_text.setPosition({324, 110});
    Slider blend_slider(133, 110, 180, 27, 1.0, blend);
    
    sf::Text adsr_text(font);
    adsr_text.setPosition({482, 60});
    adsr_text.setString("ADSR Envelope");
    adsr_text.setFillColor(sf::Color::White);
    adsr_text.setCharacterSize(26);
    
    sf::Text a_text(font);
    sf::Text d_text(font);
    sf::Text s_text(font);
    sf::Text r_text(font);
    a_text.setString("Attack");
    d_text.setString("Decay");
    s_text.setString("Sustain");
    r_text.setString("Release");
    a_text.setFillColor(sf::Color::White);
    a_text.setCharacterSize(20);
    d_text.setFillColor(sf::Color::White);
    d_text.setCharacterSize(20);
    s_text.setFillColor(sf::Color::White);
    s_text.setCharacterSize(20);
    r_text.setFillColor(sf::Color::White);
    r_text.setCharacterSize(20);
    a_text.setPosition({440, 110});
    d_text.setPosition({440, 170});
    s_text.setPosition({433, 230});
    r_text.setPosition({426, 290});
    Slider a_slider(510, 110, 180, 27, 5.0, a);
    Slider d_slider(510, 170, 180, 27, 5.0, d);
    Slider s_slider(510, 230, 180, 27, 1.0, s);
    Slider r_slider(510, 290, 180, 27, 5.0, r);
    
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
                    
                    keyboard.keyPress(code, true);
                }
            } else if (const auto* keyRelease = event->getIf<sf::Event::KeyReleased>()) {
                auto code = keyRelease->code;
                
                // If we release a key that is currently playing, turn it off
                if (activeNotes.find(code) != activeNotes.end()) {
                    synth.noteOff(activeNotes[code]);
                    activeNotes.erase(code);
                }
                
                keyboard.keyPress(code, false);
            }
            
            // Handle clicks
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2i mousePos = mousePressed->position;
                if (blend_slider.handleClick(mousePos)) {
                    blend = blend_slider.getValue();
                    synth.setBlend(blend);
                }
                if (a_slider.handleClick(mousePos)) {
                    a = a_slider.getValue();
                    synth.setADSR(a, d, s, r);
                }
                if (d_slider.handleClick(mousePos)) {
                    d = d_slider.getValue();
                    synth.setADSR(a, d, s, r);
                }
                if (s_slider.handleClick(mousePos)) {
                    s = s_slider.getValue();
                    synth.setADSR(a, d, s, r);
                }
                if (r_slider.handleClick(mousePos)) {
                    r = r_slider.getValue();
                    synth.setADSR(a, d, s, r);
                }
            }
        }
        
        window.clear();
        window.draw(osc_text);
        window.draw(adsr_text);
        window.draw(sine_text);
        window.draw(saw_text);
        window.draw(a_text);
        window.draw(d_text);
        window.draw(s_text);
        window.draw(r_text);
        blend_slider.render(window);
        a_slider.render(window);
        d_slider.render(window);
        s_slider.render(window);
        r_slider.render(window);
        keyboard.render(window);
        window.display();
    }

    return 0;
}
