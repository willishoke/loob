#ifndef ALU_HPP
#define ALU_HPP

#include <vector>
#include <cmath>
#include "Muxes.hpp"
#include "Memory.hpp"

class HalfAdder : public Component
{
  public:
    HalfAdder() : Component(2, 2) {}

    void process()
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

  private:
    XOR _gate0;
    AND _gate1;
};

class FullAdder : public Component
{
  public:
    FullAdder() : Component(3, 2) {}

    void process()
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

  private:
    HalfAdder _adder0, _adder1;
    OR _gate0;
};

template <int N>
class WordAdder : public WordComponent<N>
{
  public:
    WordAdder() : WordComponent<N>(2, 1), _adders(N) {}
 
    void process()
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

    void process()
    {
      for (auto i = 0; i < N; ++i) // Rows
      {
        for (auto j = i; j < N; ++j) // Columns
        {
          auto index = N*i+j;
          AND& g = _gates.at(index);
          g.input(0,this->_inputs.at(0).bit(j));
          g.input(1,this->_inputs.at(1).bit(j));
          g.process();
        }
      }
    }
  private:
    // These calculate partial products
    std::vector<AND> _gates;
    std::vector<WordAdder<N>> _adders;
};

class ALU : public Component
{
  public:
    void process()
    {
    }
  private:
};

class CPU : public Component
{
  public:
    void process()
    {
    }
  private:
    ALU alu;
};

#endif //ALU_HPP
