#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "Gates.hpp"
#include "Muxes.hpp"


// CLASS DECLARATIONS


// This is the NAND version used in D-type flip flops.
// Set Input 0, Reset Input 1
// Q Output 0, ~Q Output 1

class SRLatch : public Component
{
  public:
    SRLatch() : Component(2, 2) {}

    void process();

  private:
    NAND _gate0, _gate1;
};


// Data (D) is input 0
// Enable (Clk) is input 1
// Value (Q) is output 0

class FlipFlop : public ControlComponent
{
  public:
    FlipFlop() : ControlComponent(1, 1, 2) {}

    void process();

  private:
    Inverter _gate0;
    NAND _gate1, _gate2;
    SRLatch _latch0;
};


// A single word of memory.
// Parallel bank of flip flops sharing the same control signal.
// Template parameter is word size.

template <int N>
class WordMemory : public WordControlComponent<N>
{
  public:
    WordMemory() : 
      WordControlComponent<N>(1, 1, 1), 
      _flipflops(N) {}

    void process();

  private:
    std::vector<FlipFlop> _flipflops;
};


// M is number of control bits, N is word size
// Control bit 0 is reserved for read/write

template <int M, int N>
class RAM : public WordControlComponent<N>
{
  public:
    RAM() : 
      WordControlComponent<N>(1, M, 1),
      _words(pow(2, M-1)) {}
    
    void process();

  private:
    std::vector<WordMemory<N>> _words;
    Nto1WordMultiplexer<M-1, N> _multiplexer;
    OnetoNWordDemultiplexer<M-1, N> _demultiplexer0;
    OnetoNDemultiplexer<M-1> _demultiplexer1;
};


// Parallel in, parallel out shift register.
// This would be useful for building a bit shifter.
// The process() method will shift all elements to the left by one.
// First control input is write (0) / shift (1)
// Second control input is clock (enabled when high)

template <int N>
class ShiftRegister : public WordControlComponent<N>
{
  public:
    ShiftRegister() : 
      WordControlComponent<N>(1, 2, 1),
      _flipflops(N),
      _multiplexers(N) {}
 
    void process();

  private:
    std::vector<FlipFlop> _flipflops;
    std::vector<Multiplexer> _multiplexers;
};


// PROCESS DEFINITIONS

void SRLatch::process()
{
  Signal set = _inputs.at(0);
  Signal reset = _inputs.at(1);
  
  // Set, Reset -> Gate 0, Gate 1
  _gate0.input(0, set);
  _gate1.input(0, reset);

  // Non-simultaneous feedback to mimic real-life conditions.
  // If feedback is simultaneous, oscillation occurs.
  _gate0.input(1, _gate1.output());
  _gate0.process();
  _gate1.input(1, _gate0.output());
  _gate1.process();

  // Needs to be processed twice to force stable state.
  _gate0.input(1, _gate1.output());
  _gate0.process();
  _gate1.input(1, _gate0.output());
  _gate1.process();

  _outputs.at(0) = _gate0.output();
  _outputs.at(1) = _gate1.output();
}

void FlipFlop::process()
{
  Signal data = _inputs.at(0);
  Signal clk = _controls.at(0);
  
  // Data -> Gate 0 (inverter)
  _gate0.input(0, data);
  _gate0.process();

  // Data, Clk -> Gate 1 
  _gate1.input(0, data);
  _gate1.input(1, clk);
  _gate1.process();

  // Clock, Gate 0 -> Gate 2
  _gate2.input(0, clk);
  _gate2.input(1, _gate0.output());
  _gate2.process();

  // Gate 1, Gate 2 -> Latch 0
  _latch0.input(0, _gate1.output());
  _latch0.input(1, _gate2.output());
  _latch0.process();

  _outputs.at(0) = _latch0.output(0);
  _outputs.at(1) = _latch0.output(1);
}

template <int N>
void WordMemory<N>::process()
{
  for (auto i = 0; i < N; ++i)  
  {
    FlipFlop& f = _flipflops.at(i);
    f.input(0, this->_inputs.at(0).bit(i));
    f.control(0, this->_controls.at(0));
    f.process();
    this->_outputs.at(0).bit(i) = f.output();
  }
}


