#ifndef GATES_HPP
#define GATES_HPP

#include "Components.hpp"


// Everything is built from NAND gates.
// NAND gates are the only gate that directly manipulate bits.

class NAND : public Component
{
  public:
    // NAND has two inputs and a single output
    NAND() : Component(2, 1) {}

    void process();
};


// An inverter can be constructed by connecting both a signal
// to both inputs of a NAND gate.

// ~A = ~(A && A)

class Inverter : public Component
{
  public:
    // Inverter has a single input and output
    Inverter() : Component(1, 1) {}

    void process();

  private:
    NAND _gate0;
};

// A && B = ~(~(A && B))

class AND : public Component
{
  public:
    AND() : Component(2, 1) {}

    void process();

  private:
    NAND _gate0, _gate1;
};


// A || B = ~(~A && ~B)

class OR : public Component
{
  public:
    OR() : Component(2, 1) {}

    void process();

  private:
    NAND _gate0, _gate1, _gate2;
};


// A ⊕ B 

class XOR : public Component
{
  public:
    XOR() : Component(2, 1) {}

    void process();

  private:
    NAND _gate0, _gate1, _gate2, _gate3;
};


template <int N>
class WordInverter : public WordComponent<N>
{
  public:
    WordInverter() : 
      WordComponent<N>(1, 1), 
      _inverters(N) {}
  
    void process();

  private:
    std::vector<Inverter> _inverters;
}; 


template <int N>
class WordNAND : public WordComponent<N>
{
  public:
    WordNAND() : 
      WordComponent<N>(2, 1), 
      _gates(N) {}
  
    void process();

  private:
    std::vector<NAND> _gates;
}; 


template <int N>
class WordAND : public WordComponent<N>
{
  public:
    WordAND() : 
      WordComponent<N>(2, 1), 
      _gates(N) {}
  
    void process();

  private:
    std::vector<AND> _gates;
}; 


template <int N>
class WordOR : public WordComponent<N>
{
  public:
    WordOR() : 
      WordComponent<N>(2, 1), 
      _gates(N) {}
  
    void process();

  private:
    std::vector<OR> _gates;
}; 


template <int N>
class WordXOR : public WordComponent<N>
{
  public:
    WordXOR() : 
      WordComponent<N>(2, 1), 
      _gates(N) {}
  
    void process();

  private:
    std::vector<XOR> _gates;
}; 


void NAND::process()
{
  bool in1 = _inputs.at(0);
  bool in2 = _inputs.at(1);

  _outputs.at(0) = in1 && in2 ? 0 : 1;
}


void Inverter::process()
{
  Signal in0 = _inputs.at(0);

  _gate0.input(0, in0);
  _gate0.input(1, in0);
  _gate0.process();

  _outputs.at(0) = _gate0.output();      
}


void AND::process()
{
  Signal in0 = _inputs.at(0);
  Signal in1 = _inputs.at(1);

  _gate0.input(0, in0);
  _gate0.input(1, in1);
  _gate0.process();

  _gate1.input(0, _gate0.output());
  _gate1.input(1, _gate0.output());
  _gate1.process();

  _outputs.at(0) = _gate1.output();
}


void OR::process()
{
  Signal input0 = _inputs.at(0);
  Signal input1 = _inputs.at(1);

  // Input 0 -> Gate 0
  _gate0.input(0, input0);
  _gate0.input(1, input0);
  _gate0.process();

  // Input 1 -> Gate 1 
  _gate1.input(0, input1);
  _gate1.input(1, input1);
  _gate1.process();

  // Gate 0, Gate 1 -> Gate 2
  _gate2.input(0, _gate0.output());
  _gate2.input(1, _gate1.output());
  _gate2.process();

  _outputs.at(0) = _gate2.output();
}


void XOR::process()
{
  Signal input0 = _inputs.at(0);
  Signal input1 = _inputs.at(1);

  // Input 0, Input 1 -> Gate 0
  _gate0.input(0, input0);
  _gate0.input(1, input1);
  _gate0.process();

  // Input 0, Gate 0 -> Gate 1 
  _gate1.input(0, input0);
  _gate1.input(1, _gate0.output());
  _gate1.process();

  // Gate 0, Input 1 -> Gate 2
  _gate2.input(0, _gate0.output());
  _gate2.input(1, input1);
  _gate2.process();

  // Gate 1, Gate 2 -> Gate 3
  _gate3.input(0, _gate1.output());
  _gate3.input(1, _gate2.output());
  _gate3.process();

  _outputs.at(0) = _gate3.output();
}


template <int N>
void WordInverter<N>::process()
{
  for (auto i = 0; i < N; ++i)
  {
    Inverter& v = _inverters.at(i);
    v.input(0, this->_inputs.at(0).bit(i));
    v.process();
    this->_outputs.at(0).bit(i) = v.output();
  }
}


template <int N>
void WordNAND<N>::process()
{
  for (auto i = 0; i < N; ++i)
  {
    NAND& n = _gates.at(i);
    n.input(0, this->_inputs.at(0).bit(i));
    n.input(1, this->_inputs.at(1).bit(i));
    n.process();
    this->_outputs.at(0).bit(i) = n.output();
  }
}


template <int N>
void WordAND<N>::process()
{
  for (auto i = 0; i < N; ++i)
  {
    AND& n = _gates.at(i);
    n.input(0, this->_inputs.at(0).bit(i));
    n.input(1, this->_inputs.at(1).bit(i));
    n.process();
    this->_outputs.at(0).bit(i) = n.output();
  }
}


