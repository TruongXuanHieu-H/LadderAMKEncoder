#ifndef SCL_ENCODER_H
#define SCL_ENCODER_H

#include "encoder.h"
#include <map>

namespace SINGLELADDERAMK
{
  class SCLEncoder : public Encoder
  {
  public:
    SCLEncoder(ClauseContainer *clause_container, VarHandler *var_handler);
    virtual ~SCLEncoder();

    virtual void encode_and_solve_ladder_amk(int n, int w, int at_most, int initCondLength, int initCond[]) override;

  private:
    void encode_window(int window, int n, int w, int at_most);
    void glue_window(int window, int n, int w, int at_most);
    int get_aux_var(int first, int last, int value);

    std::map<std::tuple<int, int, int>, int> aux_vars;
  };
}

#endif
