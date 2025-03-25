#ifndef DUPLEX_ENCODER_H
#define DUPLEX_ENCODER_H

#include "encoder.h"
#include "bdd.h"
#include <deque>

namespace SINGLELADDERAMK
{
    class DuplexEncoder : public Encoder
    {
    public:
        DuplexEncoder(ClauseContainer *clause_container, VarHandler *var_handler);
        virtual ~DuplexEncoder();

        virtual void encode_and_solve_ladder_amo(int n, int w, int initCondLength, int initCond[]) override;

    private:
        BDDHandler bh;

        std::unordered_map<int, std::vector<int>> fwd_amo_roots;
        std::unordered_map<int, std::vector<int>> bwd_amo_roots;
        std::unordered_map<int, std::vector<int>> fwd_amz_roots;
        std::unordered_map<int, std::vector<int>> bwd_amz_roots;

        std::unordered_map<int, std::vector<std::vector<int>>> node_amz_literals;
        int number_of_windows;
        int last_window_w;

        void init_members();

        void construct_window_bdds(int n, int w);
        void glue_window_bdds();

        void make_equal_bdds(BDD_id b1, BDD_id b2);

        BDD_id build_amo(std::deque<int> vars);
        BDD_id build_amz(std::deque<int> vars);
    };
}

#endif
