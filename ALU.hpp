#ifndef ALU_HPP
#define ALU_HPP

#include <vector>
#include <algorithm>
#include "Muxes.hpp"
#include "Memory.hpp"


// Add two bits, plus a carry out

class HalfAdder : public Component
{
  public:
    HalfAdder() : Component(2, 2) {}

    void process();

  private:
    XOR _gate0;
    AND _gate1;
};


// Add two bits, plus a carry in/out bit

class FullAdder : public Component
{
  public:
    FullAdder() : Component(3, 2) {}

    void process();

  private:
    HalfAdder _adder0, _adder1;
    OR _gate0;
};


// Add two N-bit words
// Two word inputs and a single word output

template <int N>
class WordAdder : public WordComponent<N>
{
  public:
    WordAdder() : WordComponent<N>(2, 1), _adders(N) {}
 
    void process();

  private:
    std::vector<FullAdder> _adders;
};


// Parameterized on word size
// Hard to explain the logic on this one, but I ended up making
// a lot of drawings. 

template <int N>
class WordMultiplier : public WordComponent<N>
{
  public:
    WordMultiplier() : 
      WordComponent<N>(2, 1),
      _gates((N*N+N)/2), // Geometric series!
      _adders(N-1) {}

    void process();

  private:
    std::vector<AND> _gates;
    std::vector<WordAdder<N>> _adders;
};

// Parameterized on word size
template <int N>
class ALU : public WordControlComponent<N>
{
  public:
    ALU() :
      WordControlComponent<N>(2, 2, 1) {}

    void process();

  private:
    WordAdder<N> _add;
    WordMultiplier<N> _mul;
    WordAND<N> _and;
    WordOR<N> _or;

    Nto1WordMultiplexer<2, N> _mux;
};


// PROCESS DEFINITIONS


void HalfAdder::process()
{
  Signal data0 = _inputs.at(0);
  Signal data1 = _inputs.at(1);
  
  // Data 0, Data 1 -> Gate 0
  _gate0.input(0, data0);
  _gate0.input(1, data1);
  _gate0.process();
   
  // Data 0, Data 1 -> Gate 1
  _gate1.input(0, data0);
  _gate1.input(1, data1);
  _gate1.process();

  _outputs.at(0) = _gate0.output();
  _outputs.at(1) = _gate1.output();
}

void FullAdder::process()
{
  Signal data0 = _inputs.at(0);
  Signal data1 = _inputs.at(1);
  Signal carry = _inputs.at(2);

  // Data 0, Data 1 -> Adder 0 
  _adder0.input(0, data0); 
  _adder0.input(1, data1); 
  _adder0.process();

  // Adder 0 Sum, Carry -> Adder 1
  _adder1.input(0, _adder0.output(0));
  _adder1.input(1, carry);
  _adder1.process();

  // Adder 0 Carry, Adder 1 Carry -> Gate 0
  _gate0.input(0, _adder0.output(1));
  _gate0.input(1, _adder1.output(1));
  _gate0.process();
 
  _outputs.at(0) = _adder1.output(0);
  _outputs.at(1) = _gate0.output();
}

template <int N>
void WordAdder<N>::process()
{
  Word<N>& word0 = this->_inputs.at(0);
  Word<N>& word1 = this->_inputs.at(1);
  Word<N>& result = this->_outputs.at(0);

  for (auto i = N-1; i >= 0; --i)
  {
    FullAdder& a = _adders.at(i); 

    a.input(0, word0.bit(i));
    a.input(1, word1.bit(i));
     
    if (i < N-1)
    {
      a.input(2, _adders.at(i+1).output(1));    
    }

    a.process(); 
    result.bit(i) = a.output(0);
  } 
} 

template <int N>
void WordMultiplier<N>::process()
{
  for (auto i = 0; i < N; ++i) // Rows
  {
    std::vector<Signal> v0(N, 0); // Init to zeroes

    for (auto j = N - 1; j >= i; --j) // Columns
    {
      // This turns a "square" index into a flattened "triangle"
      auto index = N*i+j - ((i*i+i)/2);
      AND& g = _gates.at(index);
      g.input(0,this->_inputs.at(0).bit(N-i-1));
      g.input(1,this->_inputs.at(1).bit(j));
      g.process();
      v0.at(j-i) = g.output(); // Fill result array 
    }

    // Create a word from the vector
    Word<N> w0(v0);

    // Adder 0 takes input from first set of partial products
    if (i == 0)
    {
      _adders.at(i).input(0, w0);
    } 
    // Other adders take input from previous adder
    else
    {
      _adders.at(i-1).input(1, w0);          
      _adders.at(i-1).process();
      Word<N> w = _adders.at(i-1).output();
      if (i + 1 == N) this->_outputs.at(0) = w;
      else _adders.at(i).input(0, w);
    }
  }
}

