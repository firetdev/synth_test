#pragma once

#include <SFML/Audio.hpp>
#include <vector>
#include <atomic>
#include <array>

class Synth : public sf::SoundStream {
public:
    enum class Waveform {
        Sine,
        Saw
    };
    
    Synth();

    void noteOn(double);
    void noteOff(double);
    
    void setBlend(float b) { blend = b; }
    void setOscillators(Waveform a, Waveform b) { osc1.store(a); osc2.store(b); }
    void setOscillator1(Waveform type) { osc1.store(type); }
    void setOscillator2(Waveform type) { osc2.store(type); }
    
private:
    struct Voice {
        std::atomic<double> frequency{0.0};
        std::atomic<bool> active{false};
        double phase = 0.0;
        double phase2 = 0.0;
    };

    static constexpr int SAMPLE_RATE = 44100;
    static constexpr int BUFFER_SIZE = 512;
    static constexpr int MAX_VOICES = 8;

    std::vector<std::int16_t> samples;
    std::array<Voice, MAX_VOICES> voices;

    std::atomic<float> blend{0.8f};  // Percentage of sound that comes from oscillator 1
    std::atomic<Waveform> osc1{Waveform::Sine};
    std::atomic<Waveform> osc2{Waveform::Saw};

protected:
    static constexpr double PI = 3.14159265358979323846;
    
    double sawWave(double ph) { return (ph / PI) - 1.0; }
    
    bool onGetData(Chunk&) override;
    
    void onSeek(sf::Time) override {}
};
