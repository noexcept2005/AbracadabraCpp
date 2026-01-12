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

#include <iostream>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#include "Abracadabra.cpp"

namespace {
void PrintHelp() {
  std::cout
      << "Usage: moyue.exe [-e|-d] -i <text> [-k <key>] [--old]\n"
         "  -e         Encrypt input\n"
         "  -d         Decrypt input\n"
         "  -i <text>  Input text (omit to read from stdin)\n"
         "  -k <key>   Key (default: ABRACADABRA)\n"
         "  --old      Use old mapping mode (markers omitted by default)\n"
         "  --old-markers  Use old mode and include decrypt markers\n"
         "  --old-no-markers  Use old mode and omit decrypt markers\n"
         "  --         Treat all remaining arguments as input (for -i)\n"
         "  /?         Show this help\n";
}

std::string ReadStdin() {
  std::ostringstream buffer;
  buffer << std::cin.rdbuf();
  return buffer.str();
}

bool IsCliOption(const std::string& arg) {
  return arg == "/?" || arg == "-h" || arg == "--help" || arg == "-e" ||
         arg == "-d" || arg == "-k" || arg == "-i" || arg == "--old" ||
         arg == "--old-markers" || arg == "--old-no-markers" || arg == "--";
}
}  // namespace

int RunCli(const std::vector<std::string>& args) {
  try {
    if (args.size() <= 1) {
      PrintHelp();
      return 0;
    }

    bool encrypt = false;
    bool decrypt = false;
    bool useOld = false;
    bool oldOmitMarkers = true;
    std::string key = "ABRACADABRA";
    std::string input;

    for (size_t i = 1; i < args.size(); ++i) {
      std::string arg = args[i];
      if (arg == "/?" || arg == "-h" || arg == "--help") {
        PrintHelp();
        return 0;
      }
      if (arg == "--") {
        if (!input.empty()) {
          for (size_t j = i + 1; j < args.size(); ++j) {
            input += " ";
            input += args[j];
          }
          break;
        }
        continue;
      }
      if (arg == "-e") {
        encrypt = true;
        continue;
      }
      if (arg == "-d") {
        decrypt = true;
        continue;
      }
      if (arg == "--old") {
        useOld = true;
        continue;
      }
      if (arg == "--old-markers") {
        useOld = true;
        oldOmitMarkers = false;
        continue;
      }
      if (arg == "--old-no-markers") {
        useOld = true;
        oldOmitMarkers = true;
        continue;
      }
      if (arg == "-k") {
        if (i + 1 >= args.size()) {
          throw std::runtime_error("Missing key after -k");
        }
        key = args[++i];
        continue;
      }
      if (arg == "-i") {
        if (i + 1 >= args.size()) {
          throw std::runtime_error("Missing input after -i");
        }
        input = args[++i];
        while (i + 1 < args.size() && !IsCliOption(args[i + 1])) {
          input += " ";
          input += args[++i];
        }
        continue;
      }
      throw std::runtime_error("Unknown argument: " + arg);
    }

    if (encrypt == decrypt) {
      throw std::runtime_error("Specify exactly one of -e or -d");
    }

    if (input.empty()) {
      input = ReadStdin();
      if (input.empty()) {
        throw std::runtime_error("Input is empty");
      }
    }

    moyue::app::Abracadabra abra;
    if (useOld) {
      abra.OldInput(input,
                    encrypt ? moyue::app::Abracadabra::Mode::kEncrypt
                            : moyue::app::Abracadabra::Mode::kDecrypt,
                    key, oldOmitMarkers);
    } else {
      abra.WenyanInput(input,
                       encrypt ? moyue::app::Abracadabra::Mode::kEncrypt
                               : moyue::app::Abracadabra::Mode::kDecrypt,
                       key);
    }
    std::cout << std::get<std::string>(abra.Output());
  } catch (const std::exception& ex) {
    std::cerr << "Error: " << ex.what() << '\n';
    PrintHelp();
    return 1;
  }
  return 0;
}

#ifdef _WIN32
std::string WideToUtf8(const std::wstring& input) {
  if (input.empty()) {
    return {};
  }
  int len =
      WideCharToMultiByte(CP_UTF8, 0, input.data(),
                          static_cast<int>(input.size()), nullptr, 0, nullptr,
                          nullptr);
  if (len <= 0) {
    return {};
  }
  std::string output(static_cast<size_t>(len), '\0');
  WideCharToMultiByte(CP_UTF8, 0, input.data(),
                      static_cast<int>(input.size()), output.data(), len,
                      nullptr, nullptr);
  return output;
}

int wmain(int argc, wchar_t* argv[]) {
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
  std::vector<std::string> args;
  args.reserve(static_cast<size_t>(argc));
  for (int i = 0; i < argc; ++i) {
    args.emplace_back(WideToUtf8(argv[i]));
  }
  return RunCli(args);
}
#else
int main(int argc, char* argv[]) {
  std::vector<std::string> args;
  args.reserve(static_cast<size_t>(argc));
  for (int i = 0; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }
  return RunCli(args);
}
#endif
