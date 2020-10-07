#ifndef CA_HPP 
#define CA_HPP

#include "Memory.hpp"
#include "Muxes.hpp"

// CELLULAR AUTOMATON

// 8 rule bits + clock input
// Single word output

// Parameterized on word size
 
template <int WordSize> 
class CA : public WordControlComponent<WordSize>
{
  public:
    CA () :
      WordControlComponent<WordSize>(0, 9, 1),
      _muxes(WordSize) 
      { 
        std::vector<Signal> v(WordSize, 0);
        v.at(WordSize-1) = 1;
        _mem.input(0, v);
        _mem.control(0, 1);
        _mem.process();
        this->_outputs.at(0) = _mem.output();
      }

    void process()
    {
      auto& out = this->_outputs.at(0);
      std::vector<Signal> v(WordSize);

      for (auto i = 0; i < WordSize; ++i)
      {
        auto& first = !i ? out.bit(WordSize-1) : out.bit(i-1);
        auto& second = out.bit(i);
        auto& third = i == WordSize-1 ? out.bit(0) : out.bit(i+1);        

        auto& m = _muxes.at(i);

        for (auto j = 0; j < 8; ++j)
        {
          m.input(j, this->_controls.at(j));
        }

        m.control(0, first);
        m.control(1, second);
        m.control(2, third); 
        m.process();
        
        v.at(i) = m.output();      
      }

      Word<WordSize> w(v);

      _mem.input(0, w);
      _mem.control(0, this->_controls.at(8)); 
      _mem.process();

      this->_outputs.at(0) = _mem.output();
    } 

  private:
    WordMemory<WordSize> _mem;
    std::vector<Nto1Multiplexer<3>> _muxes;
};

#endif // CA_HPP
