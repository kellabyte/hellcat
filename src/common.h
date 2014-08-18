#pragma once
#include <iostream>
#include <sstream>
#include <chrono>

using namespace std;
using namespace chrono;

class comma_numpunct : public std::numpunct<char>
{
protected:
    virtual char do_thousands_sep() const
    {
        return ',';
    }
    
    virtual std::string do_grouping() const
    {
        return "\03";
    }
};

void print_results(std::stringstream& output, uint records, nanoseconds elapsed);

void print_results(std::stringstream& output, uint records, nanoseconds elapsed)
{
    uint rate = records / (elapsed.count() / 1000000000.00);
    
    if (elapsed.count() >= 1000000000)
    {
        output << records << " at " << rate << "/sec in " << elapsed.count() / 1000000000.00 << "s" << endl;
    }
    else if (elapsed.count() >= 1000000)
    {
        output << records << " at " << rate << "/sec in " << elapsed.count() / 1000000.00 << "ms" << endl;
    }
    else
    {
        output << records << " at " << rate << "/sec in " << elapsed.count() << "ns" << endl;
    }
}
