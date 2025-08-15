// =====================================================================================
// riftcompress.cpp - Source File
// =====================================================================================
#include "../include/riftcompress.hpp"
#include <stdexcept>
#include <cstring> // For memcpy

// Include the compression library headers.
// Make sure these are in your project's include path.
#include <lz4.h>
#include <zstd.h>

// --- Helper structure for prepending original size ---
// This ensures a consistent way to handle data for both algorithms.
#pragma pack(push, 1)
struct CompressedHeader {
    uint32_t original_size;
};
#pragma pack(pop)


// =====================================================================================
// Compressor Implementation
// =====================================================================================
Compressor::Compressor(std::unique_ptr<CompressionAlgorithm> algo) : algorithm(std::move(algo)) {
    if (!algorithm) {
        throw std::invalid_argument("Compressor cannot be initialized with a null algorithm.");
    }
}

byte_vec Compressor::compress(const byte_vec& data) {
    return algorithm->compress(data);
}

byte_vec Compressor::decompress(const byte_vec& compressed_data) {
    return algorithm->decompress(compressed_data);
}


// =====================================================================================
// LZ4Algorithm Implementation
// =====================================================================================
byte_vec LZ4Algorithm::compress(const byte_vec& data) {
    if (data.empty()) {
        return {};
    }

    // Calculate the maximum possible compressed size
    const int max_compressed_size = LZ4_compressBound(static_cast<int>(data.size()));
    if (max_compressed_size <= 0) {
        return {}; // Failed to get bound
    }

    // Allocate buffer for header + compressed data
    byte_vec compressed_output(sizeof(CompressedHeader) + max_compressed_size);

    // Write the header
    CompressedHeader header;
    header.original_size = static_cast<uint32_t>(data.size());
    memcpy(compressed_output.data(), &header, sizeof(CompressedHeader));

    // Perform compression
    const int compressed_size = LZ4_compress_default(
        reinterpret_cast<const char*>(data.data()),
        reinterpret_cast<char*>(compressed_output.data() + sizeof(CompressedHeader)),
        static_cast<int>(data.size()),
        max_compressed_size
    );

    if (compressed_size <= 0) {
        return {}; // Compression failed
    }

    // Resize the vector to the actual size used
    compressed_output.resize(sizeof(CompressedHeader) + compressed_size);

    return compressed_output;
}

byte_vec LZ4Algorithm::decompress(const byte_vec& compressed_data) {
    if (compressed_data.size() < sizeof(CompressedHeader)) {
        return {}; // Data is too small to be valid
    }

    // Read the header to get the original size
    CompressedHeader header;
    memcpy(&header, compressed_data.data(), sizeof(CompressedHeader));

    if (header.original_size == 0) {
        return {}; // Decompressing to zero size
    }

    // Allocate buffer for the decompressed data
    byte_vec decompressed_output(header.original_size);

    // Perform decompression
    const int decompressed_size = LZ4_decompress_safe(
        reinterpret_cast<const char*>(compressed_data.data() + sizeof(CompressedHeader)),
        reinterpret_cast<char*>(decompressed_output.data()),
        static_cast<int>(compressed_data.size() - sizeof(CompressedHeader)),
        static_cast<int>(decompressed_output.size())
    );

    if (decompressed_size < 0) {
        return {}; // Decompression failed
    }

    // Ensure the decompressed size matches what we expected
    if (static_cast<uint32_t>(decompressed_size) != header.original_size) {
        return {}; // Mismatch in expected size
    }

    decompressed_output.resize(decompressed_size);
    return decompressed_output;
}


// =====================================================================================
// ZstdAlgorithm Implementation
// =====================================================================================
byte_vec ZstdAlgorithm::compress(const byte_vec& data) {
    if (data.empty()) {
        return {};
    }

    // Calculate the maximum possible compressed size
    const size_t max_compressed_size = ZSTD_compressBound(data.size());

    // Allocate buffer for header + compressed data
    byte_vec compressed_output(sizeof(CompressedHeader) + max_compressed_size);

    // Write the header
    CompressedHeader header;
    header.original_size = static_cast<uint32_t>(data.size());
    memcpy(compressed_output.data(), &header, sizeof(CompressedHeader));

    // Perform compression
    const size_t compressed_size = ZSTD_compress(
        compressed_output.data() + sizeof(CompressedHeader),
        max_compressed_size,
        data.data(),
        data.size(),
        1 // Compression level (1 is fastest)
    );

    if (ZSTD_isError(compressed_size)) {
        return {}; // Compression failed
    }

    // Resize the vector to the actual size used
    compressed_output.resize(sizeof(CompressedHeader) + compressed_size);

    return compressed_output;
}

byte_vec ZstdAlgorithm::decompress(const byte_vec& compressed_data) {
    if (compressed_data.size() < sizeof(CompressedHeader)) {
        return {}; // Data is too small to be valid
    }

    // Read the header to get the original size
    CompressedHeader header;
    memcpy(&header, compressed_data.data(), sizeof(CompressedHeader));

    if (header.original_size == 0) {
        return {};
    }

    // Allocate buffer for the decompressed data
    byte_vec decompressed_output(header.original_size);

    // Perform decompression
    const size_t decompressed_size = ZSTD_decompress(
        decompressed_output.data(),
        decompressed_output.size(),
        compressed_data.data() + sizeof(CompressedHeader),
        compressed_data.size() - sizeof(CompressedHeader)
    );

    if (ZSTD_isError(decompressed_size)) {
        return {}; // Decompression failed
    }

    // Ensure the decompressed size matches what we expected
    if (decompressed_size != header.original_size) {
        return {}; // Mismatch in expected size
    }

    return decompressed_output;
}
