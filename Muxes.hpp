#ifndef MUXES_HPP
#define MUXES_HPP

#include "Gates.hpp"

class Multiplexer : public ControlComponent
{
  public:
    Multiplexer() : ControlComponent(2, 1, 1) {}
    
    void process()
    {
      Signal data0 = _inputs.at(0);
      Signal data1 = _inputs.at(1);
      Signal control = _controls.at(0);

      // Control -> Gate 0
      _gate0.input(0, control);
      _gate0.process();

      // Data 0, Control -> Gate 1
      _gate1.input(0, data0);
      _gate1.input(1, _gate0.output());
      _gate1.process();

      // Control, Data 1 -> Gate 2
      _gate2.input(0, control);
      _gate2.input(1, data1);
      _gate2.process();
    
      // Gate 1, Gate 2 -> Gate 3 
      _gate3.input(0, _gate1.output());
      _gate3.input(1, _gate2.output()); 
      _gate3.process();

      _outputs.at(0) = _gate3.output();
    }

  private:
    Inverter _gate0; 
    NAND _gate1, _gate2, _gate3;
};

// Multiplex an entire word at a time
// Parameterized on word size

template <int N> 
class WordMultiplexer : public WordControlComponent<N>
{
  public:
    WordMultiplexer() : 
      WordControlComponent<N>(2, 1, 1),
      _multiplexers(N) {}
    
    void process()
    {
      for (auto i = 0; i < N; ++i)  
      {
        Multiplexer& m = _multiplexers.at(i);
        m.input(0, this->_inputs.at(0).bit(i));
        m.input(1, this->_inputs.at(1).bit(i));
        m.control(0, this->_controls.at(0));
        m.process();
        this->_outputs.at(0).bit(i) = m.output();
      }
    }

  private:
    std::vector<Multiplexer> _multiplexers;
};

class Demultiplexer : public ControlComponent
{
  public:
    Demultiplexer() : ControlComponent(1, 1, 2) {}
    
    void process()
    {
      _gate0.input(0, _controls.at(0));
      _gate0.process();
      
      _gate1.input(0, _gate0.output());
      _gate1.input(1, _inputs.at(0));
      _gate1.process();

      _gate2.input(0, _inputs.at(0));
      _gate2.input(1, _controls.at(0));
      _gate2.process();

      _outputs.at(0) = _gate1.output();
      _outputs.at(1) = _gate2.output();
    }

  private: 
    Inverter _gate0;
    AND _gate1, _gate2; 
};

template <int N>
class WordDemultiplexer : public WordControlComponent<N>
{
  public:
    WordDemultiplexer() : 
      WordControlComponent<N>(1, 1, 2), _demultiplexers(N) {}
    
    void process()
    {
      for (auto i = 0; i < N; ++i)  
      {
        Demultiplexer& d = _demultiplexers.at(i);

        d.input(0, this->_inputs.at(0).bit(i));
        d.control(0, this->_controls.at(0));
        d.process();

        this->_outputs.at(0).bit(i) = d.output(0);
        this->_outputs.at(1).bit(i) = d.output(1);
      }
    }

  private:
    std::vector<Demultiplexer> _demultiplexers;
};

// Multiplex an arbitrary number of Signals

template <int N> // Parameterized on number of control bits
class Mux : public ControlComponent
{
  static_assert(N >= 1);

  public:
    Mux() : ControlComponent (N*N, N, 1) {}

    void process()
    {
      for (int i = 0; i < (N*N)/2; ++i)
      {
        _mux0.input(i, _inputs.at(i));
        _mux1.input(i, _inputs.at(i + (N*N)/2));
      }

      _mux0.process();
      _mux1.process();
      
      _mux2.input(0, _mux0.output());
      _mux2.input(1, _mux1.output());
      _mux2.process();
    }

  private:
    Mux<N/2> _mux0;
    Mux<N/2> _mux1;
    Multiplexer _mux2;
};

template <> 
class Mux<1> : public ControlComponent
{
  public:
    Mux() : ControlComponent(2, 1, 1) {}
    
    void process()
    {
      _mux0.input(0, _inputs.at(0));
      _mux0.input(1, _inputs.at(1)); 
      _mux0.control(0, _controls.at(0)); 
      _mux0.process();
    }

  private:
    Multiplexer _mux0;
};

// Multiplex an arbitrary number of Words

template <int N, int WordSize> // Parameterized on number of control bits
class WordMux : public WordControlComponent<WordSize>
{
  static_assert(N >= 1);

  public:
    WordMux() : WordControlComponent<WordSize> (N*N, N, 1) {}

    void process()
    {
      _mux0.input(0, this->_inputs.at(0));
      _mux0.input(1, this->_inputs.at(1));
      _mux0.control(0, this->_controls.at(N));
      _mux0.process();
      /*
      _mux1.input(0, this->_inputs.at(i + inputCount/2));
      _mux1.input(1, this->_inputs.at(i + inputCount/2));
      _mux1.control(0, this->_controls.at(N));
      
      _mux2.input(0, _mux0.output());
      _mux2.input(1, _mux1.output());
      _mux1.control(0, this->_controls.at(N-1));

      */
    }

  private:
    WordMux<N-1, WordSize> _mux0;
    WordMux<N-1, WordSize> _mux1;
    WordMultiplexer<WordSize> _mux2;
};

template <int WordSize> 
class WordMux<1, WordSize> : public WordControlComponent<WordSize>
{
  public:
    WordMux() : WordControlComponent<WordSize>(2, 1, 1) {}
    
    void process()
    {
      _mux0.input(0, this->_inputs.at(0));
      _mux0.input(1, this->_inputs.at(1)); 
      _mux0.control(0, this->_controls.at(0)); 
      _mux0.process();
    }

  private:
    WordMultiplexer<WordSize> _mux0;
};

#endif // MUXES_HPP
