#ifndef MUXES_HPP
#define MUXES_HPP

#include "Gates.hpp"
#include <cmath>

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

template <int M> // Parameterized on number of control bits
class Mux : public ControlComponent
{
  public:
    Mux() : 
      ControlComponent (pow(2, M), M, 1),
      _muxes(pow(2, M) - 1) {}

    void process()
    {
      // Reverse iterate to propogate forward.
      for (auto i = pow(2, M) - 2; i >= 0; --i)
      {
        int height = floor(log2(i+1));
        std::cout << "i:" << i << " height: " << height << " M: " << M << std::endl;
        auto& m = _muxes.at(i);
        
        // "Leaf node"
        if (height == M-1)
        {
          int x = 2 * (i - (pow(2, height) - 1));
          m.input(0, this->_inputs.at(x));
          m.input(1, this->_inputs.at(x+1));
        }
        else
        {
          int x = 2 * (i + 1);
          m.input(0, _muxes.at(x-1).output());
          m.input(1, _muxes.at(x).output());
        }

        m.control(0, this->_controls.at(height));
        m.process();
      }

      this->_outputs.at(0) = _muxes.at(0).output();
    }

  private:
    std::vector<Multiplexer> _muxes;
};


// Multiplex an arbitrary number of Words

template <int M, int N> // Parameterized on number of control bits
class WordMux : public WordControlComponent<N>
{
  public:
    WordMux() : 
      WordControlComponent<N> (pow(2, M), M, 1),
      _muxes(pow(2, M) - 1) {}

    void process()
    {
      // Reverse iterate to propogate forward.
      for (auto i = pow(2, M) - 2; i >= 0; --i)
      {
        int height = floor(log2(i+1));
        std::cout << "i:" << i << " height: " << height << " M: " << M << std::endl;
        auto& m = _muxes.at(i);
        
        // "Leaf node"
        if (height == M-1)
        {
          int x = 2 * (i - (pow(2, height) - 1));
          m.input(0, this->_inputs.at(x));
          m.input(1, this->_inputs.at(x+1));
        }
        else
        {
          int x = 2 * (i + 1);
          m.input(0, _muxes.at(x-1).output());
          m.input(1, _muxes.at(x).output());
        }

        m.control(0, this->_controls.at(height));
        m.process();
      }

      this->_outputs.at(0) = _muxes.at(0).output();
    }

  private:
    std::vector<WordMultiplexer<N>> _muxes;
};


#endif // MUXES_HPP
