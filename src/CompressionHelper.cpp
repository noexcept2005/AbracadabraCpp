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
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

#include <zlib.h>

#include "Misc.cpp"

namespace moyue::compress {

namespace {
std::string QuoteForShell(const std::string& input) {
#ifdef _WIN32
  std::string output = "\"";
  for (char c : input) {
    if (c == '"') {
      output += "\\\"";
    } else {
      output += c;
    }
  }
  output += "\"";
  return output;
#else
  std::string output = "'";
  for (char c : input) {
    if (c == '\'') {
      output += "'\\''";
    } else {
      output += c;
    }
  }
  output += "'";
  return output;
#endif
}

std::vector<std::uint8_t> RunNodeHelper(const std::vector<std::uint8_t>& data,
                                        const std::string& mode) {
  std::string input(data.begin(), data.end());
  std::string base64 = moyue::misc::Base64Encode(input);

  std::filesystem::path helperPath =
      std::filesystem::path(__FILE__).parent_path() / "unishox_helper.mjs";

  std::string command = "node " + QuoteForShell(helperPath.string()) + " " +
                        QuoteForShell(mode) + " " + QuoteForShell(base64);

  std::array<char, 256> buffer{};
  std::string output;
  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    return data;
  }
  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe)) {
    output += buffer.data();
  }
  int status = pclose(pipe);
  if (status != 0 || output.empty()) {
    return data;
  }

  while (!output.empty() &&
         (output.back() == '\n' || output.back() == '\r')) {
    output.pop_back();
  }

  std::string decoded = moyue::misc::Base64Decode(output);
  return std::vector<std::uint8_t>(decoded.begin(), decoded.end());
}

std::vector<std::uint8_t> GzipCompress(const std::vector<std::uint8_t>& data) {
  return RunNodeHelper(data, "gzip_compress");
}

std::vector<std::uint8_t> GzipDecompress(
    const std::vector<std::uint8_t>& data) {
  return RunNodeHelper(data, "gzip_decompress");
}

std::vector<std::uint8_t> UnishoxCompress(
    const std::vector<std::uint8_t>& data) {
  return RunNodeHelper(data, "compress");
}

std::vector<std::uint8_t> UnishoxDecompress(
    const std::vector<std::uint8_t>& data) {
  return RunNodeHelper(data, "decompress");
}
}  // namespace

std::vector<std::uint8_t> Compress(const std::vector<std::uint8_t>& originalData) {
  if (originalData.size() <= 1024) {
    auto compressed = UnishoxCompress(originalData);
    if (compressed.size() == originalData.size()) {
      compressed = GzipCompress(compressed);
    }
    return compressed;
  }
  return GzipCompress(originalData);
}

std::vector<std::uint8_t> Decompress(
    const std::vector<std::uint8_t>& originalData) {
  auto data = GzipDecompress(originalData);
  return UnishoxDecompress(data);
}

}  // namespace moyue::compress
