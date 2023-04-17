/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g,
                          int x, int y, int width, int height,
                          float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        float angle = rotaryStartAngle + (sliderPos * (rotaryEndAngle - rotaryStartAngle));

        juce::Rectangle<float> dialArea(x, y, width, height);

        g.setColour(juce::Colour(250u, 249u, 244u));
        g.fillEllipse(dialArea);

        g.setColour(juce::Colours::black);
        juce::Path dialHand;
        dialHand.addRectangle(-2, -30, 4, 20);
        g.fillPath(dialHand, juce::AffineTransform::rotation(angle).translated(30, 30));
    }

    void drawButtonBackground(juce::Graphics& g,
                              juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool, bool isButtonDown) override
    {
        auto buttonArea = button.getLocalBounds();
        g.setColour(juce::Colours::transparentBlack);
        g.fillRect(buttonArea);
    }
};

class CustomKnob : public juce::Slider
{
public:
    CustomKnob() {}
    ~CustomKnob() {}
    
    void mouseDown(const juce::MouseEvent& m)
    {
        juce::Slider::mouseDown(m);
    }

    void mouseUp(const juce::MouseEvent& m)
    {
        juce::Slider::mouseUp(m);
    }
};

//==============================================================================

class AnyDrum001AudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        public juce::Timer,
                                        public juce::FileDragAndDropTarget
{
public:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    AnyDrum001AudioProcessorEditor (AnyDrum001AudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~AnyDrum001AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;
    void paintHistogram(juce::Graphics&);

    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    void fileDragEnter(const juce::StringArray& files, int x, int y) override;
    void fileDragExit(const juce::StringArray& files) override;

    static const juce::Font getCustomFont()
    {
        static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::TCM_TTF, BinaryData::TCM_TTFSize);
        return juce::Font(typeface);
    }

private:
    juce::OpenGLContext openGlContext;

    juce::AudioProcessorValueTreeState& valueTreeState;

    int rectArray[554] = { 0 };
    int margin = 12;

    CustomLookAndFeel customLnF;
    juce::LookAndFeel_V4 buttonLnF;

    juce::LookAndFeel_V4 nameTextLnF;

    juce::LookAndFeel_V4 toggleSliderLnF;
    juce::LookAndFeel_V4 labelLnF;
    juce::LookAndFeel_V4 fileDragRectLnF;

    juce::TextButton mOpenButton{ "" };

    juce::Label mFileNameLabel;

    juce::Slider mTriggerToggleSlider;

    CustomKnob mGainSlider;
    CustomKnob mThresholdSlider;
    CustomKnob mOffsetSlider;
    CustomKnob mMaskSlider;
    CustomKnob mOutputSlider;

    juce::Label mGainLabel;
    juce::Label mThresLabel;
    juce::Label mOffsetLabel;
    juce::Label mMaskLabel;
    juce::Label mOutputLabel;
    juce::Label mFileDragRect;

    std::unique_ptr<SliderAttachment> mToggleAttachment;
    std::unique_ptr<SliderAttachment> mGainAttachent;
    std::unique_ptr<SliderAttachment> mThresholdAttachment;
    std::unique_ptr<SliderAttachment> mOffsetAttachent;
    std::unique_ptr<SliderAttachment> mMaskAttachent;
    std::unique_ptr<SliderAttachment> mOutputAttachent;

    AnyDrum001AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnyDrum001AudioProcessorEditor)
};
