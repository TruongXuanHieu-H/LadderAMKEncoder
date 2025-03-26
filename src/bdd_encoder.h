#ifndef BDD_ENCODER_H
#define BDD_ENCODER_H

#include "encoder.h"

namespace SINGLELADDERAMK
{
    typedef std::vector<int>::iterator vec_int_it;

    class BDDEncoder : public Encoder
    {
    public:
        BDDEncoder(ClauseContainer *cc, VarHandler *vh);
        virtual ~BDDEncoder();

        virtual void encode_and_solve_ladder_amk(int n, int w, int at_most, int initCondLength, int initCond[]) override;
    };

}

#endif
