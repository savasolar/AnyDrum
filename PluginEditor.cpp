/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AnyDrum001AudioProcessorEditor::AnyDrum001AudioProcessorEditor (AnyDrum001AudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
    openGlContext.attachTo(*this);

    Timer::startTimerHz(120);

    setSize (578, 384);
    setResizable(false, false);

    buttonLnF.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    buttonLnF.setColour(juce::ComboBox::outlineColourId, juce::Colour(169u, 169u, 169u));

    nameTextLnF.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    nameTextLnF.setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);

    toggleSliderLnF.setColour(juce::Slider::thumbColourId, juce::Colour(250u, 249u, 244u));
    toggleSliderLnF.setColour(juce::Slider::trackColourId, juce::Colours::black);
    toggleSliderLnF.setColour(juce::Slider::backgroundColourId, juce::Colours::black);

    labelLnF.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    labelLnF.setColour(juce::Label::outlineColourId, juce::Colour(169u, 169u, 169u));

    fileDragRectLnF.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    fileDragRectLnF.setColour(juce::Label::outlineColourId, juce::Colour(154u, 87u, 205u));

    //setting up open button
    mOpenButton.setLookAndFeel(&buttonLnF);
    addAndMakeVisible(&mOpenButton);
    mOpenButton.onClick = [this]
    {
        audioProcessor.openButtonClicked();
    };

    //setting up filename textbox
    addAndMakeVisible(&mFileNameLabel);
    mFileNameLabel.setLookAndFeel(&nameTextLnF);
    mFileNameLabel.setJustificationType(juce::Justification::centredLeft);
    mFileNameLabel.setFont(getCustomFont().withHeight(16.0f).withStyle(juce::Font::plain));

    //setting up trigger toggle
    mTriggerToggleSlider.setLookAndFeel(&toggleSliderLnF);
    mTriggerToggleSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mTriggerToggleSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(&mTriggerToggleSlider);

    mToggleAttachment.reset(new SliderAttachment(valueTreeState, "toggle", mTriggerToggleSlider));

    //setting up gain knob
    mGainSlider.setLookAndFeel(&customLnF);
    mGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(&mGainSlider);

    addAndMakeVisible(&mGainLabel);
    mGainLabel.setLookAndFeel(&labelLnF);
    mGainLabel.setJustificationType(juce::Justification::centred);
    mGainLabel.setFont(getCustomFont().withHeight(16.0f).withStyle(juce::Font::bold));
    mGainLabel.setText(juce::String(mGainSlider.getValue(), 2), juce::dontSendNotification);
    mGainLabel.setVisible(false);

    mGainAttachent.reset(new SliderAttachment(valueTreeState, "gain", mGainSlider));

    //setting up thresh knob
    mThresholdSlider.setLookAndFeel(&customLnF);
    mThresholdSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mThresholdSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(&mThresholdSlider);

    addAndMakeVisible(&mThresLabel);
    mThresLabel.setLookAndFeel(&labelLnF);
    mThresLabel.setJustificationType(juce::Justification::centred);
    mThresLabel.setFont(getCustomFont().withHeight(16.0f).withStyle(juce::Font::bold));
    mThresLabel.setText(juce::String(mThresholdSlider.getValue(), 2), juce::dontSendNotification);
    mThresLabel.setVisible(false);

    mThresholdAttachment.reset(new SliderAttachment(valueTreeState, "threshold", mThresholdSlider));

    //setting up offset knob
    mOffsetSlider.setLookAndFeel(&customLnF);
    mOffsetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mOffsetSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(&mOffsetSlider);

    addAndMakeVisible(&mOffsetLabel);
    mOffsetLabel.setLookAndFeel(&labelLnF);
    mOffsetLabel.setJustificationType(juce::Justification::centred);
    mOffsetLabel.setFont(getCustomFont().withHeight(16.0f).withStyle(juce::Font::bold));
    mOffsetLabel.setText(juce::String(mOffsetSlider.getValue(), 0), juce::dontSendNotification);
    mOffsetLabel.setVisible(false);

    mOffsetAttachent.reset(new SliderAttachment(valueTreeState, "offset", mOffsetSlider));

    //setting up mask knob
    mMaskSlider.setLookAndFeel(&customLnF);
    mMaskSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mMaskSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(&mMaskSlider);

    addAndMakeVisible(&mMaskLabel);
    mMaskLabel.setLookAndFeel(&labelLnF);
    mMaskLabel.setJustificationType(juce::Justification::centred);
    mMaskLabel.setFont(getCustomFont().withHeight(16.0f).withStyle(juce::Font::bold));
    mMaskLabel.setText(juce::String(mMaskSlider.getValue()), juce::dontSendNotification);
    mMaskLabel.setVisible(false);

    mMaskAttachent.reset(new SliderAttachment(valueTreeState, "mask", mMaskSlider));

    //setting up output knob
    mOutputSlider.setLookAndFeel(&customLnF);
    mOutputSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mOutputSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(&mOutputSlider);

    addAndMakeVisible(&mOutputLabel);
    mOutputLabel.setLookAndFeel(&labelLnF);
    mOutputLabel.setJustificationType(juce::Justification::centred);
    mOutputLabel.setFont(getCustomFont().withHeight(16.0f).withStyle(juce::Font::bold));
    mOutputLabel.setText(juce::String(mOutputSlider.getValue()), juce::dontSendNotification);
    mOutputLabel.setVisible(false);

    mOutputAttachent.reset(new SliderAttachment(valueTreeState, "output", mOutputSlider));

    //setting up file drag rectangle
    addAndMakeVisible(&mFileDragRect);
    mFileDragRect.setLookAndFeel(&fileDragRectLnF);
    mFileDragRect.setVisible(false);
}

