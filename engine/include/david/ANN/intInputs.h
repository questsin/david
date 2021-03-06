#pragma once

#include "fann/fann.h"
#include "fann/fann_cpp.h"
#include "david/utils/utils.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <ios>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <david/TreeGen.h>
#include <bitset>
using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::left;
using std::right;
using std::showpos;
using std::noshowpos;

namespace intNetwork {
const std::string trainingdatafile = "intNetworkTrainingFile.txt";

// Callback function that simply prints the information to cout
int print_callback(
    FANN::neural_net &net,
    FANN::training_data &train,
    unsigned int max_epochs,
    unsigned int epochs_between_reports,
    float desired_error,
    unsigned int epochs,
    void *user_data);

// Test function that demonstrates usage of the fann C++ wrapper
void train_network(
    const std::string folder,
    const unsigned int* layers,
    const unsigned int nrOfLayers,
    const float learning_rate,
    const float desired_error,
    const unsigned int max_iterations,
    const unsigned int iterations_between_reports);

void generateTrainingFile(
    const std::string folder,
    const unsigned int max_trainingSets,
    const unsigned int* layers,
    const unsigned int nrOfLayers);

void run();

}