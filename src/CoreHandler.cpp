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
#include <functional>
#include <string>
#include <vector>

#include "ChineseMappingHelper.cpp"
#include "CompressionHelper.cpp"
#include "EncryptHelper.cpp"
#include "Misc.cpp"

namespace moyue::core {

struct InputPayload {
  std::vector<std::uint8_t> output;
};

class WenyanConfig {
 public:
  /**
   * 魔曰 文言文加密参数
   *
   * @param{bool}PunctuationMark 指定是否为密文添加标点符号，默认 true/添加;
   * @param{int}RandomIndex 密文算法的随机程度，越大随机性越强，默认 50，最大100，超过100将会出错;
   * @param{bool}PianwenMode 指定是否强制生成骈文密文，默认 false;
   * @param{bool}LogicMode 指定是否强制生成逻辑密文，默认 false;
   */
  WenyanConfig(bool punctuationMark = true, int randomIndex = 50,
              bool pianwenMode = false, bool logicMode = false,
              bool traditional = false)
      : punctuationMark_(punctuationMark),
        randomIndex_(randomIndex),
        pianwenMode_(pianwenMode),
        logicMode_(logicMode),
        traditional_(traditional) {}

  bool punctuationMark() const { return punctuationMark_; }
  int randomIndex() const { return randomIndex_; }
  bool pianwenMode() const { return pianwenMode_; }
  bool logicMode() const { return logicMode_; }
  bool traditional() const { return traditional_; }

 private:
  bool punctuationMark_;
  int randomIndex_;
  bool pianwenMode_;
  bool logicMode_;
  bool traditional_;
};

class CallbackObj {
 public:
  /**
   * 魔曰 Debug 回调位点对象
   *
   * ENC/DEC_BASE64 (Base64字符串)
   *
   * ROUNDS (转轮状态)
   *
   * ENC_MAPTEMP (映射过程变量)
   *
   * ENC_SENTENCES (组句步骤变量)
   *
   * ENC_PAYLOADS (加密的载荷分配数组)
   *
   * DEC_PAYLOAD (解密提取的有效载荷)
   *
   *
   * @param{string}Type 指定回调参数的Tag
   * @param{string}Value 回调参数的值
   */
  CallbackObj(std::string type = "NORMAL", std::string value = "")
      : type_(std::move(type)), value_(std::move(value)) {}

  const std::string& type() const { return type_; }
  const std::string& value() const { return value_; }

