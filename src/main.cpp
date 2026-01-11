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
#include <string>
#include <variant>

#include "Abracadabra.cpp"

int main() {
  try {
    moyue::app::Abracadabra abra;
    std::string input = "Hello 魔曰";

    abra.WenyanInput(input, moyue::app::Abracadabra::Mode::kEncrypt,
                     "ABRACADABRA");
    auto encrypted = std::get<std::string>(abra.Output());

    abra.WenyanInput(encrypted, moyue::app::Abracadabra::Mode::kDecrypt,
                     "ABRACADABRA");
    auto decrypted = std::get<std::string>(abra.Output());

    std::cout << "Plaintext : " << input << '\n';
    std::cout << "Ciphertext: " << encrypted << '\n';
    std::cout << "Decrypted : " << decrypted << '\n';
  } catch (const std::exception& ex) {
    std::cerr << "Error: " << ex.what() << '\n';
    return 1;
  }
  return 0;
}
