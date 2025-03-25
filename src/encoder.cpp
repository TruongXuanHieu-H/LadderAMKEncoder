#include "encoder.h"

namespace SINGLELADDERAMK
{
    Encoder::Encoder(ClauseContainer *clause_container, VarHandler *var_handler) : cc(clause_container), vh(var_handler) {};
    Encoder::~Encoder() {};

    void Encoder::encode_ladder_amo(int n, int w)
    {
        encode_and_solve_ladder_amo(n, w, 0, {});
    };
}