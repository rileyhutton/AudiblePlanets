#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RandPanels.h"
#include <random>


//==============================================================================
class RandEditor : public juce::Component
{
public:
	RandEditor(APAudioProcessor& proc_);
	void resized() override;
	void randomize();
	void clearAll();
	void randomizeOSCs();
	void randomizeENVs(); void resetENVs();
	void randomizeLFOs(); void resetLFOs();

	void randENVtoOSC(); void resetENVtoOSC();
	void randENVtoTimbre(); void resetENVtoTimbre();
	void randENVtoLFO(); void resetENVtoLFO();
	void randENVtoENV(); void resetENVtoENV();

	void randLFOtoOSC(); void resetLFOtoOSC();
	void randLFOtoTimbre(); void resetLFOtoTimbre();
	void randLFOtoLFO(); void resetLFOtoLFO();
	void randLFOtoENV(); void resetLFOtoENV();

	void randKeystoOSC(); void resetKeystoOSC();
	void randKeystoTimbre(); void resetKeystoTimbre();
	void randKeystoLFO(); void resetKeystoLFO();
	void randKeystoENV(); void resetKeystoENV();
	

	void randomizeFXMods(); void clearFXMods();
	void randomizeFXSelect();
	void randomizeInharmonic(); void resetInharmonic();

private:
	APAudioProcessor& proc;
	RandMatrixBox matrix{ "matrix", proc };
	juce::TextButton randomizeButton{ "Randomize All" }, clearAllButton{ "Clear All" },
		randOSCsButton{ "Randomize OSCs" }, // toggle button here for inharmonic
		randInharmonicButton{ "Randomize Inharm." }, resetInharmonicButton{ "Reset inharmonic" },
		randLFOsButton{ "Randomize LFOs" }, resetLFOsButton{ "Reset LFOs" },
		randENVsButton{ "Randomize ENVs" }, resetENVsButton{ "Reset ENVs" },

		randENVToOSCButton{ "ENV -> OSC" }, clearENVToOSCButton{ "Clear ENV -> OSC" },
		randENVToTimbreButton{ "ENV -> Timbre" }, clearENVToTimbreButton{ "Clear ENV -> Timbre" },
		randENVToLFOButton{ "ENV -> LFO" }, clearENVToLFOButton{ "Clear ENV -> LFO" },
		randENVToENVButton{ "ENV -> ENV" }, clearENVToENVButton{ "Clear ENV -> ENV" },

		randLFOToOSCButton{ "LFO -> OSC" }, clearLFOToOSCButton{ "Clear LFO -> OSC" },
		randLFOToTimbreButton{ "LFO -> Timbre" }, clearLFOToTimbreButton{ "Clear LFO -> Timbre" },
		randLFOToLfoButton{ "LFO -> LFO" }, clearLFOToLFOButton{ "Clear LFO -> LFO" },
		randLFOToENVButton{ "LFO -> ENV" }, clearLFOToENVButton{ "Clear LFO -> ENV" },

		randKeysToOSCButton{ "Keys -> OSC" }, clearKeysToOSCButton{ "Clear Keys -> OSC" },
		randKeysToTimbreButton{ "Keys -> Timbre" }, clearKeysToTimbreButton{ "Clear Keys -> Timbre" },
		randKeysToLFOButton{ "Keys -> LFO" }, clearKeysToLFOButton{ "Clear Keys -> LFO" },
		randKeysToENVButton{ "Keys -> ENV" }, clearKeysToENVButton{ "Clear Keys -> ENV" },

		randFXModsButton{ "Randomize FX Mods" }, clearFXModsButton{ "Clear FX Mods" },
		randFXSelectButton{ "Randomize FX Choice" }, clearFXSelectButton{ "Clear FX Choice" };

	std::random_device rd;
	juce::Slider randNumber, randAmount;
	juce::Label randNumberLabel{ "", "# of Mods" }, randAmountLabel{"", "Rand. Chance/Amount" };
    juce::Label test{"", "test"};
    juce::ToggleButton inharmonic{"inharmonic"};

	// std::array<gin::ModSrcId, 8> 
	juce::Array<gin::ModSrcId> lfoSrcs{ proc.modSrcMonoLFO1, proc.modSrcMonoLFO2, proc.modSrcMonoLFO3, proc.modSrcMonoLFO4,
		proc.modSrcLFO1, proc.modSrcLFO2, proc.modSrcLFO3, proc.modSrcLFO4 };
	juce::Array<gin::ModSrcId> envSrcs{ proc.modSrcEnv1, proc.modSrcEnv2, proc.modSrcEnv3, proc.modSrcEnv4 };
	juce::Array<gin::ModSrcId> keySrcs{ proc.modSrcPressure, proc.modSrcModwheel, proc.modSrcPitchBend, proc.modSrcVelocity,
		proc.modSrcNote, proc.modSrcTimbre, proc.modSrcModwheel };
	
