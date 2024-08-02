#include "PluginProcessor.h"
#include "PluginEditor.h"

FasedAudioProcessorEditor::FasedAudioProcessorEditor(FasedAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      freq(p.getFreqParam()),
      gain(p.getGainParam()),
      Q(p.getQParam()) {
    setSize(400, 500);

    this->addAndMakeVisible(freq);
    this->addAndMakeVisible(Q);
    this->addAndMakeVisible(gain);
    this->addAndMakeVisible(filterType);

    filterType.addItemList(BIQUAD_TYPES, 1);

    filterType.onChange = [this] { this->onSelectFilterType(); };
    filterType.setSelectedId(p.getTypeParam()->getIndex() + 1);
    // TODO p.addListener
}

FasedAudioProcessorEditor::~FasedAudioProcessorEditor() {}

void FasedAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(15.0f));
}

void FasedAudioProcessorEditor::resized() {
    freq.setBounds(0, 0, 100, 100);
    Q.setBounds(100, 0, 100, 100);
    gain.setBounds(200, 0, 100, 100);

    filterType.setBounds(300, 0, 100, 40);
}

void FasedAudioProcessorEditor::onSelectFilterType() {
    int id = this->filterType.getSelectedId();
    if (id == 0) return;
    this->audioProcessor.setFilterType((enum BiquadFilterType)(id - 1));
}
