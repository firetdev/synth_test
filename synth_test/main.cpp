#include <SFML/Graphics.hpp>
#include "synth.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Elasticity Polyphonic", sf::Style::Titlebar | sf::Style::Close);

    Synth synth;
    synth.play();

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPress->code == sf::Keyboard::Key::A) synth.noteOn(261.63); // C4
                else if (keyPress->code == sf::Keyboard::Key::S) synth.noteOn(293.66); // D4
                else if (keyPress->code == sf::Keyboard::Key::D) synth.noteOn(329.63); // E4
                else if (keyPress->code == sf::Keyboard::Key::F) synth.noteOn(349.23); // F4
                else if (keyPress->code == sf::Keyboard::Key::G) synth.noteOn(392.00); // G4
            }
            else if (const auto* keyRelease = event->getIf<sf::Event::KeyReleased>()) {
                if (keyRelease->code == sf::Keyboard::Key::A) synth.noteOff(261.63);
                else if (keyRelease->code == sf::Keyboard::Key::S) synth.noteOff(293.66);
                else if (keyRelease->code == sf::Keyboard::Key::D) synth.noteOff(329.63);
                else if (keyRelease->code == sf::Keyboard::Key::F) synth.noteOff(349.23);
                else if (keyRelease->code == sf::Keyboard::Key::G) synth.noteOff(392.00);
            }
        }

        window.clear();
        window.display();
    }

    return 0;
}
