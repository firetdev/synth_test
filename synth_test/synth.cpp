#include "synth.hpp"
#include <cmath>

Synth::Synth() {
    samples.resize(BUFFER_SIZE);
    initialize(
        1,
        SAMPLE_RATE,
        {sf::SoundChannel::Mono}
    );
}

// Trigger a note on a specific frequency
void Synth::noteOn(double freq) {
    // Prevent duplicate voices if the key is held down
    for (int i = 0; i < MAX_VOICES; ++i) {
        if (voices[i].active.load() && voices[i].frequency.load() == freq) {
            return;
        }
    }

    // Find a free voice and assign the frequency
    for (int i = 0; i < MAX_VOICES; ++i) {
        if (!voices[i].active.load()) {
            voices[i].frequency.store(freq);
            voices[i].phase = 0.0;  // Reset phase for clean attack
            voices[i].phase2 = 0.0;
            voices[i].active.store(true);
            break;
        }
    }
}

// Release a specific frequency
void Synth::noteOff(double freq) {
    for (int i = 0; i < MAX_VOICES; ++i) {
        if (voices[i].active.load() && voices[i].frequency.load() == freq) {
            voices[i].active.store(false);
        }
    }
}

// Main synth function
bool Synth::onGetData(Chunk& data) {
    float currentBlend = blend.load();
    Waveform wave1 = osc1.load();
    Waveform wave2 = osc2.load();

    for (int i = 0; i < BUFFER_SIZE; i++) {
        double mixedSample = 0.0;

        // Loop through all voices and add up the sound of the active ones
        for (int v = 0; v < MAX_VOICES; ++v) {
            if (!voices[v].active.load()) continue;

            double freq1 = voices[v].frequency.load();
            double freq2 = freq1 * 1.002; // Slight detune for thickness

            double oscillator1 = 0;
            switch (wave1) {
                case Waveform::Sine: oscillator1 = std::sin(voices[v].phase); break;
                case Waveform::Saw: oscillator1 = sawWave(voices[v].phase); break;
            }
            
            double oscillator2 = 0;
            switch (wave2) {
                case Waveform::Sine: oscillator2 = std::sin(voices[v].phase2); break;
                case Waveform::Saw: oscillator2 = sawWave(voices[v].phase2); break;
            }
            
            // Mix the two oscillators for this specific voice
            double voiceMix = currentBlend * oscillator1 + (1.0f - currentBlend) * oscillator2;
            
            // Accumulate this voice into the master output
            mixedSample += voiceMix;

            // Advance phases
            voices[v].phase += 2.0 * PI * freq1 / SAMPLE_RATE;
            voices[v].phase2 += 2.0 * PI * freq2 / SAMPLE_RATE;

            if (voices[v].phase > 2.0 * PI) voices[v].phase -= 2.0 * PI;
            if (voices[v].phase2 > 2.0 * PI) voices[v].phase2 -= 2.0 * PI;
        }

        // Sample is multiplied by 3000 for volume, but divided by 4 so that you can play chords without clipping
        samples[i] = static_cast<std::int16_t>((mixedSample / 4.0) * 3000);
    }

    data.samples = samples.data();
    data.sampleCount = samples.size();

    return true;
}
