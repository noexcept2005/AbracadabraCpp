import {
  unishox2_compress_simple,
  unishox2_decompress,
  USX_HCODES_DFLT,
  USX_HCODE_LENS_DFLT,
  USX_FREQ_SEQ_DFLT,
  USX_TEMPLATES,
} from "../Abracadabra/unishox2.js";

const CHINESE_WEBPAN_LIB = [
  "https://",
  "lanzou",
  "pan.quark.cn",
  "pan.baidu.com",
  "aliyundrive.com",
  "123pan.com",
];
const INTER_WEBPAN_LIB = [
  "https://",
  "mypikpak.com",
  "mega.nz",
  "drive.google.com",
  "sharepoint.com",
  "1drv.ms",
];
const CHINESE_WEBSITE_LIB = [
  "https://",
  "baidu.com",
  "b23.tv",
  "bilibili.com",
  "weibo.com",
  "weixin.qq.com",
];
const INTER_WEBSITE_LIB = [
  "https://",
  "google.com",
  "youtube.com",
  "x.com",
  "twitter.com",
  "telegra.ph",
];
const INTER_WEBSITE_LIB_2 = [
  "https://",
  "wikipedia.org",
  "github.com",
  "pages.dev",
  "github.io",
  "netlify.app",
];
const JAPAN_WEBSITE_LIB = [
  "https://",
  "pixiv.net",
  "nicovideo.jp",
  "dlsite.com",
  "line.me",
  "dmm.com",
];
const PIRACY_WEBSITE_LIB = [
  "https://",
  "nyaa.si",
  "bangumi.moe",
  "thepiratebay.org",
  "e-hentai.org",
  "exhentai.org",
];
const GENERIC_TLINK_LIB = [
  "https://",
  "magnet:?xt=urn:btih:",
  "magnet:?xt=urn:sha1:",
  "ed2k://",
  "thunder://",
  "torrent",
];
const GENERIC_LINK_LIB_1 = ["https://", ".cn", ".com", ".net", ".org", ".xyz"];
const GENERIC_LINK_LIB_2 = ["https://", ".info", ".moe", ".cc", ".co", ".dev"];
const GENERIC_LINK_LIB_3 = ["https://", ".io", ".us", ".eu", ".jp", ".de"];
const GENERIC_LINK_LIB_4 = [
  "https://",
  ".top",
  ".one",
  ".online",
  ".me",
  ".ca",
];

function uint8ArrayToString(data) {
  return Buffer.from(data).toString("utf8");
}

