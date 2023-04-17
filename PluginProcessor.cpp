/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AnyDrum001AudioProcessor::AnyDrum001AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
    , parameters (*this, nullptr, juce::Identifier (JucePlugin_Name),
        {
            std::make_unique<juce::AudioParameterFloat>("toggle",
                                                        "Trigger Toggle",
                                                        juce::NormalisableRange<float>(0.f, 1.f, 1.f),
                                                        0.f),
            std::make_unique<juce::AudioParameterFloat>("gain",
                                                        "Gain",
                                                        0.0f,
                                                        6.0f,
                                                        1.0f),
            std::make_unique<juce::AudioParameterFloat>("threshold",
                                                        "Threshold",
                                                        0.0f,
                                                        1.0f,
                                                        1.0f),
            std::make_unique<juce::AudioParameterFloat>("offset",
                                                        "Offset",
                                                        juce::NormalisableRange<float>(0.f, 6000.f, 1.f),
                                                        512.f),
            std::make_unique<juce::AudioParameterFloat>("mask",
                                                        "Mask",
                                                        juce::NormalisableRange<float>(1000.f, 50000.f, 1.f),
                                                        14000),
            std::make_unique<juce::AudioParameterFloat>("output",
                                                        "Output",
                                                        0.0f,
                                                        2.0f,
                                                        1.0f)
        })
#endif
{
    isTriggerOn = parameters.getRawParameterValue("toggle");
    mGain = parameters.getRawParameterValue("gain");
    mThreshold = parameters.getRawParameterValue("threshold");
    mOffsetLimit = parameters.getRawParameterValue("offset");
    mMaskLimit = parameters.getRawParameterValue("mask");
    mOutputVol = parameters.getRawParameterValue("output");

    parameters.state = juce::ValueTree("savedParams");

    formatManager.registerBasicFormats();
}

AnyDrum001AudioProcessor::~AnyDrum001AudioProcessor()
{
    transport.setSource(nullptr);
}

//==============================================================================
const juce::String AnyDrum001AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AnyDrum001AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AnyDrum001AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AnyDrum001AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AnyDrum001AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AnyDrum001AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AnyDrum001AudioProcessor::getCurrentProgram()
{
    return 0;
}

void AnyDrum001AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AnyDrum001AudioProcessor::getProgramName (int index)
{
    return {};
}

void AnyDrum001AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AnyDrum001AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    transport.prepareToPlay(samplesPerBlock, sampleRate);
}

//==============================================================================
void AnyDrum001AudioProcessor::openButtonClicked()
{
    juce::FileChooser chooser("Choose a file", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), "*.wav; *.mp3; *.aif; *.aiff", true, false);

    if (chooser.browseForFileToOpen())
    {
        juce::File myFile;

        myFile = chooser.getResult();

        currentlyLoadedFile = myFile;

        juce::AudioFormatReader* reader = formatManager.createReaderFor(myFile);

        if (reader != nullptr)
        {
            std::unique_ptr<juce::AudioFormatReaderSource> tempSource(new juce::AudioFormatReaderSource(reader, true));

            transport.setSource(tempSource.get(), 0, nullptr, reader->sampleRate);
            transport.stop();
            transport.setPosition(0.0);

            playSource.reset(tempSource.release());
        }
    }
}

void AnyDrum001AudioProcessor::playButtonClicked()
{
    if (*isTriggerOn == 1)
    {
        transport.setGain(0.5);//(1.0);
        transport.setPosition(0.0);
        transport.start();
    }
}

void AnyDrum001AudioProcessor::playFile()
{
    if (*isTriggerOn == 1)
    {
        transport.setGain(mOffsetAmp);
        transport.setPosition(0.0);
        transport.start();
    }
}

