#include "card_encoder.h"

#include "pblib/pb2cnf.h"
#include "pblib/pbconstraint.h"
#include "pblib/PBConfig.h"
#include <numeric>

namespace SINGLELADDERAMK
{

    CardinalityEncoder::CardinalityEncoder(ClauseContainer *clause_container, VarHandler *var_handler) : BaseEncoder(clause_container, var_handler)
    {
    }

    CardinalityEncoder::~CardinalityEncoder() {};

    void CardinalityEncoder::encode_and_solve_ladder_amk(int n, int w, int at_most, int initCondLength, int initCond[])
    {
        PBConfig config = std::make_shared<PBConfigClass>();
        config->amk_encoder = AMK_ENCODER::CARD;
        PB2CNF pb2cnf(config);

        for (int i = 1; i <= n - w + 1; i++)
        {
            std::vector<int32_t> literals(w);
            std::iota(literals.begin(), literals.end(), i);

            std::vector<std::vector<int32_t>> clauses;

            int next_aux_var = pb2cnf.encodeAtMostK(literals, at_most, clauses, vh->get_new_var()) + 1;

            vh->set_next_to_assign(next_aux_var);
            for (const auto &clause : clauses)
                cc->add_clause(clause);
        }

        (void)initCondLength;
        (void)initCond;
    }
}
