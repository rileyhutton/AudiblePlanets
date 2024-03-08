#include <JuceHeader.h>
#include "FastMath.hpp"
#include <numbers>

using std::numbers::pi;

struct Matrix {
	friend Matrix operator*(const Matrix& m, const float s) { // scalar multiplication
		return { m.a * s, m.b * s, m.c * s, m.d * s };
	}
	float a, b, c, d;
};

struct StereoMatrix {
	Matrix left, right;
	friend StereoMatrix operator*(const StereoMatrix& m, const float s) { // scalar multiplication
			return { m.left * s, m.right * s };
		}
};

struct StereoPosition {
	float xL{ 0 }, yL{ 0 }, xR{ 0 }, yR{ 0 };
	
	friend StereoPosition operator*(const StereoPosition& p, const StereoMatrix& m) { // apply matrix to position
		return { .xL = m.left.a * p.xL + m.left.b * p.yL,
				.yL = m.left.c * p.xL + m.left.d * p.yL,
				.xR = m.right.a * p.xR + m.right.b * p.yR,
				.yR = m.right.c * p.xR + m.right.d * p.yR };
	}

	friend StereoPosition operator*(const StereoPosition& p, const float s) { // scalar multiplication
			return { p.xL * s, p.yL * s, p.xR * s, p.yR * s };
		}

	StereoPosition operator+(const StereoPosition otherPos) {
		return { this->xL + otherPos.xL, this->yL + otherPos.yL,
			this->xR + otherPos.xR, this->yR + otherPos.yR };
	}
};


enum class Wavetype
{
	sine = 0,
	sawUp = 1,
};

// utility for detune calculations
static inline float semitonePower(float x) { // about 2x faster than std::pow(SEMITONE, x) and accurate to ~ e-06
	float u = 4.6385981e-07f;
	u = u * x + -3.2122109e-05f;
	u = u * x + 0.0016682396f;
	u = u * x + -0.057762265f;
	return u * x + 1.f;
}

static inline float sineValueForPhaseAndTones(float phase_, float tones) {
	float fullTones{ 0.f }, value{ 0.0f };
	float partialToneFraction = std::modf(tones, &fullTones);

	value += FastMath<float>::minimaxSin(phase_);

	if (tones > 1.0f && tones < 2.0f)
		value += FastMath<float>::minimaxSin(phase_ * 2.0f) * partialToneFraction * 0.5f;
	else if (tones > 1.0f)
		value += FastMath<float>::minimaxSin(phase_ * 2.0f) * 0.5f; // we're over 2, so add the max level of this partial

	if (tones > 2.0f && tones < 3.0f)
		value += FastMath<float>::minimaxSin(phase_ * 3.0f) * partialToneFraction * 0.33f;
	else if (tones > 2.0f)
		value += FastMath<float>::minimaxSin(phase_ * 3.0f) * 0.33f;

	if (tones > 3.0f && tones < 4.0f)
		value += FastMath<float>::minimaxSin(phase_ * 4.0f) * partialToneFraction * 0.25f;
	else if (tones > 3.0f)
		value += FastMath<float>::minimaxSin(phase_ * 4.0f) * 0.25f;

	if (tones > 4.0f && tones < 5.0f)
		value += FastMath<float>::minimaxSin(phase_ * 5.0f) * partialToneFraction * 0.2f;
	else if (tones > 4.0f)
		value += FastMath<float>::minimaxSin(phase_ * 5.0f) * 0.2f;

	if (tones > 5.0f)
		value += FastMath<float>::minimaxSin(phase_ * 6.0f) * partialToneFraction * 0.16f;

	return value;
}

class QuadOscillator
{
public:
	QuadOscillator() = default;
	~QuadOscillator() = default;

	float freqs[4]{ 200.f }, phases[4]{ 0.f }, phaseIncs[4]{ 0.f }, gainsL[4]{ 0.f }, gainsR[4]{ 0.f };
	float freq, pan, tones, sampleRate;

	void setSampleRate(double sampleRate_)
	{
		sampleRate = (float)sampleRate_;
		recalculate();
	}

	struct Params {
		int voices = 4;
        Wavetype wave = Wavetype::sine;
		float tones{ 1.0 }, pan{ 0.f }, spread{ 0.f }, detune{ 0.f }, phaseShift{ 0.f };
	};

	Params params;

	void setParams(Params params_)
	{
		params = params_;
		recalculate();
	}

	void normalizePhases()
	{
		for (int i = 0; i < 4; i++) {
			if (phases[i] > pi) { phases[i] -= 2.f * pi; }
		}
	}

	void recalculate()
	{
		// calculate frequencies
		float baseFreq = freq * semitonePower(-params.detune); // 2x faster than std::pow(SEMITONE_INV, params.detune);
		float freqFactor = semitonePower(params.detune / (params.voices - 1)); // !!

		// CHANGE PP detuned and spread to [0, 1] range
		// pan is still [-1, 1]

		// base pan is for leftmost voice
		// if pan is 0, then max spread should put first voice at -1 and last voice at 1
		// if pan is -1, then max spread should put first voice at -1 and last voice at 0
		// if pan is 1, then max spread should put first voice at 0 and last voice at 1

		float basePan = params.pan - params.spread;
		float panDelta = 2.f * params.spread / (params.voices - 1);

		for (int i = 0; i < params.voices; i++)
		{
			float pan = juce::jlimit(-1.0f, 1.0f, basePan + panDelta * i);
			gainsL[i] = (1.0f - pan) / 2;
			gainsR[i] = (1.0f + pan) / 2;
			freqs[i] = baseFreq * std::pow(freqFactor, i);
		}

		// calculate phaseIncs
		for (int i = 0; i < 4; i++) {
			phaseIncs[i] = freqs[i] / sampleRate;
		}
	}

	void renderPositions(const float freq_, const Params params_, StereoPosition positions[], const int numSamples) {
		freq = freq_;
		params = params_;
		recalculate();
		for (int i = 0; i < numSamples; i++) {
			positions[i] = { 0.f, 0.f, 0.f, 0.f };
			for (int v = 0; i < 4; i++) {
				// do the per-voice stuff
				phases[i] += phaseIncs[i];
				if (phases[i] > pi) { phases[i] -= 2.f * pi; }
				positions[i].xL += gainsL[i] * sineValueForPhaseAndTones(phases[i] + 0.25f, params.tones);
				positions[i].yL += gainsL[i] * sineValueForPhaseAndTones(phases[i], params.tones);
				positions[i].xR += gainsR[i] * sineValueForPhaseAndTones(phases[i] + 0.25f, params.tones);
				positions[i].yR += gainsR[i] * sineValueForPhaseAndTones(phases[i], params.tones);
			}
		}
	}

	void noteOn(float initPhase)
	{
		if (initPhase != 0.f) return;
		for (int i = 0; i < 4; i++) {
			phases[i] = 0.f;
		}
	}

};