//==============================================================================
void AnyDrum001AudioProcessor::releaseResources()
{
    transport.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AnyDrum001AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AnyDrum001AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)//working on a sample-by-sample basis
        {
            channelData[sample] = channelData[sample] * *mGain;//input volume

            //determining mAmplitude
            float singleSample = std::abs(buffer.getSample(channel, sample));

            if (singleSample > mMaxPeak)
            {
                mMaxPeak = singleSample;//making maxPeak the maximum of the 256 samples that we're going through
            }

            ++mSampleCounter;

            if (mSampleCounter == mDetectionLength)
            {
                mAmplitude = mMaxPeak;
                mMaxPeak = 0.0;
                mSampleCounter = 0;
            }

            //triggering according to sensitivity variables

            if (*mThreshold < mAmplitude)
            {
                //finding mOffsetPeak
                if (singleSample > mOffsetPeak)
                {
                    mOffsetPeak = singleSample;
                }

                ++mOffsetCounter;

                if (mOffsetCounter >= *mOffsetLimit)
                {
                    mOffsetAmp = mOffsetPeak;
                    mOffsetPeak = 0.0;

                    if (mMaskCounter < *mMaskLimit)
                    {
                        if (isTriggering == 0)
                        {
                            isTriggering = 1;
                            playFile();
                            mMaskCounter = 0;
                        }
                    }
                    mOffsetCounter = 0;
                }
            }
            else
            {
                mOffsetCounter = 0;
            }

            ++mMaskCounter;

            if (mMaskCounter >= *mMaskLimit)
            {
                isTriggering = 0;
                mMaskCounter = 0;
            }
        }
    }

    if (*isTriggerOn == 1)
    {
        transport.getNextAudioBlock(juce::AudioSourceChannelInfo(buffer));//turn the sample player on; mutes all previous input signal
    }

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = channelData[sample] * *mOutputVol;//output volume
        }
    }
}

//==============================================================================
bool AnyDrum001AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AnyDrum001AudioProcessor::createEditor()
{
    return new AnyDrum001AudioProcessorEditor (*this, parameters);
}

//==============================================================================
void AnyDrum001AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    //in the demo version, this method should be empty.

//    /*

    std::unique_ptr<juce::XmlElement> xml(parameters.state.createXml());

    xml->setAttribute("toggle", *isTriggerOn);
    xml->setAttribute("gain", *mGain);
    xml->setAttribute("threshold", *mThreshold);
    xml->setAttribute("offset", *mOffsetLimit);
    xml->setAttribute("mask", *mMaskLimit);
    xml->setAttribute("output", *mOutputVol);

    xml->setAttribute("audiofile", currentlyLoadedFile.getFullPathName());

    copyXmlToBinary(*xml, destData);

//    */
}

void AnyDrum001AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    //in the demo version, this method should be empty.

//    /*
    std::unique_ptr<juce::XmlElement> theParams(getXmlFromBinary(data, sizeInBytes));

    if (theParams != nullptr)
    {
        if (theParams->hasTagName(parameters.state.getType()))
        {
            parameters.state = juce::ValueTree::fromXml(*theParams);

            *isTriggerOn = theParams->getDoubleAttribute("toggle");
            *mGain = theParams->getDoubleAttribute("gain");
            *mThreshold = theParams->getDoubleAttribute("threshold");
            *mOffsetLimit = theParams->getDoubleAttribute("offset");
            *mMaskLimit = theParams->getDoubleAttribute("mask");
            *mOutputVol = theParams->getDoubleAttribute("output");

            currentlyLoadedFile = juce::File::createFileWithoutCheckingPath(theParams->getStringAttribute("audiofile"));
            if (currentlyLoadedFile.existsAsFile())
            {
                loadFileIntoTransport();
            }
        }
    }
//    */
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AnyDrum001AudioProcessor();
}

void AnyDrum001AudioProcessor::loadFileIntoTransport()
{
    juce::AudioFormatReader* reader = formatManager.createReaderFor(currentlyLoadedFile);

    if (reader != nullptr)
    {
        std::unique_ptr<juce::AudioFormatReaderSource> tempSource(new juce::AudioFormatReaderSource(reader, true));

        transport.setSource(tempSource.get(), 0, nullptr, reader->sampleRate);
        transport.stop();
        transport.setPosition(0.0);

        playSource.reset(tempSource.release());
    }
}