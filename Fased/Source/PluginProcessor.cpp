#include "PluginProcessor.h"
#include "PluginEditor.h"

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
      freq(new juce::AudioParameterFloat({"frequency", 1}, "Frequency", 1,
                                         20000, .1f)),
      Q(new juce::AudioParameterFloat({"q value", 1}, "Q", .001f, 5, .001f)) {
    addParameter(freq);
    addParameter(Q);
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

struct biquad {
    float a0;
    float a1;
    float a2;
    float b1;
    float b2;
} params;

float biquadFilter(float xn, float& x1, float& x2, float& y1, float& y2,
                   struct biquad params) {
    float yn = params.a0 * xn + params.a1 * x1 + params.a2 * x2 -
               params.b1 * y1 - params.b2 * y2;
    x2 = x1;
    x1 = xn;
    y2 = y1;
    y1 = yn;
    return yn;
}

float pi = juce::MathConstants<float>().pi;

float processAllPass(float xn, float fc, float Q, float& x1, float& x2,
                     float& y1, float& y2) {
    float d = -std::cos(2 * pi * fc / 44100);
    float c =
        (std::tan(pi * 1000 / 44100) - 1) / (std::tan(pi * 1000 / 44100) + 1);
    return biquadFilter(
        xn, x1, x2, y1, y2,
        {.a0 = -c, .a1 = d * (1 - c), .a2 = 1, .b1 = d * (1 - c), .b2 = -c});
}

void FasedAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer& midiMessages) {
    (void)midiMessages;

    juce::ScopedNoDenormals noDenormals;
    const int inputs = getTotalNumInputChannels();
    const int outputs = getTotalNumOutputChannels();

    if (inputs < 2) return;
    if (outputs < 2) return;

    const int samples = buffer.getNumSamples();
    const double rate = getSampleRate();

    const auto l = buffer.getWritePointer(0), r = buffer.getWritePointer(1);

    float fc = *freq;
    float q = *Q;
    for (int i = 0; i < samples; i++) {
        l[i] = processAllPass(l[i], fc, q, lx1, lx2, ly1, ly2);
        r[i] = processAllPass(r[i], fc, q, rx1, rx2, ry1, ry2);
    }
}

bool FasedAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* FasedAudioProcessor::createEditor() {
    return new FasedAudioProcessorEditor(*this);
}

void FasedAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(GET_PARAM_NORMALIZED(freq));
    stream.writeFloat(GET_PARAM_NORMALIZED(Q));
}

void FasedAudioProcessor::setStateInformation(const void* data,
                                              int sizeInBytes) {
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes),
                                   false);
    freq->setValueNotifyingHost(stream.readFloat());
    Q->setValueNotifyingHost(stream.readFloat());
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new FasedAudioProcessor();
}
