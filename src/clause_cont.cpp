#include "clause_cont.h"
#include <iostream>
#include <assert.h>

namespace SINGLELADDERAMK
{

    ClauseContainer::ClauseContainer(VarHandler *v, unsigned split_limit) : vh(v), split_size(split_limit)
    {
        if (split_limit == 0)
            do_split = false;
        else
            do_split = true;
    };

    ClauseContainer::~ClauseContainer() {};

    void ClauseContainer::add_clause(const Clause &c)
    {
        if (!do_split)
        {
            do_add_clause(c);
        }
        else
        {
            Clause long_clause = c;
            while (long_clause.size() > split_size)
            {
                int split_var = vh->get_new_var();

                Clause chunk(long_clause.begin(), long_clause.begin() + split_size);
                chunk.push_back(split_var);
                do_add_clause(chunk);

                Clause rest = {-1 * split_var};
                rest.insert(rest.end(), long_clause.begin() + split_size, long_clause.end());
                long_clause = rest;
            }
            do_add_clause(long_clause);
        }
    };

    void ClauseContainer::print_clauses() const
    {
        do_print_clauses();
    };

    void ClauseContainer::print_dimacs() const
    {
        do_print_dimacs();
    };

    unsigned ClauseContainer::size() const
    {
        return do_size();
    };

    int ClauseContainer::vhSize() const
    {
        return do_vh_size();
    };
}
