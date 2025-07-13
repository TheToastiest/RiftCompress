// =====================================================================================
// riftcompress.hpp - Header File
// =====================================================================================
#ifndef RIFTFORGED_COMPRESS_HPP
#define RIFTFORGED_COMPRESS_HPP

#include <vector>
#include <memory>

// A type alias for byte vectors for clarity.
using byte_vec = std::vector<unsigned char>;

/**
 * @class CompressionAlgorithm
 * @brief An abstract base class (interface) for compression algorithms.
 *
 * This interface defines the contract for all compression algorithms within
 * the library, ensuring modularity and consistency.
 */
class CompressionAlgorithm {
public:
    virtual ~CompressionAlgorithm() = default;

    /**
     * @brief Compresses a block of data.
     * @param data The raw data to compress.
     * @return A byte vector containing the compressed data. The format may
     * include metadata (like original size) needed for decompression.
     * Returns an empty vector on failure.
     */
    virtual byte_vec compress(const byte_vec& data) = 0;

    /**
     * @brief Decompresses a block of data.
     * @param compressed_data The data to decompress.
     * @return The original, decompressed data. Returns an empty vector on failure.
     */
    virtual byte_vec decompress(const byte_vec& compressed_data) = 0;
};

/**
 * @class LZ4Algorithm
 * @brief Implements the CompressionAlgorithm interface using the LZ4 library.
 * Optimized for extremely high speed.
 */
class LZ4Algorithm : public CompressionAlgorithm {
public:
    byte_vec compress(const byte_vec& data) override;
    byte_vec decompress(const byte_vec& compressed_data) override;
};

/**
 * @class ZstdAlgorithm
 * @brief Implements the CompressionAlgorithm interface using the Zstandard library.
 * Optimized for high compression ratios.
 */
class ZstdAlgorithm : public CompressionAlgorithm {
public:
    byte_vec compress(const byte_vec& data) override;
    byte_vec decompress(const byte_vec& compressed_data) override;
};


/**
 * @class Compressor
 * @brief The main interface for performing compression and decompression.
 *
 * This class provides a simple API that abstracts away the underlying
 * compression algorithm being used.
 */
class Compressor {
private:
    std::unique_ptr<CompressionAlgorithm> algorithm;

public:
    explicit Compressor(std::unique_ptr<CompressionAlgorithm> algo);

    /**
     * @brief Compresses data using the configured algorithm.
     * @param data The data to compress.
     * @return The compressed data.
     */
    byte_vec compress(const byte_vec& data);

    /**
     * @brief Decompresses data using the configured algorithm.
     * @param compressed_data The data to decompress.
     * @return The original decompressed data.
     */
    byte_vec decompress(const byte_vec& compressed_data);
};

#endif // RIFTFORGED_COMPRESS_HPP