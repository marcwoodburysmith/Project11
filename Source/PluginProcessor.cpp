/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"



//==============================================================================

juce::String ParamString(juce::String label, int filterNum)
{
    return "Filter_" + juce::String(filterNum)+"_"+ label;
}


juce::String generateGainParamString(int filterNum)
{
    return ParamString("gain",filterNum);
}

juce::String generateQParamString(int filterNum)
{
    return ParamString("Q",filterNum);
}

juce::String generateFreqParamString(int filterNum)
{
    return ParamString("freq",filterNum);
}

juce::String generateTypeParamString(int filterNum)
{
    return ParamString("type",filterNum);
}

juce::String generateBypassParamString(int filterNum)
{
    return ParamString("bypass",filterNum);
}



//==============================================================================
Project11AudioProcessor::Project11AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

Project11AudioProcessor::~Project11AudioProcessor()
{
}

//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout Project11AudioProcessor::createParameterLayout()
{
    const auto& params = FilterInfo::GetParams();
    
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterBool>(
                                                          juce::ParameterID(generateBypassParamString(0), 1),
                                                          generateBypassParamString(0),
                                                          false));
    
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           juce::ParameterID(generateGainParamString(0), 1),
                                                           generateGainParamString(0),
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 1.f, 1.f),
                                                           0.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           juce::ParameterID(generateQParamString(0), 1),
                                                           generateQParamString(0),
                                                           juce::NormalisableRange<float>(0.1f, 10.f, 0.5f, 1.f),
                                                           1.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           juce::ParameterID(generateFreqParamString(0), 1),
                                                           generateFreqParamString(0),
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
                                                           20.f));
    
    juce::StringArray types;
    
    for (const auto& [name, stringRep] : FilterInfo::filterToString)
    {
       types.add(stringRep);
    }
    
    
//    //Alternative
//    for ( const auto& elem : FilterInfo::filterToString )
//    {
//        types.add(elem.second);
//    }

    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
                                                            juce::ParameterID(
                                                            generateTypeParamString(0), 1),
                                                            generateTypeParamString(0),
                                                            types,
                                                            0));
    
    return layout;
}




//==============================================================================
const juce::String Project11AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Project11AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Project11AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Project11AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Project11AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Project11AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Project11AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Project11AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Project11AudioProcessor::getProgramName (int index)
{
    return {};
}

void Project11AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Project11AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void Project11AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Project11AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Project11AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool Project11AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Project11AudioProcessor::createEditor()
{
//    return new Project11AudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void Project11AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Project11AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Project11AudioProcessor();
}
