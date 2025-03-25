#include "scl_encoder.h"
#include <iostream>
#include <math.h>
#include "utils.h"

namespace SINGLELADDERAMK
{

    SCLEncoder::SCLEncoder(ClauseContainer *clause_container, VarHandler *var_handler) : Encoder(clause_container, var_handler)
    {
    }

    SCLEncoder::~SCLEncoder() {};

    int SCLEncoder::get_aux_var(int first, int last)
    {

        auto pair = aux_vars.find({first, last});

        if (pair != aux_vars.end())
            return pair->second;

        if (first == last)
            return first;

        int new_aux_var = vh->get_new_var();
        aux_vars.insert({{first, last}, new_aux_var});
        return new_aux_var;
    }

    void SCLEncoder::encode_and_solve_ladder_amo(int n, int w, int initCondLength, int initCond[])
    {
        (void)initCondLength;
        (void)initCond;

        for (int gw = 0; gw < ceil((float)n / w); gw++)
        {
            encode_window(gw, n, w);
        }

        for (int gw = 0; gw < ceil((float)n / w) - 1; gw++)
        {
            glue_window(gw, n, w);
        }
    }

    /*
     * Encode each window separately.
     * The first window only has lower part.
     * The last window only has upper part.
     * Other windows have both upper part and lower part.
     *
     * window: index of window (0, 1, 2,...)
     * n : number of variables
     * w : width of window
     */
    void SCLEncoder::encode_window(int window, int n, int w)
    {
        if (window == 0)
        {
            // Encode the first window, which only have lower part
            int lastVar = window * (int)w + w;

            { // Formula 7
                // for (int i = w - 1; i >= 1; i--)
                // {
                //     int var = window * (int)w + i;
                //     cc->add_clause({-var, get_aux_var(var, lastVar)});
                // }

                // Rewrite
                for (int i = 2; i <= w - 1; i++)
                {
                    int var = window * (int)w + w + 1 - i;
                    cc->add_clause({-var, get_aux_var(var, lastVar)});
                }
            }

            { // Formula 8
                // for (int i = w; i >= 2; i--)
                // {
                //     int var = window * (int)w + i;
                //     cc->add_clause({-get_aux_var(var, lastVar), get_aux_var(var - 1, lastVar)});
                // }

                // Rewrite
                for (int i = 2; i <= w - 1; i++)
                {
                    int var = window * (int)w + w + 1 - i;
                    cc->add_clause({-get_aux_var(var + 1, lastVar), get_aux_var(var, lastVar)});
                }
            }

            { // Formula 9
                // for (int i = 1; i < (int)w; i++)
                // {
                //     int var = window * (int)w + i;
                //     int main = get_aux_var(var, lastVar);
                //     int sub = get_aux_var(var + 1, lastVar);
                //     cc->add_clause({var, sub, -main});
                // }

                for (int i = 2; i <= (int)w - 1; i++)
                {
                    int var = window * (int)w + w + 1 - i;
                    int sub = get_aux_var(var + 1, lastVar);
                    int main = get_aux_var(var, lastVar);
                    cc->add_clause({var, sub, -main});
                }
            }

            { // Formula 10
                // for (int i = 1; i < (int)w; i++)
                // {
                //     int var = window * (int)w + i;
                //     cc->add_clause({-var, -get_aux_var(var + 1, lastVar)});
                // }

                // Rewrite
                for (int i = 2; i <= (int)w; i++)
                {
                    int var = window * (int)w + w + 1 - i;
                    cc->add_clause({-var, -get_aux_var(var + 1, lastVar)});
                }
            }
        }
        else if (window == ceil((float)n / w) - 1)
        {
            // Encode the last window, which only have upper part and may have width lower than w
            int firstVar = window * (int)w + 1;

            if ((window + 1) * w > n)
            {
                int real_w = n % w; // width < w
                // Upper part
                { // Formula 7
                    for (int i = 2; i <= real_w; i++)
                    {
                        int reverse_var = window * (int)w + i;
                        cc->add_clause({-reverse_var, get_aux_var(firstVar, reverse_var)});
                    }
                }

                { // Formula 8
                    // for (int i = real_w - 1; i > 0; i--)
                    // {
                    //     int reverse_var = window * (int)w + real_w - i;
                    //     cc->add_clause({-get_aux_var(firstVar, reverse_var), get_aux_var(firstVar, reverse_var + 1)});
                    // }

                    // Rewrite
                    for (int i = 2; i <= real_w; i++)
                    {
                        int reverse_var = window * (int)w + i - 1;
                        cc->add_clause({-get_aux_var(firstVar, reverse_var), get_aux_var(firstVar, reverse_var + 1)});
                    }
                }

                { // Formula 9
                    // for (int i = 0; i < (int)real_w - 1; i++)
                    // {
                    //     int var = window * (int)w + real_w - i;
                    //     int main = get_aux_var(firstVar, var);
                    //     int sub = get_aux_var(firstVar, var - 1);
                    //     cc->add_clause({sub, var, -main});
                    // }

                    // Rewrite
                    for (int i = 2; i <= real_w; i++)
                    {
                        int var = window * (int)w + i;
                        int sub = get_aux_var(firstVar, var - 1);
                        int main = get_aux_var(firstVar, var);
                        cc->add_clause({var, sub, -main});
                    }
                }

                { // Formula 10
                    // for (int i = real_w; i > 1; i--)
                    // {
                    //     int reverse_var = window * (int)w + i;
                    //     cc->add_clause({-reverse_var, -get_aux_var(firstVar, reverse_var - 1)});
                    // }

                    // Rewrite
                    for (int i = 2; i <= real_w; i++)
                    {
                        int reverse_var = window * (int)w + i;
                        cc->add_clause({-reverse_var, -get_aux_var(firstVar, reverse_var - 1)});
                    }
                }
            }
            else
            {
                // Upper part
                { // Formula 7
                    // for (int i = 2; i <= (int)w; i++)
                    // {
                    //     int reverse_var = window * (int)w + i;
                    //     cc->add_clause({-reverse_var, get_aux_var(firstVar, reverse_var)});
                    // }

                    // Rewrite
                    for (int i = 2; i <= (int)w - 1; i++)
                    {
                        int reverse_var = window * (int)w + i;
                        cc->add_clause({-reverse_var, get_aux_var(firstVar, reverse_var)});
                    }
                }

                { // Formula 8
                    // for (int i = w - 1; i >= 1; i--)
                    // {
                    //     int reverse_var = window * (int)w + w - i;
                    //     cc->add_clause({-get_aux_var(firstVar, reverse_var), get_aux_var(firstVar, reverse_var + 1)});
                    // }

                    // Rewrite
                    for (int i = 2; i <= w - 1; i++)
                    {
                        int reverse_var = window * (int)w + i - 1;
                        cc->add_clause({-get_aux_var(firstVar, reverse_var), get_aux_var(firstVar, reverse_var + 1)});
                    }
                }

                { // Formula 9
                    // for (int i = 0; i < (int)w - 1; i++)
                    // {
                    //     int var = window * (int)w + w - i;
                    //     int main = get_aux_var(firstVar, var);
                    //     int sub = get_aux_var(firstVar, var - 1);
                    //     cc->add_clause({sub, var, -main});
                    // }

                    // Rewrite
                    for (int i = 2; i <= (int)w - 1; i++)
                    {
                        int var = window * (int)w + i;
                        int sub = get_aux_var(firstVar, var - 1);
                        int main = get_aux_var(firstVar, var);
                        cc->add_clause({var, sub, -main});
                    }
                }

                { // Formula 10
                    // for (int i = (int)w; i > 1; i--)
                    // {
                    //     int reverse_var = window * (int)w + i;
                    //     cc->add_clause({-reverse_var, -get_aux_var(firstVar, reverse_var - 1)});
                    // }

                    // Rewrite
                    for (int i = 2; i <= (int)w; i++)
                    {
                        int reverse_var = window * (int)w + i;
                        cc->add_clause({-reverse_var, -get_aux_var(firstVar, reverse_var - 1)});
                    }
                }
            }
        }
        else
        {
            // Encode the middle windows, which have both upper and lower path, and always have width w

            // Upper part
            int firstVar = window * (int)w + 1;
            { // Formula 7
                // for (int i = 2; i <= (int)w; i++)
                // {
                //     int reverse_var = window * (int)w + i;
                //     cc->add_clause({-reverse_var, get_aux_var(firstVar, reverse_var)});
                // }

                // Rewrite
                for (int i = 2; i <= (int)w - 1; i++)
                {
                    int reverse_var = window * (int)w + i;
                    cc->add_clause({-reverse_var, get_aux_var(firstVar, reverse_var)});
                }
            }

            { // Formula 8
                // for (int i = w - 1; i >= 1; i--)
                // {
                //     int reverse_var = window * (int)w + w - i;
                //     cc->add_clause({-get_aux_var(firstVar, reverse_var), get_aux_var(firstVar, reverse_var + 1)});
                // }

                // Rewrite
                for (int i = 2; i <= w - 1; i++)
                {
                    int reverse_var = window * (int)w + i - 1;
                    cc->add_clause({-get_aux_var(firstVar, reverse_var), get_aux_var(firstVar, reverse_var + 1)});
                }
            }

            { // Formula 9
                // for (int i = 0; i < (int)w - 1; i++)
                // {
                //     int var = window * (int)w + w - i;
                //     int main = get_aux_var(firstVar, var);
                //     int sub = get_aux_var(firstVar, var - 1);
                //     cc->add_clause({sub, var, -main});
                // }

                // Rewrite
                for (int i = 2; i <= (int)w - 1; i++)
                {
                    int var = window * (int)w + i;
                    int sub = get_aux_var(firstVar, var - 1);
                    int main = get_aux_var(firstVar, var);
                    cc->add_clause({var, sub, -main});
                }
            }

            { // Formula 10
                // for (int i = (int)w; i > 1; i--)
                // {
                //     int reverse_var = window * (int)w + i;
                //     cc->add_clause({-reverse_var, -get_aux_var(firstVar, reverse_var - 1)});
                // }

                // Rewrite
                for (int i = 2; i <= (int)w; i++)
                {
                    int reverse_var = window * (int)w + i;
                    cc->add_clause({-reverse_var, -get_aux_var(firstVar, reverse_var - 1)});
                }
            }

            // Lower part
            int lastVar = window * (int)w + w;
            { // Formula 7
                // for (int i = w - 1; i >= 1; i--)
                // {
                //     int var = window * (int)w + i;
                //     cc->add_clause({-var, get_aux_var(var, lastVar)});
                // }

                // Rewrite
                for (int i = 2; i <= w - 1; i++)
                {
                    int var = window * (int)w + w + 1 - i;
                    cc->add_clause({-var, get_aux_var(var, lastVar)});
                }
            }

            { // Formula 8
                // for (int i = w; i >= 2; i--)
                // {
                //     int var = window * (int)w + i;
                //     cc->add_clause({-get_aux_var(var, lastVar), get_aux_var(var - 1, lastVar)});
                // }

                // Rewrite
                for (int i = 2; i <= w - 1; i++)
                {
                    int var = window * (int)w + w + 1 - i;
                    cc->add_clause({-get_aux_var(var + 1, lastVar), get_aux_var(var, lastVar)});
                }
            }

            { // Formula 9
                // for (int i = 1; i < (int)w; i++)
                // {
                //     int var = window * (int)w + i;
                //     int main = get_aux_var(var, lastVar);
                //     int sub = get_aux_var(var + 1, lastVar);
                //     cc->add_clause({var, sub, -main});
                // }

                for (int i = 2; i <= (int)w - 1; i++)
                {
                    int var = window * (int)w + w + 1 - i;
                    int sub = get_aux_var(var + 1, lastVar);
                    int main = get_aux_var(var, lastVar);
                    cc->add_clause({var, sub, -main});
                }
            }

            // Can be disabled
            // { // Formula 10
            //     // for (int i = 1; i < (int)w; i++)
            //     // {
            //     //     int var = window * (int)w + i;
            //     //     cc->add_clause({-var, -get_aux_var(var + 1, lastVar)});
            //     // }

            //     // Rewrite
            //     for (int i = 2; i <= (int)w; i++)
            //     {
            //         int var = window * (int)w + w + 1 - i;
            //         cc->add_clause({-var, -get_aux_var(var + 1, lastVar)});
            //     }
            // }
        }
    }