template <int N>
void WordOR<N>::process()
{
  for (auto i = 0; i < N; ++i)
  {
    OR& o = _gates.at(i);
    o.input(0, this->_inputs.at(0).bit(i));
    o.input(1, this->_inputs.at(1).bit(i));
    o.process();
    this->_outputs.at(0).bit(i) = o.output();
  }
}


template <int N>
void WordXOR<N>::process()
{
  for (auto i = 0; i < N; ++i)
  {
    XOR& x = _gates.at(i);
    x.input(0, this->_inputs.at(0).bit(i));
    x.input(1, this->_inputs.at(1).bit(i));
    x.process();
    this->_outputs.at(0).bit(i) = x.output();
  }
}


// GATE TESTS

void testInverter()
{
  Inverter i;

  // ~0 == 1
  i.input(0,0);
  i.process();
  assert(i.output() == 1);

  // ~1 == 0
  i.input(0,1);
  i.process();
  assert(i.output() == 0);
}

void testAND()
{
  AND a;

  // 0 && 0 == 0
  a.input(0,0);
  a.input(1,0);
  a.process();
  assert(a.output() == 0);

  // 0 && 1 == 0
  a.input(0, 0);
  a.input(1, 1);
  a.process();
  assert(a.output() == 0);
  
  // 1 && 0 == 0
  a.input(0,1);
  a.input(1,0);
  a.process();
  assert(a.output() == 0);

  // 1 && 1 == 1
  a.input(0, 1);
  a.input(1, 1);
  a.process();
  assert(a.output() == 1);
}

void testNAND()
{
  NAND n;

  // ~(0 && 0) == 1
  n.input(0, 0);
  n.input(1, 0);
  n.process();
  assert(n.output() == 1);

  // ~(1 && 0) == 1
  n.input(0, 1);
  n.input(1, 0);
  n.process();
  assert(n.output() == 1);

  // ~(0 && 1) == 1
  n.input(0, 0);
  n.input(1, 1);
  n.process();
  assert(n.output() == 1);

  // ~(1 && 1) == 0
  n.input(0, 1);
  n.input(1, 1);
  n.process();
  assert(n.output() == 0);
}

void testOR()
{
  OR o;

  // 0 || 0 == 0
  o.input(0, 0);
  o.input(1, 0);
  o.process();
  assert(o.output() == 0);

  // 0 || 1 == 1
  o.input(0, 0);
  o.input(1, 1);
  o.process();
  assert(o.output() == 1);
  
  // 1 || 0 == 1
  o.input(0, 1);
  o.input(1, 0);
  o.process();
  assert(o.output() == 1);

  // 1 || 1 == 1
  o.input(0, 1);
  o.input(1, 1);
  o.process();
  assert(o.output() == 1);
}

void testXOR()
{
  XOR x;

  // 0 ⊕ 0 == 0
  x.input(0, 0);
  x.input(1, 0);
  x.process();
  assert(x.output() == 0);

  // 0 ⊕ 1 == 1
  x.input(0, 0);
  x.input(1, 1);
  x.process();
  assert(x.output() == 1);
  
  // 1 ⊕ 0 == 1
  x.input(0, 1);
  x.input(1, 0);
  x.process();
  assert(x.output() == 1);

  // 1 ⊕ 1 == 0
  x.input(0, 1);
  x.input(1, 1);
  x.process();
  assert(x.output() == 0);
}

void testWordInverter()
{
  WordInverter<8> i;

  Word<8> w0({0,1,0,0,1,0,1,1}); 
  Word<8> w1({1,0,1,1,0,1,0,0}); 
  
  // ~(01001011) == 10110100
  i.input(0, w0);
  i.process(); 
  assert(i.output() == w1);
}

void testWordNAND()
{
  WordNAND<8> n;

  Word<8> w0({0,1,0,0,1,0,1,1}); 
  Word<8> w1({1,0,1,0,1,1,0,1}); 
  Word<8> w2({1,1,1,1,0,1,1,0}); 

  // ~(01001011 & 10101101) == 11110110
  n.input(0, w0);
  n.input(1, w1);
  n.process(); 
  assert(n.output() == w2);
}

void testWordAND()
{
  WordAND<8> a;

  Word<8> w0({0,1,0,0,1,0,1,1}); 
  Word<8> w1({1,0,1,0,1,1,0,1}); 
  Word<8> w2({0,0,0,0,1,0,0,1}); 

  // 01001011 & 10101101 == 00001001
  a.input(0, w0);
  a.input(1, w1);
  a.process(); 
  assert(a.output() == w2);
}

void testWordOR()
{
  WordOR<8> o;

  Word<8> w0({0,1,0,0,1,0,1,1}); 
  Word<8> w1({1,0,1,0,1,1,0,1}); 
  Word<8> w2({1,1,1,0,1,1,1,1}); 

  // 01001011 | 10101101 == 11101111
  o.input(0, w0);
  o.input(1, w1);
  o.process(); 
  assert(o.output() == w2);
}

void testWordXOR()
{
  WordXOR<8> x;

  Word<8> w0({0,1,0,0,1,0,1,1}); 
  Word<8> w1({1,0,1,0,1,1,0,1}); 
  Word<8> w2({1,1,1,0,0,1,1,0}); 

  // 01001011 ^ 10101101 == 11100110
  x.input(0, w0);
  x.input(1, w1);
  x.process(); 
  assert(x.output() == w2);
}


// Run all gate tests
void testGates()
{
  testInverter();
  testNAND();
  testAND();
  testOR();
  testXOR();
  testWordInverter();
  testWordNAND();
  testWordAND();
  testWordOR();
  testWordXOR();
}


#endif // GATES_HPP
