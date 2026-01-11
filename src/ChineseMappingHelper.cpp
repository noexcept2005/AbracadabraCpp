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
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Misc.cpp"
#include "RoundObfusHelper.cpp"

namespace moyue::mapping {

class WenyanSimulator {
 public:
  explicit WenyanSimulator(const std::string& key)
      : map_(
            "{\"Actual\":{\"N\":{\"alphabet\":{\"a\":\"人\",\"b\""
            "\":\"镜\"}}},\"Virtual\":{},\"Sentences\":{}}"),
        roundObfusHelper_(key) {
    // JSON 字符串保留原始内容，请在后续阶段引入 JSON 解析以完整还原映射表。
    // CompatibilityDecodeTable = { q: ["褔"] };
    InitDecodeTable();
  }

  bool RoundKeyMatch(const std::string& keyIn) {
    return !roundObfusHelper_.RoundKeyMatch(keyIn).empty();
  }

  std::string DRoundKeyMatch(const std::string& keyIn) {
    return roundObfusHelper_.DRoundKeyMatch(keyIn);
  }

  void RoundKey() { roundObfusHelper_.RoundKey(); }

  std::string GetCryptText(const std::string& text, const std::string& type) {
    //查表函数
    (void)type;
    if (text.empty()) {
      return nullStr_;
    }
    // TODO: 完整实现 Map_Obj 映射逻辑。
    return nullStr_;
  }

  std::string FindOriginText(const std::string& text) {
    //反向查表函数
    (void)text;
    // TODO: 根据 DecodeTable 反查原文。
    return nullStr_;
  }

  void InitDecodeTable() {
    // TODO: 解析 Map_Obj JSON 并初始化 DecodeTable。
  }

  std::vector<int> AvoidAdjacentDuplicates(const std::vector<int>& arr) {
    if (arr.size() <= 1) {
      return arr;
    }
    std::vector<int> newArr = arr;
    bool hasAdjacent = true;
    int maxTries = static_cast<int>(newArr.size());

    while (hasAdjacent && maxTries > 0) {
      hasAdjacent = false;
      for (size_t i = 0; i + 1 < newArr.size(); ++i) {
        if (newArr[i] == newArr[i + 1]) {
          hasAdjacent = true;
          // 尝试在i+2位置之后找一个可以交换的元素
          for (size_t j = i + 2; j < newArr.size(); ++j) {
            if (newArr[j] != newArr[i] &&
                (j == 0 || newArr[j - 1] != newArr[i]) &&
                (j == newArr.size() - 1 || newArr[j + 1] != newArr[i])) {
              std::swap(newArr[i + 1], newArr[j]);
              break;
            }
          }
          break;
        }
      }
      maxTries--;
    }
    return newArr;
  }

  std::vector<int> MergeNumbers(const std::vector<int>& arr, double factor) {
    // 分离小于3的数字和其他数字
    std::vector<int> lessThan3;
    std::vector<int> rest;
    for (int num : arr) {
      if (num < 3) {
        lessThan3.push_back(num);
      } else {
        rest.push_back(num);
      }
    }

    // 根据因子计算需要保留的小于3的数字数量
    int preserveCount = std::max(0, static_cast<int>((1 - factor) *
                                                     static_cast<double>(
                                                         lessThan3.size())));

    // 保留部分数字
    std::vector<int> preserved;
    std::vector<int> toMerge;
    std::vector<int> temp = lessThan3;

    for (int i = 0; i < preserveCount; i++) {
      int randomIndex = moyue::misc::GetRandomIndex(static_cast<int>(temp.size()));
      preserved.push_back(temp.at(static_cast<size_t>(randomIndex)));
      temp.erase(temp.begin() + randomIndex);
    }
    toMerge.insert(toMerge.end(), temp.begin(), temp.end());

    // 如果没有需要合并的数字，直接返回
    if (toMerge.empty()) {
      auto shuffledRest = moyue::misc::Shuffle(rest);
      auto shuffledPreserved = moyue::misc::Shuffle(preserved);
      shuffledRest.insert(shuffledRest.end(), shuffledPreserved.begin(),
                          shuffledPreserved.end());
      return shuffledRest;
    }

    int sum = 0;
    for (int value : toMerge) {
      sum += value;
    }

    int minSegments = static_cast<int>(std::ceil(sum / 9.0));
    int maxSegments =
        std::min(static_cast<int>(toMerge.size()), static_cast<int>(sum / 3));
    int bestSegmentCount = minSegments;

    double minVariance = std::numeric_limits<double>::infinity();
    for (int k = minSegments; k <= maxSegments; k++) {
      int base = sum / k;
      int remainder = sum % k;
      std::vector<int> values;
      for (int i = 0; i < k; i++) {
        values.push_back(i < remainder ? base + 1 : base);
      }
      double mean = static_cast<double>(sum) / k;
      double variance = 0.0;
      for (int value : values) {
        variance += std::pow(value - mean, 2);
      }
      variance /= k;
      if (variance < minVariance) {
        minVariance = variance;
        bestSegmentCount = k;
      }
    }

    int base = sum / bestSegmentCount;
    int remainder = sum % bestSegmentCount;
    std::vector<int> merged;
    for (int i = 0; i < bestSegmentCount; i++) {
      merged.push_back(i < remainder ? base + 1 : base);
    }

    auto shuffledRest = moyue::misc::Shuffle(rest);
    auto shuffledPreserved = moyue::misc::Shuffle(preserved);
    auto shuffledMerged = moyue::misc::Shuffle(merged);

    shuffledRest.insert(shuffledRest.end(), shuffledPreserved.begin(),
                        shuffledPreserved.end());
    shuffledRest.insert(shuffledRest.end(), shuffledMerged.begin(),
                        shuffledMerged.end());
    return shuffledRest;
  }

