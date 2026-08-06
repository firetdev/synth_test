#pragma once

#include <SFML/Audio.hpp>
#include <vector>
#include <atomic>
#include <array>
#include <map>
#include <cmath>

class Synth : public sf::SoundStream {
public:
    enum class Waveform {
        Sine,
        Saw
    };
    
    Synth();
    Synth(float, float, float, float);

    void noteOn(double);
    void noteOff(double);
    
    void setBlend(float b) { blend = b; }
    
    void setOscillators(Waveform a, Waveform b) { osc1.store(a); osc2.store(b); }
    void setOscillator1(Waveform type) { osc1.store(type); }
    void setOscillator2(Waveform type) { osc2.store(type); }
    
    void setADSR(float a, float d, float s, float r) {
        adsr.a = a;
        adsr.d = d;
        adsr.s = s;
        adsr.r = r;
    }
    
    void setCutoffFrequency(float cutoffHz) { baseCutoff.store(std::clamp(cutoffHz, 20.0f, static_cast<float>(SAMPLE_RATE) / 2.0f)); }
    void setFilterDepth(float depthHz) { filterDepth.store(depthHz); }

    void setFilterADSR(float a, float d, float s, float r) {
        filterAdsr.a = a;
        filterAdsr.d = d;
        filterAdsr.s = s;
        filterAdsr.r = r;
    }
    
private:
    struct Voice {
        std::atomic<double> frequency{0.0};
        std::atomic<bool> active{false};
        std::atomic<bool> released{false};
        double phase = 0.0;
        double phase2 = 0.0;

        // Amplitude envelope
        float amplitude = 1.f;
        bool peaked = false;  // Checks whether the amplitude should be rising to 1.0 or falling to the sustain level

        // Filter envelope state
        float filterEnv = 1.f;
        bool filterPeaked = false;
        
        // Previous sample
        double filterMem = 0.0;
    };
    
    struct Envelope {
        float a = 2.f, d = 2.f, s = 0.3f, r = 2.f;
    };

    static constexpr int SAMPLE_RATE = 44100;
    static constexpr int BUFFER_SIZE = 512;
    static constexpr int MAX_VOICES = 8;

    std::vector<std::int16_t> samples;
    std::array<Voice, MAX_VOICES> voices;

    std::atomic<float> blend{0.8f};  // Percentage of sound that comes from oscillator 1
    std::atomic<Waveform> osc1{Waveform::Sine};
    std::atomic<Waveform> osc2{Waveform::Saw};
    
    Envelope adsr;
    Envelope filterAdsr;

    std::atomic<float> baseCutoff{1000.0f};  // Lowest point of the filter
    std::atomic<float> filterDepth{5000.0f};  // How much Hz the envelope adds

protected:
    static constexpr double PI = 3.14159265358979323846;
    
    double sawWave(double ph) { return (ph / PI) - 1.0; }  // Rising saw wave; falling would be 1.0 - (ph / PI)
    
    bool onGetData(Chunk&) override;
    
    void onSeek(sf::Time) override {}
};
