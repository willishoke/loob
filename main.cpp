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

int main(int argc, char** argv)
{
  testInverter();
  testNAND();
  testAND();
  testOR();
  testXOR();
  testMultiplexer();
  testFlipFlop();
  return 0;
}

void testMultiplexer()
{
  // (0, 0, 0) == 0
  Multiplexer m;
  m.input(0, 0);
  m.input(1, 0);
  m.control(0, 0);
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
  d.input(0, 1);
  d.control(0, 1);
  d.process();
  assert(d.output() == 1);

  // Input 0, Enable 1 
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
}

void testFullAdder()
{

}
