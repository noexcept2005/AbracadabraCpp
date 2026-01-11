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

#include <cstdint>
#include <string>
#include <vector>

#include "Misc.cpp"

namespace moyue::compress {

namespace {
const std::vector<std::string> kChineseWebpanLib = {
    "https://", "lanzou", "pan.quark.cn", "pan.baidu.com",
    "aliyundrive.com", "123pan.com"};
const std::vector<std::string> kInterWebpanLib = {
    "https://", "mypikpak.com", "mega.nz", "drive.google.com",
    "sharepoint.com", "1drv.ms"};
const std::vector<std::string> kChineseWebsiteLib = {
    "https://", "baidu.com", "b23.tv", "bilibili.com", "weibo.com",
    "weixin.qq.com"};
const std::vector<std::string> kInterWebsiteLib = {
    "https://", "google.com", "youtube.com", "x.com", "twitter.com",
    "telegra.ph"};
const std::vector<std::string> kInterWebsiteLib2 = {
    "https://", "wikipedia.org", "github.com", "pages.dev",
    "github.io", "netlify.app"};
const std::vector<std::string> kJapanWebsiteLib = {
    "https://", "pixiv.net", "nicovideo.jp", "dlsite.com", "line.me",
    "dmm.com"};
const std::vector<std::string> kPiracyWebsiteLib = {
    "https://", "nyaa.si", "bangumi.moe", "thepiratebay.org",
    "e-hentai.org", "exhentai.org"};
const std::vector<std::string> kGenericTlinkLib = {
    "https://", "magnet:?xt=urn:btih:", "magnet:?xt=urn:sha1:", "ed2k://",
    "thunder://", "torrent"};
const std::vector<std::string> kGenericLinkLib1 =
    {"https://", ".cn", ".com", ".net", ".org", ".xyz"};
const std::vector<std::string> kGenericLinkLib2 =
    {"https://", ".info", ".moe", ".cc", ".co", ".dev"};
const std::vector<std::string> kGenericLinkLib3 =
    {"https://", ".io", ".us", ".eu", ".jp", ".de"};
const std::vector<std::string> kGenericLinkLib4 =
    {"https://", ".top", ".one", ".online", ".me", ".ca"};

std::vector<std::uint8_t> GzipCompress(const std::vector<std::uint8_t>& data) {
  // TODO: Replace with real gzip compression (pako equivalent).
  return data;
}

std::vector<std::uint8_t> GzipDecompress(
    const std::vector<std::uint8_t>& data) {
  // TODO: Replace with real gzip decompression.
  return data;
}

std::vector<std::uint8_t> UnishoxCompress(
    const std::vector<std::uint8_t>& data) {
  // TODO: Replace with Unishox2 implementation.
  return data;
}

std::vector<std::uint8_t> UnishoxDecompress(
    const std::vector<std::uint8_t>& data) {
  // TODO: Replace with Unishox2 implementation.
  return data;
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
