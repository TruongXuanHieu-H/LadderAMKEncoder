#include "seq_encoder.h"

#include <numeric>   //iota
#include <algorithm> //generate
#include <cmath>     //floor,ceil
#include <deque>
#include <assert.h>
#include <iostream>
#include <iterator>

namespace SINGLELADDERAMK
{

    SeqEncoder::SeqEncoder(ClauseContainer *clause_container, VarHandler *var_handler) : Encoder(clause_container, var_handler)
    {
    }

    SeqEncoder::~SeqEncoder() {};

    void SeqEncoder::encode_and_solve_ladder_amo(int n, int w, int initCondLength, int initCond[])
    {
        std::cout << "start product" << std::endl;
        for (int i = 1; i <= n - w + 1; i++)
        {
            std::deque<int> items(w);
            std::iota(items.begin(), items.end(), i);
            encode_amo(items.begin(), items.end());
        }

        (void)initCondLength;
        (void)initCond;
    }

    void SeqEncoder::encode_amo(deq_int_it amo1_begin, deq_int_it amo1_end)
    {
        int prev = *amo1_begin;
        for (auto i_pos = std::next(amo1_begin); i_pos != amo1_end; i_pos++)
        {
            int curr = *i_pos;
            int next = vh->get_new_var();
            cc->add_clause({-1 * prev, -1 * curr});
            cc->add_clause({-1 * prev, next});
            cc->add_clause({-1 * curr, next});

            prev = next;
        }
    }
}
