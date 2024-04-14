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

#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::String waveshaperTypeTextFunction(const gin::Parameter&, float v)
{
    switch (int(v))
    {
        case 0: return "Sine";
        case 1: return "Atan 2";
        case 2: return "Atan 4";
        case 3: return "Atan 6";
        case 4: return "Tanh 2";
        case 5: return "Tanh 4";
        case 6: return "Tanh 6";
        case 7: return "Cubic mid";
        case 8: return "Cubic";
        case 9: return "Cheb 3";
        case 10: return "Cheb 5";
        case 11: return "Halfwave";
        case 12: return "Clipping";
        case 13: return "Bitcrush";
        case 14: return "Noise";
        case 15: return "Fullwave";
        case 16: return "Wavefolder";
        default:
            jassertfalse;
            return {};
    }
}

static juce::String lfoTextFunction(const gin::Parameter&, float v)
{
    switch ((gin::LFO::WaveShape)int(v))
    {
        case gin::LFO::WaveShape::none:          return "None";
        case gin::LFO::WaveShape::sine:          return "Sine";
        case gin::LFO::WaveShape::triangle:      return "Triangle";
        case gin::LFO::WaveShape::sawUp:         return "Saw Up";
        case gin::LFO::WaveShape::sawDown:       return "Saw Down";
        case gin::LFO::WaveShape::square:        return "Square";
        case gin::LFO::WaveShape::squarePos:     return "Square+";
        case gin::LFO::WaveShape::sampleAndHold: return "S&H";
        case gin::LFO::WaveShape::noise:         return "Noise";
        case gin::LFO::WaveShape::stepUp3:       return "Step Up 3";
        case gin::LFO::WaveShape::stepUp4:       return "Step Up 4";
        case gin::LFO::WaveShape::stepup8:       return "Step Up 8";
        case gin::LFO::WaveShape::stepDown3:     return "Step Down 3";
        case gin::LFO::WaveShape::stepDown4:     return "Step Down 4";
        case gin::LFO::WaveShape::stepDown8:     return "Step Down 8";
        case gin::LFO::WaveShape::pyramid3:      return "Pyramid 3";
        case gin::LFO::WaveShape::pyramid5:      return "Pyramid 5";
        case gin::LFO::WaveShape::pyramid9:      return "Pyramid 9";
        default:
            jassertfalse;
            return {};
    }
}

static juce::String enableTextFunction(const gin::Parameter&, float v)
{
    return v > 0.0f ? "On" : "Off";
}

static juce::String durationTextFunction(const gin::Parameter&, float v)
{
    return gin::NoteDuration::getNoteDurations()[size_t(v)].getName();
}

static juce::String percentTextFunction(const gin::Parameter&, float v)
{
    return juce::String(int(v * 1000.0f)/10.f) + "%";
}



static juce::String compressorTypeTextFunction(const gin::Parameter&, float v) {
    switch (int(v))
    {
    case 0: return "Compressor";
    case 1: return "Limiter";
    case 2: return "Expander";
    case 3: return "Gate";
    default:
        jassertfalse;
        return {};
    }

}

static juce::String filterTextFunction(const gin::Parameter&, float v)
{
    switch (int(v))
    {
        case 0: return "LP 12";
        case 1: return "LP 24";
        case 2: return "HP 12";
        case 3: return "HP 24";
        case 4: return "BP 12";
        case 5: return "BP 24";
        case 6: return "NT 12";
        case 7: return "NT 24";
        default:
            jassertfalse;
            return {};
    }
}

static juce::String millisecondsTextFunction(const gin::Parameter&, float v)
{
    return juce::String(int(v * 1000.0f)) + " ms";
}

static juce::String compressorAttackTextFunction(const gin::Parameter&, float v)
{
    return juce::String(int(v * 100000.0f)/100.f) + " ms";
}


static juce::String secondsTextFunction(const gin::Parameter&, float v)
{
    if (v < 1.f)
        return juce::String(int(v * 1000.0f)) + " ms";
    else
        return juce::String(v, 2) + " s";
}


static juce::String envSelectTextFunction(const gin::Parameter&, float v)
{
    switch (int(v))
    {
        case 0: return "Env 1";
        case 1: return "Env 2";
        case 2: return "Env 3";
        case 3: return "Env 4";
        default:
            jassertfalse;
            return {};
    }
}

static juce::String syncrepeatTextFunction(const gin::Parameter&, float v)
{
    switch (int(v))
    {
        case 0: return "Off";
        case 1: return "Sync";
        case 2: return "Free";
        default:
            jassertfalse;
            return {};
    }
}

static juce::String fxListTextFunction(const gin::Parameter&, float v)
{
    switch (int(v))
    {
        case 0: return String("--");
        case 1: return String("Waveshaper");
        case 2: return String("Dynamics");
        case 3: return String("Delay");
        case 4: return String("Chorus");
        case 5: return String("Multiband Filter");
        case 6: return String("Reverb");
        case 7: return String("Ring Modulator");
        case 8: return String("Gain");
        default:
            jassertfalse;
            return {};
    }
}


static juce::String algoTextFunction(const gin::Parameter&, float v)
{
    switch (int(v))
    {
        case 0: return String("1-2-3-(4)");
        case 1: return String("1-2-(3), 2-(4)");
        case 2: return String("1-(2), 1-3-(4)");
        case 3: return String("1-(2), 1-(3), 1-(4)");
        default:
            jassertfalse;
            return {};
    }
}

static juce::String freqTextFunction(const gin::Parameter&, float v)
{
    return juce::String (int(gin::getMidiNoteInHertz(v)));
}

static juce::String glideModeTextFunction(const gin::Parameter&, float v)
{
    switch (int(v))
    {
        case 0: return "Off";
        case 1: return "Glissando";
        case 2: return "Portamento";
        default:
            jassertfalse;
            return {};
    }
}

static juce::String msegDrawModeTextFunction(const gin::Parameter&, float v)
{
    switch (int(v))
    {
        case 0: return "Step";
        case 1: return "Half";
        case 2: return "Down";
        case 3: return "Up";
        case 4: return "Tri";
        default:
            jassertfalse;
            return {};
    }
}

static juce::String gridTextFunction(const gin::Parameter&, float v)
{
    return juce::String (v, 0);
}

//==============================================================================
void APAudioProcessor::OSCParams::setup(APAudioProcessor& p, juce::String numStr)
{
    juce::String id = "osc" + numStr;
    juce::String nm = "OSC" + numStr;

    NormalisableRange<float> osc1FineRange{ 0.0, 4.0, 0.0, 1.0 };
    NormalisableRange<float> defaultFineRange{ -2.0, 2.0, 0.0, 1.0 };
    
    switch (numStr.getIntValue()) {
    case 1:
        coarse        = p.addExtParam(id + "coarse",     nm + " Coarse",      "Coarse",    "", { 1.0, 24.0, 1.0, 1.0 }, 1.0, 0.0f);
        fine        = p.addExtParam(id + "fine", nm + " Fine", "Fine", "", osc1FineRange, 0.0, 0.0f);
        volume        = p.addExtParam(id + "volume",     nm + " Volume",      "Volume",    "", { 0.0, 1.0, 0.01f, 1.0 }, 0.5, 0.0f);
        phase = p.addExtParam(id + "phase", nm + " Phase", "Phase", "", { 0.0, 1.0, 0.0, 1.0 }, 0.15f, 0.0f);
        break;
    case 2:
        coarse        = p.addExtParam(id + "coarse", nm + " Coarse", "Coarse", "", { 1.0, 24.0, 1.0, 1.0 }, 2.0, 0.0f);
        fine        = p.addExtParam(id + "fine", nm + " Fine", "Fine", "", defaultFineRange, 0.0, 0.0f);
        volume        = p.addExtParam(id + "volume", nm + " Volume", "Volume", "", { 0.0, 1.0, 0.01f, 1.0 }, 0.5, 0.0f);
        phase = p.addExtParam(id + "phase", nm + " Phase", "Phase", "", { 0.0, 1.0, 0.0, 1.0 }, 0.3f, 0.0f);
        break;
    case 3:
        coarse        = p.addExtParam(id + "coarse", nm + " Coarse", "Coarse", "", { 1.0, 24.0, 1.0, 1.0 }, 3.0, 0.0f);
        fine        = p.addExtParam(id + "fine", nm + " Fine", "Fine", "", defaultFineRange, 0.0, 0.0f);
        volume        = p.addExtParam(id + "volume", nm + " Volume", "Volume", "", { 0.0, 1.0, 0.01f, 1.0 }, 0.35f, 0.0f);
        phase = p.addExtParam(id + "phase", nm + " Phase", "Phase", "", { 0.0, 1.0, 0.0, 1.0 }, 0.65f, 0.0f);
        break;
    case 4:
        coarse        = p.addExtParam(id + "coarse", nm + " Coarse", "Coarse", "", { 1.0, 24.0, 1.0, 1.0 }, 4.0, 0.0f);
        fine        = p.addExtParam(id + "fine", nm + " Fine", "Fine", "", defaultFineRange, 0.0, 0.0f);
        volume        = p.addExtParam(id + "volume", nm + " Volume", "Volume", "", { 0.0, 1.0, 0.01f, 1.0 }, 0.2f, 0.0f);
        phase = p.addExtParam(id + "phase", nm + " Phase", " Phase", "", { 0.0, 1.0, 0.0, 1.0 }, 0.85f, 0.0f);
        break;
    }
    
    tones     = p.addExtParam(id + "tones",      nm + " Tones",       "Tones",     "", { 1.0, 5.9f, 0.001f, 1.0 }, 1.0, 0.0f);
    detune    = p.addExtParam(id + "detune",     nm + " Detune",      "Detune",    "", { 0.0, 0.5, 0.0f, 1.0 }, 0.0, 0.0f);
    spread    = p.addExtParam(id + "spread",     nm + " Spread",      "Spread",    "%", { 0.0, 100.0, 0.0f, 1.0 }, 0.0, 0.0f);
    pan       = p.addExtParam(id + "pan",        nm + " Pan",         "Pan",       "", { -1.0, 1.0, 0.0f, 1.0 }, 0.0, 0.0f);
    env       = p.addExtParam(id + "env",        nm + " Env",         "Env",       "", { 0.0, 3.0, 1.0f, 1.0 }, (float)(numStr.getIntValue() - 1), 0.0f, envSelectTextFunction);
    saw       = p.addExtParam(id + "saw",        nm + " Saw",         "Saw",       "", { 0.0, 1.0, 1.0f, 1.0 }, 0.0, 0.0f, enableTextFunction);
    fixed     = p.addExtParam(id + "fixed",      nm + " Fixed",       "Fixed",     "", { 0.0, 1.0, 1.0f, 1.0 }, 0.0, 0.0f, enableTextFunction);
    this->num = numStr.getIntValue();
}

