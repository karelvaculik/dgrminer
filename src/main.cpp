#include <iostream>
#include <vector>
#include "DGRMiner.h"

using namespace dgrminer;

static void show_usage(std::string name)
{
    std::cerr << "DGRMiner, version 2.0.0"
              << std::endl
              << "Author: Karel Vaculik (vaculik.dev@gmail.com)"
              << std::endl
              << std::endl
              << "Usage: " << name << " <option(s)>"
              << std::endl
              << std::endl
              << "\t-h, --help\t\tShows this help message"
              << std::endl
              << "\t-v, --verbose\t\tVerbose output"
              << std::endl
              << "\t-i INPUTFILE\t\tSpecifies the input file"
              << std::endl
              << "\t-o OUTPUTFILE\t\tSpecifies the common prefix for output files"
              << std::endl
              << "\t-s K\t\t\tSpecifies the minimum support; decimal value from [0.0, 1.0] if old support measure is used"
              << std::endl
              << "\t-c M\t\t\tSpecifies the minimum confidence; decimal value from [0.0, 1.0]; if not specified, the confidence is not computed"
              << std::endl
              << "\t-w N\t\t\tSpecifies the size of window; 10 by default"
              << std::endl
              << "\t-t {bin_nodes,bin_all}\tSpecifies the type of time abstraction"
              << std::endl
              << "\t-a A\t\t\tSwitches to anomaly detection; Only anomalies with outlierness >= A will be outputted; A is a decimal value from [0.0, 1.0]"
              << std::endl
              << "\t-m NEW_MEASURES \tUse new support and dynamic measures (currently not allowed for a set of dynamic graphs)."
              << std::endl;
}

