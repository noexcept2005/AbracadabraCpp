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
#include <fstream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Misc.cpp"
#include "RoundObfusHelper.cpp"

namespace moyue::mapping {

namespace detail {
struct JsonValue;
using JsonPtr = std::shared_ptr<JsonValue>;

struct JsonValue {
  enum class Type { kObject, kArray, kString, kNull };

  Type type = Type::kNull;
  std::unordered_map<std::string, JsonPtr> object;
  std::vector<JsonPtr> array;
  std::string string;
};

class JsonParser {
 public:
  explicit JsonParser(const std::string& input) : input_(input) {}

  JsonValue Parse() {
    SkipWhitespace();
    JsonValue value = ParseValue();
    SkipWhitespace();
    return value;
  }

 private:
  const std::string& input_;
  size_t pos_ = 0;

  void SkipWhitespace() {
    while (pos_ < input_.size() &&
           (input_[pos_] == ' ' || input_[pos_] == '\n' ||
            input_[pos_] == '\t' || input_[pos_] == '\r')) {
      ++pos_;
    }
  }

  char Peek() const {
    if (pos_ >= input_.size()) {
      return '\0';
    }
    return input_[pos_];
  }

  bool Consume(char expected) {
    if (Peek() != expected) {
      return false;
    }
    ++pos_;
    return true;
  }

  JsonValue ParseValue() {
    SkipWhitespace();
    char c = Peek();
    if (c == '{') {
      return ParseObject();
    }
    if (c == '[') {
      return ParseArray();
    }
    if (c == '"') {
      JsonValue value;
      value.type = JsonValue::Type::kString;
      value.string = ParseString();
      return value;
    }
    if (input_.compare(pos_, 4, "null") == 0) {
      pos_ += 4;
      return JsonValue{};
    }
    throw std::runtime_error("Unsupported JSON value");
  }

  JsonValue ParseObject() {
    JsonValue value;
    value.type = JsonValue::Type::kObject;
    Consume('{');
    SkipWhitespace();
    if (Consume('}')) {
      return value;
    }
    while (true) {
      SkipWhitespace();
      std::string key = ParseString();
      SkipWhitespace();
      if (!Consume(':')) {
        throw std::runtime_error("Expected ':' in JSON object");
      }
      JsonValue child = ParseValue();
      value.object.emplace(std::move(key),
                           std::make_shared<JsonValue>(std::move(child)));
      SkipWhitespace();
      if (Consume('}')) {
        break;
      }
      if (!Consume(',')) {
        throw std::runtime_error("Expected ',' in JSON object");
      }
    }
    return value;
  }

  JsonValue ParseArray() {
    JsonValue value;
    value.type = JsonValue::Type::kArray;
    Consume('[');
    SkipWhitespace();
    if (Consume(']')) {
      return value;
    }
    while (true) {
      JsonValue child = ParseValue();
      value.array.emplace_back(std::make_shared<JsonValue>(std::move(child)));
      SkipWhitespace();
      if (Consume(']')) {
        break;
      }
      if (!Consume(',')) {
        throw std::runtime_error("Expected ',' in JSON array");
      }
    }
    return value;
  }