//==============================================================================
void APAudioProcessor::FilterParams::setup(APAudioProcessor& p)
{
    juce::String id = "flt";
    juce::String nm = "Filter";

    float maxFreq = float(gin::getMidiNoteFromHertz(20000.0));

    enable           = p.addIntParam(id + "enable",  nm + " Enable",  "",      "", { 0.0, 1.0, 1.0f, 1.0 }, 1.0f, 0.0f);
    type             = p.addExtParam(id + "type",    nm + " Type",    "Type",  "", { 0.0, 7.0, 1.0f, 1.0 }, 0.0, 0.0f, filterTextFunction);
    keyTracking      = p.addExtParam(id + "key",     nm + " Key",     "Key",   "%", { 0.0, 100.0, 0.0f, 1.0 }, 0.0, 0.0f);
    frequency        = p.addExtParam(id + "freq",    nm + " Freq",    "Freq",  " Hz", { 0.0, maxFreq, 0.0f, 1.0 }, 95.0, 0.0f, freqTextFunction);
    resonance        = p.addExtParam(id + "res",     nm + " Res",     "Res",   "", { 0.0, 100.0, 0.0f, 1.0 }, 0.0, 0.0f);

    keyTracking->conversionFunction      = [] (float in) { return in / 100.0f; };
}

//==============================================================================
void APAudioProcessor::LFOParams::setup(APAudioProcessor& p, String numStr)
{
    // we've got 4 in the processor as mono sources, and four for each voice as poly sources / dests
    juce::String id = "lfo" + numStr;
    juce::String nm = "LFO" + numStr;

    auto& notes = gin::NoteDuration::getNoteDurations();

    enable = p.addIntParam(id + "enable", id + "Enable", "Enable", "", { 0.0, 1.0, 1.0, 1.0 }, 0.0f, 0.0f, enableTextFunction);

    sync             = p.addExtParam(id + "sync",    nm + " Sync",    "Sync",   "", { 0.0, 1.0, 1.0, 1.0 }, 1.0, 0.0f, enableTextFunction);
    wave             = p.addExtParam(id + "wave",    nm + " Wave",    "Wave",   "", { 1.0, 17.0, 1.0, 1.0 }, 1.0, 0.0f, lfoTextFunction);
    rate             = p.addExtParam(id + "rate",    nm + " Rate",    "Rate",   " Hz", { 0.0, 50.0, 0.0, 0.3f }, 10.0, 0.0f);
    beat             = p.addExtParam(id + "beat",    nm + " Beat",    "Beat",   "", { 0.0, float (notes.size() - 1), 1.0, 1.0 }, 13.0, 0.0f, durationTextFunction);						   
    depth            = p.addExtParam(id + "depth",   nm + " Depth",   "Depth",  "", { -1.0, 1.0, 0.0, 1.0 }, 1.0, 0.0f);
    phase            = p.addExtParam(id + "phase",   nm + " Phase",   "Phase",  "", { -1.0, 1.0, 0.0, 1.0 }, 0.0, 0.0f);
    offset           = p.addExtParam(id + "offset",  nm + " Offset",  "Offset", "", { -1.0, 1.0, 0.0, 1.0 }, 0.0, 0.0f);
    fade             = p.addExtParam(id + "fade",    nm + " Fade",    "Fade",   " s", { -60.0, 60.0, 0.0, 0.2f, true }, 0.0f, 0.0f);
    delay            = p.addExtParam(id + "delay",   nm + " Delay",   "Delay",  " s", { 0.0, 60.0, 0.0, 0.2f }, 0.0f, 0.0f);

    this->num = numStr.getIntValue();
}
//==============================================================================

void APAudioProcessor::MSEGParams::setup(APAudioProcessor& p, juce::String number)
{
    sync = p.addExtParam("mseg" + number + "sync", "MSEG" + number + " Sync", "Sync", "", { 0.0, 1.0, 1.0, 1.0 }, 1.0, 0.0f, enableTextFunction);
    rate = p.addExtParam("mseg" + number + "rate", "MSEG" + number + " Rate", "Rate", " Hz", { 0.0, 50.0, 0.0, 0.3f }, 10.0, 0.0f);
    beat = p.addExtParam("mseg" + number + "beat", "MSEG" + number + " Beat", "Beat", "", { 0.0, float(gin::NoteDuration::getNoteDurations().size() - 1), 1.0, 1.0 }, 13.0, 0.0f, durationTextFunction);
    depth = p.addExtParam("mseg" + number + "depth", "MSEG" + number + " Depth", "Depth", "", { -1.0, 1.0, 0.0, 1.0 }, 1.0, 0.0f);
    phase = p.addExtParam("mseg" + number + "phase", "MSEG" + number + " Phase", "Phase", "", { -1.0, 1.0, 0.0, 1.0 }, 0.5f, 0.0f);
    offset = p.addExtParam("mseg" + number + "offset", "MSEG" + number + " Offset", "Offset", "", { -1.0, 1.0, 0.0, 1.0 }, 0.0, 0.0f);
    xgrid = p.addExtParam("mseg" + number + "xgrid", "MSEG" + number + "XGrid", "X Grid", "", { 1.0, 20.0, 0.0, 1.0 }, 10.0, 0.0f, gridTextFunction);
    ygrid = p.addExtParam("mseg" + number + "ygrid", "MSEG" + number + "YGrid", "Y Grid", "", { 1.0, 20.0, 0.0, 1.0 }, 8.0, 0.0f, gridTextFunction);
    loop = p.addExtParam("mseg" + number + "loop", "MSEG" + number + "Loop", "Loop", "", { 0.0, 1.0, 0.0, 1.0 }, 0.0, 0.0f, enableTextFunction);
    enable = p.addIntParam("mseg" + number + "enable", "MSEG" + number + "Enable", "Enable", "", { 0.0, 1.0, 0.0, 1.0 }, 1.0, 0.0f, enableTextFunction);
    draw = p.addIntParam("mseg" + number + "draw", "MSEG" + number + "Draw", "Draw", "", { 0.0, 1.0, 0.0, 1.0 }, 0.0, 0.0f, enableTextFunction);
    drawmode = p.addIntParam("mseg" + number + "drawmode", "MSEG" + number + "DrawMode", "Draw Mode", "", { 0.0, 4.0, 0.0, 1.0 }, 0.0, 0.0f, msegDrawModeTextFunction);
    this->num = number.getIntValue();
}

//==============================================================================
void APAudioProcessor::ENVParams::setup(APAudioProcessor& p, String numStr) //
{
    String id = "ENV" + numStr;
    auto& notes = gin::NoteDuration::getNoteDurations();

    
    attack = p.addExtParam(id + "attack", id + " Attack", "Attack", "", { 0.0, 60.0, 0.0, 0.2f }, 0.01f, 0.0f, secondsTextFunction);
    if (numStr.getIntValue() == 2) {
        decay = p.addExtParam(id + "decay", id + " Decay", "Decay", "", { 0.0, 60.0, 0.0, 0.2f }, 0.45f, 0.0f, secondsTextFunction);
    }
    else {
        decay = p.addExtParam(id + "decay", id + " Decay", "Decay", "", { 0.0, 60.0, 0.0, 0.2f }, 0.07f, 0.0f, secondsTextFunction);
    }
    sustain = p.addExtParam(id + "sustain", id + " Sustain", "Sustain", "%", { 0.0, 100.0, 0.0, 1.0 }, 50.0f, 0.0f);
    release = p.addExtParam(id + "release", id + " Release", "Release", "", { 0.0, 60.0, 0.0, 0.2f }, 0.1f, 0.0f, secondsTextFunction);
    acurve = p.addExtParam(id + "acurve", id + " ACurve", "At Curve", "", { -1.0, 1.0, 0.0, 1.0 }, 1.0f, 0.0f);
    drcurve = p.addExtParam(id + "drcurve", id + " DRCurve", "DR Curve", "", { -1.0, 1.0, 0.0, 1.0 }, -1.0f, 0.0f);
    syncrepeat = p.addExtParam(id + "syncrepeat", id + " SyncRepeat", "Repeat", "", { 0.0, 2.0, 1.0, 1.0 }, 0.0f, 0.0f, syncrepeatTextFunction);
    time = p.addExtParam(id + "time", id + " Time", "Time", "", { 0.0, 60.0, 0.0, 0.2f }, 0.1f, 0.0f, secondsTextFunction);
    duration = p.addExtParam(id + "beat", id + " Beat", "Beat", "", { 0.0, float(notes.size() - 1), 1.0, 1.0 }, 13.0, 0.0f, durationTextFunction);

    sustain->conversionFunction = [](float in) { return in / 100.0f; };
    this->num = numStr.getIntValue();
}


