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
#include <random>
#include <string>
#include <vector>

#include "Aes.hpp"
#include "Misc.cpp"
#include "Sha256.hpp"

namespace moyue::encrypt {

namespace {
std::mt19937& RandomEngine() {
  static std::mt19937 engine{std::random_device{}()};
  return engine;
}

std::vector<std::uint8_t> Aes256CtrEncrypt(
    const std::vector<std::uint8_t>& data, const std::string& key,
    const std::array<std::uint8_t, 2>& randomBytes) {
  auto keyHash = moyue::crypto::Sha256Bytes(key);
  std::array<std::uint8_t, 32> keyBytes{};
  std::copy_n(keyHash.begin(), keyBytes.size(), keyBytes.begin());

  std::vector<std::uint8_t> hashWithRandom = keyHash;
  hashWithRandom.push_back(randomBytes[0]);
  hashWithRandom.push_back(randomBytes[1]);
  auto hashHash = moyue::crypto::Sha256Bytes(hashWithRandom);

  std::array<std::uint8_t, 16> iv{};
  std::copy_n(hashHash.begin(), iv.size(), iv.begin());

  return moyue::crypto::Aes256CtrEncrypt(data, keyBytes, iv);
}
}  // namespace

//执行AES加密，返回UINT8数组
std::vector<std::uint8_t> Encrypt(const std::vector<std::uint8_t>& originalData,
                                  const std::string& key) {
  std::uniform_int_distribution<int> dist(0, 255);
  std::array<std::uint8_t, 2> randomBytes{
      static_cast<std::uint8_t>(dist(RandomEngine())),
      static_cast<std::uint8_t>(dist(RandomEngine()))};

  std::vector<std::uint8_t> encrypted =
      Aes256CtrEncrypt(originalData, key, randomBytes); //AES加密

  std::vector<std::uint8_t> output;
  output.reserve(encrypted.size() + 2);
  output.insert(output.end(), encrypted.begin(), encrypted.end());
  output.push_back(randomBytes[0]);
  output.push_back(randomBytes[1]);
  return output;
}

std::vector<std::uint8_t> Decrypt(const std::vector<std::uint8_t>& data,
                                  const std::string& key) {
  if (data.size() < 2) {
    return {};
  }
  std::array<std::uint8_t, 2> randomBytes{
      data[data.size() - 2], data[data.size() - 1]};

  std::vector<std::uint8_t> trimmed(data.begin(), data.end() - 2);
  return Aes256CtrEncrypt(trimmed, key, randomBytes);
}

}  // namespace moyue::encrypt
