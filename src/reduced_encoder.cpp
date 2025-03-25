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

    void ReducedEncoder::encode_and_solve_ladder_amo(int n, int w, int initCondLength, int initCond[])
    {
        std::cout << "start reduced" << std::endl;

        std::deque<int> first_item(w);
        std::iota(first_item.begin(), first_item.end(), 1);
        encode_first_amo(first_item.begin(), first_item.end());

        for (int i = 1; i <= n - w; i++)
        {
            std::deque<int> items(w);
            std::iota(items.begin(), items.end(), i);
            encode_amo(items.begin(), items.end(), items.back() + 1);
        }

        (void)initCondLength;
        (void)initCond;
    }

    void ReducedEncoder::encode_first_amo(deq_int_it it_begin, deq_int_it it_end)
    {
        for (auto i_pos = it_begin; i_pos != it_end; ++i_pos)
        {
            for (auto j_pos = std::next(i_pos); j_pos != it_end; ++j_pos)
            {
                cc->add_clause({-1 * (*i_pos), -1 * (*j_pos)});
            }
        }
    }

    void ReducedEncoder::encode_amo(deq_int_it it_begin, deq_int_it it_end, int new_g)
    {
        for (auto i_pos = std::next(it_begin); i_pos != it_end; ++i_pos)
        {
            cc->add_clause({-1 * (*i_pos), -1 * new_g});
        }
    };
}