//==============================================================================
void APAudioProcessor::TimbreParams::setup(APAudioProcessor& p)
{
    equant = p.addExtParam("equant", "Equant", "", "", { -0.5, 0.5, 0.0, 1.0 }, 0.0, 0.0f);
    pitch = p.addExtParam("pitch", "Pitch", "", "", { 0.01f, 4.0, 0.0f, 1.0 }, 1.0, 0.0f);
    blend = p.addExtParam("blend", "Blend", "", "", { 0.0, 1.0, 0.0, 1.0 }, 0.0, 0.0f);
    demodmix = p.addExtParam("demodmix", "Demodulate", "", "", { 0.0, 1.0, 0.0, 1.0 }, 0.0, 0.0f, percentTextFunction);
    algo = p.addExtParam("algo", "Algorithm", "", "", {0.0, 3.0, 1.0, 1.0}, 0.0, 0.f, algoTextFunction);
    demodVol = p.addExtParam("demodVol", "Demod Vol", "", "", { 0.0f, 4.0f, 0.0f, 1.0f }, 2.0f, 0.0f);
}

//==============================================================================
void APAudioProcessor::GlobalParams::setup(APAudioProcessor& p)
{
    velSens        = p.addExtParam("velSens", "Vel. Sens.",   "",      "%",   { 0.0, 100.0, 0.0, 1.0 }, 100.0, 0.0f);
    squash         = p.addExtParam("squash", "Squash", "", "", {0.0f, 1.0f, 0.0f, 1.0f}, 0.0f, 0.0f);
    mono           = p.addIntParam("mono",    "Mono",       "",      "",   { 0.0, 1.0, 0.0, 1.0 }, 0.0, 0.0f, enableTextFunction);
    glideMode      = p.addIntParam("gMode",   "Glide Mode", "Glide", "",   { 0.0, 2.0, 0.0, 1.0 }, 0.0f, 0.0f, glideModeTextFunction);
    glideRate      = p.addExtParam("gRate",   "Glide Rate", "Rate",  " s",   { 0.001f, 20.0, 0.0, 0.2f }, 0.3f, 0.0f);
    legato         = p.addIntParam("legato",  "Legato",     "",      "",   { 0.0, 1.0, 0.0, 1.0 }, 0.0, 0.0f, enableTextFunction);
    level          = p.addExtParam("level",   "Level",      "",      " dB", { -100.0, 12.0, 1.0, 4.0f }, 0.0, 0.0f);
    voices         = p.addIntParam("voices",  "Voices",     "",      "",   { 2.0, 8.0, 1.0, 1.0 }, 8.0f, 0.0f);
    mpe            = p.addIntParam("mpe",     "MPE",        "",      "",   { 0.0, 1.0, 1.0, 1.0 }, 0.0f, 0.0f, enableTextFunction);
    pitchbendRange = p.addIntParam("pbrange", "PB Range", "", "", {0.0, 96.0, 1.0, 1.0}, 2.0, 0.0f);
    sidechainEnable = p.addIntParam("sidechain", "Sidechain", "", "", { 0.0, 1.0, 1.0, 1.0 }, 0.0f, 0.0f, enableTextFunction);

    level->conversionFunction     = [](float in) { return juce::Decibels::decibelsToGain (in); };
    velSens->conversionFunction   = [](float in) { return in / 100.0f; };
}

//==============================================================================
void APAudioProcessor::OrbitParams::setup(APAudioProcessor& p)
{
    speed = p.addIntParam("speed", "Speed", "", "", { 0.0, 0.4f, 0.0f, 1.0 }, 0.03f, 0.0f);
    scale = p.addIntParam("scale", "Scale", "", "", { 1.0, 6.0, 0.0, 1.0 }, 1.0f, 0.0f);
}


//==============================================================================
void APAudioProcessor::WaveshaperParams::setup(APAudioProcessor& p)
{
    String pfx = "ws";
    String name = "WS ";
    drive = p.addExtParam(pfx + "drive", name + "Drive", "Drive", "", { 0.0, 60.0, 0.0, 1.0 }, 0.0, 0.0f);
    gain = p.addExtParam(pfx + "gain",  name + "Gain", "Gain", "", { -12.0f, 12.0f, 0.0f, 1.0 }, 0.0, 0.0f);
    dry = p.addExtParam(pfx + "dry",   name + "Dry", "Dry", "", { 0.0, 1.0, 0.0, 1.0 }, 1.0, 0.0f, percentTextFunction);
    wet = p.addExtParam(pfx + "wet",   name + "Wet", "Wet", "", { 0.0, 1.0, 0.0, 1.0 }, 0.25, 0.0f, percentTextFunction);
    type = p.addExtParam(pfx + "func",  name + "Function", "Function", "", { 0.0, 16.0, 1.0, 1.0 }, 0.0f, 0.0f, waveshaperTypeTextFunction);
    highshelf = p.addExtParam(pfx + "highshelf", name + "High Shelf", "High Shelf", " Hz", { 3000.0f, 12000.0f, 0.0, 1.3f }, 6500.0f, 0.0f);
    hsq = p.addExtParam(pfx + "hsq", name + "HShelf Q", "High Shelf Q", "", { 0.5f, 5.0f, 0.0, 1.0 }, 1.0f, 0.0f);
    lp = p.addExtParam(pfx + "lp", name + "Low Pass", "Low Pass", "", { 20.0f, 20000.0f, 0.0, 0.3f }, 20000.0f, 0.0f);
}

//==============================================================================
void APAudioProcessor::CompressorParams::setup(APAudioProcessor& p)
{
    String pfx = "cp";
    String name = "Comp ";
    threshold = p.addExtParam(pfx + "threshold", name + "Threshold", "Threshold", " dB", { -60.0, 0.0, 0.0, 1.0 }, -12.0f, 0.0f);
    ratio = p.addExtParam(pfx + "ratio",     name + "Ratio", "Ratio", "x", { 1.0, 20.0, 0.0, 1.0 }, 2.0f, 0.0f);
    attack = p.addExtParam(pfx + "attack",    name + "Attack", "Attack", "", { 0.00002f, 0.1f, 0.00001f, 0.3f }, 0.0005f, 0.0f, compressorAttackTextFunction);
    release = p.addExtParam(pfx + "release",   name + "Release", "Release", "", { 0.05f, 1.0, 0.001f, 1.0 }, 0.1f, 0.0f, millisecondsTextFunction);
    knee = p.addExtParam(pfx + "knee",      name + "Knee", "Knee", " dB", { 0.0, 20.0, 0.01f, 1.0 }, 0.0f, 0.0f);
    input = p.addExtParam(pfx + "input",     name + "Input", "Input", "", { 0.0, 5.0, 0.0, 1.0 }, 1.0f, 0.0f);
    output = p.addExtParam(pfx + "output",    name + "Output", "Output", "", { 0.0, 5.0, 0.0, 1.0 }, 1.0f, 0.0f);
    type = p.addExtParam(pfx + "type",      name + "Type", "Type", "", { 0.0, 3.0, 1.0, 1.0 }, 0.0f, 0.0f, compressorTypeTextFunction);
}

//==============================================================================
void APAudioProcessor::StereoDelayParams::setup(APAudioProcessor& p)
{
    String name = "Delay ";
    String pfx = "dl";
    auto& notes = gin::NoteDuration::getNoteDurations();
    timeleft = p.addExtParam(pfx + "timeleft",   name + "Time Left", "Time L", "", { 0.001f, 10.0, 0.0, 0.5 }, 0.5f, 0.0f, secondsTextFunction);
    timeright = p.addExtParam(pfx + "timeright",  name + "Time Right", "Time R", "", { 0.001f, 10.0, 0.0, 0.5 }, 0.5f, 0.0f, secondsTextFunction);
    beatsleft = p.addExtParam(pfx + "beatsleft",  name + "Beats Left", "Beats L", "", { 0.0, float(notes.size() - 1), 1.0, 1.0 }, 13.0, 0.0f, durationTextFunction);
    beatsright = p.addExtParam(pfx + "beatsright", name + "Beats Right", "Beats R", "", { 0.0, float(notes.size() - 1), 1.0, 1.0 }, 13.0, 0.0f, durationTextFunction);
    temposync = p.addIntParam(pfx + "temposync",  name + "Tempo Sync", "Tempo Sync", "", { 0.0, 1.0, 1.0, 1.0 }, 1.0f, 0.0f, enableTextFunction);
    freeze = p.addIntParam(pfx + "freeze",     name + "Freeze", "Freeze", "", { 0.0, 1.0, 1.0, 1.0 }, 0.0f, 0.0f, enableTextFunction);
    pingpong = p.addIntParam(pfx + "pingpong",   name + "Ping Pong", "Ping Pong", "", { 0.0, 1.0, 1.0, 1.0 }, 0.0f, 0.0f, enableTextFunction);
    feedback = p.addExtParam(pfx + "feedback",   name + "Feedback", "Feedback", "", { 0.0, 1.0, 0.0, 1.0 }, 0.5f, 0.0f, percentTextFunction);
    wet = p.addExtParam(pfx + "wet",        name + "Wet", "Wet", "", { 0.0, 1.0, 0.0, 1.0 }, 0.25, 0.0f, percentTextFunction);
    dry = p.addExtParam(pfx + "dry",        name + "Dry", "Dry", "", { 0.0, 1.0, 0.0, 1.0 }, 1.0f, 0.0f, percentTextFunction);
    cutoff = p.addExtParam(pfx + "cutoff",     name + "Cutoff", "LP Cutoff", " Hz", { 20.0f, 20000.0f, 0.0, 0.3f }, 10000.0f, 0.0f);
}

