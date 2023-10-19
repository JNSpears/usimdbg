/*
 * Trigger.cpp
 *
 *  Created on: Oct 12, 2023
 *      Author: james
 */
#include "string.h"

#include "Symbol.h"

using namespace std;

std::unordered_map<Word, std::string> sym_table;

// ==========================================================================================================

void addSymbol(Word addr, string symbol)
{
    sym_table[addr] = symbol;
}

void clearSymbols(void)
{
    sym_table.clear();
}

// ==========================================================================================================
const char* getSymbol(Word addr)
{
    try
    {
        return sym_table.at(addr).c_str();
    }
    catch (std::out_of_range const&)
    {
    }
    return NULL;
}

extern void yyerror(mc6809dbg cpu, TerminalDbg term, const char *msg);

Word getSymbolAddr(mc6809dbg cpu, TerminalDbg term, std::string symbol)
{
    for (auto x: sym_table)
        if (x.second == symbol)
            return x.first; //cout << hex << uppercase << setw(4) << setfill('0') << x.first << ' ' << x.second << endl;
    yyerror(cpu, term, "Symbol undefined.");
    return 0;
}





// #include <algorithm>
// #include <iterator>
// #include <vector>

// template <
//   class InputIterator,
//   class Functor>
// std::vector<typename std::iterator_traits<InputIterator>::value_type>
// filter( InputIterator begin, InputIterator end, Functor f )
// {
//   using ValueType = typename std::iterator_traits<InputIterator>::value_type;

//   std::vector<ValueType> result;
//   result.reserve( std::distance( begin, end ) );

//   std::copy_if( begin, end,
//                 std::back_inserter( result ),
//                 f );

//   return result;
// }

// std::unordered_map<Word, std::string> sym_table;

// std::vector<unsigned> a = { 2,3,4,5,6 };
// auto b = filter( a.begin(), a.end(),
//                  [] ( unsigned i ) { return i % 2 == 0; } );


void dispSymbols(void)
{
    // for (auto x: sym_table)
    //     cout << hex << uppercase << setw(4) << setfill('0') << x.first << ' ' << x.second << endl;

    std::vector<pair<Word,string>> from_vector;
    std::vector<pair<Word,string>> to_vector;
    for (auto x : sym_table)
    {
        std::pair <Word,string> foo;
        foo = std::make_pair(x.first, x.second);
        from_vector.push_back(foo);
    }

    std::sort (from_vector.begin(), from_vector.end());

    for (const pair <Word,string> x : from_vector)
        std::cout << hex << uppercase << setw(4) << setfill('0') << x.first << ' ' << x.second << endl;
}

void showSymbolsByAddressRange(TRange& range)
{
    auto low = range.addr;
    auto high = (range.typeTo) ? range.to : range.addr+range.len;

    std::vector<pair<Word,string>> from_vector;
    std::vector<pair<Word,string>> to_vector;
    for (auto x : sym_table)
    {
        std::pair <Word,string> foo;
        foo = std::make_pair(x.first, x.second);
        from_vector.push_back(foo);
    }

    to_vector.clear();
    std::copy_if(from_vector.begin(), from_vector.end(),
                 std::back_inserter(to_vector),
                 [&](std::pair <Word,string> x) { return x.first >= low && x.first < high; });

    std::sort (to_vector.begin(), to_vector.end());

    for (const pair <Word,string> x : to_vector)
        std::cout << hex << uppercase << setw(4) << setfill('0') << x.first << ' ' << x.second << endl;
}

void showSymbolsByTextMatch(char *pText)
{
    auto len = strlen(pText);


    std::vector<pair<Word,string>> from_vector;
    std::vector<pair<Word,string>> to_vector;
    for (auto x : sym_table)
    {
        std::pair <Word,string> foo;
        foo = std::make_pair(x.first, x.second);
        from_vector.push_back(foo);
    }

    to_vector.clear();
    std::copy_if(from_vector.begin(), from_vector.end(),
                 std::back_inserter(to_vector),
                 [&](std::pair <Word,string> x) { return x.second.length() >= len && (!x.second.substr(0,len).compare(pText)); });

    std::sort (to_vector.begin(), to_vector.end());

    for (const pair <Word,string> x : to_vector)
        std::cout << hex << uppercase << setw(4) << setfill('0') << x.first << ' ' << x.second << endl;
}



/*

./usimd ../tests/test_main.hex
load '../../../percom/psymon/psymon.sym
sym         # all
sym 'Com
sym 'load
sym 'In
sym 1 len 10
sym 0fc00 to 0fcff

load '../../../percom/psymon/psymon.sym
sym
sym 'lo
sym 're
sym 'RE
sym 0fdf0 len 10
sym 0fdf0 to 0fdff


*/
