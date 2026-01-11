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
std::vector<std::uint8_t> GzipCompress(const std::vector<std::uint8_t>& data) {
  if (data.empty()) {
    return data;
  }

  z_stream stream{};
  if (deflateInit2(&stream, Z_BEST_COMPRESSION, Z_DEFLATED, 15 + 16, 8,
                   Z_DEFAULT_STRATEGY) != Z_OK) {
    return data;
  }

  std::vector<std::uint8_t> output;
  output.resize(compressBound(static_cast<uLong>(data.size())));

  stream.next_in = const_cast<Bytef*>(data.data());
  stream.avail_in = static_cast<uInt>(data.size());
  stream.next_out = output.data();
  stream.avail_out = static_cast<uInt>(output.size());

  int result = deflate(&stream, Z_FINISH);
  if (result != Z_STREAM_END) {
    deflateEnd(&stream);
    return data;
  }
  output.resize(stream.total_out);
  deflateEnd(&stream);

  if (output.size() >= data.size()) {
    return data;
  }
  return output;
}

std::vector<std::uint8_t> GzipDecompress(
    const std::vector<std::uint8_t>& data) {
  if (data.size() < 2) {
    return data;
  }
  if (data[0] != 0x1f || data[1] != 0x8b) {
    return data;
  }

  z_stream stream{};
  if (inflateInit2(&stream, 15 + 32) != Z_OK) {
    return data;
  }

  std::vector<std::uint8_t> output;
  std::array<std::uint8_t, 32768> buffer{};

  stream.next_in = const_cast<Bytef*>(data.data());
  stream.avail_in = static_cast<uInt>(data.size());

  int result = Z_OK;
  while (result != Z_STREAM_END) {
    stream.next_out = buffer.data();
    stream.avail_out = static_cast<uInt>(buffer.size());
    result = inflate(&stream, Z_NO_FLUSH);
    if (result != Z_OK && result != Z_STREAM_END) {
      inflateEnd(&stream);
      return data;
    }
    size_t produced = buffer.size() - stream.avail_out;
    output.insert(output.end(), buffer.begin(), buffer.begin() + produced);
  }

  inflateEnd(&stream);
  return output;
}

std::string QuoteForShell(const std::string& input) {
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
}

std::vector<std::uint8_t> RunUnishoxHelper(const std::vector<std::uint8_t>& data,
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

std::vector<std::uint8_t> UnishoxCompress(
    const std::vector<std::uint8_t>& data) {
  return RunUnishoxHelper(data, "compress");
}

std::vector<std::uint8_t> UnishoxDecompress(
    const std::vector<std::uint8_t>& data) {
  return RunUnishoxHelper(data, "decompress");
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
