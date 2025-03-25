#include "seq_encoder.h"

#include <numeric>
#include <algorithm>
#include <cmath>
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

    int SeqEncoder::get_aux_var(int first, int last, int value)
    {
        // std::cout << "Get aux var: " << first << " " << last << " " << value << "\n";
        assert(first - last >= value - 1 || first - last <= 1 - value);

        auto pair = aux_vars.find({first, last, value});

        if (pair != aux_vars.end())
            return pair->second;

        if (first == last)
            return first;

        int new_aux_var = vh->get_new_var();
        aux_vars.insert({{first, last, value}, new_aux_var});
        return new_aux_var;
    }

    void SeqEncoder::encode_and_solve_ladder_amk(int n, int w, int at_most, int initCondLength, int initCond[])
    {
        for (int i = 1; i <= n - w + 1; i++)
        {
            std::deque<int> items(w);
            std::iota(items.begin(), items.end(), i);
            encode_amk(items.begin(), items.end(), at_most);
        }

        (void)initCondLength;
        (void)initCond;
    }

    void SeqEncoder::encode_amk(deq_int_it it_begin, deq_int_it it_end, int at_most)
    {
        std::vector<int> literals(it_begin, it_end); // Vector with w literals
        int w = (int)literals.size();
        if (w <= at_most) // If w <= k then always true
            return;

        for (int j = 2; j <= w - 1; j++)
        {
            cc->add_clause({-literals[j - 1], get_aux_var(literals[0], literals[j - 1], 1)});
        }

        for (int j = 2; j <= w - 1; j++)
        {
            int upper_s = (j - 1 < at_most) ? (j - 1) : at_most;
            for (int s = 1; s <= upper_s; s++)
            {
                cc->add_clause({-get_aux_var(literals[0], literals[j - 2], s), get_aux_var(literals[0], literals[j - 1], s)});
            }
        }

        for (int j = 2; j <= w - 1; j++)
        {
            int upper_s = j < at_most ? j : at_most;
            for (int s = 2; s <= upper_s; s++)
            {
                cc->add_clause({-literals[j - 1], -get_aux_var(literals[0], literals[j - 2], s - 1), get_aux_var(literals[0], literals[j - 1], s)});
            }
        }

        for (int j = 2; j <= w - 1; j++)
        {
            int upper_s = (j - 1 < at_most) ? (j - 1) : at_most;
            for (int s = 1; s <= upper_s; s++)
            {
                cc->add_clause({literals[j - 1], get_aux_var(literals[0], literals[j - 2], s), -get_aux_var(literals[0], literals[j - 1], s)});
            }
        }

        for (int j = at_most + 1; j <= w; j++)
        {
            cc->add_clause({-literals[j - 1], -get_aux_var(literals[0], literals[j - 2], at_most)});
        }
    }
}
