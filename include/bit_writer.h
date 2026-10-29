#pragma once
#include <iosfwd>
#include<string>
using namespace std;
class bit_writer
{
public:
    bit_writer(ofstream& out)
        :m_out(out)
    {
    }
    void write_bit(bool bit)
    {
        m_buffer=(m_buffer << 1) | bit;
        m_filled_bits++;
        if (m_filled_bits==8)
        {
            m_out.put(m_buffer);
            m_buffer=0;
            m_filled_bits=0;
        }
    }
    void write_code(string& code)
    {
        for (char bit : code)
        {
            write_bit(bit == '1');
        }
    }
    void finish()
    {
        if (m_filled_bits>0)
        {
            m_buffer <<=(8-m_filled_bits);
            m_out.put(m_buffer);
            m_buffer=0;
            m_filled_bits=0;
        }
    }


private:
    ofstream& m_out;
    char m_buffer=0;
    int m_filled_bits=0;
};