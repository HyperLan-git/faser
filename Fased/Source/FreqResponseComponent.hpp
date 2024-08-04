#pragma once

#include <JuceHeader.h>
#include <complex>
#include <cmath>
#include "BiquadFilter.hpp"

constexpr int MAX_WIDTH = 400;
constexpr float MAX_FREQ = 22000;

class FreqResponseComponent : public juce::Component {
   public:
    FreqResponseComponent(const BiquadFilter& filter,
                          juce::AudioParameterInt* filterParam);

    void paint(juce::Graphics& g) override;

    void resized() override;

   private:
    const BiquadFilter& filter;
    juce::AudioParameterInt* filterParam;

    BiquadFilterCoefficients lastCoeffs = {};
    int lastFilters = 0;
    std::complex<float> values[MAX_WIDTH] = {};
};