//==============================================================================
void APAudioProcessor::ChorusParams::setup(APAudioProcessor& p)
{
    String name = "Chorus ";
    String pfx = "ch";
    rate = p.addExtParam(pfx + "rate",     name + "Rate", "Rate", " Hz", { 0.005f, 20.0f, 0.0f, 0.3f }, 0.05f, 0.0f);
    depth = p.addExtParam(pfx + "depth",    name + "Depth", "Depth", "", { 0.0, 1.0, 0.0, 1.0 }, 0.5f, 0.0f, percentTextFunction);
    delay = p.addExtParam(pfx + "delay",    name + "Delay", "Delay", " ms", { 10.0f, 40.0f, 0.0, 1.0 }, 20.0f, 0.0f);
    feedback = p.addExtParam(pfx + "feedback", name + "Feedback", "Feedback", "", { 0.0, 1.0, 0.0, 1.0 }, 0.25f, 0.0f, percentTextFunction);
    dry = p.addExtParam(pfx + "dry",      name + "Dry", "Dry", "", { 0.0, 1.0, 0.0, 1.0 }, 1.0f, 0.0f, percentTextFunction);
    wet = p.addExtParam(pfx + "wet",      name + "Wet", "Wet", "", { 0.0, 1.0, 0.0, 1.0 }, 0.25f, 0.0f, percentTextFunction);

}

//==============================================================================
void APAudioProcessor::ReverbParams::setup(APAudioProcessor& p)
{
    String pfx = "rv";
    String name = "Reverb ";
    size = p.addExtParam(pfx + "size",     name + "Size", "Size", "", { 0.0, 2.0, 0.0, 1.0 }, 1.f, 0.0f);
    decay = p.addExtParam(pfx + "decay",    name + "Decay", "Decay", "", { 0.0, 1.0, 0.0, 1.0 }, 0.5f, 0.0f);
    damping = p.addExtParam(pfx + "damping",  name + "Damping", "Damping", " Hz", { 20.0f, 20000.0f, 0.0, 0.3f }, 10000.0f, 0.0f);
    lowpass = p.addExtParam(pfx + "lowpass",  name + "Lowpass", "Lowpass", " Hz", { 20.0f, 20000.0f, 0.0, 0.3f }, 20000.0f, 0.0f);
    predelay = p.addExtParam(pfx + "predelay", name +  "Predelay", "Predelay", "", { 0.0, 0.1f, 0.0, 1.0 }, 0.002f, 0.0f, secondsTextFunction);
    dry = p.addExtParam(pfx + "dry",      name +  "Dry", "Dry", "", { 0.0, 1.0, 0.0, 1.0 }, 1.0f, 0.0f, percentTextFunction);
    wet = p.addExtParam(pfx + "wet",      name +  "Wet", "Wet", "", { 0.0, 1.0, 0.0, 1.0 }, 0.08f, 0.0f, percentTextFunction);
}

//==============================================================================
void APAudioProcessor::MBFilterParams::setup(APAudioProcessor& p)
{
    String pfx = "mb";
    String name = "MB Filter ";
    lowshelffreq = p.addExtParam (pfx + "lowshelffreq",  name + "LS Freq", "LS Freq", " Hz", { 20.0, 20000.0, 1.0, 0.3f }, 20.0f, 0.0f);
    lowshelfgain = p.addExtParam (pfx + "lowshelfgain",  name + "LS Gain", "LS Gain", "", { 0.01f, 6.0, 0.01f, 1.0 }, 1.0f, 0.0f);
    lowshelfq = p.addExtParam    (pfx + "lowshelfq",     name + "LS Q", "LS Q", "", { 0.1f, 20.0, 0.0, 1.0 }, 1.0f, 0.0f);
    peakfreq = p.addExtParam     (pfx + "peakfreq",      name + "Peak Freq", "Peak Freq", " Hz", { 20.0, 20000.0, 1.0, 0.3f }, 1000.0f, 0.0f);
    peakgain = p.addExtParam     (pfx + "peakgain",      name + "Peak Gain", "Peak Gain", "", { 0.01f, 6.0, 0.0, 1.0 }, 1.0f, 0.0f);
    peakq = p.addExtParam        (pfx + "peakq",         name + "Peak Q", "Peak Q", "", { 0.1f, 20.0, 0.0, 1.0 }, 1.0f, 0.0f);
    highshelffreq = p.addExtParam(pfx + "highshelffreq", name + "HS Freq", "HS Freq", " Hz", { 20.0, 20000.0, 1.0, 0.3f }, 20000.0f, 0.0f);
    highshelfgain = p.addExtParam(pfx + "highshelfgain", name + "HS Gain", "HS Gain", "", { 0.01f, 6.0, 0.0, 1.0 }, 1.0f, 0.0f);
    highshelfq = p.addExtParam   (pfx + "highshelfq",    name + "HS Q", "HS Q", "", { 0.1f, 20.0, 0.0, 1.0 }, 1.0f, 0.0f);
}

//==============================================================================
void APAudioProcessor::RingModParams::setup(APAudioProcessor& p)
{
    String pfx = "rm";
    String name = "RMod ";
    modfreq1 = p.addExtParam(pfx + "modfreq1", name + "Freq 1", "Mod Freq 1", " Hz", { 1.0, 12000.0, 0.0, 0.3f }, 40.0f, 0.0f);
    shape1 = p.addExtParam  (pfx + "shape1",   name + "Shape 1", "Shape 1", "", { 0.0, 1.0, 0.0, 1.0 }, 0.0f, 0.0f, percentTextFunction);
    mix1 = p.addExtParam    (pfx + "mix1",     name + "Mix 1", "Mix 1", "", { 0.0, 1.0, 0.0, 1.0 }, 0.0f, 0.0f, percentTextFunction);
    modfreq2 = p.addExtParam(pfx + "modfreq2", name + "Freq 2", "Mod Freq 2", " Hz", { 1.0, 12000.0, 0.0, 0.3f }, 40.0f, 0.0f);
    shape2 = p.addExtParam  (pfx + "shape2",   name + "Shape 2", "Shape 2", "", { 0.0, 1.0, 0.0, 1.0 }, 0.0f, 0.0f, percentTextFunction);
    mix2 = p.addExtParam    (pfx + "mix2",     name + "Mix 2", "Mix 2", "", { 0.0, 1.0, 0.0, 1.0 }, 0.0f, 0.0f, percentTextFunction);
    spread = p.addExtParam  (pfx + "spread",   name + "Spread", "Spread", "", { 0.0, 1.0, 0.0, 1.0 }, 0.03f, 0.0f, percentTextFunction);
    lowcut = p.addExtParam  (pfx + "lowcut",   name + "Low Cut", "Low Cut", " Hz", { 20.0, 20000.0, 0.0, 0.3f }, 20.0f, 0.0f);
    highcut = p.addExtParam (pfx + "highcut",  name + "High Cut", "High Cut", " Hz", { 20.0, 20000.0, 0.0, 0.3f }, 20000.0f, 0.0f);
}

//==============================================================================
void APAudioProcessor::GainParams::setup(APAudioProcessor& p)
{
    gain = p.addExtParam("fxgain", "FX Gain", "Gain", " dB", { -60.0, 40.0, 0.0, 1.0 }, 0.0f, 0.0f);
}

static juce::String fxRouteFunction(const gin::Parameter&, float v)
{
    return v < 0.5f ? "A || B" : "A -> B";
}

static juce::String fxPrePostFunction(const gin::Parameter&, float v)
{
    return v < 0.5f ? "Pre" : "Post";
}


