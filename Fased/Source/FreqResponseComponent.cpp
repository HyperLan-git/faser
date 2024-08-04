#include "FreqResponseComponent.hpp"

FreqResponseComponent::FreqResponseComponent(
    const BiquadFilter& filter, juce::AudioParameterInt* filterParam)
    : filter(filter), filterParam(filterParam) {
    setSize(400, 400);
}

constexpr float a = std::log10(MAX_FREQ), b = std::log10(40);

constexpr float MAX_DB = 40;

void FreqResponseComponent::paint(juce::Graphics& g) {
    using namespace std::complex_literals;

    BiquadFilterCoefficients c = filter.getCoeffs();
    int filters = *filterParam;
    int w = this->getWidth(), h = this->getHeight();
    if (w <= 1) return;
    if (c != this->lastCoeffs || filters != this->lastFilters) {
        for (int i = 0; i < w; i++) {
            float f = std::pow(10, i * (a - b) / (w - 1) + b);
            std::complex<float> z = std::__complex_exp<float>(
                                    1if * juce::MathConstants<float>::pi * f /
                                    MAX_FREQ),
                                z1 = 1.f / z, z2 = 1.f / (z * z);
            values[i] =
                (c.b0 + c.b1 * z1 + c.b2 * z2) / (c.a0 + c.a1 * z1 + c.a2 * z2);
            if (filters > 1) values[i] = std::pow(values[i], filters);
        }
        this->lastFilters = filters;
        this->lastCoeffs = c;
    }

    g.fillAll(juce::Colours::black);

    juce::Path p, p2;

    p.preallocateSpace(w);
    p.startNewSubPath({-200, h / 2.f});

    p2.preallocateSpace(w);
    p2.startNewSubPath({-200, h / 2.f});
    for (int x = 0; x < w; x++) {
        if (values[x].real() == 0 && values[x].imag() == 0) {
            p.lineTo(x, h + 20);
            p2.lineTo(x, h + 20);
            continue;
        }
        const float mag = 20 * std::log10(std::abs(values[x]));
        const float phase = std::arg(values[x]);
        const int y = h / 2 - h * mag / MAX_DB / 2;
        const int y2 = h / 2 - h * phase / juce::MathConstants<float>::pi / 2;
        p.lineTo(x, y);
        if (y > 2 * h)
            p2.lineTo(x, h + 20);
        else
            p2.lineTo(x, y2);
    }
    p.lineTo(w + 200, h / 2.f);
    p.lineTo(w, h + 20);
    p.lineTo(-200, h + 20);
    p.closeSubPath();

    p2.lineTo(w + 200, h / 2.f);
    p2.lineTo(w, h + 20);
    p2.lineTo(-200, h + 20);
    p2.closeSubPath();

    g.setColour(juce::Colours::orange);
    g.fillPath(p);
    g.setColour(juce::Colours::orange.darker());
    g.strokePath(p, juce::PathStrokeType(5),
                 juce::AffineTransform().withAbsoluteTranslation(0, 3));

    g.setColour(juce::Colours::cyan.withAlpha(.3f));
    g.fillPath(p2);
    g.setColour(juce::Colours::cyan.darker().withAlpha(.3f));
    g.strokePath(p2, juce::PathStrokeType(5),
                 juce::AffineTransform().withAbsoluteTranslation(0, 3));
}

void FreqResponseComponent::resized() {}