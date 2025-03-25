#include "naive_encoder.h"

#include <numeric>   //iota
#include <algorithm> //generate
#include <cmath>     //floor,ceil
#include <deque>
#include <assert.h>
#include <iostream>
#include <iterator>

namespace SINGLELADDERAMK
{

    NaiveEncoder::NaiveEncoder(ClauseContainer *clause_container, VarHandler *var_handler) : Encoder(clause_container, var_handler)
    {
    }

    NaiveEncoder::~NaiveEncoder() {};

    void NaiveEncoder::encode_and_solve_ladder_amo(int n, int w, int initCondLength, int initCond[])
    {
        std::cout << "start naive" << std::endl;
        for (int i = 1; i <= n - w + 1; i++)
        {
            std::vector<int> items(w);
            std::iota(items.begin(), items.end(), i);
            encode_amo(items.begin(), items.end());
        }

        (void)initCondLength;
        (void)initCond;
    }

    void NaiveEncoder::encode_amo(vec_int_it it_begin, vec_int_it it_end)
    {
        for (auto i_pos = it_begin; i_pos != it_end; ++i_pos)
        {
            for (auto j_pos = std::next(i_pos); j_pos != it_end; ++j_pos)
            {
                cc->add_clause({-1 * (*i_pos), -1 * (*j_pos)});
            }
        }
    };
}