//==============================================================================
void APAudioProcessor::FXOrderParams::setup(APAudioProcessor& p)
{
    float maxFreq = float(gin::getMidiNoteFromHertz(20000.0));
    fxa1 = p.addIntParam("fxa1", "FX A1", "", "", {0.0, 8.0, 1.0, 1.0}, 0.0f, 0.0f, fxListTextFunction);
    fxa2 = p.addIntParam("fxa2", "FX A2", "", "", {0.0, 8.0, 1.0, 1.0}, 0.0f, 0.0f, fxListTextFunction);
    fxa3 = p.addIntParam("fxa3", "FX A3", "", "", {0.0, 8.0, 1.0, 1.0}, 0.0f, 0.0f, fxListTextFunction);
    fxa4 = p.addIntParam("fxa4", "FX A4", "", "", {0.0, 8.0, 1.0, 1.0}, 0.0f, 0.0f, fxListTextFunction);
    fxb1 = p.addIntParam("fxb1", "FX B1", "", "", {0.0, 8.0, 1.0, 1.0}, 0.0f, 0.0f, fxListTextFunction);
    fxb2 = p.addIntParam("fxb2", "FX B2", "", "", {0.0, 8.0, 1.0, 1.0}, 0.0f, 0.0f, fxListTextFunction);
    fxb3 = p.addIntParam("fxb3", "FX B3", "", "", {0.0, 8.0, 1.0, 1.0}, 0.0f, 0.0f, fxListTextFunction);
    fxb4 = p.addIntParam("fxb4", "FX B4", "", "", {0.0, 8.0, 1.0, 1.0}, 0.0f, 0.0f, fxListTextFunction);
    chainAtoB = p.addIntParam("chainAtoB", "FX Chain Routing", "", "", { 0.0, 1.0, 1.0, 1.0 }, 1.0f, 0.0f, fxRouteFunction);
    laneAGain = p.addExtParam("laneAGain", "FX A Pre-Gain", "Gain", " dB", { -60.0, 40.0, 0.0, 1.0 }, 0.0f, 0.0f);
    laneBGain = p.addExtParam("laneBGain", "FX B Pre-Gain", "Gain", " dB", { -60.0, 40.0, 0.0, 1.0 }, 0.0f, 0.0f);
    laneAType = p.addExtParam("laneAType", "FX A Filter", "Filter Type", "", { 0.0, 7.0, 1.0, 1.0 }, 0.0f, 0.0f, filterTextFunction);
    laneBType = p.addExtParam("laneBType", "FX B Filter", "Filter Type", "", { 0.0, 7.0, 1.0, 1.0 }, 0.0f, 0.0f, filterTextFunction);
    laneAFreq = p.addExtParam("laneAFreq", "FX A Freq", "Freq", " Hz", { 0.0, maxFreq, 0.0f, 1.5f }, maxFreq, 0.0f, freqTextFunction);
    laneBFreq = p.addExtParam("laneBFreq", "FX B Freq", "Freq", " Hz", { 0.0, maxFreq, 0.0f, 1.5f }, maxFreq, 0.0f, freqTextFunction);
    laneARes = p.addExtParam("laneARes", "FX A Res", "Resonance", "", { 0.0, 100.0, 0.0f, 1.0 }, 0.0, 0.0f);
    laneBRes = p.addExtParam("laneBRes", "FX B Res", "Resonance", "", { 0.0, 100.0, 0.0f, 1.0 }, 0.0, 0.0f);
    laneAPan = p.addExtParam("laneAPan", "FX A Pan", "Pan", "", { -1.0, 1.0, 0.0f, 1.0 }, 0.0, 0.0f);
    laneBPan = p.addExtParam("laneBPan", "FX B Pan", "Pan", "", { -1.0, 1.0, 0.0f, 1.0 }, 0.0, 0.0f);
    laneAPrePost = p.addIntParam("laneAPrePost", "Pre/Post", "", "", { 0.0, 1.0, 1.0, 1.0 }, 0.0f, 0.0f, fxPrePostFunction);
    laneBPrePost = p.addIntParam("laneBPrePost", "Pre/Post", "", "", { 0.0, 1.0, 1.0, 1.0 }, 0.0f, 0.0f, fxPrePostFunction);
}

void APAudioProcessor::MacroParams::setup(APAudioProcessor& p)
{
    String name = "Macro ";
    macro1 = p.addExtParam(name + "1", name + "1", name + "1", "", { 0.0, 1.0, 0.0, 1.0 }, 0.0f, 0.0f, percentTextFunction);
    macro2 = p.addExtParam(name + "2", name + "2", name + "2", "", { 0.0, 1.0, 0.0, 1.0 }, 0.0f, 0.0f, percentTextFunction);
    macro3 = p.addExtParam(name + "3", name + "3", name + "3", "", { 0.0, 1.0, 0.0, 1.0 }, 0.0f, 0.0f, percentTextFunction);
    macro4 = p.addExtParam(name + "4", name + "4", name + "4", "", { 0.0, 1.0, 0.0, 1.0 }, 0.0f, 0.0f, percentTextFunction);
    learning = p.addIntParam("Learn", "Learn", "Learn", "", { 0.0, 4.0, 1.0, 1.0 }, 0.0f, 0.0f);
    macro1cc = p.addIntParam("Macro1CC", "Macro 1 CC", "CC", "", { -1.0, 127.0, 1.0, 1.0 }, -1.0f, 0.0f);
    macro2cc = p.addIntParam("Macro2CC", "Macro 2 CC", "CC", "", { -1.0, 127.0, 1.0, 1.0 }, -1.0f, 0.0f);
    macro3cc = p.addIntParam("Macro3CC", "Macro 3 CC", "CC", "", { -1.0, 127.0, 1.0, 1.0 }, -1.0f, 0.0f);
    macro4cc = p.addIntParam("Macro4CC", "Macro 4 CC", "CC", "", { -1.0, 127.0, 1.0, 1.0 }, -1.0f, 0.0f);
}


void APAudioProcessor::updatePitchbend() {
    synth.setLegacyModePitchbendRange(globalParams.pitchbendRange->getUserValueInt());
}

bool APAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
//    if (layouts.getMainInputChannelSet()  == juce::AudioChannelSet::disabled()
//     || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled())
//        return false;
 
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    
    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
}


//==============================================================================
APAudioProcessor::APAudioProcessor() : gin::Processor(
    BusesProperties()
    .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    .withInput("Sidechain", juce::AudioChannelSet::stereo(), true),
    false,
    getOptions()
), synth(APSynth(*this))
{
    osc1Params.setup(*this, String{ "1" });
    osc2Params.setup(*this, String{ "2" });
    osc3Params.setup(*this, String{ "3" });
    osc4Params.setup(*this, String{ "4" });
    
    env1Params.setup(*this, String{ "1" });
    env2Params.setup(*this, String{ "2" });
    env3Params.setup(*this, String{ "3" });
    env4Params.setup(*this, String{ "4" });
    
    lfo1Params.setup(*this, String{ "1" });
    lfo2Params.setup(*this, String{ "2" });
    lfo3Params.setup(*this, String{ "3" });
    lfo4Params.setup(*this, String{ "4" });

    timbreParams.setup(*this);
    filterParams.setup(*this);
	// mono params begin in the middle of this block
    globalParams.setup(*this); 
    orbitParams.setup(*this);

    gainParams.setup(*this);
    waveshaperParams.setup(*this);
    compressorParams.setup(*this);
    stereoDelayParams.setup(*this);
    chorusParams.setup(*this);
    reverbParams.setup(*this);
    mbfilterParams.setup(*this);
    ringmodParams.setup(*this);

    fxOrderParams.setup(*this);

    mseg1Params.setup(*this, String{ "1" });
    mseg2Params.setup(*this, String{ "2" });
    mseg3Params.setup(*this, String{ "3" });
    mseg4Params.setup(*this, String{ "4" });

    mseg1Data.reset();
    mseg2Data.reset();
    mseg3Data.reset();
    mseg4Data.reset();

    laneAFilter.reset();
    laneBFilter.reset();
    laneAFilter.setNumChannels(2);
    laneBFilter.setNumChannels(2);

    macroParams.setup(*this);

    client = MTS_RegisterClient();

    lf = std::make_unique<APLNF>();

    setupModMatrix();
    init();
}

APAudioProcessor::~APAudioProcessor()
{
    MTS_DeregisterClient(client);
}

