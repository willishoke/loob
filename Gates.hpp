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

// A ∧ B = ~(~(A ∧ B))

class AND : public Component
{
  public:
    AND() : Component(2, 1) {}

    void process();

  private:
    NAND _gate0, _gate1;
};

// A ∨ B = ~(~A ∧ ~B)

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

#endif // GATES_HPP
