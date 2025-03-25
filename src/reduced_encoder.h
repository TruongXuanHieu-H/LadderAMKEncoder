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

        virtual void encode_and_solve_ladder_amk(int n, int w, int at_most, int initCondLength, int initCond[]) override;

    private:
        void encode_first_amk(deq_int_it it_begin, deq_int_it it_end, int at_most);
        void encode_amk(deq_int_it it_begin, deq_int_it it_end, int new_g, int at_most);
    };

}

#endif
