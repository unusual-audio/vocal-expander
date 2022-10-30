/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VocalExpanderAudioProcessor::VocalExpanderAudioProcessor()
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
    addParameter (threshold = new juce::AudioParameterFloat (
        "threshold", "Threshold", -96.0f, 0.0f, -72.0f));
    addParameter (ratio = new juce::AudioParameterFloat (
        "ratio", "Ratio", 1.0f, 10.0f, 1.5f));
    addParameter (attack = new juce::AudioParameterFloat (
        "attack", "Attack", 0.0f, 100.0f, 5.0f));
    addParameter (release = new juce::AudioParameterFloat (
        "release", "Release", 0.0f, 1000.0f, 200.0f));
}

VocalExpanderAudioProcessor::~VocalExpanderAudioProcessor()
{
}

//==============================================================================
const juce::String VocalExpanderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VocalExpanderAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VocalExpanderAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VocalExpanderAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VocalExpanderAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VocalExpanderAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VocalExpanderAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VocalExpanderAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String VocalExpanderAudioProcessor::getProgramName (int index)
{
    return {};
}

void VocalExpanderAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void VocalExpanderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    expander.setThreshold(*threshold);
    expander.setRatio(*ratio);
    expander.setAttack(*attack);
    expander.setRelease(*release);
    
    expander.prepare(spec);
    expander.reset();
}

void VocalExpanderAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VocalExpanderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void VocalExpanderAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    juce::dsp::AudioBlock<float> block (buffer);
    auto context = juce::dsp::ProcessContextReplacing<float> (block);
    
    expander.setThreshold(*threshold);
    expander.setRatio(*ratio);
    expander.setAttack(*attack);
    expander.setRelease(*release);
    
    expander.process(context);

}

//==============================================================================
bool VocalExpanderAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VocalExpanderAudioProcessor::createEditor()
{
    return new VocalExpanderAudioProcessorEditor (*this);
}

//==============================================================================
void VocalExpanderAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::unique_ptr<juce::XmlElement> xml (new juce::XmlElement ("VocalExpanderParameter"));
    xml->setAttribute ("threshold", (double) *threshold);
    xml->setAttribute ("ratio", (double) *ratio);
    xml->setAttribute ("attack", (double) *attack);
    xml->setAttribute ("release", (double) *release);
    copyXmlToBinary (*xml, destData);
}

void VocalExpanderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName ("VocalExpanderParameter"))
        {
            *threshold = (float) xmlState->getDoubleAttribute ("threshold", *threshold);
            *ratio = (float) xmlState->getDoubleAttribute ("ratio", *ratio);
            *attack = (float) xmlState->getDoubleAttribute ("attack", *attack);
            *release = (float) xmlState->getDoubleAttribute ("release", *release);
        }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocalExpanderAudioProcessor();
}
