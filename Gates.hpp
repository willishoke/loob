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

template <int N>
class WordInverter : public WordComponent<N>
{
  public:
    WordInverter() : 
      WordComponent<N>(1, 1), 
      _inverters(N) {}
  
    void process()
    {
      for (auto i = 0; i < N; ++i)
      {
        Inverter& v = _inverters.at(i);
        v.input(0, this->_inputs.at(0).bit(i));
        v.process();
        this->_outputs.at(0).bit(i) = v.output();
      }
    }

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
  
    void process()
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
  
    void process()
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
  
    void process()
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
  
    void process()
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

#endif // GATES_HPP
