//
// Created by markus on 5/3/17.
//


#include "../include/ann.h"
#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::setw;
using std::left;
using std::right;
using std::showpos;
using std::noshowpos;


void annTraining() {

    cout << endl << "XOR test started." << endl;
    //
    // Neuron layers and nodes
    //
    const float learning_rate = 0.7f;
    const unsigned int num_input = 3;
    const unsigned int num_output = 2;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 1;

    //
    // Error and epochs
    //
    const float desired_error = 0.001f;
    const unsigned int max_epochs = 500;
    const unsigned int epochs_between_reports = 100;

    cout << endl << "Creating network." << endl;

    FANN::neural_net net;
    net.create_standard(num_layers, num_input, num_neurons_hidden, num_output);

    net.set_learning_rate(learning_rate);

    net.set_activation_steepness_hidden(1.0);
    net.set_activation_steepness_output(1.0);

    net.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
    net.set_activation_function_output(FANN::SIGMOID_SYMMETRIC_STEPWISE);

    // Output network type and parameters
    cout << endl << "Network Type                         :  ";
    switch (net.get_network_type())
    {
        case FANN::LAYER:
            cout << "LAYER" << endl;
            break;
        case FANN::SHORTCUT:
            cout << "SHORTCUT" << endl;
            break;
        default:
            cout << "UNKNOWN" << endl;
            break;
    }
    net.print_parameters();

    cout << endl << "Training network." << endl;

    FANN::training_data data;
    if (data.read_train_from_file("xor.data"))
    {
        // Initialize and train the network with the data
        net.init_weights(data);

        cout << "Max Epochs " << setw(8) << max_epochs << ". "
             << "Desired Error: " << left << desired_error << right << endl;
        net.set_callback(print_callback, NULL);
        net.train_on_data(data, max_epochs,
                          epochs_between_reports, desired_error);
    }

    cout << endl << "Testing network." << endl;

    for (unsigned int i = 0; i < data.length_train_data(); ++i)
    {
        // Run the network on the test data
        fann_type *calc_out = net.run(data.get_input()[i]);

        cout << "XOR test (" << showpos << data.get_input()[i][0] << ", "
             << data.get_input()[i][1] << ") -> " << *calc_out
             << ", should be " << data.get_output()[i][0] << ", "
             << "difference = " << noshowpos
             << fann_abs(*calc_out - data.get_output()[i][0]) << endl;
    }

    cout << endl << "Saving network." << endl;

    // Save the network in floating point and fixed point
    net.save("xor_float.net");
    unsigned int decimal_point = net.save_to_fixed("xor_fixed.net");
    data.save_train_to_fixed("xor_fixed.data", decimal_point);

    cout << endl << "XOR test completed." << endl;
}


// Callback function that simply prints the information to cout
int print_callback(FANN::neural_net &net, FANN::training_data &train,
                   unsigned int max_epochs, unsigned int epochs_between_reports,
                   float desired_error, unsigned int epochs, void *user_data)
{
    cout << "Epochs     " << setw(8) << epochs << ". "
         << "Current Error: " << left << net.get_MSE() << right << endl;
    return 0;
}

