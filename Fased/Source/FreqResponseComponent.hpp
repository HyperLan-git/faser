#pragma once

#include "JuceHeader.h"

#include <complex>
#include <optional>
#include <functional>
#include <cmath>

#include "BiquadFilter.hpp"

constexpr int MAX_WIDTH = 400;
constexpr float MAX_FREQ = 22000;

constexpr size_t FUNCTION_BUFFER = 400;
/**
 * Called when the user clicks or scrolls on the graph, which should change the
 * parameters of the filter.
 * In order, the arguments are frequency in Hz, gain in Db and change in Q value
 * dQ (should be added to original Q value).
 */
typedef std::function<void(std::optional<float>, std::optional<float>,
                           std::optional<float>)>
    ResponseCallback;

class FreqResponseComponent : public juce::Component {
   public:
    FreqResponseComponent(const BiquadFilter& filter,
                          juce::AudioParameterInt* filterParam,
                          ResponseCallback&& callback = {});

    ~FreqResponseComponent();

    void paint(juce::Graphics& g) override;

    void resized() override;

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event,
                        const juce::MouseWheelDetails& wheel) override;

   private:
    const BiquadFilter& filter;
    juce::AudioParameterInt* filterParam;

    BiquadFilterCoefficients lastCoeffs = {};
    int lastFilters = 0;
    std::complex<double> values[MAX_WIDTH] = {};

    ResponseCallback callback;
};