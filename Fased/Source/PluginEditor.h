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
    FasedAudioProcessor& audioProcessor;

    KnobComponent freq, Q;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FasedAudioProcessorEditor)
};
