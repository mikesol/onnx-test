#pragma once

#include <JuceHeader.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_utils/juce_audio_utils.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component, public juce::ChangeListener, public juce::AudioSource
{
public:
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };

    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    // change listener
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;

    // audio source
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    //==============================================================================
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...

    void changeState (TransportState newState);
    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    

    std::unique_ptr<juce::FileChooser> chooser;

    juce::AudioDeviceManager deviceManager;
    juce::AudioSourcePlayer audioSourcePlayer;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    TransportState state;
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
