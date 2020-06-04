#include <assert.h>
#include <iostream>

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

void demoALU()
{
  std::cout << "\nALU TEST\n\n";
  ALU<16> a;

  Word<16> w0({0,0,0,0,0,0,0,0,0,1,0,0,1,0,1,1}); 
  Word<16> w1({0,0,0,0,0,0,0,0,1,0,1,0,1,1,0,1}); 

  std::cout << "Word 0:  ", w0.printValue();
  std::cout << "Word 1:  ", w1.printValue();   
  std::cout << std::endl;

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

void demoRAM()
{
  std::cout << "\n\nRAM TEST\n\n";

  RAM<5, 16> r;

  Word<16> w0({0,0,0,0,0,0,0,0,0,1,0,0,1,0,1,1}); 
  Word<16> w1({0,0,0,0,0,0,0,0,1,0,1,0,1,1,0,1}); 
  Word<16> w2({1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}); 
  Word<16> w3({1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0}); 

  std::cout << "Load 0000000001001011 into address 0000" << std::endl;
  
  r.input(0, w0);
  r.control(0, 1);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 0);
  r.control(4, 0);
  r.process();

  std::cout << "Load 0000000010101101 into address 0001" << std::endl;

  r.input(0, w1);
  r.control(0, 1);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 0);
  r.control(4, 1);
  r.process();

  std::cout << "Load 1111111111111111 into address 0010" << std::endl;

  r.input(0, w2);
  r.control(0, 1);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 1);
  r.control(4, 0);
  r.process();

  std::cout << "Load 1111111000000000 into address 0011" << std::endl;

  r.input(0, w3);
  r.control(0, 1);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 1);
  r.control(4, 1);
  r.process();

  std::cout << std::endl;

  std::cout << "Read address 0000" << std::endl;

  r.control(0, 0);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 0);
  r.control(4, 0);
  r.process();
  std::cout << "Value: ", r.printValue();

  std::cout << "Read address 0001" << std::endl;

  r.control(0, 0);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 0);
  r.control(4, 1);
  r.process();
  std::cout << "Value: ", r.printValue();

  std::cout << "Read address 0010" << std::endl;

  r.control(0, 0);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 1);
  r.control(4, 0);
  r.process();
  std::cout << "Value: ", r.printValue();

  std::cout << "Read address 0011" << std::endl;

  r.control(0, 0);
  r.control(1, 0);
  r.control(2, 0);
  r.control(3, 1);
  r.control(4, 1);
  r.process();
  std::cout << "Value: ", r.printValue();
  std::cout << std::endl;
}

int main(int argc, char** argv)
{
  testAll();
  demoALU();
  demoRAM();
}


// END
