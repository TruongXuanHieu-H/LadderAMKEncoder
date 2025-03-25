#include "encoder.h"

namespace SINGLELADDERAMK
{
    Encoder::Encoder(ClauseContainer *clause_container, VarHandler *var_handler) : cc(clause_container), vh(var_handler) {};
    Encoder::~Encoder() {};

    void Encoder::encode_ladder_amk(int n, int w, int at_most)
    {
        encode_and_solve_ladder_amk(n, w, at_most, 0, {});
    };
}