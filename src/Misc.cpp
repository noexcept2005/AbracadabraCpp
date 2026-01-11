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

#include <algorithm>
#include <cstdint>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

namespace moyue::misc {

namespace {
constexpr char kNullStr[] = "孎"; //默认忽略的占位字符，一个生僻字。

std::mt19937& RandomEngine() {
  static std::mt19937 engine{std::random_device{}()};
  return engine;
}

std::string Base64Encode(const std::string& input) {
  // TODO: Replace with a robust Base64 implementation if needed.
  return input;
}

std::string Base64Decode(const std::string& input) {
  // TODO: Replace with a robust Base64 implementation if needed.
  return input;
}
}  // namespace

class PreCheckResult {
 public:
  PreCheckResult(std::vector<std::uint8_t> output, bool isEncrypted = false)
      : output_(std::move(output)), isEncrypted_(isEncrypted) {}

  const std::vector<std::uint8_t>& output() const { return output_; }
  bool isEncrypted() const { return isEncrypted_; }

 private:
  std::vector<std::uint8_t> output_;
  bool isEncrypted_;
};

std::string RemovePadding(const std::string& base64String) {
  int paddingCount = 0;
  for (int i = static_cast<int>(base64String.size()) - 1;
       i >= static_cast<int>(base64String.size()) - 4 && i >= 0;
       --i) {
    if (base64String[static_cast<size_t>(i)] == '=') {
      paddingCount++;
    }
  }
  return base64String.substr(0, base64String.size() - paddingCount);
}

std::string AddPadding(const std::string& base64String) {
  if (base64String.size() % 4 == 3) {
    return base64String + "=";
  }
  if (base64String.size() % 4 == 2) {
    return base64String + "==";
  }
  return base64String;
}

std::string SetCharOnIndex(const std::string& input, size_t index,
                           const std::string& value) {
  if (index > input.size() - 1) {
    return input;
  }
  return input.substr(0, index) + value + input.substr(index + 1);
}

std::vector<std::uint8_t> StringToUint8Array(const std::string& input) {
  std::string encoded = Base64Encode(input);
  std::string decoded = Base64Decode(encoded);
  return std::vector<std::uint8_t>(decoded.begin(), decoded.end());
}

std::string Uint8ArrayToString(const std::vector<std::uint8_t>& data) {
  std::string temp(data.begin(), data.end());
  return Base64Decode(Base64Encode(temp));
}

int GetRandomIndex(int length) {
  if (length <= 0) {
    return 0;
  }
  std::uniform_int_distribution<int> dist(0, length - 1);
  return dist(RandomEngine());
}

std::vector<int> Difference(const std::vector<int>& arr1,
                            const std::vector<int>& arr2) {
  std::unordered_set<int> set2(arr2.begin(), arr2.end());
  std::vector<int> result;
  for (int item : arr1) {
    if (!set2.count(item)) {
      result.push_back(item);
    }
  }
  return result;
}

std::string InsertStringAtIndex(const std::string& input,
                                const std::string& value, size_t index) {
  return input.substr(0, index) + value + input.substr(index);
}

int GetLuhnBit(const std::vector<std::uint8_t>& data) {
  std::vector<int> digits;
  for (std::uint8_t num : data) {
    int value = num;
    while (value > 0) {
      digits.push_back(value % 10);
      value /= 10;
    }
  }

  int sum = 0;
  for (size_t i = 0; i < digits.size(); ++i) {
    int digit = digits[i];
    if (i % 2 != 0) {
      digit *= 2;
      if (digit >= 10) {
        digit = (digit % 10) + (digit / 10);
      }
    }
    sum += digit;
  }

  return 10 - (sum % 10);
}

bool CheckLuhnBit(const std::vector<std::uint8_t>& data) {
  if (data.empty()) {
    return false;
  }
  int check = GetLuhnBit(std::vector<std::uint8_t>(data.begin(), data.end() - 1));
  return check == data.back();
}

std::vector<int> Shuffle(std::vector<int> array) {
  for (size_t i = array.size(); i > 1; --i) {
    std::uniform_int_distribution<size_t> dist(0, i - 1);
    size_t j = dist(RandomEngine());
    std::swap(array[i - 1], array[j]);
  }
  return array;
}

PreCheckResult PreCheckOld(const std::string& input) {
  const std::string sigDecryptJp = "桜込凪雫実沢";
  const std::string sigDecryptCn = "玚俟玊欤瞐珏";

  std::string working = input;
  bool isJpFound = false;
  bool isCnFound = false;

  for (size_t i = 0; i < working.size(); ++i) {
    char temp = working[i];
    if (sigDecryptJp.find(temp) != std::string::npos) {
      working = SetCharOnIndex(working, i, kNullStr);
      isJpFound = true;
      continue;
    }
    if (sigDecryptCn.find(temp) != std::string::npos) {
      working = SetCharOnIndex(working, i, kNullStr);
      isCnFound = true;
      continue;
    }
  }

  bool isEncrypted = isJpFound && isCnFound;
  return PreCheckResult(StringToUint8Array(working), isEncrypted);
}

}  // namespace moyue::misc
