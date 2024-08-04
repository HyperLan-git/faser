#pragma once

#include "JuceHeader.h"
#include "BiquadFilter.hpp"
#include "PluginEditor.hpp"

#define GET_PARAM_NORMALIZED(param) (param->convertTo0to1(*param))
#define SET_PARAM_NORMALIZED(param, value) \
    param->setValueNotifyingHost(param->convertTo0to1(value))

#define MAX_FILTERS 100

class FasedAudioProcessor : public juce::AudioProcessor {
   public:
    FasedAudioProcessor();
    ~FasedAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    inline juce::AudioParameterFloat* getFreqParam() { return freq; }
    inline juce::AudioParameterFloat* getQParam() { return Q; }
    inline juce::AudioParameterFloat* getGainParam() { return gain; }

    inline juce::AudioParameterInt* getFiltersParam() { return filters; }

    inline juce::AudioParameterChoice* getTypeParam() { return type; }

    void setFilterType(enum BiquadFilterType type);

    inline const BiquadFilter& getFilter() const { return filter; }

   private:
    BiquadFilter filter;

    struct SOState states[MAX_FILTERS * 2] = {};

    juce::AudioParameterFloat *freq, *Q, *gain;
    juce::AudioParameterInt* filters;
    juce::AudioParameterChoice* type;

    int prevFilters = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FasedAudioProcessor)
};

/**
 * Fills a destination buffer with the last values of src buffer, preserving
 * values order
 */
inline void feedBuffer(void* dst, const void* src, std::size_t dstSize,
                       std::size_t srcSize) {
    char* dst2 = (char*)dst;
    const char* src2 = (char*)src;
    if (srcSize < dstSize) {
        std::memmove(dst, dst2 + srcSize, dstSize - srcSize);
        std::memcpy(dst2 + (dstSize - srcSize), src, srcSize);
    } else {
        std::memcpy(dst, src2 + srcSize - dstSize, dstSize);
    }
}