int main(int argc, char* argv[])
{
    // experiments as in ITAT paper (data may be a little different)
    //run_DGRMiner("data/enron_multiedges_with_edge_ids_keep", "RES", 0.10, 0.0, true, 10, "bin_nodes", false, 0.0, true, false);
//    run_DGRMiner("data/enron_multiedges_with_edge_ids_del", "RES", 0.10, 0.0, true, 10, "bin_nodes", false, 0.0, true, false);
//     run_DGRMiner("data/all_resolution_proofs_with_ids.txt", "RES", 10, 0.2, true, 10, "", false, 0.0, true, true);
//    run_DGRMiner("data/all_resolution_proofs_with_ids.txt", "RES", 0.05, 0.0, true, 10, "bin_all", false, 0.0, true, false);
    run_DGRMiner("data/synth_graph_one_with_ids.txt", "RES", 0.10, 0.0, true, 10, "bin_all", false, 0.0, true, false);
//    run_DGRMiner("data/synth_graphs_20_with_ids.txt", "RES", 0.10, 0.0, true, 10, "", false, 0.0, true, false);
    if (true)
        return 0;

    // REGULAR CODE:


    if (argc < 3) {
        show_usage(argv[0]);
        return 1;
    }
    std::vector <std::string> sources;
    std::string input_file = "";
    std::string output_file = "";
    double min_support = -1.0;
    double min_confidence = 0.0;
    double min_anomaly_outlierness = 0.0;
    bool compute_confidence = false;
    bool search_for_anomalies = false;
    bool new_measures = false;
    int window_size = 10;
    std::string str_timeabstraction = "";
    bool verbose = false;
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help"))
        {
            show_usage(argv[0]);
            return 0;
        }
        else if ((arg == "-v") || (arg == "--verbose"))
        {
            verbose = true;
        }
        else if (arg == "-i")
        {
            if (i + 1 < argc)
            { // Make sure we aren't at the end of argv!
                input_file = argv[++i]; // Increment 'i' so we don't get the argument as the next argv[i].
            }
            else { // Uh-oh, there was no argument to the destination option.
                std::cerr << "-i option requires one argument." << std::endl;
                return 1;
            }
        }
        else if (arg == "-o")
        {
            if (i + 1 < argc)
            { // Make sure we aren't at the end of argv!
                output_file = argv[++i]; // Increment 'i' so we don't get the argument as the next argv[i].
            }
            else { // Uh-oh, there was no argument to the destination option.
                std::cerr << "-o option requires one argument." << std::endl;
                return 1;
            }
        }
        else if (arg == "-s")
        {
            if (i + 1 < argc)
            { // Make sure we aren't at the end of argv!
                std::string str_support = argv[++i]; // Increment 'i' so we don't get the argument as the next argv[i].
                try
                {
                    min_support = std::stod(str_support);
                    if (min_support < 0 || min_support > 1)
                    {
                        std::cerr << "-s option requires a numeric argument from interval [0, 1]" << std::endl;
                        return 1;
                    }
                }
                catch (std::exception&)
                {
                    std::cerr << "-s option requires a numeric argument." << std::endl;
                    return 1;
                }

            }
            else { // Uh-oh, there was no argument to the destination option.
                std::cerr << "-s option requires one argument." << std::endl;
                return 1;
            }
        }
        else if (arg == "-c")
        {
            if (i + 1 < argc)
            { // Make sure we aren't at the end of argv!
                std::string str_confidence = argv[++i]; // Increment 'i' so we don't get the argument as the next argv[i].
                try
                {
                    min_confidence = std::stod(str_confidence);
                    compute_confidence = true;
                    if (min_confidence < 0 || min_confidence > 1)
                    {
                        std::cerr << "-c option requires a numeric argument from interval [0, 1]" << std::endl;
                        return 1;
                    }
                }
                catch (std::exception&)
                {
                    std::cerr << "-c option requires a numeric argument." << std::endl;
                    return 1;
                }

            }
            else { // Uh-oh, there was no argument to the destination option.
                std::cerr << "-c option requires one argument." << std::endl;
                return 1;
            }
        }
        else if (arg == "-w")
        {
            if (i + 1 < argc)
            { // Make sure we aren't at the end of argv!
                std::string str_windowsize = argv[++i]; // Increment 'i' so we don't get the argument as the next argv[i].
                try
                {
                    window_size = std::stoi(str_windowsize);
                    if (window_size < 2)
                        std::cerr << "-w option requires an integer argument >= 2." << std::endl;
                    else
                        std::cerr << "-w : " << window_size << std::endl;
                }
                catch (std::exception&)
                {
                    std::cerr << "-w option requires an integer argument >= 2." << std::endl;
                }



            }
            else { // Uh-oh, there was no argument to the destination option.
                std::cerr << "-w option requires one argument." << std::endl;
                return 1;
            }
        }
        else if (arg == "-t")
        {
            if (i + 1 < argc)
            { // Make sure we aren't at the end of argv!
                str_timeabstraction = argv[++i]; // Increment 'i' so we don't get the argument as the next argv[i].
                if (str_timeabstraction != "bin_nodes" && str_timeabstraction != "bin_all")
                {
                    std::cerr << "-t option requires either bin_nodes or bin_all." << std::endl;
                    return 1;
                }
            }
            else { // Uh-oh, there was no argument to the destination option.
                std::cerr << "-t option requires one argument." << std::endl;
                return 1;
            }
        }
        else if (arg == "-a")
        {
            if (i + 1 < argc)
            { // Make sure we aren't at the end of argv!
                std::string str_anomaly_outlierness = argv[++i]; // Increment 'i' so we don't get the argument as the next argv[i].
                try
                {
                    min_anomaly_outlierness = std::stod(str_anomaly_outlierness);
                    search_for_anomalies = true;
                    if (min_anomaly_outlierness < 0 || min_anomaly_outlierness > 1)
                    {
                        std::cerr << "-a option requires a numeric argument from interval [0, 1]" << std::endl;
                        return 1;
                    }
                }
                catch (std::exception&)
                {
                    std::cerr << "-a option requires a numeric argument." << std::endl;
                    return 1;
                }

            }
            else { // Uh-oh, there was no argument to the destination option.
                std::cerr << "-a option requires one argument." << std::endl;
                return 1;
            }
        }
        else if ((arg == "-m")) {
            new_measures = true;
        }
        else {
            sources.push_back(argv[i]);
        }
    }


    if (!search_for_anomalies && (input_file == "" || output_file == "" || min_support < 0))
    {
        std::cerr << "Options -i -o -s are mandatory." << std::endl;
        return 1;
    }
    if (search_for_anomalies && (input_file == "" || output_file == "" || min_support < 0 || !compute_confidence))
    {
        std::cerr << "Options -i -o -s -c are mandatory for anomaly detection." << std::endl;
        return 1;
    }

    if (new_measures && search_for_anomalies) {
        std::cerr << "New measures are not allowed for search anomalies." << std::endl;
    }

    run_DGRMiner(input_file, output_file, min_support, min_confidence, compute_confidence, window_size,
                 str_timeabstraction, search_for_anomalies, min_anomaly_outlierness, verbose, new_measures);

    return 0;
}