template <int M, int N>
void RAM<M, N>::process()
{
  for (auto i = 0; i < M-1; ++i)
  {
    // Demultiplexers and multiplexer share control bits 1..M-1
    _demultiplexer0.control(i, this->_controls.at(i+1));  
    _demultiplexer1.control(i, this->_controls.at(i+1));  
    _multiplexer.control(i, this->_controls.at(i+1));  
  }

  // This controls where the input is sent to
  _demultiplexer0.input(0, this->_inputs.at(0));
  _demultiplexer0.process();

  // This controls which word gets control signal
  _demultiplexer1.input(0, this->_controls.at(0));
  _demultiplexer1.process();

  for (auto i = 0; i < pow(2, M-1); ++i)
  {
    // Send demultiplexer output to memory
    _words.at(i).input(0, _demultiplexer0.output(i));
    _words.at(i).control(0, _demultiplexer1.output(i));
    _words.at(i).process(); // Noop if enable bit not set
    _multiplexer.input(i, _words.at(i).output());
  }
  
  _multiplexer.process();
  
  this->_outputs.at(0) = _multiplexer.output();
}


template <int N>
void ShiftRegister<N>::process()
{
  for (auto i = 0; i < N; ++i)
  {
    Multiplexer& m = _multiplexers.at(i);
    FlipFlop& f = _flipflops.at(i);

    m.input(0, this->_inputs.at(0).bit(i));

    if (i == N-1) // Last mux
    {
      // Fill in with zeroes
      m.input(1, 0);
    } 
    else
    {
      // Propogate value of next flip flop
      m.input(1, _flipflops.at(i+1).output());
    }

    m.control(0, this->_controls.at(0)); 
    m.process();
    
    f.input(0, m.output());
    f.control(0, this->_controls.at(1));
    f.process();

    this->_outputs.at(0).bit(i) = _flipflops.at(i).output();
  } 
} 


// MEMORY TESTS


void testSRLatch()
{
  SRLatch s;
 
  // Set 0, Reset 1 -> 1, 0 
  s.input(0, 0);
  s.input(1, 1);
  s.process();
  assert(s.output(0) == 1);
  assert(s.output(1) == 0);

  // Set 1, Reset 1 -> Outputs unchanged
  s.input(0, 1);
  s.input(1, 1);
  s.process();
  assert(s.output(0) == 1);
  assert(s.output(1) == 0);

  // Set 1, Reset 0 -> 0, 1
  s.input(0, 1);
  s.input(1, 0);
  s.process();
  assert(s.output(0) == 0);
  assert(s.output(1) == 1);

  // Set 1, Reset 1 -> Outputs unchanged
  s.input(0, 1);
  s.input(1, 1);
  s.process();
  assert(s.output(0) == 0);
  assert(s.output(1) == 1);

  // Set 0, Reset 1 -> 1, 0 
  s.input(0, 0);
  s.input(1, 1);
  s.process();
  assert(s.output(0) == 1);
  assert(s.output(1) == 0);
}

void testFlipFlop()
{
  FlipFlop d;

  // Input 1, Enable 1
  // Output should change to 1
  d.input(0, 1);
  d.control(0, 1);
  d.process();
  assert(d.output(0) == 1);

  // Input 0, Enable 1 
  // Output should change to 0
  d.input(0, 0);
  d.control(0, 1);
  d.process();
  assert(d.output() == 0);

  // Input 1, Enable 0
  // Output should remain 0
  d.input(0, 1);
  d.control(0, 0);
  d.process();
  assert(d.output() == 0);

  // Input 1, Enable 1
  // Output should change to 1
  d.input(0, 1);
  d.control(0, 1);
  d.process();
  assert(d.output() == 1);

  // Input 0, Enable 0
  // Output should remain 1
  d.input(0, 0);
  d.control(0, 0);
  d.process();
  assert(d.output() == 1);
  
  // Input 0, Enable 1
  // Output should change to 0
  d.input(0, 0);
  d.control(0, 1);
  d.process();
  assert(d.output() == 0);
}

