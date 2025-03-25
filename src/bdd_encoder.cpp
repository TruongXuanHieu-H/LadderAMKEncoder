#include "bdd_encoder.h"

#include <numeric>   //iota
#include <algorithm> //generate
#include <cmath>     //floor,ceil
#include <deque>
#include <assert.h>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include <regex>

namespace SINGLELADDERAMK
{

    BDDEncoder::BDDEncoder(ClauseContainer *clause_container, VarHandler *var_handler) : Encoder(clause_container, var_handler)
    {
    }

    BDDEncoder::~BDDEncoder() {};

    void BDDEncoder::encode_and_solve_ladder_amo(int n, int w, int initCondLength, int initCond[])
    {
        for (int i = 1; i <= n - w + 1; i++)
        {
            std::vector<int> items(w);
            std::iota(items.begin(), items.end(), i);
            encode_amo(items.begin(), items.end());
        }

        (void)initCondLength;
        (void)initCond;
    }

    void BDDEncoder::encode_amo(vec_int_it it_begin, vec_int_it it_end)
    {
        // std::cout << "Encode AMO: " << *it_begin << "\n";

        int first_free_var = vh->last_var() + 1;

        int constr_length = std::distance(it_begin, it_end);
        if (constr_length < 2)
            return;
        if (constr_length == 2)
        {
            int v1 = *it_begin;
            int v2 = *std::next(it_begin);
            cc->add_clause({v1, v2});
            cc->add_clause({-1 * v1, -1 * v2});
            return;
        }

        std::string command = "python3 bdd_amo.py " + std::to_string(constr_length) + " " + std::to_string(*it_begin) + " " + std::to_string(first_free_var);

        FILE *pipe = popen(command.c_str(), "r");
        assert(pipe);

        char buffer[1024];
        std::string result;

        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }

        pclose(pipe);

        std::stringstream ss(result);
        std::string temp = "";
        std::vector<std::string> lines;

        while (std::getline(ss, temp))
        {
            if (temp[0] == '#')
            {
                // std::cout << temp << "\n";
            }
            else
            {
                lines.push_back(temp);
            }
        }

        // Consume used aux variables
        std::regex numberPattern(R"(-?\d+)");
        std::sregex_iterator it(lines[0].begin(), lines[0].end(), numberPattern);
        std::sregex_iterator end;
        int num_aux_vars = std::distance(it, end);

        // std::cout << "Used " << num_aux_vars << ": " << lines[0] << "\n";
        for (int i = 0; i < num_aux_vars; i++)
        {
            // std::cout << "Consume: " << vh->get_new_var() << "\n";
            vh->get_new_var();
        }

        for (size_t i = 1; i < lines.size(); ++i)
        {
            // std::cout << lines[i] << "\n";
            std::sregex_iterator it_line(lines[i].begin(), lines[i].end(), numberPattern);
            std::sregex_iterator end_line;
            std::vector<int> literals;

            while (it_line != end_line)
            {
                // std::cout << "Regex: " << it_line->str() << "\n";
                literals.push_back(std::stoi(it_line->str()));
                ++it_line;
            }
            cc->add_clause(literals);
        }
    };
}