AnyDrum001AudioProcessorEditor::~AnyDrum001AudioProcessorEditor()
{
    Timer::stopTimer();

    mOpenButton.setLookAndFeel(nullptr);
    mFileNameLabel.setLookAndFeel(nullptr);

    mTriggerToggleSlider.setLookAndFeel(nullptr);

    mGainSlider.setLookAndFeel(nullptr); mGainLabel.setLookAndFeel(nullptr);
    mThresholdSlider.setLookAndFeel(nullptr); mThresLabel.setLookAndFeel(nullptr);
    mOffsetSlider.setLookAndFeel(nullptr); mOffsetLabel.setLookAndFeel(nullptr);
    mMaskSlider.setLookAndFeel(nullptr); mMaskLabel.setLookAndFeel(nullptr);
    mOutputSlider.setLookAndFeel(nullptr); mOutputLabel.setLookAndFeel(nullptr);

    mFileDragRect.setLookAndFeel(nullptr);
}

//==============================================================================
void AnyDrum001AudioProcessorEditor::paint (juce::Graphics& g)
{
    //background image should be bg006DEMO, for the demo version
    juce::Image background = juce::ImageCache::getFromMemory(BinaryData::bg006_png, BinaryData::bg006_pngSize);
    g.drawImageAt(background, 0, 0);

    paintHistogram(g);

    //painting the threshold line
    int threshHeight = static_cast<int>(valueTreeState.getParameter("threshold")->getValue() * 205);

    g.setColour(juce::Colour(154u, 87u, 205u));
    g.fillRect(margin + 0, margin + 205 - threshHeight - 2, 554, 2);

    //white line at top of visualizer
    g.setColour(juce::Colour(242u, 242u, 242u));
    g.fillRect(margin, margin - 2, 554, 2);

    //dark grey bar at top of screen
    g.setColour(juce::Colour(37u, 36u, 34u));
    g.fillRect(0, 0, 578, 10);

    //======================================================

    if (audioProcessor.currentlyLoadedFile.existsAsFile())
        mFileNameLabel.setText(juce::String(audioProcessor.currentlyLoadedFile.getFileName()), juce::dontSendNotification);
    else
        mFileNameLabel.setText(juce::String("Choose a file..."), juce::dontSendNotification);

    if (mGainSlider.isMouseButtonDown(false) == true)
    {
        mGainLabel.setVisible(true);
        mGainLabel.setText(juce::String(mGainSlider.getValue(), 2), juce::dontSendNotification);
    }
    else
        mGainLabel.setVisible(false);
    if (mThresholdSlider.isMouseButtonDown(false) == true)
    {
        mThresLabel.setVisible(true);
        mThresLabel.setText(juce::String(mThresholdSlider.getValue(), 2), juce::dontSendNotification);
    }
    else
        mThresLabel.setVisible(false);
    if (mOffsetSlider.isMouseButtonDown(false) == true)
    {
        mOffsetLabel.setVisible(true);
        mOffsetLabel.setText(juce::String(mOffsetSlider.getValue(), 0), juce::dontSendNotification);
    }
    else
        mOffsetLabel.setVisible(false);
    if (mMaskSlider.isMouseButtonDown(false) == true)
    {
        mMaskLabel.setVisible(true);
        mMaskLabel.setText(juce::String(mMaskSlider.getValue(), 0), juce::dontSendNotification);
    }
    else
        mMaskLabel.setVisible(false);
    if (mOutputSlider.isMouseButtonDown(false) == true)
    {
        mOutputLabel.setVisible(true);
        mOutputLabel.setText(juce::String(mOutputSlider.getValue(), 2), juce::dontSendNotification);
    }
    else
        mOutputLabel.setVisible(false);
}

