/*
 * Audible Planets - an expressive, quasi-Ptolemaic semi-modular synthesizer
 *
 * Copyright 2024, Greg Recco
 *
 * Audible Planets is released under the GNU General Public Licence v3
 * or later (GPL-3.0-or-later). The license is found in the "LICENSE"
 * file in the root of this repository, or at
 * https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * All source for Audible Planets is available at
 * https://github.com/gregrecco67/AudiblePlanets
 */

#pragma once

#include <JuceHeader.h>
#include "QuadOsc.h"
#include "Envelope.h"
#include "libMTSClient.h"
#include <numbers>
class APAudioProcessor;

using namespace std::numbers;
//==============================================================================
class SynthVoice : public gin::SynthesiserVoice,
                   public gin::ModVoice
{
public:
    SynthVoice(APAudioProcessor& p);
    
    void noteStarted() override;
    void noteRetriggered() override;
    void noteStopped(bool allowTailOff) override;

    void notePressureChanged() override;
    void noteTimbreChanged() override;
	void notePitchbendChanged() override {}
    void noteKeyStateChanged() override {}
    
    void setCurrentSampleRate(double newRate) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

	float getCurrentNote() override { return noteSmoother.getCurrentValue() * 127.0f; }

    bool isVoiceActive() override;

    float getFilterCutoffNormalized();
	float getMSEG1Phase();
	float getMSEG2Phase();
	float getMSEG3Phase();
	float getMSEG4Phase();
  
private:
    void updateParams(int blockSize);

    APAudioProcessor& proc;

    QuadOscillator osc1, osc2, osc3, osc4;

    gin::Filter filter;
    gin::LFO lfo1, lfo2, lfo3, lfo4;
	gin::MSEG mseg1, mseg2, mseg3, mseg4;
	gin::MSEG::Parameters mseg1Params, mseg2Params, mseg3Params, mseg4Params;

    Envelope env1, env2, env3, env4;
    std::array<Envelope*, 4> envs{&env1, &env2, &env3, &env4};
    
	StereoPosition epi1{ 1.0f, 0.0f, 1.0f, 0.0f};
	StereoPosition epi2{ 1.0, 0.0f, 1.0f, 0.0f };
	StereoPosition epi3{ 1.0f, 0.0f, 1.0f, 0.0f };
	StereoPosition epi4{ 1.0f, 0.0f, 1.0f, 0.0f };

	StereoPosition osc1Positions[32];
	StereoPosition osc2Positions[32];
	StereoPosition osc3Positions[32];
	StereoPosition osc4Positions[32];

    float currentMidiNote = -1;
    QuadOscillator::Params osc1Params, osc2Params, osc3Params, osc4Params;
	float osc1Freq = 0.0f, osc2Freq = 0.0f, osc3Freq = 0.0f, osc4Freq = 0.0f;
	float osc1Vol = 0.0f, osc2Vol = 0.0f, osc3Vol = 0.0f, osc4Vol = 0.0f;
	int algo{ 0 };
	float equant{ 0.f }, demodVol{ 2.0f };
    
	float baseAmplitude = 0.12f; 

    gin::EasedValueSmoother<float> noteSmoother;
    
    friend class APSynth;
    juce::MPENote curNote;
	
};