//==============================================================================
void APAudioProcessor::setupModMatrix()
{
    modSrcPressure  = modMatrix.addPolyModSource("mpep", "MPE Pressure", false);
    modSrcTimbre    = modMatrix.addPolyModSource("mpet", "MPE Timbre", false);
    modSrcModwheel  = modMatrix.addMonoModSource("mw", "Mod Wheel", false);
    modSrcMonoPitchbend = modMatrix.addMonoModSource("pb", "Pitch Wheel", true);
    modPolyAT = modMatrix.addPolyModSource("polyAT", "Poly AT", false);
    
    modSrcNote      = modMatrix.addPolyModSource("note", "MIDI Note #", false);
    modSrcVelocity  = modMatrix.addPolyModSource("vel", "MIDI Velocity", false);

    modSrcMonoLFO1 = modMatrix.addMonoModSource("mlfo1", "Mono LFO1", true);
    modSrcMonoLFO2 = modMatrix.addMonoModSource("mlfo2", "Mono LFO2", true);
    modSrcMonoLFO3 = modMatrix.addMonoModSource("mlfo3", "Mono LFO3", true);
    modSrcMonoLFO4 = modMatrix.addMonoModSource("mlfo4", "Mono LFO4", true);

    modSrcLFO1 = modMatrix.addPolyModSource("lfo1", "Poly LFO1", true);
    modSrcLFO2 = modMatrix.addPolyModSource("lfo2", "Poly LFO2", true);
    modSrcLFO3 = modMatrix.addPolyModSource("lfo3", "Poly LFO3", true);
    modSrcLFO4 = modMatrix.addPolyModSource("lfo4", "Poly LFO4", true);

    modSrcEnv1 = modMatrix.addPolyModSource("env1", "ENV1", false);
    modSrcEnv2 = modMatrix.addPolyModSource("env2", "ENV2", false);
    modSrcEnv3 = modMatrix.addPolyModSource("env3", "ENV3", false);
    modSrcEnv4 = modMatrix.addPolyModSource("env4", "ENV4", false);

    modSrcMSEG1 = modMatrix.addPolyModSource("mseg1", "MSEG1", false);
    modSrcMSEG2 = modMatrix.addPolyModSource("mseg2", "MSEG2", false);
    modSrcMSEG3 = modMatrix.addPolyModSource("mseg3", "MSEG3", false);
    modSrcMSEG4 = modMatrix.addPolyModSource("mseg4", "MSEG4", false);

    macroSrc1 = modMatrix.addMonoModSource("macro1", "Macro 1", false);
    macroSrc2 = modMatrix.addMonoModSource("macro2", "Macro 2", false);
    macroSrc3 = modMatrix.addMonoModSource("macro3", "Macro 3", false);
    macroSrc4 = modMatrix.addMonoModSource("macro4", "Macro 4", false);
        
    auto firstMonoParam = globalParams.mono;
    bool polyParam = true;
    for (auto pp : getPluginParameters())
    {
        if (pp == firstMonoParam)
            polyParam = false;

        if (! pp->isInternal())
            modMatrix.addParameter(pp, polyParam);
    }

    modMatrix.build();
}

void APAudioProcessor::stateUpdated() // called when loading a preset
{
    modMatrix.stateUpdated(state);

    if (state.getOrCreateChildWithName("mseg1", nullptr).getNumChildren() > 0) {
        mseg1Data.fromValueTree(state.getChildWithName("mseg1"));
    }
    else {
        mseg1Data.reset();
    }

    if (state.getOrCreateChildWithName("mseg2", nullptr).getNumChildren() > 0) {
        mseg2Data.fromValueTree(state.getChildWithName("mseg2"));
    }
    else {
        mseg2Data.reset();
    }

    if (state.getOrCreateChildWithName("mseg3", nullptr).getNumChildren() > 0) {
        mseg3Data.fromValueTree(state.getChildWithName("mseg3"));
    }
    else {
        mseg3Data.reset();
    }

    if (state.getOrCreateChildWithName("mseg4", nullptr).getNumChildren() > 0) {
        mseg4Data.fromValueTree(state.getChildWithName("mseg4"));
    }
    else {
        mseg4Data.reset();
    }
}

void APAudioProcessor::updateState() // called when saving a preset
{
    modMatrix.updateState(state);
    
    state.getOrCreateChildWithName("mseg1", nullptr).removeAllChildren(nullptr);
    mseg1Data.toValueTree(state.getChildWithName("mseg1"));
    
    state.getOrCreateChildWithName("mseg2", nullptr).removeAllChildren(nullptr);
    mseg2Data.toValueTree(state.getChildWithName("mseg2"));

    state.getOrCreateChildWithName("mseg3", nullptr).removeAllChildren(nullptr);
    mseg3Data.toValueTree(state.getChildWithName("mseg3"));

    state.getOrCreateChildWithName("mseg4", nullptr).removeAllChildren(nullptr);
    mseg4Data.toValueTree(state.getChildWithName("mseg4"));
    
}

void APAudioProcessor::reset()
{
    Processor::reset();

    lfo1.reset();
    lfo2.reset();
    lfo3.reset();
    lfo4.reset();

    waveshaper.reset();
    compressor.reset();
    
    
}

void APAudioProcessor::prepareToPlay(double newSampleRate, int newSamplesPerBlock)
{
    Processor::prepareToPlay(newSampleRate, newSamplesPerBlock);
    juce::dsp::ProcessSpec spec{newSampleRate, (juce::uint32)newSamplesPerBlock, 2};

    //oversampler = std::make_unique<juce::dsp::Oversampling<float>>(
    //    2, // channels
    //    1, // 2^1 oversampling
    //    dsp::Oversampling<float>::FilterType::filterHalfBandFIREquiripple,
    //    true,
    //    false);

    //oversampler->reset();
    //oversampler->initProcessing(spec.maximumBlockSize);
    
    //synth.setCurrentPlaybackSampleRate(newSampleRate * 2.0);

    synth.setCurrentPlaybackSampleRate(newSampleRate); // rollback to 1x

    modMatrix.setSampleRate(newSampleRate);

    stereoDelay.prepare(spec);
    effectGain.prepare(spec);
    waveshaper.prepare(spec);
    compressor.setSampleRate(newSampleRate);
    compressor.setNumChannels(2);
    chorus.prepare(spec);
    reverb.prepare(spec);
    mbfilter.prepare(spec);
    ringmod.prepare(spec);
    limiter.prepare(spec);
    limiter.setThreshold(-0.3f);
    limiter.setRelease(0.05f);

    lfo1.setSampleRate(newSampleRate);
    lfo2.setSampleRate(newSampleRate);
    lfo3.setSampleRate(newSampleRate);
    lfo4.setSampleRate(newSampleRate);

    laneAFilter.setSampleRate(newSampleRate);
    laneBFilter.setSampleRate(newSampleRate);
    laneAFilterCutoff.reset(newSampleRate, 0.02f);
    laneBFilterCutoff.reset(newSampleRate, 0.02f);

    *dcFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(newSampleRate, 5.0f);
    dcFilter.prepare(spec);
}

void APAudioProcessor::releaseResources()
{
}

void APAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    auto numSamples = buffer.getNumSamples();

    if (MTS_HasMaster(client)) {
        scaleName = MTS_GetScaleName(client);
    }
    
    //downsampledBuffer.setSize(2, numSamples);
    //auto downsampledBlock = juce::dsp::AudioBlock<float>(downsampledBuffer);
    
    //juce::dsp::AudioBlock<float> block(buffer);
    //
    //juce::dsp::AudioBlock<float> oversampledBlock = oversampler->processSamplesUp(block);
    //
    //float* pointers[2];
    //pointers[0] = oversampledBlock.getChannelPointer(0);
    //pointers[1] = oversampledBlock.getChannelPointer(1);
    //juce::AudioBuffer<float> oversampledBuffer{  pointers, 2, buffer.getNumSamples() * 2 };
    //
    if (presetLoaded)
    {
        presetLoaded = false;
        synth.turnOffAllVoices(false);
    }

    synth.startBlock();
    synth.setMPE(globalParams.mpe->isOn());

    playhead = getPlayHead();

    int pos = 0;
    int todo = numSamples;

    sidechainBuffer.setSize(2, numSamples, false, false, true);
    sidechainBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
    sidechainBuffer.copyFrom(1, 0, buffer, 1, 0, buffer.getNumSamples()); // copy input

    buffer.clear(); // then clear it from output buffer

    synth.setMono(globalParams.mono->isOn());
    synth.setLegato(globalParams.legato->isOn());
    synth.setGlissando(globalParams.glideMode->getProcValue() == 1.0f);
    synth.setPortamento(globalParams.glideMode->getProcValue() == 2.0f);
    synth.setGlideRate(globalParams.glideRate->getProcValue());
    synth.setNumVoices(int (globalParams.voices->getProcValue()));

    while (todo > 0)
    {
        int thisBlock = std::min(todo, 32);

        updateParams(thisBlock);
        
        //synth.renderNextBlock(oversampledBuffer, midi, pos * 2, thisBlock * 2);
        
        sidechainSlice = gin::sliceBuffer(sidechainBuffer, pos, thisBlock);
        synth.renderNextBlock(buffer, midi, pos, thisBlock); // rollback

        //oversampler->processSamplesDown(downsampledBlock);
        //
        //buffer.copyFrom(0, pos, downsampledBuffer, 0, pos, thisBlock);
        //buffer.copyFrom(1, pos, downsampledBuffer, 1, pos, thisBlock);
        
        auto bufferSlice = gin::sliceBuffer(buffer, pos, thisBlock);
        applyEffects(bufferSlice);

        modMatrix.finishBlock(thisBlock);

        pos += thisBlock;
        todo -= thisBlock;
    }

    playhead = nullptr;

    levelTracker.trackBuffer(buffer);

    synth.endBlock(buffer.getNumSamples());
}

juce::Array<float> APAudioProcessor::getLiveFilterCutoff()
{
    return synth.getLiveFilterCutoff();
}

