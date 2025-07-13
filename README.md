# RiftCompress — Real-Time Compression for Secure Networking

> “Bandwidth is a bottleneck. Compression is the blade.”  
> — *RiftForged Development Doctrine*

---

**RiftCompress** is a modular, lightweight C++ compression library designed for real-time systems where performance, clarity, and control matter.

Originally built as a subsystem for [RiftNet](https://github.com/TheToastiest/RiftNet), it can be used independently in any project that needs reliable, low-latency data compression without bloated dependencies.

---

## 📦 Core Features

- 🔁 **LZ4 and Zstd Support** — Fast and flexible, with runtime codec selection
- ⚡ **Zero Runtime Overhead Design** — Avoids unnecessary heap allocations
- 🧩 **Modular Architecture** — Plug in new codecs or disable unused ones
- 📏 **Buffer Size Checks** — Prevent overflow and minimize memory footprint
- 🚀 **Real-Time Ready** — Designed for use in multiplayer engines and simulations

---

## 🛠️ Usage

### 1. Include the headers

Add `RiftCompress/include/` to your project.

### 2. Link compression backends

You’ll need to link against:
- `lz4` for LZ4 support
- `zstd` for Zstandard

Static or dynamic linking is fine — this library does not bundle the codec implementations.

### 3. Use the API

```cpp
#include <RiftCompress/Compressor.hpp>

std::vector<uint8_t> compressed;
std::vector<uint8_t> restored;

Compressor comp(CompressionMode::Zstd);

if (comp.compress(original, compressed)) {
    std::cout << "Compressed size: " << compressed.size() << std::endl;
}

if (comp.decompress(compressed, restored)) {
    std::cout << "Restored size: " << restored.size() << std::endl;
}