  std::vector<std::vector<int>> ProcessArray(
      std::vector<std::vector<int>> twoDArray, double factor) {
    for (auto& subArray : twoDArray) {
      // 检查是否需要合并
      if (subArray.size() > 6) {
        int countLessThan3 =
            static_cast<int>(std::count_if(subArray.begin(), subArray.end(),
                                           [](int num) { return num < 3; }));
        if (static_cast<double>(countLessThan3) / subArray.size() > 0.35) {
          subArray = MergeNumbers(subArray, factor);
        }
      }
      subArray = moyue::misc::Shuffle(subArray);
      subArray = AvoidAdjacentDuplicates(subArray);
    }
    return twoDArray;
  }

  std::vector<int> DistributeInteger(int num) {
    //把文言文密文的载荷根据一定比例分成三份(一段)
    if (num <= 3) {
      // 如果 num 小于等于 3，则无法满足每个元素都大于 0 的要求
      return {}; // 返回空数组，表示无法分配
    }

    int maxPart = static_cast<int>(num * 0.2); // 计算每个部分的最大值
    int remaining = num - 2 * maxPart; // 计算剩余部分

    return {maxPart, remaining, maxPart};
  }

  std::vector<int> DistributePayload(int n) {
    // 如果载荷量太大，那么自动把载荷分为等大小的部分(段落)，然后分别处理
    if (n == 0) return {0};
    int k = static_cast<int>(std::ceil(static_cast<double>(n) / 100.0));
    int base = n / k;
    int remainder = n % k;
    std::vector<int> parts;
    for (int i = 0; i < remainder; i++) {
      parts.push_back(base + 1);
    }
    for (int i = remainder; i < k; i++) {
      parts.push_back(base);
    }
    return parts;
  }

  std::vector<std::vector<std::string>> SelectSentence(int payloadLength,
                                                       int randomIndex,
                                                       bool p, bool l) {
    //句式选择函数。
    //P 强制对仗骈文
    //L 强制多用逻辑句式
    //句式选择算法
    //RandomIndex 随机指数，越大，给出的句式越随机，最大100。
    /* v8 ignore next 7 */
    if (randomIndex > 100 || randomIndex < 0) {
      //错误的输入。
      throw std::runtime_error("Incorrect Random Index");
    }
    if (p && l) {
      throw std::runtime_error("Contradictory Mode Setting");
    }

    if (payloadLength > 100) {
      //如果密文太长了，那么自动分段
      std::vector<int> distributedPayload = DistributePayload(payloadLength);
      std::vector<std::vector<std::string>> result;
      for (int payload : distributedPayload) {
        auto partial = SelectSentence(payload, randomIndex, p, l);
        result.insert(result.end(), partial.begin(), partial.end());
        if (!result.empty()) {
          result.back().push_back("Z");
        }
      }
      return result;
    }

    // TODO: 句式选择依赖 Map_Obj，后续引入 JSON 解析后补齐。
    return {};
  }

  std::string EnMap(const std::string& originStr, bool q, int r, bool p, bool l,
                    bool t) {
    (void)q;
    (void)r;
    (void)p;
    (void)l;
    (void)t;
    // TODO: 完整实现 enMap 映射逻辑。
    return originStr;
  }

  std::string DeMap(const std::string& originStr) {
    // TODO: 完整实现 deMap 逆映射逻辑。
    return originStr;
  }

 private:
  std::string map_;
  std::string nullStr_ = "孎"; //默认忽略的占位字符，一个生僻字。
  moyue::round::RoundObfus roundObfusHelper_;
};

class OldMapper {
 public:
  explicit OldMapper(const std::string& key)
      : map_(
            "{\"basic\":{\"alphabet\":{\"a\":[\"请\"]}},"
            "\"special\":{\"DECRYPT\":{\"JP\":[\"桜\"],"
            "\"CN\":[\"玚\"]}}}"),
        roundObfusHelper_(key) {}

  std::string RoundKeyMatch(const std::string& keyIn) {
    return roundObfusHelper_.RoundKeyMatch(keyIn);
  }

  std::string DRoundKeyMatch(const std::string& keyIn) {
    return roundObfusHelper_.DRoundKeyMatch(keyIn);
  }

  void RoundKey() { roundObfusHelper_.RoundKey(); }

  std::string GetCryptText(const std::string& text) {
    (void)text;
    return nullStr_;
  }

  std::string FindOriginText(const std::string& text) {
    (void)text;
    return nullStr_;
  }

  std::string EnMap(const std::string& originStr, bool q) {
    (void)q;
    // TODO: 实现 OldMapper 的映射逻辑。
    return originStr;
  }

  std::string DeMap(const std::string& originStr) {
    (void)originStr;
    // TODO: 实现 OldMapper 的逆映射逻辑。
    return originStr;
  }

 private:
  std::string map_;
  std::string nullStr_ = "孎"; //默认忽略的占位字符，一个生僻字。
  moyue::round::RoundObfusOld roundObfusHelper_;
};

}  // namespace moyue::mapping
