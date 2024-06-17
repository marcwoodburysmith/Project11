/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================



//==============================================================================

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

const std::map<FilterType, juce::String> filterToString
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
} //end namespace FilterInfo

//==============================================================================

struct FilterParametersBase
{
    float frequency {440.f};
    bool bypassed {false};
    float quality {1.f};
    double sampleRate {44100};
    
};

inline bool operator==(const FilterParametersBase& lhs, const FilterParametersBase& rhs)
{
    return ( lhs.frequency == rhs.frequency && lhs.quality == rhs.quality );
}

struct FilterParameters : public FilterParametersBase
{
    FilterInfo::FilterType filterType {FilterInfo::FilterType::LowPass};
    float gainInDecibels {0.0f};
    
};

inline bool operator==(const FilterParameters& lhs, const FilterParameters& rhs)
{
    return (lhs.frequency == rhs.frequency && lhs.quality == rhs.quality &&
            static_cast<FilterParametersBase>(lhs) == static_cast<FilterParametersBase>(rhs) );
}

struct HighCutLowCutParameters : public FilterParametersBase
{
    int order {1};
    bool isLowcut {true};
    
};

inline bool operator==(const HighCutLowCutParameters& lhs, const HighCutLowCutParameters& rhs)
{
    return (lhs.order == rhs.order && lhs.isLowcut == rhs.isLowcut && static_cast<FilterParametersBase>(lhs) == static_cast<FilterParametersBase>(rhs) );
}



//==============================================================================


static auto makeCoefficients(FilterInfo::FilterType type, float freq, float q, float gain, float sampleRate)
{
    using namespace FilterInfo;
    
    switch (type) {
        case FilterType::FirstOrderLowPass:
                return juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(sampleRate, freq);
        case FilterType::FirstOrderHighPass:
                return juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(sampleRate, freq);
        case FilterType::FirstOrderAllPass:
            return juce::dsp::IIR::Coefficients<float>::makeFirstOrderAllPass(sampleRate, freq);
        case FilterType::LowPass:
            return juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, freq);
        case FilterType::HighPass:
            return juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, freq);
        case FilterType::BandPass:
            return juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, freq);
        case FilterType::Notch:
            return juce::dsp::IIR::Coefficients<float>::makeNotch(sampleRate, freq);
        case FilterType::AllPass:
            return juce::dsp::IIR::Coefficients<float>::makeAllPass(sampleRate, freq);
        case FilterType::LowShelf:
            return juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, freq, q, gain);
        case FilterType::HighShelf:
            return juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, freq, q, gain);
        case FilterType::Peak:
            return juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, freq, q, gain);
    }
}

static auto makeCoefficients(FilterParameters filterParams)
{
    return makeCoefficients(filterParams.filterType, filterParams.frequency, filterParams.quality, filterParams.gainInDecibels, filterParams.sampleRate);
}


/*
 For info on these see: JUCE/modules/juce_dsp/filter_design/juce_FilterDesign.h
 */
static auto makeCoefficients(HighCutLowCutParameters highLowParams)
{
    if (highLowParams.isLowcut)
    {
        return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod (highLowParams.frequency, highLowParams.sampleRate, highLowParams.order);
    }
    else
    {
        return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod (highLowParams.frequency, highLowParams.sampleRate, highLowParams.order);
    }
}








//==============================================================================
//HELPER FUNCTIONS FOR createParameterLayout()

juce::String generateGainParamString(int filterNum);

juce::String generateQParamString(int filterNum);

juce::String generateFreqParamString(int filterNum);

juce::String generateFilterTypeString(int filterNum);

juce::String generateBypassParamString(int filterNum);




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
    
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout() };
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    void updateFilterParams();

private:
    
    using Filter = juce::dsp::IIR::Filter<float>;
    
    using Filterchain = juce::dsp::ProcessorChain<Filter>;
    
    Filterchain leftChain, rightChain;
    
    HighCutLowCutParameters existingHighLow;
    FilterParameters existingFilterParams;
    
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Project11AudioProcessor)
};