	std::array<gin::Parameter::Ptr, 28> oscDstsBasic{
	proc.osc1Params.coarse, proc.osc1Params.volume, proc.osc1Params.tones, proc.osc1Params.fixed, proc.osc1Params.env, proc.osc1Params.pan, proc.osc1Params.spread,
	proc.osc2Params.coarse, proc.osc2Params.volume, proc.osc2Params.tones, proc.osc2Params.fixed, proc.osc2Params.env, proc.osc2Params.pan, proc.osc2Params.spread,
	proc.osc3Params.coarse, proc.osc3Params.volume, proc.osc3Params.tones, proc.osc3Params.fixed, proc.osc3Params.env, proc.osc3Params.pan, proc.osc3Params.spread,
	proc.osc4Params.coarse, proc.osc4Params.volume, proc.osc4Params.tones, proc.osc4Params.fixed, proc.osc4Params.env, proc.osc4Params.pan, proc.osc4Params.spread};

	std::array<gin::Parameter::Ptr, 32> oscDstsPlus{
		proc.osc1Params.coarse, proc.osc1Params.volume, proc.osc1Params.tones, proc.osc1Params.fixed, 
		proc.osc1Params.env, proc.osc1Params.pan, proc.osc1Params.spread, proc.osc1Params.fine,
		proc.osc2Params.coarse, proc.osc2Params.volume, proc.osc2Params.tones, proc.osc2Params.fixed, 
		proc.osc2Params.env, proc.osc2Params.pan, proc.osc2Params.spread, proc.osc2Params.fine,
		proc.osc3Params.coarse, proc.osc3Params.volume, proc.osc3Params.tones, proc.osc3Params.fixed, 
		proc.osc3Params.env, proc.osc3Params.pan, proc.osc3Params.spread, proc.osc3Params.fine,
		proc.osc4Params.coarse, proc.osc4Params.volume, proc.osc4Params.tones, proc.osc4Params.fixed, 
		proc.osc4Params.env, proc.osc4Params.pan, proc.osc4Params.spread, proc.osc4Params.fine
	};

	std::array<gin::Parameter::Ptr, 6> timbreDstsBasic{
		proc.timbreParams.algo, proc.timbreParams.demodmix, proc.timbreParams.blend, proc.timbreParams.equant, proc.filterParams.frequency, proc.filterParams.resonance
	};

	std::array<gin::Parameter::Ptr, 7> timbreDstsPlus{
		proc.timbreParams.algo, proc.timbreParams.demodmix, proc.timbreParams.blend, proc.timbreParams.equant, proc.filterParams.frequency, proc.filterParams.resonance, proc.timbreParams.pitch
	};


	std::array<gin::Parameter::Ptr, 24> envDsts{
		proc.env1Params.attack, proc.env1Params.decay, proc.env1Params.sustain, proc.env1Params.release, proc.env1Params.acurve, proc.env1Params.drcurve,
		proc.env2Params.attack, proc.env2Params.decay, proc.env2Params.sustain, proc.env2Params.release, proc.env2Params.acurve, proc.env2Params.drcurve,
		proc.env3Params.attack, proc.env3Params.decay, proc.env3Params.sustain, proc.env3Params.release, proc.env3Params.acurve, proc.env3Params.drcurve,
		proc.env4Params.attack, proc.env4Params.decay, proc.env4Params.sustain, proc.env4Params.release, proc.env4Params.acurve, proc.env4Params.drcurve
	};

	std::array<gin::Parameter::Ptr, 24> lfoDsts{
		proc.lfo1Params.rate, proc.lfo1Params.depth, proc.lfo1Params.phase, proc.lfo1Params.delay, proc.lfo1Params.fade, proc.lfo1Params.offset, 
		proc.lfo2Params.rate, proc.lfo2Params.depth, proc.lfo2Params.phase, proc.lfo2Params.delay, proc.lfo2Params.fade, proc.lfo2Params.offset, 
		proc.lfo3Params.rate, proc.lfo3Params.depth, proc.lfo3Params.phase, proc.lfo3Params.delay, proc.lfo3Params.fade, proc.lfo3Params.offset, 
		proc.lfo4Params.rate, proc.lfo4Params.depth, proc.lfo4Params.phase, proc.lfo4Params.delay, proc.lfo4Params.fade, proc.lfo4Params.offset
	};



};



