#include <iostream>
#include <assert.h>
#include "ALU.hpp"
#include "Memory.hpp"

void testInverter();
void testNAND();
void testAND();
void testOR();
void testXOR();
void testMultiplexer();
void testFlipFlop();
void testFullAdder();
void testWordAdder();
void testWordMultiplexer();
void testWordMemory();


void testAll()
{
  testInverter();
  testNAND();
  testAND();
  testOR();
  testXOR();
  testFlipFlop();
  testFullAdder();
  testWordAdder();
  testMultiplexer();
  testWordMultiplexer();
  testWordMemory();
}

int main(int argc, char** argv)
{
  testAll();
}


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

void testFlipFlop()
{
  FlipFlop d;

  // Input 1, Enable 1
  // Output should change to 1
  d.input(0, 1);
  d.control(0, 1);
  d.process();
  assert(d.output() == 1);

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
  m.printValue();
  assert(m.output() == w0);

  // Word 1, Enable 0 -> Q (Word 0)
  m.input(0, w1);
  m.control(0, 0);
  m.process();
  m.printValue();
  assert(m.output() == w0);

  // Word 0, Enable 0 -> Q (Word 0)
  m.input(0, w0);
  m.control(0, 0);
  m.process();
  m.printValue();
  assert(m.output() == w0);

  // Word 0, Enable 1 -> Word 0
  m.input(0, w0);
  m.control(0, 1);
  m.process();
  m.printValue();
  assert(m.output() == w0);

  // Word 1, Enable 1 -> Word 1 
  m.input(0, w1);
  m.control(0, 1);
  m.process();
  m.printValue();
  assert(m.output() == w1);
  
  // Word 1, Enable 0 -> Q (Word 1)
  m.input(0, w1);
  m.control(0, 0);
  m.process();
  m.printValue();
  assert(m.output() == w1);

  // Word 1, Enable 1 -> Word 1
  m.input(0, w1);
  m.control(0, 1);
  m.process();
  m.printValue();
  assert(m.output() == w1);

  // Word 0, Enable 1 -> Word 1
  m.input(0, w0);
  m.control(0, 1);
  m.process();
  m.printValue();
  assert(m.output() == w0);
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
  d.input(1, 0);
  d.control(0, 0);
  d.process();
  assert(d.output() == 0);
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
