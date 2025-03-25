#ifndef REDUCED_ENCODER_H
#define REDUCED_ENCODER_H

#include "encoder.h"
#include <deque>

namespace SINGLELADDERAMK
{
    typedef std::deque<int>::iterator deq_int_it;

    class ReducedEncoder : public Encoder
    {
    public:
        ReducedEncoder(ClauseContainer *cc, VarHandler *vh);
        virtual ~ReducedEncoder();

        virtual void encode_and_solve_ladder_amo(int n, int w, int initCondLength, int initCond[]) override;

    private:
        void encode_first_amo(deq_int_it it_begin, deq_int_it it_end);
        void encode_amo(deq_int_it it_begin, deq_int_it it_end, int new_g);
    };

}

#endif
