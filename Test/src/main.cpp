// This is a script to measure the performance of Pytorch models and RTNeural

#include <iostream>
#include <chrono>
#include <torch/torch.h>
#include <RTNeural/RTNeural.h>
#include <random>

// Function to measure the performance of a PyTorch LSTM model
void measure_performance_torch(torch::nn::LSTM& model, int n_samples) {
    auto signal = torch::rand({ 1,1,1 });

    // Execute the model and measure the time taken
    auto start = std::chrono::steady_clock::now();

    for (size_t i = 0; i < n_samples; ++i) {
        auto out = model->forward(signal);
    }

    auto end = std::chrono::steady_clock::now();

    // Calculate the execution duration in milliseconds
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "PyTorch execution time: " << duration << " milliseconds" << std::endl;
}

template<typename ModelType>

// Function to measure the performance of an RTNeural model
void measure_performance_rt(ModelType model, std::vector<std::vector<float>> signal, int n_samples) {

    auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < n_samples; ++i)
        model.forward(signal[i].data());
    auto end = std::chrono::steady_clock::now();

    // Calculate the execution duration in milliseconds
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "RTNeural execution time: " << duration << " milliseconds" << std::endl;
}

namespace fs = std::filesystem;

// Function that generates a random signal
std::vector<std::vector<float>> generate_signal(size_t n_samples,
    size_t in_size)
{
    std::vector<std::vector<float>> signal(n_samples);
    for (auto& x : signal)
        x.resize(in_size, 0.0f);

    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

    for (size_t i = 0; i < n_samples; ++i)
        for (size_t k = 0; k < in_size; ++k)
            signal[i][k] = distribution(generator);

    return std::move(signal);
}


int main() {

    // Generate signal

    const int64_t input_size = 1;
    const auto n_samples = 48000 * 1;
    auto signal = generate_signal(n_samples, input_size);

    // Hidden_size values

    std::vector<int> hidden_sizes{ 10, 20, 30, 40, 50, 60,70,80 };

    // Open an output file

    std::cout << "Search in the build folder the file output.txt!" << std::endl;

    std::ofstream outputFile("output.txt");

    // Check if the file was opened successfully

    if (!outputFile.is_open()) {
        std::cerr << "Unable to open the output file." << std::endl;
        return 1;
    }

    // Redirect the output stream from the console to the file

    std::streambuf* coutbuf = std::cout.rdbuf(); // Save the current cout buffer
    std::cout.rdbuf(outputFile.rdbuf()); // Redirect cout to outputFile

    std::cout << "Running \"rtneural static model\" example..." << std::endl;

    // Define static model: modify this code to test different model
    {
        RTNeural::ModelT<float, 1, 1,
            RTNeural::LSTMLayerT<float, 1, 10>,
            RTNeural::DenseT<float, 10, 1>> model;

        measure_performance_rt(model, signal, n_samples);
    }
    {

        RTNeural::ModelT<float, 1, 1,
            RTNeural::LSTMLayerT<float, 1, 20>,
            RTNeural::DenseT<float, 20, 1>> model;


        measure_performance_rt(model, signal, n_samples);
    }
    {
        RTNeural::ModelT<float, 1, 1,
            RTNeural::LSTMLayerT<float, 1, 30>,
            RTNeural::DenseT<float, 30, 1>> model;

        measure_performance_rt(model, signal, n_samples);
    }
    {
        RTNeural::ModelT<float, 1, 1,
            RTNeural::LSTMLayerT<float, 1, 40>,
            RTNeural::DenseT<float, 40, 1>> model;

        measure_performance_rt(model, signal, n_samples);

    }
    {
        RTNeural::ModelT<float, 1, 1,
            RTNeural::LSTMLayerT<float, 1, 50>,
            RTNeural::DenseT<float, 50, 1>> model;

        measure_performance_rt(model, signal, n_samples);

    }

    {
        RTNeural::ModelT<float, 1, 1,
            RTNeural::LSTMLayerT<float, 1, 60>,
            RTNeural::DenseT<float, 60, 1>> model;

        measure_performance_rt(model, signal, n_samples);

    }
    {
        RTNeural::ModelT<float, 1, 1,
            RTNeural::LSTMLayerT<float, 1, 70>,
            RTNeural::DenseT<float, 70, 1>> model;

        measure_performance_rt(model, signal, n_samples);
    }

    {
        RTNeural::ModelT<float, 1, 1,
            RTNeural::LSTMLayerT<float, 1, 80>,
            RTNeural::DenseT<float, 80, 1>> model;

        measure_performance_rt(model, signal, n_samples);
    }

    for (int hidden_size : hidden_sizes) {
        // Create LSTM model with variable hidden size
        torch::nn::LSTM lstm_model(torch::nn::LSTMOptions(input_size, hidden_size).num_layers(1));
        std::cout << "Performance of LSTM model with hidden size " << hidden_size << std::endl;
        measure_performance_torch(lstm_model, n_samples);
    }

    // Restore cout buffer
    std::cout.rdbuf(coutbuf); // Restore original cout buffer

    // Close the output file
    outputFile.close();

    return 0;
}