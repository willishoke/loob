#include <vector>
#include <cmath>
#include "Muxes.hpp"

// Data (D) is input 0
// Enable (Clk) is input 1
// Value (Q) is output 0

class DFlipFlop : public ControlComponent
{
  public:
    DFlipFlop() : ControlComponent(1, 1, 1) {}

    void process()
    {
      Signal data = _inputs.at(0);
      Signal clk = _controls.at(0);
 
      // Data -> Gate 0 (inverter)
      _gate0.listen(0, data);

      // Data, Clk -> Gate 1 
      _gate1.listen(0, data);
      _gate1.listen(1, clk);

      // Clock, Gate 0 -> Gate 2
      _gate2.listen(0, clk);
      _gate2.listen(1, _gate0.output());

      // Gate 1 -> Gate 3
      _gate3.listen(0, _gate1.output());

      // Gate 2 -> Gate 4
      _gate4.listen(0, _gate3.output());

      // Gate 4 -> Gate 3
      _gate3.listen(1, _gate4.output());

      // Gate 3 -> Gate 4, Output 0
      _gate4.listen(1, _gate3.output());
      _outputs.at(0) = _gate3.output();
    
    }
  private:
    Inverter _gate0;
    NAND _gate1, _gate2, _gate3, _gate4;
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
      
      // Data 0, Data 1 -> Gate 0
      _gate0.listen(0, data0);
      _gate0.listen(1, data1);
       
      // Carry, Gate 0 -> Gate 1
      _gate1.listen(0, carry);
      _gate1.listen(1, _gate0.output());

      // Data 0, Data 1 -> Gate 2
      _gate2.listen(0, data0);
      _gate2.listen(1, data1);

      // Gate 0, Carry -> Gate 3
      _gate3.listen(0, _gate0.output());
      _gate3.listen(1, carry);

      // Gate 1, Gate 2 -> Gate 4 
      _gate4.listen(0, _gate1.output());
      _gate4.listen(1, _gate2.output());

      _outputs.at(0) = _gate3.output();
      _outputs.at(1) = _gate4.output();
    }

  private:
    XOR _gate0, _gate3; 
    AND _gate1, _gate2;
    OR _gate4;
};

class Word8Adder : public Word8Component
{
  public:
    Word8Adder(unsigned int n) : Word8Component(2, 1), _adders(n) {}
 
    void process()
    {
      for (auto i = 0; i < 8; ++i)
      {
        FullAdder& a = _adders.at(i); 
        //a.listen(0, _inputs.at(i).output(i));
        //a.listen(1, _inputs.at(i+s).output(i));
        
        if (i > 0)
        {
           
        }
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
