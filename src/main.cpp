#include <iostream>
#include <string>

#include "moyue_cipher.hpp"

int main() {
  try {
    // Small, educational OOP demo inspired by "魔曰" style transformations.
    MoYueCipher cipher("Abracadabra");

    std::string message = "Hello 魔曰!";
    std::string encrypted = cipher.Encrypt(message);
    std::string decrypted = cipher.Decrypt(encrypted);

    std::cout << "Plaintext : " << message << '\n';
    std::cout << "Ciphertext: " << encrypted << '\n';
    std::cout << "Decrypted : " << decrypted << '\n';
  } catch (const std::exception& ex) {
    std::cerr << "Error: " << ex.what() << '\n';
    return 1;
  }

  return 0;
}
