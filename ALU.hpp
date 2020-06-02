#ifndef ALU_HPP
#define ALU_HPP

#include <vector>
#include <cmath>
#include "Muxes.hpp"


class FullAdder : public Component
{
  public:
    FullAdder() : Component(3, 2) {}

    void process()
    {
      Signal data0 = _inputs.at(0);
      Signal data1 = _inputs.at(1);
      Signal carry = _inputs.at(2);
      
      // Data 0, Data 1 -> Gate 0
      _gate0.input(0, data0);
      _gate0.input(1, data1);
      _gate0.process();
       
      // Carry, Gate 0 -> Gate 1
      _gate1.input(0, carry);
      _gate1.input(1, _gate0.output());
      _gate1.process();

      // Data 0, Data 1 -> Gate 2
      _gate2.input(0, data0);
      _gate2.input(1, data1);
      _gate2.process();

      // Gate 0, Carry -> Gate 3
      _gate3.input(0, _gate0.output());
      _gate3.input(1, carry);
      _gate3.process();

      // Gate 1, Gate 2 -> Gate 4 
      _gate4.input(0, _gate1.output());
      _gate4.input(1, _gate2.output());
      _gate4.process();

      _outputs.at(0) = _gate3.output();
      _outputs.at(1) = _gate4.output();
    }

  private:
    XOR _gate0, _gate3; 
    AND _gate1, _gate2;
    OR _gate4;
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

class Multiplier : public Component
{
  public:
    void process()
    {
    }
  private:

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
