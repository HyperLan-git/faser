#include "PluginProcessor.hpp"

FasedAudioProcessor::FasedAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              )
#endif
      ,
      freq(new juce::AudioParameterFloat({"frequency", 1}, "Frequency", 5,
                                         20000, 500)),
      Q(new juce::AudioParameterFloat({"Q", 1}, "Q value", .01f, 20, .707f)),
      gain(new juce::AudioParameterFloat({"gain", 1}, "Gain", -30, 30, 6)),
      filters(new juce::AudioParameterInt({"filters", 1}, "Filter slope", 1,
                                          MAX_FILTERS, 1)),
      type(new juce::AudioParameterChoice(
          {"type", 1}, "Filter type",
          {"Allpass", "Lowpass", "Highpass", "Bandpass", "Notch", "Peak",
           "Lowshelf", "Highshelf"},
          1)) {
    addParameter(freq);
    addParameter(Q);
    addParameter(gain);
    addParameter(type);
    addParameter(filters);
}

FasedAudioProcessor::~FasedAudioProcessor() {}

const juce::String FasedAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool FasedAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool FasedAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool FasedAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double FasedAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int FasedAudioProcessor::getNumPrograms() { return 1; }

int FasedAudioProcessor::getCurrentProgram() { return 0; }

void FasedAudioProcessor::setCurrentProgram(int index) { (void)index; }

const juce::String FasedAudioProcessor::getProgramName(int index) {
    (void)index;
    return {};
}

void FasedAudioProcessor::changeProgramName(int index,
                                            const juce::String& newName) {
    (void)index;
    (void)newName;
}

void FasedAudioProcessor::prepareToPlay(double sampleRate,
                                        int samplesPerBlock) {
    (void)sampleRate;
    (void)samplesPerBlock;
}

void FasedAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FasedAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void FasedAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer& midiMessages) {
    (void)midiMessages;

    juce::ScopedNoDenormals noDenormals;
    const int inputs = getTotalNumInputChannels();
    const int outputs = getTotalNumOutputChannels();

    if (inputs == 0) return;
    if (outputs == 0) return;

    const int samples = buffer.getNumSamples();
    const double rate = getSampleRate();

    if (rate != filter.getSampleRate()) filter.setSampleRate(rate);

    const float* l = buffer.getWritePointer(0);
    const float* r =
        inputs < 2 || outputs < 2 ? nullptr : buffer.getWritePointer(1);

    const int n = *filters;
    float fc = *freq;
    float q = *Q;
    float g = (*gain) / n;
    enum BiquadFilterType t =
        static_cast<enum BiquadFilterType>(type->getIndex() + 1);

    if (t != filter.getType())
        std::memset(this->states, 0, 2 * MAX_FILTERS * sizeof(struct SOState));

    BiquadFilterParams b = filter.getParameters();
    if (t != filter.getType() || b.f != fc || b.Q != q || b.gain != g ||
        prevFilters != n) {
        if (prevFilters != n && n < MAX_FILTERS) {
            std::memset(this->states + n + 1, 0,
                        sizeof(struct SOState) * (MAX_FILTERS - n - 1));
            std::memset(this->states + MAX_FILTERS + n, 0,
                        sizeof(struct SOState) * (MAX_FILTERS - n - 1));
        }
        filter.setParameters(t, {.f = fc, .Q = q, .gain = g});
        juce::AudioProcessorEditor* editor = this->getActiveEditor();
        if (editor) {
            juce::MessageManager::callAsync(
                [editor]() { editor->repaint(editor->getBounds()); });
        }
    }

    // TODO think of an optimisation since y1 and y2 of a filter is equal to x1
    // and x2 of the next
    for (int i = 0; i < n; i++) {
        struct SOState &left = this->states[i],
                       &right = this->states[i + MAX_FILTERS];
        filter.processBlock(l, samples, left);
        if (r) filter.processBlock(r, samples, right);
    }
    this->prevFilters = n;
}

void FasedAudioProcessor::setFilterType(enum BiquadFilterType type) {
    this->type->setValueNotifyingHost(
        this->type->convertTo0to1(static_cast<float>(type)));
}

bool FasedAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* FasedAudioProcessor::createEditor() {
    return new FasedAudioProcessorEditor(*this);
}

void FasedAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(GET_PARAM_NORMALIZED(freq));
    stream.writeFloat(GET_PARAM_NORMALIZED(Q));
    stream.writeFloat(GET_PARAM_NORMALIZED(gain));
    stream.writeInt(*filters);
    stream.writeInt(GET_PARAM_NORMALIZED(type));
}

void FasedAudioProcessor::setStateInformation(const void* data,
                                              int sizeInBytes) {
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes),
                                   false);
    freq->setValueNotifyingHost(stream.readFloat());
    Q->setValueNotifyingHost(stream.readFloat());
    gain->setValueNotifyingHost(stream.readFloat());
    filters->setValueNotifyingHost(filters->convertTo0to1(stream.readInt()));
    type->setValueNotifyingHost(stream.readInt());
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new FasedAudioProcessor();
}
