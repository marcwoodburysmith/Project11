/*
  ==============================================================================

    Fifo.h
    Created: 17 Jul 2024 9:13:05am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <array>

/*This will be the FIFO used to pass audio buffers between threads. This is pretty complicated so I am annotating with
 plenty of text. The AbstactFifo class just handles the read and write pointers in the Fifo, essentially locking these at the appropriate times for thread safety. See Audio Programmer Juce Tutorial 57 or Matkat::PFM Chapter 7, Part 5 for more detailed discussion*/


template<typename T, size_t Size>
struct Fifo
{
    size_t getSize() const noexcept
    {
        return Size;
    }
    
    /*Used when T is AudioBuffer<float>. 'buffer' is your actual AudioBuffer<float> as far as I can tell.
     For juce::AudioBuffer you need to call setSize(5 params, see documentation) and clear()
     */
    void prepare(int numSamples, int numChannels)
    {
        static_assert( std::is_same_v<T, juce::AudioBuffer<float> >,
                "prepare(numChannels, numSamples) should only be used when the Fifo is holding juce::AudioBuffer<float>");
        for (auto& audioBuffer:buffer)
        {
            audioBuffer.setSize(numChannels,
                                numSamples,
                                false,
                                true,
                                true);
            audioBuffer.clear();
        }
        
    }
    
    //used when T is std::vector<float>.
    void prepare(size_t numElements)
    {
        static_assert( std::is_same_v<T, std::vector<float> >,
                "prepare(numElements) should only be used when the Fifo is holding std::vector<float>");
        for( auto& buffer : buffers )
        {
            buffer.clear();
            buffer.resize(numElements, 0);
        }

    }
    
    bool push(const T& t)
    {
        /*This function returns an object which contains the start indices and block sizes, and also automatically finishes the write operation when it goes out of scope.*/
        auto writeHandle = fifo.write(1);
        if( write.blockSize1 > 0 )
        {
            
        }
        
        buffer[writeHandle.startIndex1] = t;
        
        return true;
        
    }
    
    
    bool pull(T& t)
    {
        
    }
    
    int getNumAvailableForReading() const;
    int getAvailableSpace() const;
    
private:
    /*
     The AbstactFifo object doesn't actually hold any data itself, but your FIFO class can use one of these to manage its position and status when reading or writing to it.*/
//    juce::AbstractFifo fifo { Size };
    

    
    //The actual audio buffer/vector
    std::array<T, Size> buffer;
};
