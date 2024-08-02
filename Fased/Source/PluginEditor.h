#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "KnobComponent.hpp"

class FasedAudioProcessorEditor : public juce::AudioProcessorEditor {
   public:
    FasedAudioProcessorEditor(FasedAudioProcessor&);
    ~FasedAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

   private:
    void onSelectFilterType();

    FasedAudioProcessor& audioProcessor;

    KnobComponent freq, Q, gain;

    juce::ComboBox filterType;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FasedAudioProcessorEditor)
};
