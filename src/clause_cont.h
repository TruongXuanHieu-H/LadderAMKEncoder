#ifndef CLAUSE_CONT_H
#define CLAUSE_CONT_H

#include "utils.h"
#include <vector>

namespace SINGLELADDERAMK
{

  typedef std::vector<int> Clause;
  typedef std::vector<Clause> Clauses;

  class ClauseContainer
  {
  public:
    ClauseContainer(const ClauseContainer &) = delete;
    ClauseContainer &operator=(const ClauseContainer &) = delete;

    virtual ~ClauseContainer();

    Clauses clause_list;

    void add_clause(const Clause &c);
    void print_dimacs() const;
    void print_clauses() const;
    unsigned size() const;
    int vhSize() const;

  protected:
    VarHandler *vh;

    ClauseContainer(VarHandler *, unsigned split_size = 0);
    const unsigned split_size;
    bool do_split = false;

  private:
    virtual void do_add_clause(const Clause &c) = 0;
    virtual unsigned do_size() const = 0;
    virtual int do_vh_size() const = 0;
    virtual void do_print_dimacs() const = 0;
    virtual void do_clear() = 0;
    virtual void do_print_clauses() const = 0;
  };
}

#endif
