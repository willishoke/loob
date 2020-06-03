#ifndef MUXES_HPP
#define MUXES_HPP

#include "Gates.hpp"
#include <cmath>


// Simple 2 to 1 multiplexer

class Multiplexer : public ControlComponent
{
  public:
    Multiplexer() : ControlComponent(2, 1, 1) {}
    
    void process();

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
    
    void process();

  private:
    std::vector<Multiplexer> _multiplexers;
};


// N to 1 multiplexer

template <int M> // Parameterized on number of control bits
class Nto1Multiplexer : public ControlComponent
{
  public:
    Nto1Multiplexer() : 
      ControlComponent (pow(2, M), M, 1),
      _muxes(pow(2, M) - 1) {}

    void process();

  private:
    std::vector<Multiplexer> _muxes;
};


// 2^M to 1 multiplexer for N bit words
// M is number of control bits
// N is word size

template <int M, int N>
class Nto1WordMultiplexer : public WordControlComponent<N>
{
  public:
    Nto1WordMultiplexer() : 
      WordControlComponent<N> (pow(2, M), M, 1),
      _muxes(pow(2, M) - 1) {}

    void process();

  private:
    // Tree of multiplexers stored in a vector
    std::vector<WordMultiplexer<N>> _muxes;
};


// Simple 1 to 2 decoder

class Decoder : public ControlComponent
{
  public:
    Decoder() : ControlComponent(1, 1, 2) {}
    
    void process();

  private: 
    Inverter _gate0;
    AND _gate1, _gate2; 
};


// 1 to 2 decoder for N-bit word

template <int N>
class WordDecoder : public WordControlComponent<N>
{
  public:
    WordDecoder() : 
      WordControlComponent<N>(1, 1, 2), _demultiplexers(N) {}
    
    void process();

  private:
    std::vector<Decoder> _demultiplexers;
};


void Multiplexer::process()
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


template <int N>
void WordMultiplexer<N>::process()
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


