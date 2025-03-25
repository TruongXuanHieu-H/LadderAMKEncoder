#include "duplex_encoder.h"

#include <iostream>
#include <numeric>
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <unordered_map>

namespace SINGLELADDERAMK
{
    DuplexEncoder::DuplexEncoder(ClauseContainer *clause_container, VarHandler *var_handler) : Encoder(clause_container, var_handler)
    {
        init_members();
    }

    DuplexEncoder::~DuplexEncoder() {};

    void DuplexEncoder::init_members()
    {
        fwd_amo_roots = std::unordered_map<int, std::vector<int>>();
        bwd_amo_roots = std::unordered_map<int, std::vector<int>>();
        fwd_amz_roots = std::unordered_map<int, std::vector<int>>();
        bwd_amz_roots = std::unordered_map<int, std::vector<int>>();

        node_amz_literals = std::unordered_map<int, std::vector<std::vector<int>>>();
    }

    void DuplexEncoder::encode_and_solve_ladder_amo(int n, int w, int initCondLength, int initCond[])
    {
        (void)initCondLength;
        (void)initCond;

        construct_window_bdds(n, w);
        glue_window_bdds();

        node_amz_literals.clear();
    }

    void DuplexEncoder::construct_window_bdds(int n, int w)
    {
        number_of_windows = n / w;
        last_window_w = n % w;
        if (last_window_w != 0)
            number_of_windows++;

        for (int i = 0; i < 1; ++i)
        {
            fwd_amo_roots[i] = std::vector<int>();
            bwd_amo_roots[i] = std::vector<int>();
            fwd_amz_roots[i] = std::vector<int>();
            bwd_amz_roots[i] = std::vector<int>();

            for (int gw = 0; gw < number_of_windows; ++gw)
            {
                bool last_window = (gw == number_of_windows - 1);
                int p1 = (i * n) + (gw * w) + 1;
                int p2 = p1 + w - 1;
                if (last_window)
                    p2 = (i + 1) * n;

                std::deque<int> window_vars(p2 - p1 + 1);
                std::iota(window_vars.begin(), window_vars.end(), p1);

                int fwd_amo_id;
                int fwd_amz_id;
                if (gw != number_of_windows - 1)
                {
                    fwd_amo_id = build_amo(window_vars);
                    fwd_amz_id = build_amz(window_vars);
                    fwd_amo_roots[i].push_back(fwd_amo_id);
                    fwd_amz_roots[i].push_back(fwd_amz_id);
                }

                int bwd_amo_id;
                int bwd_amz_id;
                if (gw != 0)
                {
                    std::reverse(std::begin(window_vars), std::end(window_vars));

                    bwd_amo_id = build_amo(window_vars);
                    bwd_amz_id = build_amz(window_vars);

                    bwd_amo_roots[i].push_back(bwd_amo_id);
                    bwd_amz_roots[i].push_back(bwd_amz_id);
                }

                if (gw == number_of_windows - 1)
                {
                    fwd_amo_roots[i].push_back(bwd_amo_id);
                    fwd_amz_roots[i].push_back(bwd_amz_id);
                }
                else if (gw == 0)
                {
                    bwd_amo_roots[i].push_back(fwd_amo_id);
                    bwd_amz_roots[i].push_back(fwd_amz_id);
                }
                else
                {
                    make_equal_bdds(fwd_amo_id, bwd_amo_id);
                    make_equal_bdds(fwd_amz_id, bwd_amz_id);
                }

                if (window_vars.size() > 1)
                {
                    cc->add_clause({fwd_amo_id});
                }
            }

            assert(!fwd_amz_roots[i].empty());
        }
    }

