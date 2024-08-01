#include "PluginProcessor.h"
#include "PluginEditor.h"

FasedAudioProcessorEditor::FasedAudioProcessorEditor(FasedAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      freq(p.getFreqParam()),
      Q(p.getQParam()) {
    setSize(400, 500);
    this->addAndMakeVisible(freq);
    this->addAndMakeVisible(Q);
}

FasedAudioProcessorEditor::~FasedAudioProcessorEditor() {}

void FasedAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(15.0f));
}

void FasedAudioProcessorEditor::resized() {
    freq.setBounds({0, 0, 100, 100});
    Q.setBounds({100, 0, 100, 100});
}
