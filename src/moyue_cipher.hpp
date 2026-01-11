#pragma once

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class MoYueCipher {
 public:
  explicit MoYueCipher(std::string key) : key_(std::move(key)) {
    if (key_.empty()) {
      throw std::invalid_argument("key must not be empty");
    }
  }

  std::string Encrypt(const std::string& plaintext) const {
    std::vector<std::uint8_t> bytes(plaintext.begin(), plaintext.end());
    ApplyXor(bytes);
    ApplyIndexShift(bytes, /*decrypt=*/false);
    ApplyReverse(bytes);
    return ToHex(bytes);
  }

  std::string Decrypt(const std::string& ciphertext_hex) const {
    auto bytes = FromHex(ciphertext_hex);
    ApplyReverse(bytes);
    ApplyIndexShift(bytes, /*decrypt=*/true);
    ApplyXor(bytes);
    return std::string(bytes.begin(), bytes.end());
  }

 private:
  std::string key_;

  void ApplyXor(std::vector<std::uint8_t>& bytes) const {
    for (size_t i = 0; i < bytes.size(); ++i) {
      bytes[i] = static_cast<std::uint8_t>(bytes[i] ^ key_[i % key_.size()]);
    }
  }

  void ApplyIndexShift(std::vector<std::uint8_t>& bytes, bool decrypt) const {
    for (size_t i = 0; i < bytes.size(); ++i) {
      std::uint8_t shift = static_cast<std::uint8_t>(i % 256);
      bytes[i] = decrypt ? static_cast<std::uint8_t>(bytes[i] - shift)
                         : static_cast<std::uint8_t>(bytes[i] + shift);
    }
  }

  void ApplyReverse(std::vector<std::uint8_t>& bytes) const {
    std::reverse(bytes.begin(), bytes.end());
  }

  std::string ToHex(const std::vector<std::uint8_t>& bytes) const {
    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (auto byte : bytes) {
      out << std::setw(2) << static_cast<int>(byte);
    }
    return out.str();
  }

  std::vector<std::uint8_t> FromHex(const std::string& hex) const {
    if (hex.size() % 2 != 0) {
      throw std::invalid_argument("ciphertext hex length must be even");
    }
    std::vector<std::uint8_t> bytes;
    bytes.reserve(hex.size() / 2);
    for (size_t i = 0; i < hex.size(); i += 2) {
      unsigned int value = 0;
      std::istringstream in(hex.substr(i, 2));
      in >> std::hex >> value;
      if (in.fail()) {
        throw std::invalid_argument("ciphertext contains non-hex characters");
      }
      bytes.push_back(static_cast<std::uint8_t>(value));
    }
    return bytes;
  }
};
