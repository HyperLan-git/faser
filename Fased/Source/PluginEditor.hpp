#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.hpp"
#include "KnobComponent.hpp"
#include "FreqResponseComponent.hpp"
#include "SelectorComponent.hpp"

class FasedAudioProcessor;

class FasedAudioProcessorEditor : public juce::AudioProcessorEditor {
   public:
    FasedAudioProcessorEditor(FasedAudioProcessor&);
    ~FasedAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

   private:
    FasedAudioProcessor& audioProcessor;

    KnobComponent freq, Q, gain, filters;
    SelectorComponent filterType;

    FreqResponseComponent graph;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FasedAudioProcessorEditor)
};
