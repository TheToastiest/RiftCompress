// =====================================================================================
// main.cpp - Benchmark and Test Runner for RiftCompress
// =====================================================================================
#include "../include/riftcompress.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <iomanip>
#include <numeric>
#include <string>

// --- Helper to generate test data ---
byte_vec generate_sample_data(size_t size) {
    byte_vec data(size);
    // Create somewhat compressible data (repeating patterns)
    for (size_t i = 0; i < size; ++i) {
        data[i] = static_cast<unsigned char>(i % 256);
    }
    return data;
}

// --- Helper to print results in a formatted way ---
void print_header() {
    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << std::left
        << std::setw(15) << "Algorithm"
        << std::setw(15) << "Payload Size"
        << std::setw(15) << "Comp. Ratio"
        << std::setw(25) << "Compress Speed (MB/s)"
        << std::setw(25) << "Decompress Speed (MB/s)" << std::endl;
    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
}

void print_result(const std::string& name, size_t original_size, double ratio, double comp_speed, double decompress_speed) {
    std::cout << std::left << std::fixed << std::setprecision(2)
        << std::setw(15) << name
        << std::setw(15) << (std::to_string(original_size) + " B")
        << std::setw(15) << (std::to_string(ratio) + "x")
        << std::setw(25) << comp_speed
        << std::setw(25) << decompress_speed << std::endl;
}


/**
 * @brief Runs a benchmark for a given compression algorithm.
 */
void run_benchmark(std::unique_ptr<CompressionAlgorithm> algo, const std::string& name, size_t payload_size) {
    try {
        Compressor compressor(std::move(algo));
        byte_vec original_data = generate_sample_data(payload_size);

        const int iterations = (payload_size < 4096) ? 10000 : 1000;
        double total_comp_time = 0.0;
        double total_decompress_time = 0.0;
        size_t total_compressed_size = 0;

        // --- Run Benchmark ---
        for (int i = 0; i < iterations; ++i) {
            // Measure Compression
            auto start_comp = std::chrono::high_resolution_clock::now();
            byte_vec compressed = compressor.compress(original_data);
            auto end_comp = std::chrono::high_resolution_clock::now();

            if (compressed.empty()) throw std::runtime_error("Compression failed.");
            total_compressed_size += compressed.size();
            total_comp_time += std::chrono::duration<double>(end_comp - start_comp).count();

            // Measure Decompression
            auto start_decompress = std::chrono::high_resolution_clock::now();
            byte_vec decompressed = compressor.decompress(compressed);
            auto end_decompress = std::chrono::high_resolution_clock::now();

            if (decompressed.empty()) throw std::runtime_error("Decompression failed.");
            total_decompress_time += std::chrono::duration<double>(end_decompress - start_decompress).count();

            // Verify correctness on the first iteration
            if (i == 0 && original_data != decompressed) {
                throw std::runtime_error("Verification failed: Data mismatch.");
            }
        }

        // --- Calculate Metrics ---
        double avg_compressed_size = static_cast<double>(total_compressed_size) / iterations;
        double compression_ratio = static_cast<double>(original_data.size()) / avg_compressed_size;

        double total_data_processed_gb = (static_cast<double>(original_data.size()) * iterations) / (1024 * 1024);

        double compression_speed = total_data_processed_gb / total_comp_time;
        double decompression_speed = total_data_processed_gb / total_decompress_time;

        print_result(name, payload_size, compression_ratio, compression_speed, decompression_speed);

    }
    catch (const std::exception& e) {
        std::cerr << "ERROR in " << name << " benchmark: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "Starting RiftCompress Benchmark..." << std::endl;

    const std::vector<size_t> payload_sizes = {
        512,       // Medium packet
        1400,      // Large packet (near MTU)
        16 * 1024, // 16 KB block
        128 * 1024 // 128 KB block
    };

    print_header();

    for (size_t size : payload_sizes) {
        run_benchmark(std::make_unique<LZ4Algorithm>(), "LZ4", size);
    }

    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;

    for (size_t size : payload_sizes) {
        run_benchmark(std::make_unique<ZstdAlgorithm>(), "Zstd", size);
    }

    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "Benchmark finished." << std::endl;

    return 0;
}
