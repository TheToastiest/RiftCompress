// =====================================================================================
// main.cpp - Example Usage
// =====================================================================================
#include "../include/riftcompress.hpp"
#include <iostream>
#include <string>
#include <cassert>
#include <random>
#include <algorithm>
#include <iomanip>

// --- Helper function to generate a compressible data packet ---
byte_vec generate_compressible_packet(size_t size) {
    // Create a base pattern that has repetition.
    std::string pattern_str = "PlayerState:pos_x=123.45,pos_y=67.89,health=100,mana=50,inventory=[item1,item2,item3];";
    byte_vec pattern(pattern_str.begin(), pattern_str.end());

    byte_vec data;
    data.reserve(size);

    // Repeat the pattern until the desired size is reached.
    while (data.size() < size) {
        data.insert(data.end(), pattern.begin(), pattern.end());
    }

    // Trim to the exact size.
    data.resize(size);
    return data;
}

// --- Helper function to run a full compression/decompression test ---
void run_compression_test(Compressor& compressor, const byte_vec& original_data, const std::string& algo_name) {
    std::cout << "\n--- Testing " << algo_name << " ---" << std::endl;
    std::cout << "Original packet size: " << original_data.size() << " bytes." << std::endl;

    // 1. Compress
    byte_vec compressed_data = compressor.compress(original_data);
    assert(!compressed_data.empty());
    double ratio = static_cast<double>(compressed_data.size()) / original_data.size() * 100.0;
    std::cout << "Compressed size: " << compressed_data.size() << " bytes." << std::endl;
    std::cout << "Compression ratio: " << std::fixed << std::setprecision(2) << ratio << "%" << std::endl;

    // 2. Decompress
    byte_vec decompressed_data = compressor.decompress(compressed_data);
    assert(!decompressed_data.empty());
    std::cout << "Decompression successful." << std::endl;

    // 3. Verify
    assert(original_data.size() == decompressed_data.size());
    assert(original_data == decompressed_data);
    std::cout << "Verification successful: Original and decompressed packets match." << std::endl;
}

//int main() {
//    std::cout << "RiftForged Compression Library Demo" << std::endl;
//    std::cout << "===================================" << std::endl;
//
//    // --- Simulate a packet between 2500 and 5000 bytes ---
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_int_distribution<> size_distrib(2500, 5000);
//    const size_t packet_size = size_distrib(gen);
//
//    // Use the new function to generate compressible data
//    byte_vec original_packet = generate_compressible_packet(packet_size);
//    std::cout << "Generated a compressible packet to simulate game state data." << std::endl;
//
//    try {
//        // --- Test LZ4 ---
//        auto lz4_algo = std::make_unique<LZ4Algorithm>();
//        Compressor lz4_compressor(std::move(lz4_algo));
//        run_compression_test(lz4_compressor, original_packet, "LZ4 (High Speed)");
//
//        // --- Test Zstd ---
//        auto zstd_algo = std::make_unique<ZstdAlgorithm>();
//        Compressor zstd_compressor(std::move(zstd_algo));
//        run_compression_test(zstd_compressor, original_packet, "Zstd (High Ratio)");
//
//    }
//    catch (const std::exception& e) {
//        std::cerr << "\nAn error occurred: " << e.what() << std::endl;
//        return 1;
//    }
//
//    std::cout << "\nAll compression tests passed successfully!" << std::endl;
//
//    return 0;
//}