  std::string ParseString() {
    if (!Consume('"')) {
      throw std::runtime_error("Expected string");
    }
    std::string result;
    while (pos_ < input_.size()) {
      char c = input_[pos_++];
      if (c == '"') {
        break;
      }
      if (c == '\\') {
        if (pos_ >= input_.size()) {
          break;
        }
        char next = input_[pos_++];
        switch (next) {
          case '"':
          case '\\':
          case '/':
            result.push_back(next);
            break;
          case 'b':
            result.push_back('\b');
            break;
          case 'f':
            result.push_back('\f');
            break;
          case 'n':
            result.push_back('\n');
            break;
          case 'r':
            result.push_back('\r');
            break;
          case 't':
            result.push_back('\t');
            break;
          default:
            result.push_back(next);
            break;
        }
      } else {
        result.push_back(c);
      }
    }
    return result;
  }
};

std::string LoadFile(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    if (path.find('/') == std::string::npos &&
        path.find('\\') == std::string::npos) {
      std::string fallback = "src/" + path;
      file.open(fallback);
      if (file.is_open()) {
        std::ostringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
      }
    }
    throw std::runtime_error("Unable to open mapping file: " + path);
  }
  std::ostringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

std::string StripComments(const std::string& input) {
  std::string output = input;
  size_t start = output.find("/*");
  size_t end = output.find("*/");
  if (start != std::string::npos && end != std::string::npos && end > start) {
    output.erase(start, end - start + 2);
  }
  return output;
}

struct TokenMap {
  std::unordered_map<char, std::string> alphabet;
  std::unordered_map<char, std::string> numberSymbol;
};

struct WenyanMappingData {
  std::unordered_map<std::string, TokenMap> actualMaps;
  std::vector<std::string> modalVerbs;
  std::unordered_map<std::string, std::vector<std::string>> virtualWords;
  std::vector<std::string> sentencesBegin;
  std::vector<std::string> sentencesEnd;
};

struct OldMappingData {
  std::unordered_map<char, std::vector<std::string>> alphabet;
  std::unordered_map<char, std::vector<std::string>> numberSymbol;
  std::vector<std::string> decryptJp;
  std::vector<std::string> decryptCn;
};

std::vector<std::string> JsonArrayToStrings(const JsonValue& value) {
  std::vector<std::string> items;
  if (value.type != JsonValue::Type::kArray) {
    return items;
  }
  items.reserve(value.array.size());
  for (const auto& item : value.array) {
    if (item && item->type == JsonValue::Type::kString) {
      items.push_back(item->string);
    }
  }
  return items;
}

TokenMap ParseTokenMap(const JsonValue& value) {
  TokenMap map;
  if (value.type != JsonValue::Type::kObject) {
    return map;
  }
  auto alphabetIt = value.object.find("alphabet");
  if (alphabetIt != value.object.end() &&
      alphabetIt->second && alphabetIt->second->type == JsonValue::Type::kObject) {
    for (const auto& [key, val] : alphabetIt->second->object) {
      if (!key.empty() && val && val->type == JsonValue::Type::kString) {
        map.alphabet.emplace(key[0], val->string);
      }
    }
  }
  auto numberIt = value.object.find("numbersymbol");
  if (numberIt != value.object.end() &&
      numberIt->second && numberIt->second->type == JsonValue::Type::kObject) {
    for (const auto& [key, val] : numberIt->second->object) {
      if (!key.empty() && val && val->type == JsonValue::Type::kString) {
        map.numberSymbol.emplace(key[0], val->string);
      }
    }
  }
  return map;
}

WenyanMappingData LoadWenyanMapping(const std::string& path) {
  std::string content = StripComments(LoadFile(path));
  JsonParser parser(content);
  JsonValue root = parser.Parse();
  WenyanMappingData data;

  auto actualIt = root.object.find("Actual");
  if (actualIt != root.object.end() &&
      actualIt->second && actualIt->second->type == JsonValue::Type::kObject) {
    const auto& actual = actualIt->second->object;
    const std::vector<std::string> tokenKeys = {"N", "V", "A", "AD"};
    for (const auto& key : tokenKeys) {
      auto tokenIt = actual.find(key);
      if (tokenIt != actual.end() && tokenIt->second) {
        data.actualMaps.emplace(key, ParseTokenMap(*tokenIt->second));
      }
    }
    auto mvIt = actual.find("MV");
    if (mvIt != actual.end() && mvIt->second) {
      data.modalVerbs = JsonArrayToStrings(*mvIt->second);
    }
  }

  auto virtualIt = root.object.find("Virtual");
  if (virtualIt != root.object.end() &&
      virtualIt->second && virtualIt->second->type == JsonValue::Type::kObject) {
    for (const auto& [key, value] : virtualIt->second->object) {
      if (value) {
        data.virtualWords.emplace(key, JsonArrayToStrings(*value));
      }
    }
  }

  auto sentencesIt = root.object.find("Sentences");
  if (sentencesIt != root.object.end() &&
      sentencesIt->second && sentencesIt->second->type == JsonValue::Type::kObject) {
    auto beginIt = sentencesIt->second->object.find("Begin");
    if (beginIt != sentencesIt->second->object.end() && beginIt->second) {
      data.sentencesBegin = JsonArrayToStrings(*beginIt->second);
    }
    auto endIt = sentencesIt->second->object.find("End");
    if (endIt != sentencesIt->second->object.end() && endIt->second) {
      data.sentencesEnd = JsonArrayToStrings(*endIt->second);
    }
  }

  return data;
}

OldMappingData LoadOldMapping(const std::string& path) {
  std::string content = StripComments(LoadFile(path));
  JsonParser parser(content);
  JsonValue root = parser.Parse();
  OldMappingData data;

  auto basicIt = root.object.find("basic");
  if (basicIt != root.object.end() &&
      basicIt->second && basicIt->second->type == JsonValue::Type::kObject) {
    auto alphabetIt = basicIt->second->object.find("alphabet");
    if (alphabetIt != basicIt->second->object.end() && alphabetIt->second &&
        alphabetIt->second->type == JsonValue::Type::kObject) {
      for (const auto& [key, value] : alphabetIt->second->object) {
        if (!key.empty()) {
          data.alphabet.emplace(key[0], JsonArrayToStrings(*value));
        }
      }
    }
    auto numberIt = basicIt->second->object.find("numbersymbol");
    if (numberIt != basicIt->second->object.end() && numberIt->second &&
        numberIt->second->type == JsonValue::Type::kObject) {
      for (const auto& [key, value] : numberIt->second->object) {
        if (!key.empty()) {
          data.numberSymbol.emplace(key[0], JsonArrayToStrings(*value));
        }
      }
    }
  }

  auto specialIt = root.object.find("special");
  if (specialIt != root.object.end() &&
      specialIt->second && specialIt->second->type == JsonValue::Type::kObject) {
    auto decryptIt = specialIt->second->object.find("DECRYPT");
    if (decryptIt != specialIt->second->object.end() && decryptIt->second &&
        decryptIt->second->type == JsonValue::Type::kObject) {
      auto jpIt = decryptIt->second->object.find("JP");
      if (jpIt != decryptIt->second->object.end() && jpIt->second) {
        data.decryptJp = JsonArrayToStrings(*jpIt->second);
      }
      auto cnIt = decryptIt->second->object.find("CN");
      if (cnIt != decryptIt->second->object.end() && cnIt->second) {
        data.decryptCn = JsonArrayToStrings(*cnIt->second);
      }
    }
  }

  return data;
}

std::vector<std::string> SplitTokens(const std::string& text) {
  std::vector<std::string> tokens;
  size_t start = 0;
  while (start <= text.size()) {
    size_t end = text.find('/', start);
    if (end == std::string::npos) {
      tokens.push_back(text.substr(start));
      break;
    }
    tokens.push_back(text.substr(start, end - start));
    start = end + 1;
  }
  return tokens;
}

std::string StripTokenPrefix(const std::string& token) {
  size_t idx = 0;
  while (idx < token.size() && token[idx] >= '0' && token[idx] <= '9') {
    ++idx;
  }
  return token.substr(idx);
}

size_t CountPayloadSlots(const std::vector<std::string>& tokens) {
  size_t count = 0;
  for (const auto& token : tokens) {
    std::string stripped = StripTokenPrefix(token);
    if (stripped == "N" || stripped == "V" || stripped == "A" ||
        stripped == "AD") {
      ++count;
    }
  }
  return count;
}

std::string RandomChoice(const std::vector<std::string>& items) {
  if (items.empty()) {
    return "";
  }
  int idx = moyue::misc::GetRandomIndex(static_cast<int>(items.size()));
  return items[static_cast<size_t>(idx)];
}

std::vector<std::string> SplitUtf8(const std::string& input) {
  std::vector<std::string> chars;
  for (size_t i = 0; i < input.size();) {
    unsigned char c = static_cast<unsigned char>(input[i]);
    size_t len = 1;
    if (c >= 0xF0) {
      len = 4;
    } else if (c >= 0xE0) {
      len = 3;
    } else if (c >= 0xC0) {
      len = 2;
    }
    chars.push_back(input.substr(i, len));
    i += len;
  }
  return chars;
}

void InsertUtf8Tokens(std::vector<std::string>& chars, size_t index,
                      const std::string& value) {
  auto tokens = SplitUtf8(value);
  if (tokens.empty()) {
    return;
  }
  if (index > chars.size()) {
    index = chars.size();
  }
  chars.insert(chars.begin() + static_cast<std::ptrdiff_t>(index),
               tokens.begin(), tokens.end());
}
}  // namespace detail

class WenyanSimulator {
 public:
  explicit WenyanSimulator(const std::string& key)
      : mappingData_(detail::LoadWenyanMapping("mapping_next.json")),
        roundObfusHelper_(key) {
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
    if (text.empty()) {
      return nullStr_;
    }
    auto mapIt = mappingData_.actualMaps.find(type);
    if (mapIt == mappingData_.actualMaps.end()) {
      return nullStr_;
    }
    char token = text.front();
    const auto& map = mapIt->second;
    auto alphaIt = map.alphabet.find(token);
    if (alphaIt != map.alphabet.end()) {
      return alphaIt->second;
    }
    auto numIt = map.numberSymbol.find(token);
    if (numIt != map.numberSymbol.end()) {
      return numIt->second;
    }
    return nullStr_;
  }

