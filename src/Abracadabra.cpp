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

#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "CoreHandler.cpp"
#include "Misc.cpp"

namespace moyue::app {

class Abracadabra {
 public:
  //主类

  enum class DataType { kText, kUint8 };
  enum class Mode { kEncrypt, kDecrypt, kAuto };

  Abracadabra(DataType inputType = DataType::kText,
              DataType outputType = DataType::kText)
      : inputType_(inputType), outputType_(outputType) {}

  /**
   * 魔曰 文言文加密模式
   *
   * @param{string | Uint8Array}input 输入的数据，根据此前指定的输入类型，可能是字符串或字节数组
   * @param{string}mode 指定模式，可以是 ENCRYPT DECRYPT 中的一种;
   * @param{string}key 指定密钥，默认是 ABRACADABRA;
   * @param{WenyanConfig}WenyanConfigObj 文言文的生成配置;
   * @param{function}callback 文言文的生成配置;
   */
  int WenyanInput(const std::variant<std::string, std::vector<std::uint8_t>>& input,
                  Mode mode, const std::string& key = "ABRACADABRA",
                  const moyue::core::WenyanConfig& config =
                      moyue::core::WenyanConfig(true, 50, false, false),
                  const moyue::core::Callback& callback = nullptr) {
    if (inputType_ == DataType::kUint8) {
      if (!std::holds_alternative<std::vector<std::uint8_t>>(input)) {
        throw std::runtime_error("Unexpected Input Type");
      }
      moyue::core::InputPayload payload;
      payload.output = std::get<std::vector<std::uint8_t>>(input);
      if (mode == Mode::kEncrypt) {
        res_ = moyue::core::Enc(payload, key, config, callback);
      } else if (mode == Mode::kDecrypt) {
        res_ = moyue::core::Dec(payload, key, callback);
      }
      return 0;
    }

    if (!std::holds_alternative<std::string>(input)) {
      throw std::runtime_error("Unexpected Input Type");
    }
    std::string text = std::get<std::string>(input);
    moyue::core::InputPayload payload;
    payload.output = moyue::misc::StringToUint8Array(text);
    if (mode == Mode::kEncrypt) {
      res_ = moyue::core::Enc(payload, key, config, callback);
    } else if (mode == Mode::kDecrypt) {
      res_ = moyue::core::Dec(payload, key, callback);
    }
    return 0;
  }

  /**
   * 魔曰 传统加密模式
   *
   * @param{string | Uint8Array}input 输入的数据，根据此前指定的输入类型，可能是字符串或字节数组
   * @param{string}mode 指定模式，可以是 ENCRYPT DECRYPT AUTO 中的一种;
   * @param{string}key 指定密钥，默认是 ABRACADABRA;
   * @param{bool}q 指定是否在加密后省略标志位，默认 false/不省略;
   */
  int OldInput(const std::variant<std::string, std::vector<std::uint8_t>>& input,
               Mode mode, const std::string& key = "ABRACADABRA",
               bool q = false) {
    if (inputType_ == DataType::kUint8) {
      if (!std::holds_alternative<std::vector<std::uint8_t>>(input)) {
        throw std::runtime_error("Unexpected Input Type");
      }
      std::vector<std::uint8_t> bytes =
          std::get<std::vector<std::uint8_t>>(input);
      std::string inputString = moyue::misc::Uint8ArrayToString(bytes);
      auto preCheckRes = moyue::misc::PreCheckOld(inputString);
      if ((preCheckRes.isEncrypted() && mode != Mode::kEncrypt) ||
          mode == Mode::kDecrypt) {
        res_ = moyue::core::DecOld({preCheckRes.output()}, key);
      } else {
        res_ = moyue::core::EncOld({preCheckRes.output()}, key, q);
      }
      return 0;
    }

    if (!std::holds_alternative<std::string>(input)) {
      throw std::runtime_error("Unexpected Input Type");
    }
    std::string text = std::get<std::string>(input);
    auto preCheckRes = moyue::misc::PreCheckOld(text);
    if ((preCheckRes.isEncrypted() && mode != Mode::kEncrypt) ||
        mode == Mode::kDecrypt) {
      res_ = moyue::core::DecOld({preCheckRes.output()}, key);
    } else {
      res_ = moyue::core::EncOld({preCheckRes.output()}, key, q);
    }
    return 0;
  }

  /**
   * 魔曰 获取加密/解密后的结果
   */
  std::variant<std::string, std::vector<std::uint8_t>> Output() const {
    if (std::holds_alternative<std::string>(res_)) {
      if (outputType_ == DataType::kText) {
        return std::get<std::string>(res_);
      }
      const std::string& text = std::get<std::string>(res_);
      return std::vector<std::uint8_t>(text.begin(), text.end());
    }

    if (outputType_ == DataType::kText) {
      const auto& bytes = std::get<std::vector<std::uint8_t>>(res_);
      return moyue::misc::Uint8ArrayToString(bytes);
    }
    return std::get<std::vector<std::uint8_t>>(res_);
  }

 private:
  DataType inputType_;
  DataType outputType_;
  std::variant<std::string, std::vector<std::uint8_t>> res_;
};

}  // namespace moyue::app
