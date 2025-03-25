#ifndef LADDER_AMK_H
#define LADDER_AMK_H

#include "utils.h"

#include "clause_cont.h"
#include "cadical_clauses.h"
#include "reduced_encoder.h"
#include "seq_encoder.h"
#include <unordered_map>

namespace SINGLELADDERAMK
{
    enum EncodeType
    {
        naive,
        reduced,
        seq,
        BDD,
        product,
        duplex,
        scl,
        card,
    };

    const std::unordered_map<EncodeType, std::string> encode_type_map = {
        {naive, "naive"},
        {reduced, "reduced"},
        {seq, "seq"},
        {BDD, "BDD"},
        {product, "product"},
        {duplex, "duplex"},
        {scl, "scl"},
        {card, "card"},
    };

    class LadderAMKEncoder
    {
    public:
        LadderAMKEncoder();
        virtual ~LadderAMKEncoder();

        EncodeType enc_choice = scl;

        bool verbose = true;
        bool check_solution = true;

        bool force_phase = false;
        std::string sat_configuration = "sat";

        int split_limit = 0;

        int at_most = 2;

        bool overwrite_lb = false;
        bool overwrite_ub = false;
        int forced_lb = 0;
        int forced_ub = 0;

        void set_n(int n);
        void encode_and_solve_ladder_amk();

    protected:
        int n;
        VarHandler *vh;
        Encoder *enc;
        ClauseContainer *cc;
        CaDiCaL::Solver *solver;

        int SAT_res = 0;

        void encode_and_solve_ladder_amk_problem(int start_w, int stop_w, int step, int prev_res);
        bool encode_and_solve_ladder_amk_problem(int w);

        void setup_solving();
        void cleanup_solving();

        void setup_cadical();
        void setup_encoder();

        void setup_bounds(int &w_from, int &w_to);
        void lookup_bounds(int &lb, int &ub);

        bool calculate_sat_solution(int w);
    };
}

#endif