void AnyDrum001AudioProcessorEditor::resized()
{
    mOpenButton.setBounds(10, 226, 38, 31);
    mFileNameLabel.setBounds(54, 228, 116, 27);

    mTriggerToggleSlider.setBounds(187, 232, 44, 20);

    mGainSlider.setBounds(35, 280, 60, 60); mGainLabel.setBounds(29, 352, 72, 22);
    mThresholdSlider.setBounds(147, 280, 60, 60); mThresLabel.setBounds(141, 352, 72, 22);
    mOffsetSlider.setBounds(259, 280, 60, 60); mOffsetLabel.setBounds(253, 352, 72, 22);
    mMaskSlider.setBounds(371, 280, 60, 60); mMaskLabel.setBounds(365, 352, 72, 22);
    mOutputSlider.setBounds(483, 280, 60, 60); mOutputLabel.setBounds(477, 352, 72, 22);

    mFileDragRect.setBounds(0, 0, 578, 384);
}

void AnyDrum001AudioProcessorEditor::paintHistogram(juce::Graphics& g)
{
    int ampHeight = static_cast<int>(audioProcessor.mAmplitude * 205);//205 is the height of the graph area

    for (int i = 0; i < 553; i++)//553 = width of graph area -1
    {
        rectArray[i] = rectArray[i + 1];
    }
    rectArray[553] = ampHeight;

    for (int i = 0; i < 554; i++)
    {
        g.setColour(juce::Colours::lightblue);
        g.fillRect(margin + i, margin + 205 - rectArray[i], 1, rectArray[i]);
    }
}

void AnyDrum001AudioProcessorEditor::timerCallback()
{
    if (this->isMouseOverOrDragging(true) == true)
        Timer::startTimerHz(120);
    else
        Timer::stopTimer();

    repaint();
}

void AnyDrum001AudioProcessorEditor::mouseEnter(const juce::MouseEvent& event)
{
    timerCallback();
}

void AnyDrum001AudioProcessorEditor::mouseExit(const juce::MouseEvent& event)
{
}

bool AnyDrum001AudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (auto file : files)
    {
        if (file.containsIgnoreCase(".wav") || file.containsIgnoreCase(".mp3") || file.containsIgnoreCase(".aif") || file.containsIgnoreCase(".aiff"))
        {
            return true;
        }
    }

    return false;
}

void AnyDrum001AudioProcessorEditor::filesDropped(const juce::StringArray& files, int x, int y)
{
    for (auto file : files)
    {
        if (isInterestedInFileDrag(files))
        {
            audioProcessor.currentlyLoadedFile = file;

            audioProcessor.loadFileIntoTransport();
        }
    }

    mFileDragRect.setVisible(false);
}

void AnyDrum001AudioProcessorEditor::fileDragEnter(const juce::StringArray& files, int x, int y)
{
    mFileDragRect.setVisible(true);
}

void AnyDrum001AudioProcessorEditor::fileDragExit(const juce::StringArray& files)
{
    mFileDragRect.setVisible(false);
}