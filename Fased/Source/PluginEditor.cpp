#include "PluginEditor.hpp"

ResponseCallback createGraphCallback(FasedAudioProcessor& p) {
    juce::AudioParameterFloat *fParam = p.getFreqParam(),
                              *gParam = p.getGainParam(),
                              *qParam = p.getQParam();
    return [=](std::optional<float> freq, std::optional<float> gain,
               std::optional<float> dQ) {
        if (freq) fParam->setValueNotifyingHost(fParam->convertTo0to1(*freq));
        if (gain) gParam->setValueNotifyingHost(gParam->convertTo0to1(*gain));
        if (dQ)
            qParam->setValueNotifyingHost(
                qParam->convertTo0to1((*dQ) + *qParam));
    };
}

FasedAudioProcessorEditor::FasedAudioProcessorEditor(FasedAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      freq(p.getFreqParam()),
      gain(p.getGainParam()),
      Q(p.getQParam()),
      filters(p.getFiltersParam()),
      filterType(p.getTypeParam()),
      graph(p.getFilter(), p.getFiltersParam(), createGraphCallback(p)) {
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
    g.fillAll(juce::Colours::black.brighter(.5f));
}

void FasedAudioProcessorEditor::resized() {
    freq.setBounds(0, 3, 100, 100);
    Q.setBounds(100, 3, 100, 100);
    gain.setBounds(200, 3, 100, 100);
    filters.setBounds(300, 3, 100, 100);

    filterType.setBounds(100, 107, 200, 40);

    graph.setBounds(0, 150, 400, 400);
}
