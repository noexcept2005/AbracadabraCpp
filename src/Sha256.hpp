#pragma once
/*
 * Copyright (C) 2025 SheepChef (a.k.a. Haruka Hokuto)
 *
 * 这是一个自由软件。
 * 在遵守AIPL-1.1许可证的前提下，
 * 你可以自由复制，修改，分发，使用它。
 *
 * 查阅 Academic Innovation Protection License(AIPL) 来了解更多 .
 * 本作品应随附一份完整的 AIPL-1.1 许可证全文。
 *
 */

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace moyue::crypto {

namespace detail {
constexpr std::array<std::uint32_t, 64> kSha256K = {
    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu,
    0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u, 0xd807aa98u, 0x12835b01u,
    0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u,
    0xc19bf174u, 0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
    0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau, 0x983e5152u,
    0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u,
    0x06ca6351u, 0x14292967u, 0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu,
    0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
    0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u,
    0xd6990624u, 0xf40e3585u, 0x106aa070u, 0x19a4c116u, 0x1e376c08u,
    0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu,
    0x682e6ff3u, 0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
    0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u};

constexpr std::array<std::uint32_t, 8> kSha256Init = {
    0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
    0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u};

inline std::uint32_t RotR(std::uint32_t x, std::uint32_t n) {
  return (x >> n) | (x << (32 - n));
}

inline std::uint32_t Ch(std::uint32_t x, std::uint32_t y, std::uint32_t z) {
  return (x & y) ^ (~x & z);
}

inline std::uint32_t Maj(std::uint32_t x, std::uint32_t y, std::uint32_t z) {
  return (x & y) ^ (x & z) ^ (y & z);
}

inline std::uint32_t Sigma0(std::uint32_t x) {
  return RotR(x, 2) ^ RotR(x, 13) ^ RotR(x, 22);
}

inline std::uint32_t Sigma1(std::uint32_t x) {
  return RotR(x, 6) ^ RotR(x, 11) ^ RotR(x, 25);
}

inline std::uint32_t sigma0(std::uint32_t x) {
  return RotR(x, 7) ^ RotR(x, 18) ^ (x >> 3);
}

inline std::uint32_t sigma1(std::uint32_t x) {
  return RotR(x, 17) ^ RotR(x, 19) ^ (x >> 10);
}

inline void WriteUint32(std::vector<std::uint8_t>& out, std::uint32_t value) {
  out.push_back(static_cast<std::uint8_t>((value >> 24) & 0xFF));
  out.push_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
  out.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
  out.push_back(static_cast<std::uint8_t>(value & 0xFF));
}
}  // namespace detail

inline std::vector<std::uint8_t> Sha256Bytes(const std::string& input) {
  std::vector<std::uint8_t> data(input.begin(), input.end());
  std::uint64_t bitLen = static_cast<std::uint64_t>(data.size()) * 8;
  data.push_back(0x80);
  while ((data.size() % 64) != 56) {
    data.push_back(0x00);
  }
  for (int i = 7; i >= 0; --i) {
    data.push_back(static_cast<std::uint8_t>((bitLen >> (i * 8)) & 0xFF));
  }

  std::array<std::uint32_t, 8> hash = detail::kSha256Init;

  for (size_t offset = 0; offset < data.size(); offset += 64) {
    std::array<std::uint32_t, 64> w{};
    for (size_t i = 0; i < 16; ++i) {
      size_t idx = offset + i * 4;
      w[i] = (static_cast<std::uint32_t>(data[idx]) << 24) |
             (static_cast<std::uint32_t>(data[idx + 1]) << 16) |
             (static_cast<std::uint32_t>(data[idx + 2]) << 8) |
             static_cast<std::uint32_t>(data[idx + 3]);
    }
    for (size_t i = 16; i < 64; ++i) {
      w[i] = detail::sigma1(w[i - 2]) + w[i - 7] +
             detail::sigma0(w[i - 15]) + w[i - 16];
    }

    std::uint32_t a = hash[0];
    std::uint32_t b = hash[1];
    std::uint32_t c = hash[2];
    std::uint32_t d = hash[3];
    std::uint32_t e = hash[4];
    std::uint32_t f = hash[5];
    std::uint32_t g = hash[6];
    std::uint32_t h = hash[7];

    for (size_t i = 0; i < 64; ++i) {
      std::uint32_t t1 = h + detail::Sigma1(e) + detail::Ch(e, f, g) +
                         detail::kSha256K[i] + w[i];
      std::uint32_t t2 = detail::Sigma0(a) + detail::Maj(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + t1;
      d = c;
      c = b;
      b = a;
      a = t1 + t2;
    }

    hash[0] += a;
    hash[1] += b;
    hash[2] += c;
    hash[3] += d;
    hash[4] += e;
    hash[5] += f;
    hash[6] += g;
    hash[7] += h;
  }

  std::vector<std::uint8_t> output;
  output.reserve(32);
  for (auto value : hash) {
    detail::WriteUint32(output, value);
  }
  return output;
}

}  // namespace moyue::crypto
