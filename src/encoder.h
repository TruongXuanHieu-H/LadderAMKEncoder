#ifndef ENCODER_H
#define ENCODER_H

#include "clause_cont.h"

namespace SINGLELADDERAMK
{
    class Encoder
    {
    public:
        Encoder(ClauseContainer *clause_container, VarHandler *var_handler);
        virtual ~Encoder();

        Encoder(Encoder const &) = delete;
        Encoder &operator=(Encoder const &) = delete;

        bool is_debug_mode = false;

        virtual void encode_and_solve_ladder_amk(int n, int w, int at_most, int initCondLength, int initCond[]) = 0;

        ClauseContainer *cc;
        VarHandler *vh;

        void encode_ladder_amk(int n, int w, int at_most);
    };
}

#endif