void APAudioProcessor::applyEffects(juce::AudioSampleBuffer& fxALaneBuffer)
{
    auto fxa1 = fxOrderParams.fxa1->getUserValueInt();
    auto fxa2 = fxOrderParams.fxa2->getUserValueInt();
    auto fxa3 = fxOrderParams.fxa3->getUserValueInt();
    auto fxa4 = fxOrderParams.fxa4->getUserValueInt();
    auto fxb1 = fxOrderParams.fxb1->getUserValueInt();
    auto fxb2 = fxOrderParams.fxb2->getUserValueInt();
    auto fxb3 = fxOrderParams.fxb3->getUserValueInt();
    auto fxb4 = fxOrderParams.fxb4->getUserValueInt();

    int numSamples = fxALaneBuffer.getNumSamples();
    float laneAQ = gin::Q / (1.0f - (modMatrix.getValue(fxOrderParams.laneARes) / 100.0f) * 0.99f);
    float laneBQ = gin::Q / (1.0f - (modMatrix.getValue(fxOrderParams.laneBRes) / 100.0f) * 0.99f);
    bool laneAPre = fxOrderParams.laneAPrePost->getProcValue() < 0.5f;
    bool laneBPre = fxOrderParams.laneBPrePost->getProcValue() < 0.5f;
    float laneAPan = modMatrix.getValue(fxOrderParams.laneAPan);
    float laneBPan = modMatrix.getValue(fxOrderParams.laneBPan);

    switch (int(fxOrderParams.laneAType->getProcValue()))
    {
    case 0:
        laneAFilter.setType(gin::Filter::lowpass);
        laneAFilter.setSlope(gin::Filter::db12);
        break;
    case 1:
        laneAFilter.setType(gin::Filter::lowpass);
        laneAFilter.setSlope(gin::Filter::db24);
        break;
    case 2:
        laneAFilter.setType(gin::Filter::highpass);
        laneAFilter.setSlope(gin::Filter::db12);
        break;
    case 3:
        laneAFilter.setType(gin::Filter::highpass);
        laneAFilter.setSlope(gin::Filter::db24);
        break;
    case 4:
        laneAFilter.setType(gin::Filter::bandpass);
        laneAFilter.setSlope(gin::Filter::db12);
        break;
    case 5:
        laneAFilter.setType(gin::Filter::bandpass);
        laneAFilter.setSlope(gin::Filter::db24);
        break;
    case 6:
        laneAFilter.setType(gin::Filter::notch);
        laneAFilter.setSlope(gin::Filter::db12);
        break;
    case 7:
        laneAFilter.setType(gin::Filter::notch);
        laneAFilter.setSlope(gin::Filter::db24);
        break;
    }

    float f = gin::getMidiNoteInHertz(modMatrix.getValue(fxOrderParams.laneAFreq));
    laneAFilterCutoff.setTargetValue(f);
    laneAFilter.setParams(laneAFilterCutoff.getCurrentValue(), laneAQ);
    laneAFilterCutoff.skip(numSamples);

    switch (int(fxOrderParams.laneBType->getProcValue()))
    {
    case 0:
        laneBFilter.setType(gin::Filter::lowpass);
        laneBFilter.setSlope(gin::Filter::db12);
        break;
    case 1:
        laneBFilter.setType(gin::Filter::lowpass);
        laneBFilter.setSlope(gin::Filter::db24);
        break;
    case 2:
        laneBFilter.setType(gin::Filter::highpass);
        laneBFilter.setSlope(gin::Filter::db12);
        break;
    case 3:
        laneBFilter.setType(gin::Filter::highpass);
        laneBFilter.setSlope(gin::Filter::db24);
        break;
    case 4:
        laneBFilter.setType(gin::Filter::bandpass);
        laneBFilter.setSlope(gin::Filter::db12);
        break;
    case 5:
        laneBFilter.setType(gin::Filter::bandpass);
        laneBFilter.setSlope(gin::Filter::db24);
        break;
    case 6:
        laneBFilter.setType(gin::Filter::notch);
        laneBFilter.setSlope(gin::Filter::db12);
        break;
    case 7:
        laneBFilter.setType(gin::Filter::notch);
        laneBFilter.setSlope(gin::Filter::db24);
        break;
    }
    
    f = gin::getMidiNoteInHertz(modMatrix.getValue(fxOrderParams.laneBFreq));
    laneBFilterCutoff.setTargetValue(f);
    laneBFilter.setParams(laneBFilterCutoff.getCurrentValue(), laneBQ);
    laneBFilterCutoff.skip(numSamples);

    // case 1: lane A feeds into lane B
    if (fxOrderParams.chainAtoB->isOn()) {
        auto outBlock = juce::dsp::AudioBlock<float>(fxALaneBuffer);
        auto outContext = juce::dsp::ProcessContextReplacing<float>(outBlock);

        if (laneAPre) {
            laneAFilter.process(fxALaneBuffer);
            float gain = juce::Decibels::decibelsToGain(fxOrderParams.laneAGain->getUserValue());
            fxALaneBuffer.applyGain(0, 0, numSamples, gain * std::min(1 - laneAPan, 1.0f));
            fxALaneBuffer.applyGain(1, 0, numSamples, gain * std::min(1 + laneAPan, 1.0f));
        }

        for (int fx : {fxa1, fxa2, fxa3, fxa4} )
        {
            switch (fx)
            {
            case 0:
                break;
            case 1:
                waveshaper.process(outContext);
                break;
            case 2:
                compressor.process(fxALaneBuffer);
                break;
            case 3:
                stereoDelay.process(outContext);
                break;
            case 4:
                chorus.process(outContext);
                break;
            case 5:
                mbfilter.process(outContext);
                break;
            case 6:
                reverb.process(outContext);
                break;
            case 7:
                ringmod.process(outContext);
                break;
            case 8:
                effectGain.process(outContext);
                break;
            default:
                break;
            }

        }

        if (!laneAPre) {
            laneAFilter.process(fxALaneBuffer);
            float gain = juce::Decibels::decibelsToGain(fxOrderParams.laneAGain->getUserValue());
            fxALaneBuffer.applyGain(0, 0, numSamples, gain * std::min(1 - laneAPan, 1.0f));
            fxALaneBuffer.applyGain(1, 0, numSamples, gain * std::min(1 + laneAPan, 1.0f));
        }

        if (laneBPre) {
            laneBFilter.process(fxALaneBuffer);
            float gain = juce::Decibels::decibelsToGain(fxOrderParams.laneBGain->getUserValue());
            fxALaneBuffer.applyGain(0, 0, numSamples, gain * std::min(1 - laneBPan, 1.0f));
            fxALaneBuffer.applyGain(1, 0, numSamples, gain * std::min(1 + laneBPan, 1.0f));
        }

        for (int fx : {fxb1, fxb2, fxb3, fxb4})
        {
            switch (fx)
            {
            case 0:
                break;
            case 1:
                waveshaper.process(outContext);
                break;
            case 2:
                compressor.process(fxALaneBuffer);
                break;
            case 3:
                stereoDelay.process(outContext);
                break;
            case 4:
                chorus.process(outContext);
                break;
            case 5:
                mbfilter.process(outContext);
                break;
            case 6:
                reverb.process(outContext);
                break;
            case 7:
                ringmod.process(outContext);
                break;
            case 8:
                effectGain.process(outContext);
                break;
            default:
                break;
            }
        }

        if (!laneBPre) {
            laneBFilter.process(fxALaneBuffer);
            float gain = juce::Decibels::decibelsToGain(fxOrderParams.laneBGain->getUserValue());
            fxALaneBuffer.applyGain(0, 0, numSamples, gain * std::min(1 - laneBPan, 1.0f));
            fxALaneBuffer.applyGain(1, 0, numSamples, gain * std::min(1 + laneBPan, 1.0f));
        }
    }
    // case 2: lanes A and B are run in parallel
    else {
        juce::AudioBuffer<float> fxBLaneBuffer;
        fxBLaneBuffer.makeCopyOf(fxALaneBuffer);

        if (laneAPre) {
            laneAFilter.process(fxALaneBuffer);
            float gain = juce::Decibels::decibelsToGain(fxOrderParams.laneAGain->getUserValue());
            fxALaneBuffer.applyGain(0, 0, numSamples, gain * 0.5f * std::min(1 - laneAPan, 1.0f));
            fxALaneBuffer.applyGain(1, 0, numSamples, gain * 0.5f * std::min(1 + laneAPan, 1.0f));
        }
        
        if (laneBPre) {
            laneBFilter.process(fxBLaneBuffer);
            float gain = juce::Decibels::decibelsToGain(fxOrderParams.laneBGain->getUserValue());
            fxBLaneBuffer.applyGain(0, 0, numSamples, gain * 0.5f * std::min(1 - laneBPan, 1.0f));
            fxBLaneBuffer.applyGain(1, 0, numSamples, gain * 0.5f * std::min(1 + laneBPan, 1.0f));
        }
        
        auto ABlock = juce::dsp::AudioBlock<float>(fxALaneBuffer);
        auto AContext = juce::dsp::ProcessContextReplacing<float>(ABlock);
        auto BBlock = juce::dsp::AudioBlock<float>(fxBLaneBuffer);
        auto BContext = juce::dsp::ProcessContextReplacing<float>(BBlock);
        for (int fx : {fxa1, fxa2, fxa3, fxa4})
        {
            switch (fx)
            {
            case 0:
                break;
            case 1:
                waveshaper.process(AContext);
                break;
            case 2:
                compressor.process(fxALaneBuffer);
                break;
            case 3:
                stereoDelay.process(AContext);
                break;
            case 4:
                chorus.process(AContext);
                break;
            case 5:
                mbfilter.process(AContext);
                break;
            case 6:
                reverb.process(AContext);
                break;
            case 7:
                ringmod.process(AContext);
                break;
            case 8:
                effectGain.process(AContext);
                break;
            default:
                break;
            }
        }
        for (int fx : { fxb1, fxb2, fxb3, fxb4 })
        {
            switch (fx)
            {
            case 0:
                break;
            case 1:
                waveshaper.process(BContext);
                break;
            case 2:
                compressor.process(fxBLaneBuffer);
                break;
            case 3:
                stereoDelay.process(BContext);
                break;
            case 4:
                chorus.process(BContext);
                break;
            case 5:
                mbfilter.process(BContext);
                break;
            case 6:
                reverb.process(BContext);
                break;
            case 7:
                ringmod.process(BContext);
                break;
            case 8:
                effectGain.process(BContext);
                break;
            default:
                break;
            }
        }

        if (!laneAPre) {
            laneAFilter.process(fxALaneBuffer);
            float gain = juce::Decibels::decibelsToGain(fxOrderParams.laneAGain->getUserValue());
            fxALaneBuffer.applyGain(0, 0, numSamples, gain * 0.5f * std::min(1 - laneAPan, 1.0f));
            fxALaneBuffer.applyGain(1, 0, numSamples, gain * 0.5f * std::min(1 + laneAPan, 1.0f));
        }

        if (!laneBPre) {
            laneBFilter.process(fxBLaneBuffer);
            float gain = juce::Decibels::decibelsToGain(fxOrderParams.laneBGain->getUserValue());
            fxBLaneBuffer.applyGain(0, 0, numSamples, gain * 0.5f * std::min(1 - laneBPan, 1.0f));
            fxBLaneBuffer.applyGain(1, 0, numSamples, gain * 0.5f * std::min(1 + laneBPan, 1.0f));
        }

        fxALaneBuffer.addFrom(0, 0, fxBLaneBuffer, 0, 0, numSamples);
        fxALaneBuffer.addFrom(1, 0, fxBLaneBuffer, 1, 0, numSamples);
    }

    outputGain.process(fxALaneBuffer);
    auto ABlock = juce::dsp::AudioBlock<float>(fxALaneBuffer);
    auto AContext = juce::dsp::ProcessContextReplacing<float>(ABlock);
    dcFilter.process(AContext);
    limiter.process(AContext);
}

