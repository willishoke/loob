#include <iostream>
#include <assert.h>
#include "ALU.hpp"
#include "Memory.hpp"

void testInverter();
void testAND();
void testNAND();
void testOR();
void testXOR();

void testWordInverter();
void testWordNAND();
void testWordAND();
void testWordOR();
void testWordXOR();

void testMultiplexer();
void testWordMultiplexer();
void testDemultiplexer();
void testWordDemultiplexer();

void testHalfAdder();
void testFullAdder();
void testWordAdder();
void testWordMultiplier();

void testSRLatch();
void testFlipFlop();
void testWordMemory();
void testShiftLeft();

void testGates()
{
  testInverter();
  testNAND();
  testAND();
  testOR();
  testXOR();
}

void testBitwise()
{
  testWordInverter();
  testWordNAND();
  testWordAND();
  testWordOR();
  testWordXOR();
}

void testArithmetic()
{
  testHalfAdder();
  testFullAdder();
  testWordAdder();
  testWordMultiplier();
}

void testMultiplexers()
{
  testMultiplexer();
  testWordMultiplexer();
  testDemultiplexer();
  testWordDemultiplexer();
}

void testMemory()
{
  testSRLatch();
  testFlipFlop();
  testWordMemory();
  testShiftLeft();
}

void testAll()
{
  testGates();
  testBitwise();
  testArithmetic();
  testMultiplexers();
  testMemory();
}

int main(int argc, char** argv)
{
  testAll();
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


// BITWISE OPERATOR TESTS

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

void testDemultiplexer()
{
  Demultiplexer d;

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

void testWordDemultiplexer()
{
  WordDemultiplexer<4> d;

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
  WordMultiplier<8> m;
  Word<8> w0({0,0,0,0,1,0,1,1}); 
  Word<8> w1({0,0,0,0,0,0,1,1}); 
  Word<8> w2({0,0,1,0,0,0,0,1}); 

  // 00001011 * 00000011 = 00100001
  // (3 * 11 == 33)
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

void testShiftLeft()
{
  ShiftLeft<8> s;

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
