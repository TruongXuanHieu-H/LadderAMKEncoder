#ifndef ADDER_ENCODER_H
#define ADDER_ENCODER_H

#include "encoder.h"

namespace SINGLELADDERAMK
{
    class AdderEncoder : public BaseEncoder
    {
    public:
        AdderEncoder(ClauseContainer *cc, VarHandler *vh);
        virtual ~AdderEncoder();

        virtual void encode_and_solve_ladder_amk(int n, int w, int at_most, int initCondLength, int initCond[]) override;
    };
}

#endif
