#include <vector>
#include <stdint.h>

// Signal alias
// Bool would work too, but this lets us use 1's and 0's
// and avoids the problems associated with vector<bool>.

using Signal = uint8_t;

// Component is an abstract base class.
// Components have inputs and outputs.
// An external object can call listen to update a component's input.
// Anytime listen is called, the process method will be called to
// update the component's output value(s).

class Component
{
  public:
    Component(int inputs, int outputs) :
      _inputs(inputs, 0), _outputs(outputs, 0) {}

    // Process propogates a Signal through the component to its output
    virtual void process() = 0;

    // This happens ANY time ANY input is changed
    // Set input, then call process to update outputs
    void listen(int channel, Signal s)
    {
      _inputs.at(channel) = s;
      process(); 
    }

    // Get output from a component with a single output
    Signal output()
    {
      return _outputs.at(0);
    }

    // Get output from a multi-output component on a given channel
    Signal output(int channel)
    {
      return _outputs.at(channel);
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
    void controlListen(int channel, Signal s)
    {
      _controls.at(channel) = s;
      process();
    }

  protected:
    std::vector<Signal> _controls;
};

class Word8
{
  public:
    Word8() : _word(8) {}
    
    Signal bit(int position)
    {
      return _word.at(position);
    }
     
  private:
    std::vector<Signal> _word;
};

class Word8Component
{
  public:
    Word8Component(int inputs, int outputs) :
      _inputs(inputs), _outputs(outputs) {} 
    
    // Get output from a component with a single output
    const Word8& output()
    {
      return _outputs.at(0);
    }
    
    // Process propogates a Signal through the component to its output
    virtual void process() = 0;

    // This happens ANY time ANY input is changed
    // Set input, then call process to update outputs
    void listen(int channel, Word8& s)
    {
      _inputs.at(channel) = s;
      process(); 
    }

    // Get output from a multi-output component on a given channel
    const Word8& output(int channel)
    {
      return _outputs.at(channel);
    }
     
  protected:
    std::vector<Word8> _inputs;
    std::vector<Word8> _outputs;
};

class Word8ControlComponent : public Word8Component
{
  public:
    Word8ControlComponent(int inputs, int controls, int outputs) : 
      Word8Component(inputs, outputs), _controls(controls) {}

    void controlListen(int channel, Signal s)
    {
      _controls.at(channel) = s;
    }
  protected:
    std::vector<Signal> _controls;
};
