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
#include <functional>
#include <string>
#include <vector>

#include "Misc.cpp"

namespace moyue::round {

namespace {
std::vector<std::uint8_t> HashKeyToBytes(const std::string& key) {
  // TODO: Replace with SHA256 hashing of the key (CryptoJS.SHA256 equivalent).
  std::vector<std::uint8_t> bytes(32, 0);
  for (size_t i = 0; i < key.size(); ++i) {
    bytes[i % bytes.size()] ^= static_cast<std::uint8_t>(key[i]);
  }
  return bytes;
}

std::string RotateString(const std::string& input, size_t count) {
  if (input.empty()) {
    return input;
  }
  size_t shift = count % input.size();
  return input.substr(shift) + input.substr(0, shift);
}

std::string RotateStringLeft(const std::string& input, size_t count) {
  if (input.empty()) {
    return input;
  }
  size_t shift = count % input.size();
  return input.substr(input.size() - shift) + input.substr(0, input.size() - shift);
}
}

using RoundCallback = std::function<void(const std::string&)>;

class RoundObfus {
 public:
  explicit RoundObfus(const std::string& key,
                      const RoundCallback& callback = nullptr)
      : roundFlip_(0),
        lettersRound1_("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"),
        lettersRound2_(
            "FbPoDRStyJKAUcdahfVXlqwnOGpHZejzvmrBCigQILxkYMuWTEsN"),
        lettersRound3_("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"),
        numberSymbolRound1_("1234567890+/="),
        numberSymbolRound2_("5=0764+389/12"),
        numberSymbolRound3_("1234567890+/="),
        normalCharacters_(
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+/=1234567890"),
        letters_("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"),
        bigLetters_("ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
        numbers_("1234567890"),
        symbols_("+/="),
        numberSymbol_("0123456789+/="),
        nullStr_("孎"),
        callback_(callback) {
    //初始化转轮操作的数组
    auto hashArray = HashKeyToBytes(key);
    roundControl_.assign(hashArray.begin(), hashArray.end());
  }

  std::string RoundKeyMatch(const std::string& keyIn) {
    //查询轮换密钥的键值
    int idx1 = static_cast<int>(letters_.find(keyIn));
    int idx2 = static_cast<int>(numberSymbol_.find(keyIn));

    if (idx1 != -1) {
      int idx1_1 = static_cast<int>(letters_.find(lettersRound1_.at(idx1)));
      int idx1_2 = static_cast<int>(letters_.find(lettersRound2_.at(idx1_1)));
      return std::string(1, lettersRound3_.at(idx1_2));
    }
    if (idx2 != -1) {
      int idx2_1 = static_cast<int>(numberSymbol_.find(numberSymbolRound1_.at(idx2)));
      int idx2_2 = static_cast<int>(numberSymbol_.find(numberSymbolRound2_.at(idx2_1)));
      return std::string(1, numberSymbolRound3_.at(idx2_2));
    }
    return nullStr_;
  }

  std::string DRoundKeyMatch(const std::string& keyIn) {
    //查询轮换密钥的键值
    int idx1 = static_cast<int>(lettersRound3_.find(keyIn));
    int idx2 = static_cast<int>(numberSymbolRound3_.find(keyIn));

    if (idx1 != -1) {
      int idx1_1 = static_cast<int>(lettersRound2_.find(letters_.at(idx1)));
      int idx1_2 = static_cast<int>(lettersRound1_.find(letters_.at(idx1_1)));
      return std::string(1, letters_.at(idx1_2));
    }
    if (idx2 != -1) {
      int idx2_1 = static_cast<int>(numberSymbolRound2_.find(numberSymbol_.at(idx2)));
      int idx2_2 = static_cast<int>(numberSymbolRound1_.find(numberSymbol_.at(idx2_1)));
      return std::string(1, numberSymbol_.at(idx2_2));
    }
    return nullStr_;
  }

  void RoundKey() {
    int controlNum = 0;
    if (roundFlip_ == 32) {
      roundFlip_ = 0;
    }
    controlNum = roundControl_[roundFlip_] % 10; //哈希字节对十取余即操作数
    if (controlNum == 0) {
      //等于零就赋值为10
      controlNum = 10;
    }

    if (controlNum % 2 == 0) {
      //操作数是偶数
      lettersRound1_ = RotateString(lettersRound1_, 6); //将第一个密钥轮向右轮6位
      numberSymbolRound1_ = RotateString(numberSymbolRound1_, 6);

      lettersRound2_ = RotateStringLeft(lettersRound2_, controlNum);
      numberSymbolRound2_ = RotateStringLeft(numberSymbolRound2_, controlNum);

      lettersRound3_ = RotateString(lettersRound3_, controlNum / 2 + 1);
      numberSymbolRound3_ = RotateString(numberSymbolRound3_, controlNum / 2 + 1);
    } else {
      //操作数是奇数
      lettersRound1_ = RotateStringLeft(lettersRound1_, 3);
      numberSymbolRound1_ = RotateStringLeft(numberSymbolRound1_, 3);

      lettersRound2_ = RotateString(lettersRound2_, controlNum);
      numberSymbolRound2_ = RotateString(numberSymbolRound2_, controlNum);

      lettersRound3_ = RotateStringLeft(lettersRound3_, (controlNum + 7) / 2);
      numberSymbolRound3_ = RotateStringLeft(numberSymbolRound3_, (controlNum + 7) / 2);
    }
    roundFlip_++;

    if (callback_) {
      callback_(lettersRound1_ + lettersRound2_ + lettersRound3_ +
                numberSymbolRound1_ + numberSymbolRound2_ + numberSymbolRound3_);
    }
  }

 private:
  int roundFlip_;
  std::vector<std::uint8_t> roundControl_;
  std::string lettersRound1_;
  std::string lettersRound2_;
  std::string lettersRound3_;
  std::string numberSymbolRound1_;
  std::string numberSymbolRound2_;
  std::string numberSymbolRound3_;
  std::string normalCharacters_;
  std::string letters_;
  std::string bigLetters_;
  std::string numbers_;
  std::string symbols_;
  std::string numberSymbol_;
  std::string nullStr_;
  RoundCallback callback_;
};

class RoundObfusOld {
 public:
  explicit RoundObfusOld(const std::string& key)
      : roundFlip_(0),
        lettersRound1_("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"),
        lettersRound2_(
            "FbPoDRStyJKAUcdahfVXlqwnOGpHZejzvmrBCigQILxkYMuWTEsN"),
        lettersRound3_("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"),
        numberSymbolRound1_("1234567890+=_-/?.>,<|`~!@#$%^&*(){}[];:"),
        numberSymbolRound2_(
            "~3{8}_-$[6(2^&#5|1*%0,<9:`+@7/?.>4=];!)"),
        numberSymbolRound3_("1234567890+=_-/?.>,<|`~!@#$%^&*(){}[];:"),
        normalCharacters_(
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+=_-/?.>,<|`~!@#$%^&*(){}[];:1234567890"),
        letters_("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"),
        bigLetters_("ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
        numbers_("1234567890"),
        symbols_("+=_-/?.>,<|`~!@#$%^&*(){}[];:"),
        numberSymbol_("1234567890+=_-/?.>,<|`~!@#$%^&*(){}[];:"),
        nullStr_("孎") {
    auto hashArray = HashKeyToBytes(key);
    roundControl_.assign(hashArray.begin(), hashArray.end());
  }

  std::string RoundKeyMatch(const std::string& keyIn) {
    int idx1 = static_cast<int>(letters_.find(keyIn));
    int idx2 = static_cast<int>(numberSymbol_.find(keyIn));

    if (idx1 != -1) {
      int idx1_1 = static_cast<int>(letters_.find(lettersRound1_.at(idx1)));
      int idx1_2 = static_cast<int>(letters_.find(lettersRound2_.at(idx1_1)));
      return std::string(1, lettersRound3_.at(idx1_2));
    }
    if (idx2 != -1) {
      int idx2_1 = static_cast<int>(numberSymbol_.find(numberSymbolRound1_.at(idx2)));
      int idx2_2 = static_cast<int>(numberSymbol_.find(numberSymbolRound2_.at(idx2_1)));
      return std::string(1, numberSymbolRound3_.at(idx2_2));
    }
    return nullStr_;
  }

  std::string DRoundKeyMatch(const std::string& keyIn) {
    int idx1 = static_cast<int>(lettersRound3_.find(keyIn));
    int idx2 = static_cast<int>(numberSymbolRound3_.find(keyIn));

    if (idx1 != -1) {
      int idx1_1 = static_cast<int>(lettersRound2_.find(letters_.at(idx1)));
      int idx1_2 = static_cast<int>(lettersRound1_.find(letters_.at(idx1_1)));
      return std::string(1, letters_.at(idx1_2));
    }
    if (idx2 != -1) {
      int idx2_1 = static_cast<int>(numberSymbolRound2_.find(numberSymbol_.at(idx2)));
      int idx2_2 = static_cast<int>(numberSymbolRound1_.find(numberSymbol_.at(idx2_1)));
      return std::string(1, numberSymbol_.at(idx2_2));
    }
    return nullStr_;
  }

  void RoundKey() {
    int controlNum = 0;
    if (roundFlip_ == 32) {
      roundFlip_ = 0;
    }
    controlNum = roundControl_[roundFlip_] % 10; //哈希字节对十取余即操作数
    if (controlNum == 0) {
      //等于零就赋值为10
      controlNum = 10;
    }

    if (controlNum % 2 == 0) {
      //操作数是偶数
      lettersRound1_ = RotateString(lettersRound1_, 6);
      numberSymbolRound1_ = RotateString(numberSymbolRound1_, 6);

      lettersRound2_ = RotateStringLeft(lettersRound2_, controlNum * 2);
      numberSymbolRound2_ = RotateStringLeft(numberSymbolRound2_, controlNum * 2);

      lettersRound3_ = RotateString(lettersRound3_, controlNum / 2 + 1);
      numberSymbolRound3_ = RotateString(numberSymbolRound3_, controlNum / 2 + 1);
    } else {
      //操作数是奇数
      lettersRound1_ = RotateStringLeft(lettersRound1_, 3);
      numberSymbolRound1_ = RotateStringLeft(numberSymbolRound1_, 3);

      lettersRound2_ = RotateString(lettersRound2_, controlNum);
      numberSymbolRound2_ = RotateString(numberSymbolRound2_, controlNum);

      lettersRound3_ = RotateStringLeft(lettersRound3_, (controlNum + 7) / 2);
      numberSymbolRound3_ = RotateStringLeft(numberSymbolRound3_, (controlNum + 7) / 2);
    }
    roundFlip_++;
  }

 private:
  int roundFlip_;
  std::vector<std::uint8_t> roundControl_;
  std::string lettersRound1_;
  std::string lettersRound2_;
  std::string lettersRound3_;
  std::string numberSymbolRound1_;
  std::string numberSymbolRound2_;
  std::string numberSymbolRound3_;
  std::string normalCharacters_;
  std::string letters_;
  std::string bigLetters_;
  std::string numbers_;
  std::string symbols_;
  std::string numberSymbol_;
  std::string nullStr_;
};

}  // namespace moyue::round
