#include "MainComponent.h"

void MainComponent::openButtonClicked()
{
    chooser = std::make_unique<juce::FileChooser> ("Select a Wave file to play...",
                                                   juce::File{},
                                                   "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode
                      | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync (chooserFlags, [this] (const juce::FileChooser& fc)
    {
        auto file = fc.getResult();

        if (file != juce::File{})
        {
            auto* reader = formatManager.createReaderFor (file);

            if (reader != nullptr)
            {
                auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
                transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
                playButton.setEnabled (true);
                readerSource.reset (newSource.release());
            }
        }
    });
}

/////
void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock (bufferToFill);
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // Initialization code before playback starts.
    // For example, you might prepare the transportSource here.
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::releaseResources()
{
    // Code to release any resources you've allocated in prepareToPlay.
    // For example, you might release the transportSource resources here.
    transportSource.releaseResources();
}
/////
void MainComponent::playButtonClicked()
{
    changeState (Starting);
}

void MainComponent::stopButtonClicked()
{
    changeState (Stopping);
}
//==============================================================================
MainComponent::MainComponent() : state(Stopped)
{
    // buttons
    addAndMakeVisible (&openButton);
    openButton.setButtonText ("Open...");
    openButton.onClick = [this] { openButtonClicked(); };

    addAndMakeVisible (&playButton);
    playButton.setButtonText ("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour (juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled (false);

    addAndMakeVisible (&stopButton);
    stopButton.setButtonText ("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour (juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled (false);
    
    // formats
    formatManager.registerBasicFormats();
    
    // listen to changes in the transport
    transportSource.addChangeListener (this);
    
    // link to speaker
    deviceManager.initialise (2, 2, nullptr, true, {}, nullptr);
    audioSourcePlayer.setSource(this);
    deviceManager.addAudioCallback(&audioSourcePlayer);
    setSize (300, 100);
}

void MainComponent::changeState (TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
            case Stopped:                           // [3]
                stopButton.setEnabled (false);
                playButton.setEnabled (true);
                transportSource.setPosition (0.0);
                break;

            case Starting:                          // [4]
                playButton.setEnabled (false);
                transportSource.start();
                break;

            case Playing:                           // [5]
                stopButton.setEnabled (true);
                break;

            case Stopping:                          // [6]
                transportSource.stop();
                break;
        }
    }
}
void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{

    if (source == &transportSource)
    {
        if (transportSource.isPlaying())
            changeState (Playing);
        else
            changeState (Stopped);
    }
}

MainComponent::~MainComponent()
{
    audioSourcePlayer.setSource(nullptr);
    deviceManager.removeAudioCallback(&audioSourcePlayer);
}

//==============================================================================


void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    openButton.setBounds (10, 10, 80, 30);       // x, y, width, height
    playButton.setBounds (100, 10, 80, 30);      // Adjust as necessary
    stopButton.setBounds (190, 10, 80, 30);
}