  std::string FindOriginText(const std::string& text) {
    //反向查表函数
    auto it = decodeTable_.find(text);
    if (it == decodeTable_.end()) {
      return nullStr_;
    }
    return std::string(1, it->second);
  }

  void InitDecodeTable() {
    decodeTable_.clear();
    for (const auto& [type, map] : mappingData_.actualMaps) {
      for (const auto& [key, value] : map.alphabet) {
        decodeTable_[value] = key;
      }
      for (const auto& [key, value] : map.numberSymbol) {
        decodeTable_[value] = key;
      }
    }
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
    if (originStr.empty()) {
      return "";
    }
    std::string output;
    size_t index = 0;
    size_t length = originStr.size();
    const auto& beginTemplates = mappingData_.sentencesBegin;
    const auto& endTemplates = mappingData_.sentencesEnd;

    roundObfusHelper_.RoundKey();
    while (index < length) {
      size_t remaining = length - index;
      const auto& templates = (remaining <= 6 && !endTemplates.empty())
                                  ? endTemplates
                                  : beginTemplates;
      std::vector<std::string> selectedTokens;
      size_t attempts = 0;
      while (attempts < templates.size()) {
        std::string tpl = templates[static_cast<size_t>(
            moyue::misc::GetRandomIndex(static_cast<int>(templates.size())))];
        auto tokens = detail::SplitTokens(tpl);
        size_t slots = detail::CountPayloadSlots(tokens);
        if (slots > 0 && slots <= remaining) {
          selectedTokens = std::move(tokens);
          break;
        }
        ++attempts;
      }
      if (selectedTokens.empty()) {
        selectedTokens = {"N"};
      }

      for (const auto& token : selectedTokens) {
        std::string stripped = detail::StripTokenPrefix(token);
        bool hadPrefix = stripped != token;
        if (stripped == "N" || stripped == "V" || stripped == "A" ||
            stripped == "AD") {
          if (index >= length) {
            continue;
          }
          std::string obfuscated =
              roundObfusHelper_.RoundKeyMatch(std::string(1, originStr[index]));
          roundObfusHelper_.RoundKey();
          output += GetCryptText(obfuscated, stripped);
          ++index;
          continue;
        }
        if (stripped == "MV") {
          output += detail::RandomChoice(mappingData_.modalVerbs);
          continue;
        }
        auto virtualIt = mappingData_.virtualWords.find(stripped);
        if (virtualIt != mappingData_.virtualWords.end()) {
          output += detail::RandomChoice(virtualIt->second);
          continue;
        }
        if (stripped == "P") {
          output += "。";
          continue;
        }
        if (stripped == "Q") {
          output += "？";
          continue;
        }
        if (stripped == "R") {
          output += "：";
          continue;
        }
        if (stripped == "Z") {
          output += "。";
          continue;
        }
        if (hadPrefix && stripped.empty()) {
          continue;
        }
        if (hadPrefix && stripped.size() == 1 &&
            stripped[0] >= 'A' && stripped[0] <= 'Z') {
          continue;
        }
        if (stripped.size() == 1 &&
            (stripped[0] < 'a' || stripped[0] > 'z') &&
            (stripped[0] < 'A' || stripped[0] > 'Z')) {
          continue;
        }
        output += stripped;
      }
    }
    return output;
  }