function unishoxCompress(data) {
  const compressed = new Uint8Array(2048);
  const dataStr = uint8ArrayToString(data);
  let libmark = 255;

  for (let i = 1; i < 6; i++) {
    if (dataStr.includes(CHINESE_WEBPAN_LIB[i])) {
      libmark = 254;
      break;
    }
    if (dataStr.includes(INTER_WEBPAN_LIB[i])) {
      libmark = 245;
      break;
    }
  }
  if (libmark === 255) {
    for (let i = 1; i < 6; i++) {
      if (dataStr.includes(CHINESE_WEBSITE_LIB[i])) {
        libmark = 253;
        break;
      }
      if (dataStr.includes(INTER_WEBSITE_LIB[i])) {
        libmark = 252;
        break;
      }
      if (dataStr.includes(INTER_WEBSITE_LIB_2[i])) {
        libmark = 244;
        break;
      }
      if (dataStr.includes(JAPAN_WEBSITE_LIB[i])) {
        libmark = 251;
        break;
      }
      if (dataStr.includes(PIRACY_WEBSITE_LIB[i])) {
        libmark = 250;
        break;
      }
    }
  }
  if (libmark === 255) {
    for (let i = 1; i < 6; i++) {
      if (dataStr.includes(GENERIC_TLINK_LIB[i])) {
        libmark = 249;
        break;
      }
      if (dataStr.includes(GENERIC_LINK_LIB_1[i])) {
        libmark = 248;
        break;
      }
      if (dataStr.includes(GENERIC_LINK_LIB_2[i])) {
        libmark = 247;
        break;
      }
      if (dataStr.includes(GENERIC_LINK_LIB_3[i])) {
        libmark = 246;
        break;
      }
      if (dataStr.includes(GENERIC_LINK_LIB_4[i])) {
        libmark = 243;
        break;
      }
    }
  }

  let outLen;
  switch (libmark) {
    case 255:
      outLen = unishox2_compress_simple(data, data.byteLength, compressed);
      break;
    case 254:
      outLen = unishox2_compress_simple(
        data,
        data.byteLength,
        compressed,
        CHINESE_WEBPAN_LIB
      );
      break;
    case 245:
      outLen = unishox2_compress_simple(
        data,
        data.byteLength,
        compressed,
        INTER_WEBPAN_LIB
      );
      break;
    case 253:
      outLen = unishox2_compress_simple(
        data,
        data.byteLength,
        compressed,
        CHINESE_WEBSITE_LIB
      );
      break;
    case 252:
      outLen = unishox2_compress_simple(
        data,
        data.byteLength,
        compressed,
        INTER_WEBSITE_LIB
      );
      break;
    case 244:
      outLen = unishox2_compress_simple(
        data,
        data.byteLength,
        compressed,
        INTER_WEBSITE_LIB_2
      );
      break;
    case 251:
      outLen = unishox2_compress_simple(
        data,
        data.byteLength,
        compressed,
        JAPAN_WEBSITE_LIB
      );
      break;
    case 250:
      outLen = unishox2_compress_simple(
        data,
        data.byteLength,
        compressed,
        PIRACY_WEBSITE_LIB
      );
      break;
    case 249:
      outLen = unishox2_compress_simple(
        data,
        data.byteLength,
        compressed,
        GENERIC_TLINK_LIB
      );
      break;
    case 248:
      outLen = unishox2_compress_simple(
        data,
        data.byteLength,
        compressed,
        GENERIC_LINK_LIB_1
      );
      break;
    case 247:
      outLen = unishox2_compress_simple(
        data,
        data.byteLength,
        compressed,
        GENERIC_LINK_LIB_2
      );
      break;
    case 246:
      outLen = unishox2_compress_simple(
        data,
        data.byteLength,
        compressed,
        GENERIC_LINK_LIB_3
      );
      break;
    case 243:
      outLen = unishox2_compress_simple(
        data,
        data.byteLength,
        compressed,
        GENERIC_LINK_LIB_4
      );
      break;
    default:
      outLen = unishox2_compress_simple(data, data.byteLength, compressed);
      break;
  }

  let result = compressed.subarray(0, outLen);
  if (result.byteLength >= data.byteLength) {
    return data;
  }

  const withMarker = new Uint8Array(result.byteLength + 2);
  withMarker.set(result, 0);
  withMarker.set([libmark, 255], result.byteLength);
  return withMarker;
}

