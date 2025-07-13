// =====================================================================================
// riftcompress.cpp - Source File
// =====================================================================================
#include "../include/RiftCompress.hpp"
#include <stdexcept>
#include <lz4.h>
#include <zstd.h>
#include <cstring> // For memcpy

// --- LZ4Algorithm Implementation ---

byte_vec LZ4Algorithm::compress(const byte_vec& data) {
    const int max_dst_size = LZ4_compressBound(static_cast<int>(data.size()));
    if (max_dst_size <= 0) {
        throw std::runtime_error("LZ4: Invalid input size.");
    }

    const size_t original_size = data.size();
    byte_vec compressed_data(sizeof(original_size) + max_dst_size);

    std::memcpy(compressed_data.data(), &original_size, sizeof(original_size));

    const int compressed_size = LZ4_compress_default(
        reinterpret_cast<const char*>(data.data()),
        reinterpret_cast<char*>(compressed_data.data() + sizeof(original_size)),
        static_cast<int>(data.size()),
        max_dst_size
    );

    if (compressed_size <= 0) {
        throw std::runtime_error("LZ4 compression failed.");
    }

    compressed_data.resize(sizeof(original_size) + compressed_size);
    return compressed_data;
}

byte_vec LZ4Algorithm::decompress(const byte_vec& compressed_data) {
    if (compressed_data.size() < sizeof(uint64_t)) {
        throw std::runtime_error("LZ4 decompress failed: insufficient data for header.");
    }

    uint64_t original_size;
    std::memcpy(&original_size, compressed_data.data(), sizeof(original_size));

    byte_vec decompressed_data(original_size);
    const int decompressed_size = LZ4_decompress_safe(
        reinterpret_cast<const char*>(compressed_data.data() + sizeof(original_size)),
        reinterpret_cast<char*>(decompressed_data.data()),
        static_cast<int>(compressed_data.size() - sizeof(original_size)),
        static_cast<int>(original_size)
    );

    if (decompressed_size < 0) {
        throw std::runtime_error("LZ4 decompress failed: decompression error.");
    }

    return decompressed_data;
}

// --- ZstdAlgorithm Implementation ---

byte_vec ZstdAlgorithm::compress(const byte_vec& data) {
    const size_t max_dst_size = ZSTD_compressBound(data.size());
    const size_t original_size = data.size();

    byte_vec compressed_data(sizeof(original_size) + max_dst_size);
    std::memcpy(compressed_data.data(), &original_size, sizeof(original_size));

    const size_t compressed_size = ZSTD_compress(
        compressed_data.data() + sizeof(original_size),
        max_dst_size,
        data.data(),
        data.size(),
        1
    );

    if (ZSTD_isError(compressed_size)) {
        throw std::runtime_error("Zstd compression failed: " + std::string(ZSTD_getErrorName(compressed_size)));
    }

    compressed_data.resize(sizeof(original_size) + compressed_size);
    return compressed_data;
}

byte_vec ZstdAlgorithm::decompress(const byte_vec& compressed_data) {
    if (compressed_data.size() < sizeof(uint64_t)) {
        throw std::runtime_error("Zstd: Invalid data - missing size prefix.");
    }

    uint64_t original_size;
    std::memcpy(&original_size, compressed_data.data(), sizeof(original_size));

    byte_vec decompressed_data(original_size);
    const size_t decompressed_size = ZSTD_decompress(
        decompressed_data.data(),
        original_size,
        compressed_data.data() + sizeof(original_size),
        compressed_data.size() - sizeof(original_size)
    );

    if (ZSTD_isError(decompressed_size)) {
        throw std::runtime_error("Zstd decompression failed: " + std::string(ZSTD_getErrorName(decompressed_size)));
    }

    if (decompressed_size != original_size) {
        throw std::runtime_error("Zstd decompression size mismatch.");
    }

    return decompressed_data;
}

// --- Compressor Wrapper Implementation ---

Compressor::Compressor(std::unique_ptr<CompressionAlgorithm> algo)
    : algorithm(std::move(algo)) {
}

byte_vec Compressor::compress(const byte_vec& data) {
    if (!algorithm) {
        throw std::runtime_error("Compressor: No algorithm configured.");
    }
    return algorithm->compress(data);
}

byte_vec Compressor::decompress(const byte_vec& compressed_data) {
    if (!algorithm) {
        throw std::runtime_error("Compressor: No algorithm configured.");
    }
    return algorithm->decompress(compressed_data);
}