  std::string DeMap(const std::string& originStr) {
    std::string output;
    auto chars = detail::SplitUtf8(originStr);
    roundObfusHelper_.RoundKey();
    for (const auto& ch : chars) {
      std::string mapped = FindOriginText(ch);
      if (mapped == nullStr_) {
        continue;
      }
      std::string original = roundObfusHelper_.DRoundKeyMatch(mapped);
      roundObfusHelper_.RoundKey();
      output += original;
    }
    return output;
  }

 private:
  detail::WenyanMappingData mappingData_;
  std::string nullStr_ = "孎"; //默认忽略的占位字符，一个生僻字。
  moyue::round::RoundObfus roundObfusHelper_;
  std::unordered_map<std::string, char> decodeTable_;
};

class OldMapper {
 public:
  explicit OldMapper(const std::string& key)
      : mappingData_(detail::LoadOldMapping("mapping.json")),
        roundObfusHelper_(key) {
    InitDecodeTable();
  }

  std::string RoundKeyMatch(const std::string& keyIn) {
    return roundObfusHelper_.RoundKeyMatch(keyIn);
  }

  std::string DRoundKeyMatch(const std::string& keyIn) {
    return roundObfusHelper_.DRoundKeyMatch(keyIn);
  }

  void RoundKey() { roundObfusHelper_.RoundKey(); }

