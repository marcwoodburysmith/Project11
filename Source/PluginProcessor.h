/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace FilterInfo
{

enum FilterType
{
    FirstOrderLowPass,
    FirstOrderHighPass,
    FirstOrderAllPass,
    LowPass,
    HighPass,
    BandPass,
    Notch,
    AllPass,
    LowShelf,
    HighShelf,
    Peak
};

inline const std::map<FilterInfo::FilterType, juce::String>& GetParams()
{
    static std::map<FilterInfo::FilterType, juce::String> params =
    {
        {FirstOrderLowPass, "FirstOrder LowPass"},
        {FirstOrderHighPass, "FirstOrder HighPass"},
        {FirstOrderAllPass, "FirstOrder AllPass"},
        {LowPass, "LowPass"},
        {HighPass, "HighPass"},
        {BandPass, "BandPass"},
        {Notch, "Notch"},
        {AllPass, "Allpass"},
        {LowShelf, "LowShelf"},
        {HighShelf, "HighShelf"},
        {Peak, "Peak"}
    };

    return params;
}


} //end namespace FilterInfo

//==============================================================================
/**
*/
class Project11AudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    Project11AudioProcessor();
    ~Project11AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Project11AudioProcessor)
};
