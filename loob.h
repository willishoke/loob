#include <memory>
#include <vector>


// Basis for a simple sum type.
// A signal can be either high or low.

struct High {};
struct Low {}; 

// Forward declare this for Output
class Component;

class Signal 
{
  public:
    Signal() : _signal(Low()) {};
    Signal(High h) : _signal(High()) {};
    Signal(Low l) : _signal(Low()) {};
    bool high() const
    {
      return std::holds_alternative<High>(_signal);
    }
    bool low() const
    {
      return std::holds_alternative<Low>(_signal);
    }
  private:
    std::variant<High, Low> _signal;
};


// EXTERNAL stuff
// Interaction with outside world
// Each time listen is called, 

class Component
{
  public:
    Component(int inputs, int outputs) :
      _inputs(inputs), _outputs(outputs) {}

    // Listen is called by a preceding component

    virtual void process() = 0;

    // This happens ANY time ANY input is changed
    void listen(int channel, Signal s)
    {
      // Set input
      _inputs.at(channel) = s;

      // Check to see if all inputs have recieved a signal
      if (++_listenCount == _inputs.size())
      {
        // Call to process will update outputs
        process(); 
    
        // Now each of the components it's connected to get signalled
        for (auto c : _connections)
          {
            c._other->listen(c._channelOther, _outputs.at(c._channelSelf));
          }

        // Reset listen count
        _listenCount = 0;
      }
    }

  protected:
    // These are for signals.
    // They represent the last known state of inputs and outputs.
    std::vector<Signal> _inputs;
    std::vector<Signal> _outputs;

    // These are for connections.
    // Each output points to some other module's input.

    struct Connection
    {
      Component* _other;
      int _channelSelf; // The output from which we are broadcasting
      int _channelOther;  // The channel where receiving component listens
    };

    // All outgoing connections
    std::vector<Connection> _connections;
    
    // Use this to keep track of how many inputs have been updated
    int _listenCount; 
};

class NAND : public Component
{
  public:
    NAND() : Component(2, 1) {}
    // propogation is INTERNAL
    void process()
    {
      auto in1 = _inputs.at(0);
      auto in2 = _inputs.at(1);
      auto out = _outputs.at(0);
      if (in1.high() && in2.high())
        out = Low();
      else 
        out = High();
    }
};


class DFlipFlop : public Component
{
  public:
    DFlipFlop() : Component(2, 1), _gates(5) {}
    void process()
    {
    }
  private: 
    std::vector<NAND> _gates;
};


class Multiplexer : public Component
{
  public:
    void process()
    {
    }
  private:
    NAND gate1, gate2, gate3, gate4;
  
};


class Adder : public Component
{
  public:
    void process()
    {
    }
  private:

};

class Multiplier : public Component
{
  public:
    void process()
    {
    }
  private:

};

class ALU : public Component
{
  public:
    void process()
    {
    }
  private:
    Adder add;
    Multiplier mult;

};

class CPU : public Component
{
  public:
    void process()
    {
    }
  private:
    ALU alu;
};
