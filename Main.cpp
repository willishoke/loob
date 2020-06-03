#include <iostream>
#include <assert.h>

#include "ALU.hpp"
#include "Memory.hpp"

/*

11        0000      0000     1111
11       00  00    00  00    11  11
11       00  00    00  00    11111
11       00  00    00  00    11  11
111111    0000      0000     11111

             --loob--

A library for digital systems design

*/


void testAll()
{
  testGates();
  testArithmetic();
  testMultiplexers();
  testMemory();
}

int main(int argc, char** argv)
{
  testAll();

  ALU<16> a;

  Word<16> w0({0,0,0,0,0,0,0,0,0,1,0,0,1,0,1,1}); 
  Word<16> w1({0,0,0,0,0,0,0,0,1,0,1,0,1,1,0,1}); 

  std::cout << "Word 0:  ", w0.printValue();
  std::cout << "Word 1:  ", w1.printValue();   

  a.input(0, w0);
  a.input(1, w1);

  a.control(0, 0);
  a.control(1, 0);
  a.process();
  std::cout << "Sum:     ", a.printValue(); 

  a.control(0, 0);
  a.control(1, 1);
  a.process();
  std::cout << "Product: ", a.printValue(); 

  a.control(0, 1);
  a.control(1, 0);
  a.process();
  std::cout << "AND:     ", a.printValue(); 

  a.control(0, 1);
  a.control(1, 1);
  a.process();
  std::cout << "OR:      ", a.printValue(); 
}


// END
