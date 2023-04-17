/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class AnyDrum001AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AnyDrum001AudioProcessor();
    ~AnyDrum001AudioProcessor() override;

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

    //==============================================================================
    std::atomic<float> mAmplitude;//a number between 0 and 1.

    //file player functions
    void openButtonClicked();
    void playButtonClicked();
    void playFile();

    void loadFileIntoTransport();
    juce::File currentlyLoadedFile;

    bool isTriggering{ 0 };

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;

    std::atomic<float>* isTriggerOn = nullptr;

    std::atomic<float>* mGain = nullptr;
    std::atomic<float>* mThreshold = nullptr;
    std::atomic<float>* mOffsetLimit = nullptr;
    std::atomic<float>* mMaskLimit = nullptr;
    std::atomic<float>* mOutputVol = nullptr;

    //==============================================================================
    float mMaxPeak = 0.0;
    int mSampleCounter = 0;
    int mDetectionLength = 256;

    float mOffsetPeak = 0.0;
    float mOffsetAmp = 0.0;
    int mOffsetCounter = 0;
    int mMaskCounter = 0;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> playSource;

    juce::AudioTransportSource transport;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnyDrum001AudioProcessor)
};
