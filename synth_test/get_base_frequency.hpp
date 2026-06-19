#pragma once

#include <SFML/Graphics.hpp>

// Helper function to map computer keys to base frequencies (C4 Octave)
double getBaseFrequency(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::A: return 261.63;  // C4
        case sf::Keyboard::Key::W: return 277.18;  // C#4
        case sf::Keyboard::Key::S: return 293.66;  // D4
        case sf::Keyboard::Key::E: return 311.13;  // D#4
        case sf::Keyboard::Key::D: return 329.63;  // E4
        case sf::Keyboard::Key::F: return 349.23;  // F4
        case sf::Keyboard::Key::T: return 369.99;  // F#4
        case sf::Keyboard::Key::G: return 392.00;  // G4
        case sf::Keyboard::Key::Y: return 415.30;  // G#4
        case sf::Keyboard::Key::H: return 440.00;  // A4
        case sf::Keyboard::Key::U: return 466.16;  // A#4
        case sf::Keyboard::Key::J: return 493.88;  // B4
        case sf::Keyboard::Key::K: return 523.25;  // C5
        default: return 0.0;  // Unmapped keys
    }
}