void testWordMemory()
{
  WordMemory<4> m;

  Word<4> w0({0,1,1,1}); 
  Word<4> w1({0,0,1,0}); 

  // Word 0, Enable 1 -> Word 0
  m.input(0, w0);
  m.control(0, 1);
  m.process();
  assert(m.output() == w0);

  // Word 1, Enable 0 -> Q (Word 0)
  m.input(0, w1);
  m.control(0, 0);
  m.process();
  assert(m.output() == w0);

  // Word 0, Enable 0 -> Q (Word 0)
  m.input(0, w0);
  m.control(0, 0);
  m.process();
  assert(m.output() == w0);

  // Word 0, Enable 1 -> Word 0
  m.input(0, w0);
  m.control(0, 1);
  m.process();
  assert(m.output() == w0);

  // Word 1, Enable 1 -> Word 1 
  m.input(0, w1);
  m.control(0, 1);
  m.process();
  assert(m.output() == w1);
  
  // Word 1, Enable 0 -> Q (Word 1)
  m.input(0, w1);
  m.control(0, 0);
  m.process();
  assert(m.output() == w1);

  // Word 1, Enable 1 -> Word 1
  m.input(0, w1);
  m.control(0, 1);
  m.process();
  assert(m.output() == w1);

  // Word 0, Enable 1 -> Word 1
  m.input(0, w0);
  m.control(0, 1);
  m.process();
  assert(m.output() == w0);
}

void testRAM()
{
  RAM<5, 16> r;

  Word<16> w0({0,0,0,0,0,0,0,0,0,1,0,0,1,0,1,1}); 
  Word<16> w1({0,0,0,0,0,0,0,0,1,0,1,0,1,1,0,1}); 
  Word<16> w2({1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}); 
  Word<16> w3({1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0}); 
  
  r.input(0, w0);
  r.control(0, 1);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 0);
  r.control(4, 0);
  r.process();

  r.input(0, w1);
  r.control(0, 1);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 0);
  r.control(4, 1);
  r.process();

  r.input(0, w2);
  r.control(0, 1);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 1);
  r.control(4, 0);
  r.process();

  r.input(0, w3);
  r.control(0, 1);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 1);
  r.control(4, 1);
  r.process();

  r.control(0, 0);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 0);
  r.control(4, 0);
  r.process();
  assert(r.output() == w0);

  r.control(0, 0);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 0);
  r.control(4, 1);
  r.process();
  assert(r.output() == w1);

  r.control(0, 0);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 1);
  r.control(4, 0);
  r.process();
  assert(r.output() == w2);

  r.control(0, 0);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 1);
  r.control(4, 1);
  r.process();
  assert(r.output() == w3);
}

void testShiftRegister()
{
  ShiftRegister<8> s;

  Word<8> w0({0,1,0,0,1,0,1,1}); 
  Word<8> w1({1,0,0,1,0,1,1,0}); 
  Word<8> w2({0,0,1,0,1,1,0,0}); 

  // Input Word 0, Load, Clock 1 -> Word 0
  s.input(0, w0);
  s.control(0, 0);
  s.control(1, 1);
  s.process(); 
  assert(s.output() == w0);

  // Input Word 1, Load, Clock 0 -> Word 0
  s.input(0, w1);
  s.control(0, 0);
  s.control(1, 0);
  s.process(); 
  assert(s.output() == w0);
  
  // Input Word 0, Shift, Clock 1 -> Word 1
  s.input(0, w0);
  s.control(0, 1);
  s.control(1, 1);
  s.process(); 
  assert(s.output() == w1);

  // Input Word 0, Shift, Clock 1 -> Word 2
  s.input(0, w0);
  s.control(0, 1);
  s.control(1, 1);
  s.process(); 
  assert(s.output() == w2);
}


// Run all memory tests
void testMemory()
{
  testSRLatch();
  testFlipFlop();
  testWordMemory();
  testRAM();
  testShiftRegister();
}


#endif // MEMORY_HPP
