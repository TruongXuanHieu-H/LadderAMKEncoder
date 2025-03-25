#ifndef SEQ_ENCODER_H
#define SEQ_ENCODER_H

#include "encoder.h"
#include <deque>

namespace SINGLELADDERAMK
{
    typedef std::deque<int>::iterator deq_int_it;

    class SeqEncoder : public Encoder
    {
    public:
        SeqEncoder(ClauseContainer *cc, VarHandler *vh);
        virtual ~SeqEncoder();

        virtual void encode_and_solve_ladder_amo(int n, int w, int initCondLength, int initCond[]) override;

    private:
        void encode_amo(deq_int_it it_begin, deq_int_it it_end);
    };

}

#endif