gin::ProcessorOptions APAudioProcessor::getOptions()
{
    gin::ProcessorOptions options;
    options.pluginName = "Audible Planets";
    options.programmingCredits = { "Greg Recco" };
    options.url = "https://github.com/gregrecco67/AudiblePlanets";
    options.urlTitle = "Visit https://github.com/gregrecco67/AudiblePlanets";
    return options.withoutNewsChecker().withoutUpdateChecker();
}



void APAudioProcessor::updateParams(int newBlockSize)
{
    // Update Mono LFOs
    for(auto lfoparams : {&lfo1Params, &lfo2Params, &lfo3Params, &lfo4Params})
    {
        gin::LFO::Parameters classparams;
        auto& lfo = this->monoLFOs[lfoparams->num-1];
        float freq = 0;
        if (lfoparams->sync->getProcValue() > 0.0f)
            freq = 1.0f / gin::NoteDuration::getNoteDurations()[size_t(modMatrix.getValue(lfoparams->beat))].toSeconds(playhead);
        else
            freq = modMatrix.getValue(lfoparams->rate);

        classparams.waveShape = (gin::LFO::WaveShape) int(lfoparams->wave->getProcValue());
        classparams.frequency = freq;
        classparams.phase = modMatrix.getValue(lfoparams->phase);
        classparams.offset = modMatrix.getValue(lfoparams->offset);
        classparams.depth = modMatrix.getValue(lfoparams->depth);
        classparams.delay = 0;
        classparams.fade = 0;

        lfo->setParameters(classparams);
        lfo->process(newBlockSize);

        modMatrix.setMonoValue(*(this->lfoIds[lfoparams->num - 1]), lfo->getOutput());
    }

    modMatrix.setMonoValue(macroSrc1, modMatrix.getValue(macroParams.macro1));
    modMatrix.setMonoValue(macroSrc2, modMatrix.getValue(macroParams.macro2));
    modMatrix.setMonoValue(macroSrc3, modMatrix.getValue(macroParams.macro3));
    modMatrix.setMonoValue(macroSrc4, modMatrix.getValue(macroParams.macro4));

    effectGain.setGainLevel(modMatrix.getValue(gainParams.gain));

    waveshaper.setGain(modMatrix.getValue(waveshaperParams.drive), modMatrix.getValue(waveshaperParams.gain));
    waveshaper.setDry(modMatrix.getValue(waveshaperParams.dry));
    waveshaper.setWet(modMatrix.getValue(waveshaperParams.wet));
    waveshaper.setFunctionToUse(int(modMatrix.getValue(waveshaperParams.type)));
    waveshaper.setHighShelfFreqAndQ(modMatrix.getValue(waveshaperParams.highshelf), modMatrix.getValue(waveshaperParams.hsq));
    waveshaper.setLPCutoff(modMatrix.getValue(waveshaperParams.lp));

    compressor.setParams(
        modMatrix.getValue(compressorParams.attack),
        0.0f,
        modMatrix.getValue(compressorParams.release),
        modMatrix.getValue(compressorParams.threshold),
        modMatrix.getValue(compressorParams.ratio),
        modMatrix.getValue(compressorParams.knee)
    );
    compressor.setInputGain(modMatrix.getValue(compressorParams.input));
    compressor.setOutputGain(modMatrix.getValue(compressorParams.output));
    compressor.setMode((gin::Dynamics::Type)(int)modMatrix.getValue(compressorParams.type));

    auto& notes = gin::NoteDuration::getNoteDurations();

    bool tempoSync = stereoDelayParams.temposync->getProcValue() > 0.0f;
    if (!tempoSync) {
        stereoDelay.setTimeL(modMatrix.getValue(stereoDelayParams.timeleft));
        stereoDelay.setTimeR(modMatrix.getValue(stereoDelayParams.timeright));
    }
    else {
        stereoDelay.setTimeL(notes[size_t(modMatrix.getValue(stereoDelayParams.beatsleft))].toSeconds(playhead));
        stereoDelay.setTimeR(notes[size_t(modMatrix.getValue(stereoDelayParams.beatsright))].toSeconds(playhead));
    }
    stereoDelay.setFB(modMatrix.getValue(stereoDelayParams.feedback));
    stereoDelay.setWet(modMatrix.getValue(stereoDelayParams.wet));
    stereoDelay.setDry(modMatrix.getValue(stereoDelayParams.dry));
    stereoDelay.setFreeze(stereoDelayParams.freeze->getProcValue() > 0.0f);
    stereoDelay.setPing(stereoDelayParams.pingpong->getProcValue() > 0.0f);
    stereoDelay.setCutoff(modMatrix.getValue(stereoDelayParams.cutoff));

    chorus.setRate(modMatrix.getValue(chorusParams.rate));
    chorus.setDepth(modMatrix.getValue(chorusParams.depth));
    chorus.setCentreDelay(modMatrix.getValue(chorusParams.delay));
    chorus.setFeedback(modMatrix.getValue(chorusParams.feedback));
    chorus.setWet(modMatrix.getValue(chorusParams.wet));
    chorus.setDry(modMatrix.getValue(chorusParams.dry));

    reverb.setSize(modMatrix.getValue(reverbParams.size));
    reverb.setDecay(modMatrix.getValue(reverbParams.decay));
    reverb.setDamping(modMatrix.getValue(reverbParams.damping));
    reverb.setLowpass(modMatrix.getValue(reverbParams.lowpass));
    reverb.setPredelay(modMatrix.getValue(reverbParams.predelay));
    reverb.setDry(modMatrix.getValue(reverbParams.dry));
    reverb.setWet(modMatrix.getValue(reverbParams.wet));

    mbfilter.setParams(
        modMatrix.getValue(mbfilterParams.lowshelffreq),
        modMatrix.getValue(mbfilterParams.lowshelfgain),
        modMatrix.getValue(mbfilterParams.lowshelfq),
        modMatrix.getValue(mbfilterParams.peakfreq),
        modMatrix.getValue(mbfilterParams.peakgain),
        modMatrix.getValue(mbfilterParams.peakq),
        modMatrix.getValue(mbfilterParams.highshelffreq),
        modMatrix.getValue(mbfilterParams.highshelfgain),
        modMatrix.getValue(mbfilterParams.highshelfq)
    );

    RingModulator::RingModParams rmparams;
    rmparams.mod1freq = modMatrix.getValue(ringmodParams.modfreq1);
    rmparams.shape1 = modMatrix.getValue(ringmodParams.shape1);
    rmparams.mix1 = modMatrix.getValue(ringmodParams.mix1);
    rmparams.mod2freq = modMatrix.getValue(ringmodParams.modfreq2);
    rmparams.shape2 = modMatrix.getValue(ringmodParams.shape2);
    rmparams.mix2 = modMatrix.getValue(ringmodParams.mix2);
    rmparams.spread = modMatrix.getValue(ringmodParams.spread);
    rmparams.lowcut = modMatrix.getValue(ringmodParams.lowcut);
    rmparams.highcut = modMatrix.getValue(ringmodParams.highcut);
    ringmod.setParams(rmparams);

    // Output gain
    outputGain.setGain(modMatrix.getValue(globalParams.level));
}

//==============================================================================
bool APAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* APAudioProcessor::createEditor()
{
    return new gin::ScaledPluginEditor(new APAudioProcessorEditor(*this), state);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new APAudioProcessor();
}
