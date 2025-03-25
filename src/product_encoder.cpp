#include "product_encoder.h"

#include <numeric>   //iota
#include <algorithm> //generate
#include <cmath>     //floor,ceil
#include <deque>
#include <assert.h>
#include <iostream>
#include <iterator>

namespace SINGLELADDERAMK
{

    ProductEncoder::ProductEncoder(ClauseContainer *clause_container, VarHandler *var_handler) : Encoder(clause_container, var_handler)
    {
    }

    ProductEncoder::~ProductEncoder() {};

    void ProductEncoder::encode_and_solve_ladder_amo(int n, int w, int initCondLength, int initCond[])
    {
        std::cout << "start product" << std::endl;
        for (int i = 1; i <= n - w + 1; i++)
        {
            std::vector<int> items(w);
            std::iota(items.begin(), items.end(), i);
            encode_amo(items.begin(), items.end());
        }

        (void)initCondLength;
        (void)initCond;
    }

    void ProductEncoder::encode_amo(vec_int_it it_begin, vec_int_it it_end)
    {
        int constr_length = std::distance(it_begin, it_end);
        if (constr_length < 2)
            return;
        if (constr_length == 2)
        {
            int v1 = *it_begin;
            int v2 = *std::next(it_begin);
            cc->add_clause({v1, v2});
            cc->add_clause({-1 * v1, -1 * v2});
            return;
        }

        int p = std::ceil(std::sqrt(constr_length));
        int q = std::ceil((float)constr_length / (float)p);

        std::vector<int> u_vars;
        std::vector<int> v_vars;
        for (int i = 1; i <= p; ++i)
            u_vars.push_back(vh->get_new_var());
        for (int j = 1; j <= q; ++j)
            v_vars.push_back(vh->get_new_var());

        int i, j, curr, idx = 0;
        for (auto i_pos = it_begin; i_pos != it_end; ++i_pos)
        {
            i = std::floor(idx / p);
            j = idx % p;
            curr = *i_pos;
            cc->add_clause({-1 * curr, v_vars[i]});
            cc->add_clause({-1 * curr, u_vars[j]});
            ++idx;
        }
        encode_amo(u_vars.begin(), u_vars.end());
        encode_amo(v_vars.begin(), v_vars.end());
    };
}
