#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <stdint.h>
#include <vector>
#include <iostream>


// Signals are just unsigned ints.
// Bool would work too, but this lets us use 1's and 0's
// and avoids the problems associated with vector<bool>.

using Signal = uint8_t;


// N-bit word
// By convention, these are big-endian.

template <int N>
class Word
{
  public:
    Word() : _word(N) {}
    Word(const std::vector<Signal>& w) : _word(w) {}
    
    Signal& bit(int position)
    {
      return _word.at(position);
    }
    
    void printValue() const
    {
      for (auto s : _word) std::cout << (int) s;
      std::cout << std::endl;
    } 

    bool compare(const std::vector<Signal>& w) const
    {
      return w == _word;
    } 

    bool operator==(const Word<N>& other) const
    {
      return other.compare(_word); 
    }

  private:
    std::vector<Signal> _word;
};


// Abstract base class for devices with inputs and outputs.

class Component
{
  public:
    // Constructor takes count of inputs and outputs
    Component(int inputs, int outputs) :
      _inputs(inputs), _outputs(outputs) {}

    // All components must have a process method.
    // This propogates a signal through the component to the outputs.
    virtual void process() = 0;
    
    // Call input to update a component's input.
    // Channels are 0-indexed.
    void input(int channel, Signal s)
    {
      _inputs.at(channel) = s;
    }

    // Get Output 0 from a component
    Signal output() const
    {
      return _outputs.at(0);
    }

    // Get output from a multi-output component on a given channel
    Signal output(int channel) const
    {
      return _outputs.at(channel);
    }

    // Print all outputs of a component
    void printValue() const
    {
      for (auto o : _outputs) std::cout << (int) o;
      std::cout << std::endl;
    }

  protected:
    // These represent current state of inputs and outputs.
    std::vector<Signal> _inputs;
    std::vector<Signal> _outputs;
};


// Abstract base class for components that also have control inputs.

class ControlComponent : public Component
{
  public:
    // Constructor takes counts of inputs, controls, and outputs
    ControlComponent(int inputs, int controls, int outputs) :
      Component(inputs, outputs), _controls(controls) {}

    // Set control, then call process to update outputs
    void control(int channel, Signal s)
    {
      _controls.at(channel) = s;
    }

  protected:
    std::vector<Signal> _controls;
};


// Abstract base class for components that use N-bit words
// for input and output.

template <int N>
class WordComponent
{
  public:
    // Constructor takes number of inputs and outputs
    WordComponent(int inputs, int outputs) :
      _inputs(inputs), _outputs(outputs) {} 
    
    // Get output from a component with a single output
    const Word<N>& output()
    {
      return _outputs.at(0);
    }
    
    // Process propogates a Signal through the component to its output
    virtual void process() = 0;

    // This happens ANY time ANY input is changed
    // Set input, then call process to update outputs
    void input(int channel, const Word<N>& s)
    {
      _inputs.at(channel) = s;
    }

    // Get output from a multi-output component on a given channel
    Word<N>& output(int channel)
    {
      return _outputs.at(channel);
    }

    void printValue()
    {
      for (auto o : _outputs) o.printValue();
    } 
     
  protected:
    std::vector<Word<N>> _inputs;
    std::vector<Word<N>> _outputs;
};


// Abstract base class for components that use N-bit words
// for input and output, but with single-bit control inputs.

template <int N>
class WordControlComponent : public WordComponent<N>
{
  public:
    // Constructor takes number of inputs, controls, and outputs
    WordControlComponent(int inputs, int controls, int outputs) : 
      WordComponent<N>(inputs, outputs), _controls(controls) {}
    
    // Change value of control bit on a channel
    // Channels are 0-indexed
    void control(int channel, Signal s)
    {
      _controls.at(channel) = s;
    }

  protected:
    std::vector<Signal> _controls;
};


#endif // COMPONENTS_HPP