 private:
  std::string type_;
  std::string value_;
};

using Callback = std::function<void(const CallbackObj&)>;

/**
 * @param{WenyanConfig}WenyanConfigObj 文言文的生成配置;
 */
std::string Enc(const InputPayload& input, const std::string& key,
                const WenyanConfig& config, const Callback& callback = nullptr) {
  //初始化
  //input.output Uint8Array
  moyue::mapping::WenyanSimulator simulator(key);

  std::vector<std::uint8_t> originalData = input.output;
  std::vector<std::uint8_t> tempArray(originalData.size() + 1);
  std::copy(originalData.begin(), originalData.end(), tempArray.begin());

  //对未处理的数据计算校验和，放在末尾
  tempArray.back() = static_cast<std::uint8_t>(moyue::misc::GetLuhnBit(originalData));

  //压缩
  originalData = moyue::compress::Compress(tempArray);
  //加密
  originalData = moyue::encrypt::Encrypt(originalData, key);

  std::string encryptedStr(originalData.begin(), originalData.end());
  std::string originStr = moyue::misc::RemovePadding(
      moyue::misc::Base64Encode(encryptedStr));
  if (callback) {
    callback(CallbackObj("ENC_BASE64", originStr));
  }

  //映射
  return simulator.EnMap(originStr, config.punctuationMark(),
                         config.randomIndex(), config.pianwenMode(),
                         config.logicMode(), config.traditional());
}

std::string Dec(const InputPayload& input, const std::string& key,
                const Callback& callback = nullptr) {
  //初始化
  //input.output Uint8Array
  moyue::mapping::WenyanSimulator simulator(key);
  std::string originStr = moyue::misc::Uint8ArrayToString(input.output);

  //解映射
  std::string tempStr1 = simulator.DeMap(originStr);

  std::vector<std::uint8_t> tempStr2Int;
  std::string padded = moyue::misc::AddPadding(tempStr1);
  if (!moyue::misc::IsBase64String(padded)) {
    throw std::runtime_error("Error Decoding. Bad Input or Incorrect Key.");
  }
  std::string decoded = moyue::misc::Base64Decode(padded);
  tempStr2Int.assign(decoded.begin(), decoded.end());
  if (callback) {
    callback(CallbackObj("DEC_BASE64", tempStr1));
  }

  //解密
  tempStr2Int = moyue::encrypt::Decrypt(tempStr2Int, key);
  //解压缩
  tempStr2Int = moyue::compress::Decompress(tempStr2Int);

  if (!moyue::misc::CheckLuhnBit(tempStr2Int)) {
    throw std::runtime_error("Error Decrypting. Checksum Mismatch.");
  }
  tempStr2Int.pop_back();

  return moyue::misc::Uint8ArrayToString(tempStr2Int);
}

std::string EncOld(const InputPayload& input, const std::string& key, bool q) {
  //初始化
  moyue::mapping::OldMapper mapper(key);

  std::vector<std::uint8_t> originalData = input.output;

  std::vector<std::uint8_t> tempArray(originalData.size() + 1);
  std::copy(originalData.begin(), originalData.end(), tempArray.begin());
  tempArray.back() = static_cast<std::uint8_t>(moyue::misc::GetLuhnBit(originalData));

  //压缩
  originalData = moyue::compress::Compress(tempArray);
  //加密
  originalData = moyue::encrypt::Encrypt(originalData, key);

  std::string encryptedStr(originalData.begin(), originalData.end());
  std::string originStr = moyue::misc::RemovePadding(
      moyue::misc::Base64Encode(encryptedStr));
  //映射
  return mapper.EnMap(originStr, q);
}

std::string DecOld(const InputPayload& input, const std::string& key) {
  //初始化
  moyue::mapping::OldMapper mapper(key);
  std::string originStr = moyue::misc::Uint8ArrayToString(input.output);
  auto preCheckRes = moyue::misc::PreCheckOld(originStr);
  originStr = moyue::misc::Uint8ArrayToString(preCheckRes.output());

  //解映射
  std::string tempStr1 = mapper.DeMap(originStr);

  std::vector<std::uint8_t> tempStr2Int;
  std::string padded = moyue::misc::AddPadding(tempStr1);
  if (!moyue::misc::IsBase64String(padded)) {
    throw std::runtime_error("Error Decoding. Bad Input or Incorrect Key.");
  }
  std::string decoded = moyue::misc::Base64Decode(padded);
  tempStr2Int.assign(decoded.begin(), decoded.end());
  tempStr2Int = moyue::encrypt::Decrypt(tempStr2Int, key);
  tempStr2Int = moyue::compress::Decompress(tempStr2Int);

  if (!moyue::misc::CheckLuhnBit(tempStr2Int)) {
    if (tempStr2Int.size() >= 3 &&
        tempStr2Int[tempStr2Int.size() - 1] == 2 &&
        tempStr2Int[tempStr2Int.size() - 2] == 2 &&
        tempStr2Int[tempStr2Int.size() - 3] == 2) {
      tempStr2Int.resize(tempStr2Int.size() - 3);
    } else {
      throw std::runtime_error("Error Decrypting. Checksum Mismatch.");
    }
  } else {
    tempStr2Int.pop_back();
  }

  return moyue::misc::Uint8ArrayToString(tempStr2Int);
}

}  // namespace moyue::core