  std::string GetCryptText(const std::string& text) {
    if (text.empty()) {
      return nullStr_;
    }
    char token = text.front();
    auto alphaIt = mappingData_.alphabet.find(token);
    if (alphaIt != mappingData_.alphabet.end()) {
      return detail::RandomChoice(alphaIt->second);
    }
    auto numIt = mappingData_.numberSymbol.find(token);
    if (numIt != mappingData_.numberSymbol.end()) {
      return detail::RandomChoice(numIt->second);
    }
    return nullStr_;
  }

  std::string FindOriginText(const std::string& text) {
    auto it = decodeTable_.find(text);
    if (it == decodeTable_.end()) {
      return nullStr_;
    }
    return std::string(1, it->second);
  }

  std::string EnMap(const std::string& originStr, bool q) {
    std::string output;
    roundObfusHelper_.RoundKey();
    for (char c : originStr) {
      std::string obfuscated = roundObfusHelper_.RoundKeyMatch(std::string(1, c));
      roundObfusHelper_.RoundKey();
      output += GetCryptText(obfuscated);
    }

    if (!q && (!mappingData_.decryptJp.empty() || !mappingData_.decryptCn.empty())) {
      auto chars = detail::SplitUtf8(output);
      std::vector<int> avoid;
      if (!mappingData_.decryptJp.empty() && !chars.empty()) {
        std::vector<int> positions;
        positions.reserve(chars.size());
        for (size_t i = 0; i < chars.size(); ++i) {
          positions.push_back(static_cast<int>(i));
        }
        int jpIndex = positions[static_cast<size_t>(moyue::misc::GetRandomIndex(
            static_cast<int>(positions.size())))];
        std::string jpMarker = detail::RandomChoice(mappingData_.decryptJp);
        detail::InsertUtf8Tokens(chars, static_cast<size_t>(jpIndex), jpMarker);
        auto markerTokens = detail::SplitUtf8(jpMarker);
        for (size_t i = 1; i < markerTokens.size(); ++i) {
          avoid.push_back(jpIndex + static_cast<int>(i));
        }
      }

      if (!mappingData_.decryptCn.empty() && !chars.empty()) {
        std::vector<int> positions;
        positions.reserve(chars.size());
        for (size_t i = 0; i < chars.size(); ++i) {
          positions.push_back(static_cast<int>(i));
        }
        std::vector<int> available;
        available.reserve(positions.size());
        for (int pos : positions) {
          if (std::find(avoid.begin(), avoid.end(), pos) == avoid.end()) {
            available.push_back(pos);
          }
        }
        if (!available.empty()) {
          int cnIndex = available[static_cast<size_t>(moyue::misc::GetRandomIndex(
              static_cast<int>(available.size())))];
          std::string cnMarker = detail::RandomChoice(mappingData_.decryptCn);
          detail::InsertUtf8Tokens(chars, static_cast<size_t>(cnIndex), cnMarker);
        }
      }

      output.clear();
      for (const auto& ch : chars) {
        output += ch;
      }
    }
    return output;
  }

  std::string DeMap(const std::string& originStr) {
    std::string output;
    auto chars = detail::SplitUtf8(originStr);
    roundObfusHelper_.RoundKey();
    for (const auto& ch : chars) {
      std::string mapped = FindOriginText(ch);
      if (mapped == nullStr_) {
        continue;
      }
      std::string original = roundObfusHelper_.DRoundKeyMatch(mapped);
      roundObfusHelper_.RoundKey();
      output += original;
    }
    return output;
  }

 private:
  void InitDecodeTable() {
    decodeTable_.clear();
    for (const auto& [key, values] : mappingData_.alphabet) {
      for (const auto& value : values) {
        decodeTable_[value] = key;
      }
    }
    for (const auto& [key, values] : mappingData_.numberSymbol) {
      for (const auto& value : values) {
        decodeTable_[value] = key;
      }
    }
  }

  detail::OldMappingData mappingData_;
  std::string nullStr_ = "孎"; //默认忽略的占位字符，一个生僻字。
  moyue::round::RoundObfusOld roundObfusHelper_;
  std::unordered_map<std::string, char> decodeTable_;
};

}  // namespace moyue::mapping
