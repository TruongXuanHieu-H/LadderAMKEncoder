#include "scl_encoder.h"
#include <iostream>
#include <math.h>
#include <assert.h>
#include "utils.h"

namespace SINGLELADDERAMK
{

    SCLEncoder::SCLEncoder(ClauseContainer *clause_container, VarHandler *var_handler) : Encoder(clause_container, var_handler)
    {
    }

    SCLEncoder::~SCLEncoder() {};

    int SCLEncoder::get_aux_var(int first, int last, int value)
    {
        // std::cout << "Get aux var: " << first << " " << last << " " << value << "\n";
        assert(first <= last);
        assert(last - first >= value - 1);

        auto pair = aux_vars.find({first, last, value});

        if (pair != aux_vars.end())
            return pair->second;

        if (first == last)
            return first;

        int new_aux_var = vh->get_new_var();
        aux_vars.insert({{first, last, value}, new_aux_var});
        return new_aux_var;
    }

    void SCLEncoder::encode_and_solve_ladder_amk(int n, int w, int at_most, int initCondLength, int initCond[])
    {
        (void)initCondLength;
        (void)initCond;

        for (int gw = 0; gw < ceil((float)n / w); gw++)
        {
            encode_window(gw, n, w, at_most);
        }

        for (int gw = 0; gw < ceil((float)n / w) - 1; gw++)
        {
            glue_window(gw, n, w, at_most);
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
    void SCLEncoder::encode_window(int window, int n, int w, int at_most)
    {
        if (window == 0)
        {
            // Encode the first window, which only have lower part
            int lastVar = window * w + w;

            // Lower part
            { // Formula 1
                for (int j = 2; j <= w - 1; j++)
                {
                    int var = window * w + w + 1 - j;
                    cc->add_clause({-var, get_aux_var(var, lastVar, 1)});
                }
            }

            { // Formula 2
                for (int j = 2; j <= w - 1; j++)
                {
                    int var = window * w + w + 1 - j;
                    int upper_s = (j - 1 < at_most) ? (j - 1) : at_most;
                    for (int s = 1; s <= upper_s; s++)
                    {
                        cc->add_clause({-get_aux_var(var + 1, lastVar, s), get_aux_var(var, lastVar, s)});
                    }
                }
            }

            { // Formula 3
                for (int j = 2; j <= w - 1; j++)
                {
                    int var = window * w + w + 1 - j;
                    int upper_s = j < at_most ? j : at_most;
                    for (int s = 2; s <= upper_s; s++)
                    {
                        int sub = get_aux_var(var + 1, lastVar, s - 1);
                        int main = get_aux_var(var, lastVar, s);
                        cc->add_clause({-var, -sub, main});
                    }
                }
            }

            { // Formula 4
                for (int j = 2; j <= w - 1; j++)
                {
                    int var = window * w + w + 1 - j;
                    int upper_s = (j - 1 < at_most) ? (j - 1) : at_most;
                    for (int s = 1; s <= upper_s; s++)
                    {
                        int sub = get_aux_var(var + 1, lastVar, s);
                        int main = get_aux_var(var, lastVar, s);
                        cc->add_clause({var, sub, -main});
                    }
                }
            }

            { // Formula 5
                for (int j = at_most + 1; j <= w; j++)
                {
                    int var = window * w + w + 1 - j;
                    cc->add_clause({-var, -get_aux_var(var + 1, lastVar, at_most)});
                }
            }
        }
        else if (window == ceil((float)n / w) - 1)
        {
            // Encode the last window, which only have upper part and may have width lower than w
            int firstVar = window * w + 1;

            if ((window + 1) * w > n)
            {
                int real_w = n % w; // width < w
                // Upper part
                { // Formula 1
                    for (int j = 2; j <= real_w; j++)
                    {
                        int reverse_var = window * w + j;
                        cc->add_clause({-reverse_var, get_aux_var(firstVar, reverse_var, 1)});
                    }
                }

                { // Formula 2
                    for (int j = 2; j <= real_w; j++)
                    {
                        int reverse_var = window * w + j;
                        int upper_s = (j - 1 < at_most) ? (j - 1) : at_most;
                        for (int s = 1; s <= upper_s; s++)
                        {
                            cc->add_clause({-get_aux_var(firstVar, reverse_var - 1, s), get_aux_var(firstVar, reverse_var, s)});
                        }
                    }
                }

                { // Formula 3
                    for (int j = 2; j <= real_w; j++)
                    {
                        int reverse_var = window * w + j;
                        int upper_s = j < at_most ? j : at_most;
                        for (int s = 2; s <= upper_s; s++)
                        {
                            int sub = get_aux_var(firstVar, reverse_var - 1, s - 1);
                            int main = get_aux_var(firstVar, reverse_var, s);
                            cc->add_clause({-reverse_var, -sub, main});
                        }
                    }
                }

                { // Formula 4
                    for (int j = 2; j <= real_w; j++)
                    {
                        int reverse_var = window * w + j;
                        int upper_s = (j - 1 < at_most) ? (j - 1) : at_most;
                        for (int s = 1; s <= upper_s; s++)
                        {
                            int sub = get_aux_var(firstVar, reverse_var - 1, s);
                            int main = get_aux_var(firstVar, reverse_var, s);
                            cc->add_clause({reverse_var, sub, -main});
                        }
                    }
                }

                { // Formula 5
                    for (int i = at_most + 1; i <= real_w; i++)
                    {
                        int reverse_var = window * w + i;
                        cc->add_clause({-reverse_var, -get_aux_var(firstVar, reverse_var - 1, at_most)});
                    }
                }
            }
            else
            {
                // Upper part
                { // Formula 1
                    for (int j = 2; j <= w - 1; j++)
                    {
                        int reverse_var = window * w + j;
                        cc->add_clause({-reverse_var, get_aux_var(firstVar, reverse_var, 1)});
                    }
                }

                { // Formula 2
                    for (int j = 2; j <= w - 1; j++)
                    {
                        int reverse_var = window * w + j;
                        int upper_s = (j - 1 < at_most) ? (j - 1) : at_most;
                        for (int s = 1; s <= upper_s; s++)
                        {
                            cc->add_clause({-get_aux_var(firstVar, reverse_var - 1, s), get_aux_var(firstVar, reverse_var, s)});
                        }
                    }
                }

                { // Formula 3
                    for (int j = 2; j <= w - 1; j++)
                    {
                        int reverse_var = window * w + j;
                        int upper_s = j < at_most ? j : at_most;
                        for (int s = 2; s <= upper_s; s++)
                        {
                            int sub = get_aux_var(firstVar, reverse_var - 1, s - 1);
                            int main = get_aux_var(firstVar, reverse_var, s);
                            cc->add_clause({-reverse_var, -sub, main});
                        }
                    }
                }

                { // Formula 4
                    for (int j = 2; j <= w - 1; j++)
                    {
                        int reverse_var = window * w + j;
                        int upper_s = (j - 1 < at_most) ? (j - 1) : at_most;
                        for (int s = 1; s <= upper_s; s++)
                        {
                            int sub = get_aux_var(firstVar, reverse_var - 1, s);
                            int main = get_aux_var(firstVar, reverse_var, s);
                            cc->add_clause({reverse_var, sub, -main});
                        }
                    }
                }

                { // Formula 5
                    for (int i = at_most + 1; i <= w; i++)
                    {
                        int reverse_var = window * w + i;
                        cc->add_clause({-reverse_var, -get_aux_var(firstVar, reverse_var - 1, at_most)});
                    }
                }
            }
        }
        else
        {
            // Encode the middle windows, which have both upper and lower path, and always have width w

            // Upper part
            int firstVar = window * w + 1;

            { // Formula 1
                for (int j = 2; j <= w - 1; j++)
                {
                    int reverse_var = window * w + j;
                    cc->add_clause({-reverse_var, get_aux_var(firstVar, reverse_var, 1)});
                }
            }

            { // Formula 2
                for (int j = 2; j <= w - 1; j++)
                {
                    int reverse_var = window * w + j;
                    int upper_s = (j - 1 < at_most) ? (j - 1) : at_most;
                    for (int s = 1; s <= upper_s; s++)
                    {
                        cc->add_clause({-get_aux_var(firstVar, reverse_var - 1, s), get_aux_var(firstVar, reverse_var, s)});
                    }
                }
            }

            { // Formula 3
                for (int j = 2; j <= w - 1; j++)
                {
                    int reverse_var = window * w + j;
                    int upper_s = j < at_most ? j : at_most;
                    for (int s = 2; s <= upper_s; s++)
                    {
                        int sub = get_aux_var(firstVar, reverse_var - 1, s - 1);
                        int main = get_aux_var(firstVar, reverse_var, s);
                        cc->add_clause({-reverse_var, -sub, main});
                    }
                }
            }

            { // Formula 4
                for (int j = 2; j <= w - 1; j++)
                {
                    int reverse_var = window * w + j;
                    int upper_s = (j - 1 < at_most) ? (j - 1) : at_most;
                    for (int s = 1; s <= upper_s; s++)
                    {
                        int sub = get_aux_var(firstVar, reverse_var - 1, s);
                        int main = get_aux_var(firstVar, reverse_var, s);
                        cc->add_clause({reverse_var, sub, -main});
                    }
                }
            }

            { // Formula 5
                for (int i = at_most + 1; i <= w; i++)
                {
                    int reverse_var = window * w + i;
                    cc->add_clause({-reverse_var, -get_aux_var(firstVar, reverse_var - 1, at_most)});
                }
            }

            // Lower part
            int lastVar = window * w + w;

            { // Formula 1
                for (int j = 2; j <= w - 1; j++)
                {
                    int var = window * w + w + 1 - j;
                    cc->add_clause({-var, get_aux_var(var, lastVar, 1)});
                }
            }

            { // Formula 2
                for (int j = 2; j <= w - 1; j++)
                {
                    int var = window * w + w + 1 - j;
                    int upper_s = (j - 1 < at_most) ? (j - 1) : at_most;
                    for (int s = 1; s <= upper_s; s++)
                    {
                        cc->add_clause({-get_aux_var(var + 1, lastVar, s), get_aux_var(var, lastVar, s)});
                    }
                }
            }

            { // Formula 3
                for (int j = 2; j <= w - 1; j++)
                {
                    int var = window * w + w + 1 - j;
                    int upper_s = j < at_most ? j : at_most;
                    for (int s = 2; s <= upper_s; s++)
                    {
                        int sub = get_aux_var(var + 1, lastVar, s - 1);
                        int main = get_aux_var(var, lastVar, s);
                        cc->add_clause({-var, -sub, main});
                    }
                }
            }

            { // Formula 4
                for (int j = 2; j <= w - 1; j++)
                {
                    int var = window * w + w + 1 - j;
                    int upper_s = (j - 1 < at_most) ? (j - 1) : at_most;
                    for (int s = 1; s <= upper_s; s++)
                    {
                        int sub = get_aux_var(var + 1, lastVar, s);
                        int main = get_aux_var(var, lastVar, s);
                        cc->add_clause({var, sub, -main});
                    }
                }
            }

            // { // Formula 5
            //     for (int j = at_most + 1; j <= w; j++)
            //     {
            //         int var = window * w + w + 1 - j;
            //         cc->add_clause({-var, -get_aux_var(var + 1, lastVar, at_most)});
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
    void SCLEncoder::glue_window(int window, int n, int w, int at_most)
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
                int left_first_var = window * w + i + 1;
                int left_last_var = window * w + w;
                int left_count = left_last_var - left_first_var + 1;

                int right_first_var = (window + 1) * w + 1;
                int right_last_var = (window + 1) * w + i;
                int right_count = right_last_var - right_first_var + 1;

                for (int i = 1; i <= at_most; i++)
                {
                    int bound_left = at_most - i + 1;
                    int bound_right = i;

                    if (left_count <= bound_left - 1 || right_count <= bound_right - 1)
                        continue;
                    int var_left = get_aux_var(left_first_var, left_last_var, bound_left);
                    int var_right = get_aux_var(right_first_var, right_last_var, bound_right);
                    cc->add_clause({-var_left, -var_right});
                }
            }
        }
        else
        {
            for (int i = 1; i <= w; i++)
            {
                int left_first_var = window * w + i + 1;
                int left_last_var = window * w + w;
                int left_count = left_last_var - left_first_var + 1;

                int right_first_var = (window + 1) * w + 1;
                int right_last_var = (window + 1) * w + i;
                int right_count = right_last_var - right_first_var + 1;

                for (int i = 1; i <= at_most; i++)
                {
                    int bound_left = at_most - i + 1;
                    int bound_right = i;

                    if (left_count <= bound_left - 1 || right_count <= bound_right - 1)
                        continue;
                    int var_left = get_aux_var(left_first_var, left_last_var, bound_left);
                    int var_right = get_aux_var(right_first_var, right_last_var, bound_right);
                    cc->add_clause({-var_left, -var_right});
                }
            }
        }
    }
}