    void DuplexEncoder::glue_window_bdds()
    {
        for (int var_group = 0; var_group < 1; ++var_group)
        {
            node_amz_literals[var_group] = std::vector<std::vector<int>>();
            for (int curr_window = 0; curr_window < number_of_windows - 1; ++curr_window)
            {
                node_amz_literals[var_group].push_back({fwd_amz_roots[var_group][curr_window]});

                int next_window = curr_window + 1;

                int curr_fwd_amo = fwd_amo_roots[var_group][curr_window]; // bdd_amo_it
                int next_bwd_amo = bwd_amo_roots[var_group][next_window];

                int fwd_from = bh.bdds[curr_fwd_amo].i_from;
                int fwd_to = bh.bdds[curr_fwd_amo].i_to;

                int bwd_from = bh.bdds[next_bwd_amo].i_from;
                int bwd_to = bh.bdds[next_bwd_amo].i_to;

                assert(bwd_to == fwd_to + 1);

                if (fwd_from != fwd_to)
                {
                    cc->add_clause({curr_fwd_amo});
                }
                if (bwd_from != bwd_to)
                {
                    cc->add_clause({next_bwd_amo});
                }

                std::deque<int> fwd_window(fwd_to - fwd_from + 1);
                std::iota(fwd_window.begin(), fwd_window.end(), fwd_from);
                fwd_window.pop_front(); // the full window was already saved as unit clause

                std::deque<int> bwd_window;
                bwd_window.push_back(fwd_to + 1);

                std::pair<int, int> fwd_p, bwd_p;

                while (fwd_window.size() > 0 && bwd_window.back() <= bwd_from)
                {

                    fwd_p = std::pair<int, int>(fwd_window.front(), fwd_window.back());
                    bwd_p = std::pair<int, int>(bwd_window.back(), bwd_window.front());

                    int b1_amo, b1_amz, b2_amo, b2_amz;

                    bh.lookup_amo(fwd_p, b1_amo);
                    assert(b1_amo != 0);
                    bh.lookup_amz(fwd_p, b1_amz);
                    assert(b1_amz != 0);
                    bh.lookup_amo(bwd_p, b2_amo);
                    assert(b2_amo != 0);
                    bh.lookup_amz(bwd_p, b2_amz);
                    assert(b2_amz != 0);

                    if (fwd_window.size() > 1)
                    {
                        cc->add_clause({b1_amo});
                    }
                    if (bwd_window.size() > 1)
                    {
                        cc->add_clause({b2_amo});
                    }

                    cc->add_clause({b1_amz, b2_amz});

                    node_amz_literals[var_group].push_back({b1_amz, b2_amz});

                    fwd_window.pop_front();
                    bwd_window.push_back(bwd_window.back() + 1);
                }
            }
            node_amz_literals[var_group].push_back({bwd_amz_roots[var_group][number_of_windows - 1]});
        }
    };

    BDD_id DuplexEncoder::build_amo(std::deque<int> vars)
    {
        BDD_id lookup;
        int from = vars.front();
        int to = vars.back();
        std::pair<int, int> from_to_pair(from, to);

        if (bh.lookup_amo(from_to_pair, lookup))
        {
            return lookup;
        }

        BDD new_bdd = BDD(from, to, 1);
        BDD_id true_child, false_child;

        if (vars.size() == 1)
        {
            new_bdd.id = from;
            true_child = 0;
            false_child = 0;
        }
        else
        {
            new_bdd.id = vh->get_new_var();
            vars.pop_front();
            false_child = build_amo(vars);
            true_child = build_amz(vars);

            cc->add_clause({-1 * from, -1 * new_bdd.id, true_child});
            if (vars.size() > 1)
            {
                cc->add_clause({new_bdd.id * -1, false_child});
            }
        }

        new_bdd.true_child_id = true_child;
        new_bdd.false_child_id = false_child;

        bh.save_amo(new_bdd);

        return new_bdd.id;
    };

    BDD_id DuplexEncoder::build_amz(std::deque<int> vars)
    {
        BDD_id lookup;
        int from = vars.front();
        int to = vars.back();
        std::pair<int, int> from_to_pair(from, to);

        if (bh.lookup_amz(from_to_pair, lookup))
        {
            return lookup;
        }

        BDD new_bdd = BDD(from, to, 0);
        BDD_id true_child, false_child;

        if (vars.size() == 1)
        {
            new_bdd.id = -1 * from;
            true_child = 0;  //\bot BDD
            false_child = 0; //\top BDD
        }
        else
        {
            new_bdd.id = vh->get_new_var();
            true_child = 0; //\bot BDD
            vars.pop_front();
            false_child = build_amz(vars);

            cc->add_clause({-1 * from, -1 * new_bdd.id});
            cc->add_clause({from, -1 * new_bdd.id, false_child});
            cc->add_clause({from, new_bdd.id, -1 * false_child});
        }

        new_bdd.true_child_id = true_child;
        new_bdd.false_child_id = false_child;

        bh.save_amz(new_bdd);
        return new_bdd.id;
    };

    void DuplexEncoder::make_equal_bdds(BDD_id b1, BDD_id b2)
    {
        if (b1 == b2)
            return;
        if (b1 == -1)
        {
            cc->add_clause({b2});
            return;
        }

        if (b2 == -1)
        {
            cc->add_clause({b1});
            return;
        }

        assert(b1 > 0 && b2 > 0);

        cc->add_clause({-1 * b1, b2});
        cc->add_clause({b1, -1 * b2});
    };
}