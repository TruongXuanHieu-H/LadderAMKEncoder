#ifndef SEQ_ENCODER_H
#define SEQ_ENCODER_H

#include "encoder.h"
#include <deque>
#include <map>

namespace SINGLELADDERAMK
{
    typedef std::deque<int>::iterator deq_int_it;

    class SeqEncoder : public Encoder
    {
    public:
        SeqEncoder(ClauseContainer *cc, VarHandler *vh);
        virtual ~SeqEncoder();

        virtual void encode_and_solve_ladder_amk(int n, int w, int at_most, int initCondLength, int initCond[]) override;

    private:
        int get_aux_var(int first, int last, int value);
        void encode_amk(deq_int_it it_begin, deq_int_it it_end, int at_most);

        std::map<std::tuple<int, int, int>, int> aux_vars;
    };

}

#endif