template <int M>
void Nto1Multiplexer<M>::process()
{
  // Reverse iterate to propogate forward.
  for (auto i = pow(2, M) - 2; i >= 0; --i)
  {
    int height = floor(log2(i+1));
    //std::cout << "i:" << i << " height: " << height << " M: " << M << std::endl;
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


template <int M, int N>
void Nto1WordMultiplexer<M, N>::process()
{
  // Reverse iterate to propogate forward.
  for (auto i = pow(2, M) - 2; i >= 0; --i)
  {
    int height = floor(log2(i+1));
    // std::cout << "i:" << i << " height: " << height << " M: " << M << std::endl;
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


void Decoder::process()
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


template <int N>
void WordDecoder<N>::process()
{
  for (auto i = 0; i < N; ++i)  
  {
    Decoder& d = _demultiplexers.at(i);

    d.input(0, this->_inputs.at(0).bit(i));
    d.control(0, this->_controls.at(0));
    d.process();

    this->_outputs.at(0).bit(i) = d.output(0);
    this->_outputs.at(1).bit(i) = d.output(1);
  }
}


// MULTIPLEXER TESTS

void testMultiplexer()
{
  Multiplexer m;

  // (0, 0, 0) == 0
  m.input(0, 0);
  m.input(1, 0);
  m.control(0, 0);
  m.process();
  assert(m.output() == 0);

  // (0, 0, 1) == 0
  m.input(0, 0);
  m.input(1, 0);
  m.control(0, 1);
  m.process();
  assert(m.output() == 0);

  // (0, 1, 0) == 0
  m.input(0, 0);
  m.input(1, 1);
  m.control(0, 0);
  m.process();
  assert(m.output() == 0);

  // (0, 1, 1) == 1
  m.input(0, 0);
  m.input(1, 1);
  m.control(0, 1);
  m.process();
  assert(m.output() == 1);

  // (1, 0, 0) == 1
  m.input(0, 1);
  m.input(1, 0);
  m.control(0, 0);
  m.process();
  assert(m.output() == 1);

  // (1, 0, 1) == 0
  m.input(0, 1);
  m.input(1, 0);
  m.control(0, 1);
  m.process();
  assert(m.output() == 0);

  // (1, 1, 0) == 1
  m.input(0, 1);
  m.input(1, 1);
  m.control(0, 0);
  m.process();
  assert(m.output() == 1);

  // (1, 1, 1) == 1
  m.input(0, 1);
  m.input(1, 1);
  m.control(0, 1);
  m.process();
  assert(m.output() == 1);
}

void testNto1Multiplexer()
{
  Nto1Multiplexer<3> m;
  
  m.input(0, 1);
  m.input(1, 1);
  m.input(2, 0);
  m.input(3, 1); 
  m.input(4, 1); 
  m.input(5, 1); 
  m.input(6, 1); 
  m.input(7, 1); 

  m.control(0, 0);
  m.control(1, 1);
  m.control(2, 0);
  
  m.process();
  assert(m.output() == 0);

  m.input(2, 1);
  m.process();
  assert(m.output() == 1);
}

void testDecoder()
{
  Decoder d;

  // Input 0, Control 0 = (0, 0)
  d.input(0, 0);
  d.control(0, 0);
  d.process();
  assert(d.output(0) == 0);
  assert(d.output(1) == 0);

  // Input 0, Control 1 = (0, 0)
  d.input(0, 0);
  d.control(0, 1);
  d.process();
  assert(d.output(0) == 0);
  assert(d.output(1) == 0);

  // Input 1, Control 0 = (1, 0)
  d.input(0, 1);
  d.control(0, 0);
  d.process();
  assert(d.output(0) == 1);
  assert(d.output(1) == 0);

  // Input 1, Control 1 = (0, 1)
  d.input(0, 1);
  d.control(0, 1);
  d.process();
  assert(d.output(0) == 0);
  assert(d.output(1) == 1);
}

void testWordMultiplexer()
{
  WordMultiplexer<4> m;

  Word<4> w0({0,1,1,1}); 
  Word<4> w1({0,0,1,0}); 

  m.input(0, w0);
  m.input(1, w1);

  // Control 0 should return Word 0
  m.control(0, 0);
  m.process();
  assert(m.output() == w0);

  // Control 1 should return Word 1
  m.control(0, 1);
  m.process();
  assert(m.output() == w1);
}

void testWordDecoder()
{
  WordDecoder<4> d;

  Word<4> w0({0,1,1,1}); 
  Word<4> zeroes({0,0,0,0}); 

  d.input(0, w0);
  // Control 0 -> Word 0, Zeroes
  d.control(0, 0);
  d.process();
  assert(d.output(0) == w0);
  assert(d.output(1) == zeroes);

  // Control 1 -> Zeroes, Word 0
  d.control(0, 1);
  d.process();
  assert(d.output(0) == zeroes);
  assert(d.output(1) == w0);
}

void testNto1WordMultiplexer()
{
  Nto1WordMultiplexer<3, 4> m;

  Word<4> w0({0,0,0,0}); 
  Word<4> w1({0,0,0,1}); 
  Word<4> w2({0,0,1,0}); 
  Word<4> w3({0,0,1,1}); 
  Word<4> w4({0,1,0,0}); 
  Word<4> w5({0,1,0,1}); 
  Word<4> w6({0,1,1,0}); 
  Word<4> w7({0,1,1,1}); 

  m.input(0, w0); 
  m.input(1, w1); 
  m.input(2, w2); 
  m.input(3, w3); 
  m.input(4, w4); 
  m.input(5, w5); 
  m.input(6, w6); 
  m.input(7, w7); 

  m.control(0, 0);
  m.control(1, 0);
  m.control(2, 0);
  m.process();  
  assert(m.output() == w0);

  m.control(0, 0);
  m.control(1, 0);
  m.control(2, 1);
  m.process();  
  assert(m.output() == w1);

  m.control(0, 0);
  m.control(1, 1);
  m.control(2, 0);
  m.process();  
  assert(m.output() == w2);

  m.control(0, 0);
  m.control(1, 1);
  m.control(2, 1);
  m.process();  
  assert(m.output() == w3);

  m.control(0, 1);
  m.control(1, 0);
  m.control(2, 0);
  m.process();  
  assert(m.output() == w4);

  m.control(0, 1);
  m.control(1, 0);
  m.control(2, 1);
  m.process();  
  assert(m.output() == w5);

  m.control(0, 1);
  m.control(1, 1);
  m.control(2, 0);
  m.process();  
  assert(m.output() == w6);

  m.control(0, 1);
  m.control(1, 1);
  m.control(2, 1);
  m.process();  
  assert(m.output() == w7);
}


// Run all tests
void testMultiplexers()
{
  testMultiplexer();
  testNto1Multiplexer();
  testWordMultiplexer();
  testNto1WordMultiplexer();
  testDecoder();
  testWordDecoder();
}


#endif // MUXES_HPP
