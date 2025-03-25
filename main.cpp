#include <iostream>
#include <signal.h>
#include <iomanip>
#include <map>

#include "src/ladder_amk_encoder.h"

using namespace SINGLELADDERAMK;

const int ver{1};

static void SIGINT_exit(int);

static void (*signal_SIGINT)(int);
static void (*signal_SIGXCPU)(int);
static void (*signal_SIGSEGV)(int);
static void (*signal_SIGTERM)(int);
static void (*signal_SIGABRT)(int);

static void SIGINT_exit(int signum)
{
    signal(SIGINT, signal_SIGINT);
    signal(SIGXCPU, signal_SIGXCPU);
    signal(SIGSEGV, signal_SIGSEGV);
    signal(SIGTERM, signal_SIGTERM);
    signal(SIGABRT, signal_SIGABRT);

    std::cout << "c Signal interruption." << std::endl;

    fflush(stdout);
    fflush(stderr);

    raise(signum);
}

static const std::map<std::string, std::string> option_list = {
    {"--help", "Print usage message with all possible options"},
    {"--naive", "Use naive encoding for ladder AMO constraints"},
    {"--reduced", "Use reduced naive encoding for ladder AMO constraints"},
    {"--seq", "Use sequential encoding for ladder AMO constraints"},
    {"--BDD", "Use BDD encoding for ladder AMO constraints (use pblib)"},
    {"--product", "Use 2-Product encoding for ladder AMO constraints"},
    {"--card", "Use cardinality network encoding for ladder AMO constraints (use pblib)"},
    {"--duplex", "Use duplex encoding for ladder AMO constraints"},
    {"--scl", "Use scl encoding for ladder AMO constraints"},
    {"--check-solution", "Calculate the antibandwidth of the found SAT solution and compare it to the actual width [default: false]"},
    {"-set-at-most <new k>", "Set the at-most value for AMK constraint, has to be positive [default: 2, min: 1]"},
    {"-set-lb <new LB>", "Overwrite predefined LB with <new LB>, has to be at least 2"},
    {"-set-ub <new UB>", "Overwrite predefined UB with <new UB>, has to be positive"},
};

void print_usage()
{
    std::cout << "usage: ladder_amo_enc number_of_elements [ <option> ... ] " << std::endl;
    std::cout << "where '<option>' is one of the following options:" << std::endl;
    std::cout << std::endl;
    for (auto option : option_list)
    {
        std::cout << std::left << "\t" << std::setw(30) << option.first << "\t" << option.second << std::endl;
    }
    std::cout << std::endl;
}

int get_number_arg(std::string const &arg)
{
    try
    {
        std::size_t pos;
        int x = std::stoi(arg, &pos);
        if (pos < arg.size())
        {
            std::cerr << "Trailing characters after number: " << arg << '\n';
        }
        return x;
    }
    catch (std::invalid_argument const &ex)
    {
        std::cerr << "Invalid number: " << arg << '\n';
        return 0;
    }
    catch (std::out_of_range const &ex)
    {
        std::cerr << "Number out of range: " << arg << '\n';
        return 0;
    }
}

int main(int argc, char **argv)
{
    (void)argc;

    signal_SIGINT = signal(SIGINT, SIGINT_exit);
    signal_SIGXCPU = signal(SIGXCPU, SIGINT_exit);
    signal_SIGSEGV = signal(SIGSEGV, SIGINT_exit);
    signal_SIGTERM = signal(SIGTERM, SIGINT_exit);
    signal_SIGABRT = signal(SIGABRT, SIGINT_exit);

    LadderAMKEncoder *ladder_amo_enc;

    if (argc < 2)
    {
        std::cout << "c Ladder Encoder 0." << ver << "." << std::endl;
        std::cerr << "c Error, number of elements was not specified." << std::endl;
        print_usage();
        return 1;
    }

    std::cout << "c Ladder Encoder 0." << ver << "." << std::endl;

    ladder_amo_enc = new LadderAMKEncoder();

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            ladder_amo_enc->set_n(atoi(argv[i]));
        }
        else if (argv[i] == std::string("--help"))
        {
            print_usage();
            delete ladder_amo_enc;
            return 1;
        }
        else if (argv[i] == std::string("--naive"))
        {
            ladder_amo_enc->enc_choice = EncodeType::naive;
        }
        else if (argv[i] == std::string("--reduced"))
        {
            ladder_amo_enc->enc_choice = EncodeType::reduced;
        }
        else if (argv[i] == std::string("--seq"))
        {
            ladder_amo_enc->enc_choice = EncodeType::seq;
        }
        else if (argv[i] == std::string("--BDD"))
        {
            ladder_amo_enc->enc_choice = EncodeType::BDD;
        }
        else if (argv[i] == std::string("--product"))
        {
            ladder_amo_enc->enc_choice = EncodeType::product;
        }
        else if (argv[i] == std::string("--duplex"))
        {
            ladder_amo_enc->enc_choice = EncodeType::duplex;
        }
        else if (argv[i] == std::string("--scl"))
        {
            ladder_amo_enc->enc_choice = EncodeType::scl;
        }
        else if (argv[i] == std::string("--card"))
        {
            ladder_amo_enc->enc_choice = EncodeType::card;
        }
        else if (argv[i] == std::string("--check-solution"))
        {
            ladder_amo_enc->check_solution = true;
        }
        else if (argv[i] == std::string("-set-at-most"))
        {
            ladder_amo_enc->at_most = get_number_arg(argv[++i]);
            if (ladder_amo_enc->at_most < 1)
            {
                std::cout << "Error, at-most value has to be at least 1." << std::endl;
                delete ladder_amo_enc;
                return 1;
            }
            std::cout << "c At-most value is set to " << ladder_amo_enc->at_most << "." << std::endl;
        }
        else if (argv[i] == std::string("-set-lb"))
        {
            ladder_amo_enc->forced_lb = get_number_arg(argv[++i]);
            if (ladder_amo_enc->forced_lb < 2)
            {
                std::cout << "Error, width has to be at least 2." << std::endl;
                delete ladder_amo_enc;
                return 1;
            }
            ladder_amo_enc->overwrite_lb = true;
            std::cout << "c LB is predefined as " << ladder_amo_enc->forced_lb << "." << std::endl;
        }
        else if (argv[i] == std::string("-set-ub"))
        {
            ladder_amo_enc->forced_ub = get_number_arg(argv[++i]);
            if (ladder_amo_enc->forced_ub < 0)
            {
                std::cout << "Error, width has to be positive." << std::endl;
                delete ladder_amo_enc;
                return 1;
            }
            ladder_amo_enc->overwrite_ub = true;
            std::cout << "c UB is predefined as " << ladder_amo_enc->forced_ub << "." << std::endl;
        }
        else
        {
            std::cerr << "Unrecognized option: " << argv[i] << std::endl;

            delete ladder_amo_enc;
            return 1;
        }
    }

    ladder_amo_enc->encode_and_solve_ladder_amk();

    delete ladder_amo_enc;
}
