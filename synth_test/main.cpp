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
    
    float blend = 0.8;
    synth.setBlend(blend);
    
    float a = 0.5;
    float d = 3.0;
    float s = 0.5;
    float r = 0.5;
    synth.setADSR(a, d, s, r);
    
    float cutoff = 20000.0;
    float depth = 20000.0;
    float filter_a = 0.0;
    float filter_d = 0.0;
    float filter_s = 1.0;
    float filter_r = 0.0;
    
    float minFreq = 20.0f;
    float maxFreq = 20000.0f;
    
    // Because hearing is logarithmic, the slider needs to imitate this
    float initialCutoffFreq = 8000.0f;
    float initialCutoffSliderPos = std::log(initialCutoffFreq / minFreq) / std::log(maxFreq / minFreq);
    
    synth.setCutoffFrequency(cutoff);
    synth.setFilterDepth(depth);
    synth.setFilterADSR(filter_a, filter_d, filter_s, filter_r);

    // UI stuff
    Keyboard keyboard;
    
    sf::Font font;
    if (!font.openFromFile("arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }
    
    sf::Text osc_text(font);
    osc_text.setPosition({132, 30});
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
    sine_text.setPosition({80, 80});
    saw_text.setPosition({324, 80});
    Slider blend_slider(133, 80, 180, 27, 1.0, blend);
    
    sf::Text adsr_text(font);
    adsr_text.setPosition({502, 60});
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
    a_text.setPosition({460, 110});
    d_text.setPosition({460, 170});
    s_text.setPosition({453, 230});
    r_text.setPosition({446, 290});
    Slider a_slider(530, 110, 180, 27, 5.0, a);
    Slider d_slider(530, 170, 180, 27, 5.0, d);
    Slider s_slider(530, 230, 180, 27, 1.0, s);
    Slider r_slider(530, 290, 180, 27, 5.0, r);
    
    sf::Text filter_text(font);
    filter_text.setPosition({80, 140});
    filter_text.setString("Filter");
    filter_text.setFillColor(sf::Color::White);
    filter_text.setCharacterSize(26);
    
    sf::Text cutoff_text(font);
    sf::Text depth_text(font);
    cutoff_text.setString("Filter cutoff");
    depth_text.setString("Filter depth");
    cutoff_text.setFillColor(sf::Color::White);
    cutoff_text.setCharacterSize(20);
    depth_text.setFillColor(sf::Color::White);
    depth_text.setCharacterSize(20);
    cutoff_text.setPosition({80, 180});
    depth_text.setPosition({250, 180});
    Slider cutoff_slider(80, 210, 160, 27, 1.0f, initialCutoffSliderPos);
    Slider depth_slider(250, 210, 160, 27, 40000.0, depth);
    
    sf::Text filter_a_text(font);
    sf::Text filter_d_text(font);
    sf::Text filter_s_text(font);
    sf::Text filter_r_text(font);
    filter_a_text.setString("Attack");
    filter_d_text.setString("Decay");
    filter_s_text.setString("Sustain");
    filter_r_text.setString("Release");
    filter_a_text.setFillColor(sf::Color::White);
    filter_a_text.setCharacterSize(20);
    filter_d_text.setFillColor(sf::Color::White);
    filter_d_text.setCharacterSize(20);
    filter_s_text.setFillColor(sf::Color::White);
    filter_s_text.setCharacterSize(20);
    filter_r_text.setFillColor(sf::Color::White);
    filter_r_text.setCharacterSize(20);
    filter_a_text.setPosition({80, 247});
    filter_d_text.setPosition({250, 247});
    filter_s_text.setPosition({80, 314});
    filter_r_text.setPosition({250, 314});
    Slider filter_a_slider(80, 277, 160, 27, 5.0, filter_a);
    Slider filter_d_slider(250, 277, 160, 27, 5.0, filter_d);
    Slider filter_s_slider(80, 344, 160, 27, 1.0, filter_s);
    Slider filter_r_slider(250, 344, 160, 27, 5.0, filter_r);
    
    int octaveShift = 0;
    std::map<sf::Keyboard::Key, double> activeNotes;

    // Start synth
    synth.play();
    
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
                if (cutoff_slider.handleClick(mousePos)) {
                    float sliderPos = std::clamp(cutoff_slider.getValue(), 0.0f, 1.0f);
                    cutoff = minFreq * std::pow(maxFreq / minFreq, sliderPos);
                    synth.setCutoffFrequency(cutoff);
                }
                if (depth_slider.handleClick(mousePos)) {
                    depth = std::clamp(depth_slider.getValue(), 0.f, 40000.f) - 20000;
                    synth.setFilterDepth(depth);
                }
                if (filter_a_slider.handleClick(mousePos)) {
                    filter_a = filter_a_slider.getValue();
                    synth.setFilterADSR(filter_a, filter_d, filter_s, filter_r);
                }
                if (filter_d_slider.handleClick(mousePos)) {
                    filter_d = filter_d_slider.getValue();
                    synth.setFilterADSR(filter_a, filter_d, filter_s, filter_r);
                }
                if (filter_s_slider.handleClick(mousePos)) {
                    filter_s = filter_s_slider.getValue();
                    synth.setFilterADSR(filter_a, filter_d, filter_s, filter_r);
                }
                if (filter_r_slider.handleClick(mousePos)) {
                    filter_r = filter_r_slider.getValue();
                    synth.setFilterADSR(filter_a, filter_d, filter_s, filter_r);
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
        window.draw(filter_text);
        window.draw(cutoff_text);
        window.draw(depth_text);
        window.draw(filter_a_text);
        window.draw(filter_d_text);
        window.draw(filter_s_text);
        window.draw(filter_r_text);
        blend_slider.render(window);
        a_slider.render(window);
        d_slider.render(window);
        s_slider.render(window);
        r_slider.render(window);
        keyboard.render(window);
        cutoff_slider.render(window);
        depth_slider.render(window);
        filter_a_slider.render(window);
        filter_d_slider.render(window);
        filter_s_slider.render(window);
        filter_r_slider.render(window);
        window.display();
    }

    return 0;
}
