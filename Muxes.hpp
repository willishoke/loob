#include "Gates.hpp"

class Multiplexer : public ControlComponent
{
  public:
    Multiplexer() : ControlComponent(2, 1, 1) {}
    
    void process()
    {
      Signal data0 = _inputs.at(0);
      Signal data1 = _inputs.at(1);
      Signal control = _controls.at(0);

      // Control -> Gate 0
      _gate0.listen(0, control);

      // Data 0, Control -> Gate 1
      _gate1.listen(0, data0);
      _gate1.listen(1, control);

      // Control, Data 1 -> Gate 2
      _gate2.listen(0, control);
      _gate2.listen(1, data1);
    
      // Gate 1, Gate 2 -> Gate 3 
      _gate3.listen(0, _gate1.output());
      _gate3.listen(1, _gate2.output()); 

      _outputs.at(0) = _gate3.output();
    }

  private:
    Inverter _gate0; 
    NAND _gate1, _gate2, _gate3;
};

class Word8Multiplexer : public Word8ControlComponent
{
  public:
    Word8Multiplexer() : Word8ControlComponent(2, 1, 1) {}
    
    void process()
    {
      for (auto i = 0; i < 8; ++i)  
      {
        Multiplexer& m = _multiplexers.at(i);

        m.listen(i, _inputs.at(0).bit(i));
        m.listen(i, _inputs.at(1).bit(i));
        m.controlListen(i, _controls.at(0));
      }
    }

  private:
    std::vector<Multiplexer> _multiplexers;
};

template <int N> 
class Mux : public ControlComponent
{
  static_assert(N >= 2);

  // Size must be a power of 2
  static_assert(pow(2, (int) log2(N)) == (int) N);

  public:
    Mux() : ControlComponent (N, log2(N), 1) {}

    void process()
    {
      int inputCount = _inputs.size();
      for (int i = 0; i < inputCount/2; ++i)
      {
        _mux0.listen(i, _inputs.at(i));
        _mux1.listen(i, _inputs.at(i + inputCount/2));
      }
      
      _mux2.listen(0, _mux0.output());
      _mux2.listen(1, _mux1.output());
    }

  private:
    Mux<N/2> _mux0;
    Mux<N/2> _mux1;
    Multiplexer _mux2;
};

template <> 
class Mux<2> : public ControlComponent
{
  public:
    Mux() : ControlComponent(2, 1, 1) {}
    
    void process()
    {
      _mux0.listen(0, _inputs.at(0));
      _mux0.listen(1, _inputs.at(1)); 
      _mux0.controlListen(0, _controls.at(0)); 
    }

  private:
    Multiplexer _mux0;
};

template <int N> 
class Word8Mux : public Word8ControlComponent
{
  static_assert(N >= 2);

  // Size must be a power of 2
  static_assert(pow(2, (int) log2(N)) == (int) N);

  public:
    Word8Mux() : Word8ControlComponent (N, log2(N), 1) {}

    void process()
    {
      int inputCount = _inputs.size();
      for (int i = 0; i < inputCount/2; ++i)
      {
        _mux0.listen(i, _inputs.at(i));
        _mux1.listen(i, _inputs.at(i + inputCount/2));
      }
      
      _mux2.listen(0, _mux0.output());
      _mux2.listen(1, _mux1.output());
    }

  private:
    Word8Mux<N/2> _mux0;
    Word8Mux<N/2> _mux1;
    Word8Multiplexer _mux2;
};

template <> 
class Word8Mux<2> : public Word8ControlComponent
{
  public:
    Word8Mux() : Word8ControlComponent(2, 1, 1) {}
    
    void process()
    {
      _mux0.listen(0, _inputs.at(0));
      _mux0.listen(1, _inputs.at(1)); 
      _mux0.controlListen(0, _controls.at(0)); 
    }

  private:
    Word8Multiplexer _mux0;
};