    /*
     * Glue adjacent windows with each other.
     * Using lower part of the previous window and upper part of the next window
     * as anchor points to glue.
     *
     *
     * window: index of window (0, 1, 2,...)
     * n : number of variables
     * w : width of window
     */
    void SCLEncoder::glue_window(int window, int n, int w)
    {
        /*  The ladder AMO look like this:
         *      Window 1        Window 2        Window 3        Window 4
         *      1   2   3   |               |               |
         *          2   3   |   4           |               |
         *              3   |   4   5       |               |
         *                  |   4   5   6   |               |
         *                  |       5   6   |   7           |
         *                  |           6   |   7   8       |
         *                  |               |   7   8   9   |
         *                  |               |       8   9   |   10
         *                  |               |           9   |   10  11
         *
         *
         */
        if ((window + 2) * w > n)
        {
            int real_w = n % w;
            for (int i = 1; i <= real_w; i++)
            {
                int first_reverse_var = (window + 1) * (int)w + 1;
                int last_var = window * (int)w + w;

                int reverse_var = (window + 1) * (int)w + i;
                int var = window * (int)w + i + 1;

                cc->add_clause({-get_aux_var(var, last_var), -get_aux_var(first_reverse_var, reverse_var)});
            }
        }
        else
        {
            for (int i = 1; i < (int)w; i++)
            {
                int first_reverse_var = (window + 1) * (int)w + 1;
                int last_var = window * (int)w + w;

                int reverse_var = (window + 1) * (int)w + i;
                int var = window * (int)w + i + 1;

                cc->add_clause({-get_aux_var(var, last_var), -get_aux_var(first_reverse_var, reverse_var)});
            }
        }
    }
}
