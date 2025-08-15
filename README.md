## RiftCompress
RiftCompress is a high-performance, modern C++ compression library designed for real-time applications where speed and efficiency are critical. It provides a clean, unified C++ wrapper for the industry-leading LZ4 and Zstandard (Zstd) compression algorithms.

## Features
Modern C++ Interface: A simple, object-oriented API using std::vector for byte streams and smart pointers for resource management (std::unique_ptr).

Two Powerful Algorithms:

## LZ4: Included for its extremely fast compression and decompression speeds, making it ideal for latency-sensitive tasks.

## Zstandard (Zstd): Included for its excellent compression ratios, perfect for reducing bandwidth usage when CPU is less of a concern.

Type-Safe & Modular: The design uses a virtual interface (CompressionAlgorithm) to decouple the core logic from the specific algorithm, making the system clean and extensible.

Minimal Dependencies: Only requires a C++17 compiler and the respective compression libraries (lz4, zstd).

High Performance: Benchmarked to showcase the impressive throughput of both integrated algorithms.

## Requirements
C++17 (or newer) compiler

LZ4 Library

Zstandard Library

For Windows users, using a package manager like vcpkg is the recommended way to install the required libraries:

vcpkg install lz4 zstd

## Quick Start: API Usage
Using RiftCompress is simple. Create a Compressor instance by choosing an algorithm, then use it to compress and decompress your data.
```
#include "riftcompress.hpp"
#include <iostream>
#include <string>

int main() {
    try {
        // 1. Create a Compressor with the desired algorithm (e.g., LZ4 for speed)
        Compressor compressor(std::make_unique<LZ4Algorithm>());

        // 2. Prepare data
        std::string original_string = "Hello Hello Hello RiftNet! Repetition is key for compression.";
        byte_vec original_data(original_string.begin(), original_string.end());

        // 3. Compress the data
        byte_vec compressed_data = compressor.compress(original_data);
        if (compressed_data.empty()) {
            std::cerr << "Compression failed!" << std::endl;
            return 1;
        }
        std::cout << "Original size: " << original_data.size() << " bytes" << std::endl;
        std::cout << "Compressed size: " << compressed_data.size() << " bytes" << std::endl;

        // 4. Decompress the data
        byte_vec decompressed_data = compressor.decompress(compressed_data);
        if (decompressed_data.empty()) {
            std::cerr << "Decompression failed!" << std::endl;
            return 1;
        }

        // 5. Verify correctness
        if (original_data == decompressed_data) {
            std::cout << "Success! Decompressed data matches the original." << std::endl;
        } else {
            std::cout << "Verification failed!" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```
## Performance Benchmarks
The library has been benchmarked for end-to-end compress/decompress cycles. The results below were captured on an x64 Release build and highlight the strengths of each algorithm.

## LZ4 (Optimized for Speed)

512 B Payload: 1.89x ratio | 2129 MB/s compress | 7735 MB/s decompress

1400 B Payload: 5.09x ratio | 4463 MB/s compress | 12398 MB/s decompress

16 KB Payload: 49.05x ratio | 14149 MB/s compress | 24746 MB/s decompress

128 KB Payload: 167.40x ratio | 21189 MB/s compress | 32155 MB/s decompress

## Zstd (Optimized for Ratio)

512 B Payload: 1.83x ratio | 112 MB/s compress | 1836 MB/s decompress

1400 B Payload: 5.00x ratio | 438 MB/s compress | 5617 MB/s decompress

16 KB Payload: 58.51x ratio | 1945 MB/s compress | 18543 MB/s decompress

128 KB Payload: 463.15x ratio | 10851 MB/s compress | 23864 MB/s decompress

## License
This project is licensed under the MIT License. See the LICENSE file for details.
