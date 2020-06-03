#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <vector>
#include <stdint.h>
#include <iostream>
#include <algorithm>

// Signal alias
// Bool would work too, but this lets us use 1's and 0's
// and avoids the problems associated with vector<bool>.

using Signal = uint8_t;

// Component is an abstract base class.
// Components have inputs and outputs.
// An external object can call input to update a component's input.
// Anytime input is called, the process method will be called to
// update the component's output value(s).

class Component
{
  public:
    Component(int inputs, int outputs) :
      _inputs(inputs), _outputs(outputs) {}

    // Process propogates a Signal through the component to its output
    virtual void process() = 0;
    
    void input(int channel, Signal s)
    {
      _inputs.at(channel) = s;
    }

    // Get output from a component with a single output
    Signal output() const
    {
      return _outputs.at(0);
    }

    // Get output from a multi-output component on a given channel
    Signal output(int channel) const
    {
      return _outputs.at(channel);
    }

    void printValue() const
    {
      for (auto o : _outputs) std::cout << (int) o;
      std::cout << std::endl;
    }

  protected:
    // These are for Signals.
    // They represent the last known state of inputs and outputs.
    std::vector<Signal> _inputs;
    std::vector<Signal> _outputs;
};

class ControlComponent : public Component
{
  public:
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

// Big-endian words

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

template <int N>
class WordComponent
{
  public:
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

template <int N>
class WordControlComponent : public WordComponent<N>
{
  public:
    WordControlComponent(int inputs, int controls, int outputs) : 
      WordComponent<N>(inputs, outputs), _controls(controls) {}

    void control(int channel, Signal s)
    {
      _controls.at(channel) = s;
    }

  protected:
    std::vector<Signal> _controls;
};

#endif // COMPONENTS_HPP