template <int N>
void ALU<N>::process()
{
  _add.input(0, this->_inputs.at(0));
  _add.input(1, this->_inputs.at(1));
  _add.process();

  _mul.input(0, this->_inputs.at(0));
  _mul.input(1, this->_inputs.at(1));
  _mul.process();

  _and.input(0, this->_inputs.at(0));
  _and.input(1, this->_inputs.at(1));
  _and.process();

  _or.input(0, this->_inputs.at(0));
  _or.input(1, this->_inputs.at(1));
  _or.process();

  _mux.input(0, _add.output());
  _mux.input(1, _mul.output());
  _mux.input(2, _and.output());
  _mux.input(3, _or.output());

  _mux.control(0, this->_controls.at(0));
  _mux.control(1, this->_controls.at(1));
  _mux.process();

  this->_outputs.at(0) = _mux.output();
}

// ARITHMETIC TESTS


void testHalfAdder()
{
  HalfAdder a;

  // 0 + 0 == (0, 0)
  a.input(0, 0);
  a.input(1, 0);
  a.process();
  assert(a.output(0) == 0);
  assert(a.output(1) == 0);

  // 0 + 1 == (1, 0)
  a.input(0, 0);
  a.input(1, 1);
  a.process();
  assert(a.output(0) == 1);
  assert(a.output(1) == 0);

  // 1 + 0 == (1, 0)
  a.input(0, 1);
  a.input(1, 0);
  a.process();
  assert(a.output(0) == 1);
  assert(a.output(1) == 0);

  // 1 + 1 == (0, 1)
  a.input(0, 1);
  a.input(1, 1);
  a.process();
  assert(a.output(0) == 0);
  assert(a.output(1) == 1);
}

void testFullAdder()
{
  FullAdder a;
  
  // 0 + 0 + 0 == (0, 0)
  a.input(0, 0);
  a.input(1, 0);
  a.input(2, 0);
  a.process();
  assert(a.output(0) == 0);
  assert(a.output(1) == 0);

  // 0 + 0 + 1 == (1, 0)
  a.input(0, 0);
  a.input(1, 0);
  a.input(2, 1);
  a.process();
  assert(a.output(0) == 1);
  assert(a.output(1) == 0);

  // 0 + 1 + 0 == (1, 0)
  a.input(0, 0);
  a.input(1, 1);
  a.input(2, 0);
  a.process();
  assert(a.output(0) == 1);
  assert(a.output(1) == 0);
  
  // 0 + 1 + 1 == (0, 1)
  a.input(0, 0);
  a.input(1, 1);
  a.input(2, 1);
  a.process();
  assert(a.output(0) == 0);
  assert(a.output(1) == 1);
  
  // 1 + 0 + 0 == (1, 0)
  a.input(0, 1);
  a.input(1, 0);
  a.input(2, 0);
  a.process();
  assert(a.output(0) == 1);
  assert(a.output(1) == 0);

  // 1 + 0 + 1 == (0, 1)
  a.input(0, 1);
  a.input(1, 0);
  a.input(2, 1);
  a.process();
  assert(a.output(0) == 0);
  assert(a.output(1) == 1);

  // 1 + 1 + 0 == (0, 1)
  a.input(0, 1);
  a.input(1, 1);
  a.input(2, 0);
  a.process();
  assert(a.output(0) == 0);
  assert(a.output(1) == 1);
  
  // 1 + 1 + 1 == (1, 1)
  a.input(0, 1);
  a.input(1, 1);
  a.input(2, 1);
  a.process();
  assert(a.output(0) == 1);
  assert(a.output(1) == 1);
}

void testWordAdder()
{
  // 0111 + 0010 == 1001
  WordAdder<4> a;

  Word<4> w0({0,1,1,1}); 
  Word<4> w1({0,0,1,0}); 
  a.input(0, w0);
  a.input(1, w1);
  a.process();
  assert(a.output() == Word<4>({1,0,0,1}));

  // 01001011 + 00000011 == 01001110
  WordAdder<8> b;
  Word<8> w2({0,1,0,0,1,0,1,1}); 
  Word<8> w3({0,0,0,0,0,0,1,1}); 
  b.input(0, w2);
  b.input(1, w3);
  b.process();
  assert(b.output() == Word<8>({0,1,0,0,1,1,1,0}));
}

void testWordMultiplier()
{
  WordMultiplier<4> m0;
  Word<4> w0({0,0,1,1}); 
  Word<4> w1({0,0,1,0}); 
  Word<4> w2({0,1,1,0}); 

  // 0011 * 0010 = 0110
  // (3 * 2 == 6)
  m0.input(0, w0);
  m0.input(1, w1);
  m0.process(); 
  assert(m0.output() == w2);

  WordMultiplier<8> m1;
  Word<8> w3({0,0,0,0,1,0,1,1}); 
  Word<8> w4({0,0,0,0,0,0,1,1}); 
  Word<8> w5({0,0,1,0,0,0,0,1}); 

  // 00001011 * 00000011 = 00100001
  // (3 * 11 == 33)
  m1.input(0, w3);
  m1.input(1, w4);
  m1.process(); 
  assert(m1.output() == w5);
}


// Run all tests on ALU components
void testArithmetic()
{
  testHalfAdder();
  testFullAdder();
  testWordAdder();
  testWordMultiplier();
}


#endif //ALU_HPP
