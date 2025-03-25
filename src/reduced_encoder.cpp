#include "reduced_encoder.h"

#include <numeric>   //iota
#include <algorithm> //generate
#include <cmath>     //floor,ceil
#include <deque>
#include <assert.h>
#include <iostream>
#include <iterator>

namespace SINGLELADDERAMK
{

    ReducedEncoder::ReducedEncoder(ClauseContainer *clause_container, VarHandler *var_handler) : Encoder(clause_container, var_handler)
    {
    }

    ReducedEncoder::~ReducedEncoder() {};

    void ReducedEncoder::encode_and_solve_ladder_amk(int n, int w, int at_most, int initCondLength, int initCond[])
    {
        std::deque<int> first_item(w);
        std::iota(first_item.begin(), first_item.end(), 1);
        encode_first_amk(first_item.begin(), first_item.end(), at_most);

        for (int i = 1; i <= n - w; i++)
        {
            std::deque<int> items(w);
            std::iota(items.begin(), items.end(), i);
            encode_amk(items.begin(), items.end(), items.back() + 1, at_most);
        }

        (void)initCondLength;
        (void)initCond;
    }

    void ReducedEncoder::encode_first_amk(deq_int_it it_begin, deq_int_it it_end, int at_most)
    {
        std::vector<int> literals(it_begin, it_end); // Vector with w literals
        if ((int)literals.size() <= at_most)         // If w <= k then always true
            return;

        std::vector<bool> bitmask(at_most + 1, true);
        bitmask.resize(literals.size(), false);

        // std::cout << "First AMO: ";
        // for (int i = 0; i < (int)literals.size(); i++)
        // {
        //     std::cout << literals[i] << " ";
        // }
        // std::cout << "\n";

        do
        {
            std::vector<int> subset;
            for (int i = 0; i < (int)literals.size(); ++i)
            {
                if (bitmask[i])
                    subset.push_back(-literals[i]);
            }
            cc->add_clause(subset);
        } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    }

    void ReducedEncoder::encode_amk(deq_int_it it_begin, deq_int_it it_end, int new_g, int at_most)
    {
        std::vector<int> literals(std::next(it_begin), it_end); // Vector with w - 1 literals
        if ((int)literals.size() <= at_most - 1)                // If w - 1 <= k - 1 then always true
            return;

        std::vector<bool> bitmask(at_most, true);
        bitmask.resize(literals.size(), false);

        // std::cout << "Next AMO: ";
        // for (int i = 0; i < (int)literals.size(); i++)
        // {
        //     std::cout << literals[i] << " ";
        // }
        // std::cout << new_g << "\n";

        do
        {
            std::vector<int> subset;
            for (int i = 0; i < (int)literals.size(); ++i)
            {
                if (bitmask[i])
                    subset.push_back(-literals[i]);
            }
            subset.push_back(-1 * new_g);
            cc->add_clause(subset);
        } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    };
}
