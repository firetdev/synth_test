#include "synth.hpp"
#include <cmath>

Synth::Synth() {
    samples.resize(BUFFER_SIZE);
    initialize(
        1,
        SAMPLE_RATE,
        {sf::SoundChannel::Mono}
    );
    
    setCutoffFrequency(static_cast<float>(SAMPLE_RATE) / 2.0f);
    setFilterDepth(0.0f);
    setFilterADSR(0.0f, 0.0f, 1.0f, 0.0f);
}

Synth::Synth(float a, float d, float s, float r) {
    samples.resize(BUFFER_SIZE);
    initialize(
        1,
        SAMPLE_RATE,
        {sf::SoundChannel::Mono}
    );
    
    adsr.a = a;
    adsr.d = d;
    adsr.s = s;
    adsr.r = r;
    
    setCutoffFrequency(static_cast<float>(SAMPLE_RATE) / 2.0f);
    setFilterDepth(0.0f);
    setFilterADSR(0.0f, 0.0f, 1.0f, 0.0f);
}

// Trigger a note on a specific frequency
void Synth::noteOn(double freq) {
    int index = -1;
    
    // Prevent duplicate voices if the key is held down
    for (int i = 0; i < MAX_VOICES; ++i) {
        if (voices[i].active.load() && voices[i].frequency.load() == freq) {
            voices[i].released.store(false);
            voices[i].peaked = false;
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
            voices[i].released.store(false);  // Make sure it's not releasing
            index = i;
            break;
        }
    }
    
    if (index != -1) {
        // Prepare ADSR
        voices[index].amplitude = (adsr.a > 0.0) ? 0.0 : 1.0;
        voices[index].peaked = false;
        
        voices[index].filterEnv = (filterAdsr.a > 0.0) ? 0.0 : 1.0;
        voices[index].filterPeaked = false;
        voices[index].filterMem = 0.0;
        
        voices[index].peaked = false;
    }
}

// Release a specific frequency
void Synth::noteOff(double freq) {
    for (int i = 0; i < MAX_VOICES; ++i) {
        if (voices[i].active.load() && voices[i].frequency.load() == freq) {
            if (adsr.r > 0.0) {
                voices[i].released.store(true);
            } else {
                voices[i].active.store(false);
            }
        }
    }
}

// Main synth function
bool Synth::onGetData(Chunk& data) {
    float currentBlend = blend.load();
    Waveform wave1 = osc1.load();
    Waveform wave2 = osc2.load();

    float currentBaseCutoff = baseCutoff.load();
    float currentDepth = filterDepth.load();
    
    for (int i = 0; i < BUFFER_SIZE; i++) {
        double mixedSample = 0.0;

        // Loop through all voices and add up the sound of the active ones
        for (int v = 0; v < MAX_VOICES; ++v) {
            if (!voices[v].active.load()) continue;
            
            // Update amplitude (ADSR)
            if (voices[v].released.load()) {
                if (voices[v].amplitude > 0.0) {
                    voices[v].amplitude -= 1.0 / (adsr.r * SAMPLE_RATE);
                }
                
                // Check if the voice has fully faded out
                if (voices[v].amplitude <= 0.0 || adsr.r <= 0.0) {
                    voices[v].amplitude = 0.0;
                    voices[v].active.store(false);
                    voices[v].released.store(false);
                    continue;
                }
            } else if (!voices[v].peaked) {
                if (adsr.a > 0.0 && voices[v].amplitude < 1.0) {
                    voices[v].amplitude += 1.0 / (adsr.a * SAMPLE_RATE);
                } else {
                    voices[v].amplitude = 1.0;
                    voices[v].peaked = true;
                }
            } else {
                if (adsr.d > 0.0 && voices[v].amplitude > adsr.s) {
                    voices[v].amplitude -= (1.0 - adsr.s) / (adsr.d * SAMPLE_RATE);
                }
                
                if (voices[v].amplitude <= adsr.s || adsr.d <= 0.0) {
                    voices[v].amplitude = adsr.s;
                }
            }
            
            // Update filter ADSR
            if (voices[v].released.load()) {
                if (voices[v].filterEnv > 0.0) {
                    voices[v].filterEnv -= 1.0 / (filterAdsr.r * SAMPLE_RATE);
                }
                if (voices[v].filterEnv <= 0.0 || filterAdsr.r <= 0.0) {
                    voices[v].filterEnv = 0.0;
                }
            } else if (!voices[v].filterPeaked) {
                if (filterAdsr.a > 0.0 && voices[v].filterEnv < 1.0) {
                    voices[v].filterEnv += 1.0 / (filterAdsr.a * SAMPLE_RATE);
                } else {
                    voices[v].filterEnv = 1.0; voices[v].filterPeaked = true;
                }
            } else {
                if (filterAdsr.d > 0.0 && voices[v].filterEnv > filterAdsr.s) {
                    voices[v].filterEnv -= (1.0 - filterAdsr.s) / (filterAdsr.d * SAMPLE_RATE);
                }
                if (voices[v].filterEnv <= filterAdsr.s || filterAdsr.d <= 0.0) {
                    voices[v].filterEnv = filterAdsr.s;
                }
            }

            double freq1 = voices[v].frequency.load();
            double freq2 = freq1 * 1.002;  // Slight detune for thickness

            double oscillator1 = 0;
            switch (wave1) {
                case Waveform::Sine: oscillator1 = std::sin(voices[v].phase) * 2; break;  // Double sin wave for volume
                case Waveform::Saw: oscillator1 = sawWave(voices[v].phase); break;
            }
            
            double oscillator2 = 0;
            switch (wave2) {
                case Waveform::Sine: oscillator2 = std::sin(voices[v].phase2) * 2; break;  // Double sin wave for volume
                case Waveform::Saw: oscillator2 = sawWave(voices[v].phase2); break;
            }
            
            // Mix the two oscillators for this specific voice
            double voiceMix = currentBlend * oscillator1 + (1.0f - currentBlend) * oscillator2;
            
            // Get cutoff
            float currentVoiceCutoff = currentBaseCutoff + (voices[v].filterEnv * currentDepth);
            currentVoiceCutoff = std::clamp(currentVoiceCutoff, 20.0f, static_cast<float>(SAMPLE_RATE) / 2.0f);
            
            // Calculate alpha
            float dt = 1.0f / SAMPLE_RATE;
            float rc = 1.0f / (2.0f * PI * currentVoiceCutoff);
            float voiceAlpha = dt / (rc + dt);

            // Filter the voice
            voices[v].filterMem = voices[v].filterMem + voiceAlpha * (voiceMix - voices[v].filterMem);
            
            // Add to final sample
            mixedSample += voices[v].filterMem * voices[v].amplitude;

            // Advance phases
            voices[v].phase += 2.0 * PI * freq1 / SAMPLE_RATE;
            voices[v].phase2 += 2.0 * PI * freq2 / SAMPLE_RATE;

            if (voices[v].phase > 2.0 * PI) voices[v].phase -= 2.0 * PI;
            if (voices[v].phase2 > 2.0 * PI) voices[v].phase2 -= 2.0 * PI;
        }

        // Sample is multiplied by 4000 for volume and divided by 2 to prevent clipping
        samples[i] = static_cast<std::int16_t>((mixedSample / 2.0) * 4000);
    }

    data.samples = samples.data();
    data.sampleCount = samples.size();

    return true;
}
