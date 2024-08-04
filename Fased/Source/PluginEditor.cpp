#include "PluginProcessor.h"
#include "PluginEditor.h"

FasedAudioProcessorEditor::FasedAudioProcessorEditor(FasedAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      freq(p.getFreqParam()),
      gain(p.getGainParam()),
      Q(p.getQParam()),
      filters(p.getFiltersParam()),
      filterType(p.getTypeParam()),
      graph(p.getFilter(), p.getFiltersParam()) {
    setSize(400, 550);

    this->addAndMakeVisible(freq);
    this->addAndMakeVisible(Q);
    this->addAndMakeVisible(gain);
    this->addAndMakeVisible(filters);
    this->addAndMakeVisible(filterType);
    this->addAndMakeVisible(graph);
}

FasedAudioProcessorEditor::~FasedAudioProcessorEditor() {}

void FasedAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::grey);
}

void FasedAudioProcessorEditor::resized() {
    freq.setBounds(0, 0, 100, 100);
    Q.setBounds(100, 0, 100, 100);
    gain.setBounds(200, 0, 100, 100);
    filters.setBounds(300, 0, 100, 100);

    filterType.setBounds(0, 100, 200, 40);

    graph.setBounds(0, 150, 400, 400);
}
