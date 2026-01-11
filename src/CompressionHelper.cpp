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
#include <string_view>
#include <vector>

#include <zlib.h>

#include "Misc.cpp"

namespace moyue::compress {

namespace {
#ifdef _WIN32
std::wstring QuoteForShellW(std::wstring_view input) {
  std::wstring output = L"\"";
  for (wchar_t c : input) {
    if (c == L'"') {
      output += L"\\\"";
    } else {
      output += c;
    }
  }
  output += L"\"";
  return output;
}

std::wstring ToWideAscii(const std::string& input) {
  return std::wstring(input.begin(), input.end());
}
#else
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
#endif

std::vector<std::uint8_t> RunNodeHelper(const std::vector<std::uint8_t>& data,
                                        const std::string& mode) {
  std::string input(data.begin(), data.end());
  std::string base64 = moyue::misc::Base64Encode(input);

  std::filesystem::path helperPath = "unishox_helper.mjs";
  if (!std::filesystem::exists(helperPath)) {
    helperPath = std::filesystem::path("src") / "unishox_helper.mjs";
  }
  if (!std::filesystem::exists(helperPath)) {
    helperPath =
        std::filesystem::path(__FILE__).parent_path() / "unishox_helper.mjs";
  }

#ifdef _WIN32
  std::wstring command =
      L"node " + QuoteForShellW(helperPath.wstring()) + L" " +
      QuoteForShellW(ToWideAscii(mode)) + L" " +
      QuoteForShellW(ToWideAscii(base64));
#else
  std::string command = "node " + QuoteForShell(helperPath.string()) + " " +
                        QuoteForShell(mode) + " " + QuoteForShell(base64);
#endif

  std::array<char, 256> buffer{};
  std::string output;
#ifdef _WIN32
  FILE* pipe = _wpopen(command.c_str(), L"r");
#else
  FILE* pipe = popen(command.c_str(), "r");
#endif
  if (!pipe) {
    return data;
  }
  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe)) {
    output += buffer.data();
  }
#ifdef _WIN32
  int status = _pclose(pipe);
#else
  int status = pclose(pipe);
#endif
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