function unishoxDecompress(data) {
  const last = data[data.byteLength - 1];
  const secondLast = data[data.byteLength - 2];
  if (last !== 255 || secondLast < 243 || secondLast > 255) {
    return data;
  }

  const libmark = secondLast;
  const newData = data.subarray(0, data.byteLength - 2);
  const output = new Uint8Array(2048);

  let outLen;
  switch (libmark) {
    case 255:
      outLen = unishox2_decompress(
        newData,
        newData.byteLength,
        output,
        USX_HCODES_DFLT,
        USX_HCODE_LENS_DFLT,
        USX_FREQ_SEQ_DFLT,
        USX_TEMPLATES
      );
      break;
    case 254:
      outLen = unishox2_decompress(
        newData,
        newData.byteLength,
        output,
        USX_HCODES_DFLT,
        USX_HCODE_LENS_DFLT,
        CHINESE_WEBPAN_LIB,
        USX_TEMPLATES
      );
      break;
    case 245:
      outLen = unishox2_decompress(
        newData,
        newData.byteLength,
        output,
        USX_HCODES_DFLT,
        USX_HCODE_LENS_DFLT,
        INTER_WEBPAN_LIB,
        USX_TEMPLATES
      );
      break;
    case 253:
      outLen = unishox2_decompress(
        newData,
        newData.byteLength,
        output,
        USX_HCODES_DFLT,
        USX_HCODE_LENS_DFLT,
        CHINESE_WEBSITE_LIB,
        USX_TEMPLATES
      );
      break;
    case 252:
      outLen = unishox2_decompress(
        newData,
        newData.byteLength,
        output,
        USX_HCODES_DFLT,
        USX_HCODE_LENS_DFLT,
        INTER_WEBSITE_LIB,
        USX_TEMPLATES
      );
      break;
    case 244:
      outLen = unishox2_decompress(
        newData,
        newData.byteLength,
        output,
        USX_HCODES_DFLT,
        USX_HCODE_LENS_DFLT,
        INTER_WEBSITE_LIB_2,
        USX_TEMPLATES
      );
      break;
    case 251:
      outLen = unishox2_decompress(
        newData,
        newData.byteLength,
        output,
        USX_HCODES_DFLT,
        USX_HCODE_LENS_DFLT,
        JAPAN_WEBSITE_LIB,
        USX_TEMPLATES
      );
      break;
    case 250:
      outLen = unishox2_decompress(
        newData,
        newData.byteLength,
        output,
        USX_HCODES_DFLT,
        USX_HCODE_LENS_DFLT,
        PIRACY_WEBSITE_LIB,
        USX_TEMPLATES
      );
      break;
    case 249:
      outLen = unishox2_decompress(
        newData,
        newData.byteLength,
        output,
        USX_HCODES_DFLT,
        USX_HCODE_LENS_DFLT,
        GENERIC_TLINK_LIB,
        USX_TEMPLATES
      );
      break;
    case 248:
      outLen = unishox2_decompress(
        newData,
        newData.byteLength,
        output,
        USX_HCODES_DFLT,
        USX_HCODE_LENS_DFLT,
        GENERIC_LINK_LIB_1,
        USX_TEMPLATES
      );
      break;
    case 247:
      outLen = unishox2_decompress(
        newData,
        newData.byteLength,
        output,
        USX_HCODES_DFLT,
        USX_HCODE_LENS_DFLT,
        GENERIC_LINK_LIB_2,
        USX_TEMPLATES
      );
      break;
    case 246:
      outLen = unishox2_decompress(
        newData,
        newData.byteLength,
        output,
        USX_HCODES_DFLT,
        USX_HCODE_LENS_DFLT,
        GENERIC_LINK_LIB_3,
        USX_TEMPLATES
      );
      break;
    case 243:
      outLen = unishox2_decompress(
        newData,
        newData.byteLength,
        output,
        USX_HCODES_DFLT,
        USX_HCODE_LENS_DFLT,
        GENERIC_LINK_LIB_4,
        USX_TEMPLATES
      );
      break;
    default:
      outLen = unishox2_decompress(
        newData,
        newData.byteLength,
        output,
        USX_HCODES_DFLT,
        USX_HCODE_LENS_DFLT,
        USX_FREQ_SEQ_DFLT,
        USX_TEMPLATES
      );
      break;
  }

  return output.subarray(0, outLen);
}

const mode = process.argv[2];
const base64 = process.argv[3];
if (!mode || !base64) {
  process.exit(1);
}

const input = new Uint8Array(Buffer.from(base64, "base64"));
let result;
if (mode === "compress") {
  result = unishoxCompress(input);
} else if (mode === "decompress") {
  result = unishoxDecompress(input);
} else {
  process.exit(1);
}

process.stdout.write(Buffer.from(result).toString("base64"));
