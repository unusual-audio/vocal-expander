/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VocalExpanderAudioProcessorEditor::VocalExpanderAudioProcessorEditor (VocalExpanderAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
        thresholdAttachment(*p.threshold, thresholdSlider),
        ratioAttachment(*p.ratio, ratioSlider),
        attackAttachment(*p.attack, attackSlider),
        releaseAttachment(*p.release, releaseSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (440, 320);
    
    addAndMakeVisible (thresholdSlider);
    thresholdSlider.setTextValueSuffix (" dBFS");
    thresholdSlider.setRange(thresholdSlider.getRange(), 1.0f);
    thresholdSlider.textFromValueFunction = [](double value)
    {
        return juce::String(value, 0);
    };
    thresholdSlider.updateText();
    thresholdSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    thresholdSlider.setColour(juce::Slider::ColourIds::thumbColourId, red);
    thresholdSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, red);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
    
    addAndMakeVisible (ratioSlider);
    ratioSlider.setTextValueSuffix (":1");
    ratioSlider.setRange(ratioSlider.getRange(), 0.1f);
    ratioSlider.textFromValueFunction = [](double value)
    {
        return juce::String(value, 1);
    };
    ratioSlider.updateText();
    ratioSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    ratioSlider.setColour(juce::Slider::ColourIds::thumbColourId, red);
    ratioSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    ratioSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    ratioSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    ratioSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, red);
    ratioSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
    
    addAndMakeVisible (attackSlider);
    attackSlider.setTextValueSuffix (" ms");
    attackSlider.setRange(attackSlider.getRange(), 1.0f);
    attackSlider.textFromValueFunction = [](double value)
    {
        return juce::String(value, 0);
    };
    attackSlider.updateText();
    attackSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    attackSlider.setColour(juce::Slider::ColourIds::thumbColourId, blue);
    attackSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    attackSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    attackSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    attackSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, blue);
    attackSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
    
    addAndMakeVisible (releaseSlider);
    releaseSlider.setTextValueSuffix (" ms");
    releaseSlider.setRange(releaseSlider.getRange(), 1.0f);
    releaseSlider.textFromValueFunction = [](double value)
    {
        return juce::String(value, 0);
    };
    releaseSlider.updateText();
    releaseSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    releaseSlider.setColour(juce::Slider::ColourIds::thumbColourId, blue);
    releaseSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    releaseSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    releaseSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    releaseSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, blue);
    releaseSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
}

VocalExpanderAudioProcessorEditor::~VocalExpanderAudioProcessorEditor()
{
}

//==============================================================================
void VocalExpanderAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (black);
    g.setColour(white);
    g.setFont(18);
    g.drawFittedText("Vocal Expander", 40,  30, 320, 30, juce::Justification::left, 1);
    
    g.setColour(red);
    g.setFont(15);
    g.drawFittedText("Threshold", 40, 90, 100, 30, juce::Justification::left, 1);
    
    g.setColour(red);
    g.setFont(15);
    g.drawFittedText("Ratio", 40,  130, 100, 30, juce::Justification::left, 1);
    
    g.setColour(blue);
    g.setFont(15);
    g.drawFittedText("Attack", 40, 170, 100, 30, juce::Justification::left, 1);
    
    g.setColour(blue);
    g.setFont(15);
    g.drawFittedText("Release", 40, 210, 100, 30, juce::Justification::left, 1);
    
    g.setColour(grey);
    g.setFont(12);
    g.drawFittedText("Unusual Audio", 40, getHeight() - 60, 300, 30, juce::Justification::left, 1);
}

void VocalExpanderAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    thresholdSlider .setBounds (140,  95, getWidth() - 140 - 40, 20);
    ratioSlider     .setBounds (140, 135, getWidth() - 140 - 40, 20);
    attackSlider    .setBounds (140, 175, getWidth() - 140 - 40, 20);
    releaseSlider   .setBounds (140, 215, getWidth() - 140 - 40, 20);
}
