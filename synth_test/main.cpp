#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <vector>
#include <string>
#include <atomic>

class Synth : public sf::SoundStream {
private:
    std::vector<std::int16_t> samples;
    
    std::atomic<double> frequency{440.0};
    std::atomic<double> frequency2{440.5};
    double phase = 0.0;
    double phase2 = 0.0;

    std::atomic<bool> noteOn{false};
    std::atomic<float> blend{0.8f};  // Percentage of sound that comes from oscillator 1
    
    enum class Waveform {
        Sine,
        Saw
    };
    
    std::atomic<Waveform> osc1{Waveform::Sine};
    std::atomic<Waveform> osc2{Waveform::Saw};

    static constexpr int SAMPLE_RATE = 44100;
    static constexpr int BUFFER_SIZE = 512;

public:
    Synth() {
        samples.resize(BUFFER_SIZE);
        initialize(
            1,
            44100,
            {sf::SoundChannel::Mono}
        );
    }

    void setFrequency(double freq) {
        if (freq != 0.0) {
            frequency = freq;
            frequency2 = freq * 1.002;
        }
    }
    
    void setNoteOn(bool on) {
        noteOn = on;
    }
    
    void setBlend(float b) {
        blend = b;
    }
    
    void setOscillators(Waveform a, Waveform b) {
        osc1.store(a);
        osc2.store(b);
    }
    
    void setOscillator1(Waveform type) {
        osc1.store(type);
    }

    void setOscillator2(Waveform type) {
        osc2.store(type);
    }

protected:
    double sawWave(double ph) {
        return (ph / 3.141592653589793) - 1.0;
    }
    
    bool onGetData(Chunk& data) override {
        constexpr double PI = 3.141592653589793;
        
        double freq1 = frequency.load();
        double freq2 = frequency2.load();

        float currentBlend = blend.load();
        bool currentNoteOn = noteOn.load();

        Waveform wave1 = osc1.load();
        Waveform wave2 = osc2.load();

        for (int i = 0; i < BUFFER_SIZE; i++) {
            double oscillator1 = 0;
            switch (wave1) {
                case Waveform::Sine:
                    oscillator1 = std::sin(phase);
                    break;

                case Waveform::Saw:
                    oscillator1 = sawWave(phase);
                    break;
            }
            
            double oscillator2 = 0;
            switch (wave2) {
                case Waveform::Sine:
                    oscillator2 = std::sin(phase2);
                    break;

                case Waveform::Saw:
                    oscillator2 = sawWave(phase2);
                    break;
            }
            
            double mixed =
                currentBlend * oscillator1 +
                (1.0f - currentBlend) * oscillator2;

            samples[i] = currentNoteOn
                ? static_cast<std::int16_t>(mixed * 3000)
                : 0;

            phase += 2.0 * PI * freq1 / SAMPLE_RATE;
            phase2 += 2.0 * PI * freq2 / SAMPLE_RATE;

            if (phase > 2.0 * PI)
                phase -= 2.0 * PI;

            if (phase2 > 2.0 * PI)
                phase2 -= 2.0 * PI;
        }

        data.samples = samples.data();
        data.sampleCount = samples.size();

        return true;
    }

    void onSeek(sf::Time) override {}
};

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Elasticity", sf::Style::Titlebar | sf::Style::Close);

    Synth synth;
    synth.play();

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            // Listen for window-scoped key presses instead of global state
            else if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPress->code == sf::Keyboard::Key::A) {
                    synth.setFrequency(261.63); // C4
                    synth.setNoteOn(true);
                }
                else if (keyPress->code == sf::Keyboard::Key::S) {
                    synth.setFrequency(293.66); // D4
                    synth.setNoteOn(true);
                }
                else if (keyPress->code == sf::Keyboard::Key::D) {
                    synth.setFrequency(329.63); // E4
                    synth.setNoteOn(true);
                }
                else if (keyPress->code == sf::Keyboard::Key::F) {
                    synth.setFrequency(349.23); // F4
                    synth.setNoteOn(true);
                }
                else if (keyPress->code == sf::Keyboard::Key::G) {
                    synth.setFrequency(392.00); // G4
                    synth.setNoteOn(true);
                }
            }
            
            else if (const auto* keyRelease = event->getIf<sf::Event::KeyReleased>()) {
                if (keyRelease->code == sf::Keyboard::Key::A ||
                    keyRelease->code == sf::Keyboard::Key::S ||
                    keyRelease->code == sf::Keyboard::Key::D ||
                    keyRelease->code == sf::Keyboard::Key::F ||
                    keyRelease->code == sf::Keyboard::Key::G)
                {
                    synth.setNoteOn(false);
                }
            }
        }

        window.clear();
        window.display();
    }

    return 0;
}
