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

#include "Abracadabra.cpp"

namespace {
void PrintHelp() {
  std::cout
      << "Usage: moyue.exe [-e|-d] -i <text> [-k <key>] [--old]\n"
         "  -e         Encrypt input\n"
         "  -d         Decrypt input\n"
         "  -i <text>  Input text (omit to read from stdin)\n"
         "  -k <key>   Key (default: ABRACADABRA)\n"
         "  --old      Use old mapping mode\n"
         "  /?         Show this help\n";
}

std::string ReadStdin() {
  std::ostringstream buffer;
  buffer << std::cin.rdbuf();
  return buffer.str();
}
}  // namespace

int main(int argc, char* argv[]) {
  try {
    if (argc == 1) {
      PrintHelp();
      return 0;
    }

    bool encrypt = false;
    bool decrypt = false;
    bool useOld = false;
    std::string key = "ABRACADABRA";
    std::string input;

    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "/?" || arg == "-h" || arg == "--help") {
        PrintHelp();
        return 0;
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
      if (arg == "-k") {
        if (i + 1 >= argc) {
          throw std::runtime_error("Missing key after -k");
        }
        key = argv[++i];
        continue;
      }
      if (arg == "-i") {
        if (i + 1 >= argc) {
          throw std::runtime_error("Missing input after -i");
        }
        input = argv[++i];
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
                    key);
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
