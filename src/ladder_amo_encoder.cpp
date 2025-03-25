#include "ladder_amo_encoder.h"

#include <iostream>
#include <fstream>
#include <assert.h>
#include <chrono>

namespace SINGLELADDERAMK
{
    LadderAMKEncoder::LadderAMKEncoder() {};

    LadderAMKEncoder::~LadderAMKEncoder()
    {
    }

    void LadderAMKEncoder::set_n(int n)
    {
        this->n = n;
    }

    void LadderAMKEncoder::encode_and_solve_ladder_amk()
    {
        int w_from, w_to;
        setup_bounds(w_from, w_to);
        encode_and_solve_ladder_amk_problem(w_from, w_to, 1, 10);
    }

    void LadderAMKEncoder::setup_bounds(int &w_from, int &w_to)
    {
        lookup_bounds(w_from, w_to);

        if (overwrite_lb)
        {
            std::cout << "c LB " << w_from << " is overwritten with " << forced_lb << "." << std::endl;
            w_from = forced_lb;
        }
        if (overwrite_ub)
        {
            std::cout << "c UB " << w_to << " is overwritten with " << forced_ub << "." << std::endl;
            w_to = forced_ub;
        }
        if (w_from > w_to)
        {
            int tmp = w_from;
            w_from = w_to;
            w_to = tmp;
            std::cout << "c Flipped LB and UB to avoid LB > UB ";
            std::cout << "(LB = " << w_from << ", UB = " << w_to << ")." << std::endl;
        }

        assert((w_from <= w_to) && (w_from >= 1));
    }

    void LadderAMKEncoder::lookup_bounds(int &lb, int &ub)
    {
        lb = 2;
        ub = n;
    }

    void LadderAMKEncoder::encode_and_solve_ladder_amk_problem(int start_w, int stop_w, int step, int prev_res)
    {
        for (int w = start_w; (w > 0 && w != stop_w); w += step)
        {
            bool error = encode_and_solve_ladder_amk_problem(w);
            if (error)
                break;
            if (SAT_res != prev_res)
                break; // reached UNSAT->SAT or SAT->UNSAT point
            prev_res = SAT_res;
        }
    }

    void LadderAMKEncoder::setup_solving()
    {
        setup_cadical();

        vh = new VarHandler(1, n);

        // Do not split clauses, so split_clause is set to 0
        cc = new CadicalClauseContainer(vh, 0, solver);

        setup_encoder();
    }

    void LadderAMKEncoder::cleanup_solving()
    {
        delete enc;
        delete cc;
        delete vh;
        delete solver;
    }

    void LadderAMKEncoder::setup_cadical()
    {
        std::cout << "c Initializing CaDiCaL ";
        solver = new CaDiCaL::Solver;
        std::cout << "(version " << solver->version() << ")." << std::endl;
    }

    void LadderAMKEncoder::setup_encoder()
    {

        switch (enc_choice)
        {
        case naive:
            std::cout << "c Initializing a NAIVE encoder with n = " << n << "." << std::endl;
            enc = new NaiveEncoder(cc, vh);
            break;
        case reduced:
            std::cout << "c Initializing a REDUCED encoder with n = " << n << "." << std::endl;
            enc = new ReducedEncoder(cc, vh);
            break;
        case seq:
            std::cout << "c Initializing a SEQ encoder with n = " << n << "." << std::endl;
            enc = new SeqEncoder(cc, vh);
            break;
        case BDD:
            std::cout << "c Initializing a BDD encoder with n = " << n << "." << std::endl;
            enc = new BDDEncoder(cc, vh);
            break;
        case product:
            std::cout << "c Initializing a Product encoder with n = " << n << "." << std::endl;
            enc = new ProductEncoder(cc, vh);
            break;
        case duplex:
            std::cout << "c Initializing a Duplex encoder with n = " << n << "." << std::endl;
            enc = new DuplexEncoder(cc, vh);
            break;
        case scl:
            std::cout << "c Initializing a Ladder encoder with n = " << n << "." << std::endl;
            enc = new SCLEncoder(cc, vh);
            break;
        case card:
            std::cout << "c Initializing a Cardinality network encoder with n = " << n << "." << std::endl;
            enc = new CardinalityEncoder(cc, vh);
            break;
        default:
            std::cout << "c Initializing a null encoder with n = " << n << "." << std::endl;
            break;
        }
    }

    bool LadderAMKEncoder::encode_and_solve_ladder_amk_problem(int w)
    {
        std::cout << "c Ladder AMK problem with n = " << n << " and w = " << w << std::endl;
        if (n < 1)
        {
            std::cout << "c The input elements is too small, there is nothing to encode here." << std::endl;
            SAT_res = 0; // should break loop
            return 0;
        }
        if (w < 2)
        {
            std::cout << "c There is always at least 1 distance between any pair of elements. There is nothing to encode here." << std::endl;
            SAT_res = 10;
            return 0;
        }

        setup_solving();
        std::cout << "c Encoding starts with w = " << w << ":" << std::endl;

        auto t1 = std::chrono::high_resolution_clock::now();
        enc->encode_ladder_amo(n, w);
        auto t2 = std::chrono::high_resolution_clock::now();
        auto encode_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

        std::cout << "c\tEncoding duration: " << encode_duration << "ns" << std::endl;
        std::cout << "c\tNumber of clauses: " << cc->size() << std::endl;
        std::cout << "c\tNumber of irredundant clauses: " << solver->irredundant() << std::endl;
        std::cout << "c\tNumber of variables: " << vh->size() << std::endl;
        std::cout << "c SAT Solving starts:" << std::endl;

        t1 = std::chrono::high_resolution_clock::now();
        SAT_res = solver->solve();
        t2 = std::chrono::high_resolution_clock::now();
        auto solving_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        std::cout << "c\tSolving duration: " << solving_duration << " ns" << std::endl;
        std::cout << "c\tAnswer: " << std::endl;
        if (SAT_res == 10)
        {
            std::cout << "s SAT (w = " << w << ")" << std::endl;
        }
        else if (SAT_res == 20)
            std::cout << "s UNSAT (w = " << w << ")" << std::endl;
        else
        {
            std::cout << "s Error at w = " << w << ", SAT result: " << SAT_res << std::endl;
            cleanup_solving();
            return 1;
        }

        if (check_solution && SAT_res == 10)
        {
            bool verify_value = calculate_sat_solution(w);
            if (!verify_value)
            {
                std::cerr << "c Error, the solution is not correct." << std::endl;

                cleanup_solving();
                return 1;
            }
        }

        std::ofstream outFile;
        outFile.open("./report/" + encode_type_map.find(enc_choice)->second + "_report.txt", std::ios::app);
        if (!outFile)
            return 1;
        outFile << w << ": " << solving_duration << " ns" << std::endl;
        outFile.close();

        cleanup_solving();

        std::cout << "c" << std::endl
                  << "c" << std::endl;
        return 0;
    }

    bool LadderAMKEncoder::calculate_sat_solution(int w)
    {
        bool *solution = new bool[n];
        for (int i = 0; i < n; i++)
        {
            solution[i] = solver->val(i + 1) > 0;
        }

        int max_count_true = 1;
        for (int i = 0; i < n - w + 1; i++)
        {
            int count_true = 0;
            for (int j = i; j < i + w; j++)
            {
                if (solution[j])
                {
                    if (count_true < max_count_true)
                    {
                        count_true++;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }

        delete[] solution;
        return true;
    }
}