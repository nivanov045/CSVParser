#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <set>

#include "CSVParser.h"

int main()
{
  for (const std::string& name : {
    "test-easy-wrong.csv",
    "test-easy-ok.csv",
    })
  {
    std::string fullName = "Tests/" + name;
    std::ifstream file(fullName);
    CSVParser<int, std::string, std::string> parser(file, 0);
    std::cout << fullName << ":" << std::endl;
    try {
      auto res = parser.Parse();
      for (std::tuple<int, std::string, std::string> rs : res) {
        std::cout << rs << std::endl;
      }
    }
    catch (std::invalid_argument) {
      
    }
    std::cout << std::endl;
  }

  std::string fullName = "Tests/test-easy-ok-feat.csv";
  std::ifstream file(fullName);
  CSVParser<int, std::string, std::string> parser(file, 1, ';', '\'');
  std::cout << fullName << ":" << std::endl;
  auto res = parser.Parse();
  for (std::tuple<int, std::string, std::string> rs : res) {
    std::cout << rs << std::endl;
  }
  system("pause");

  return 0;
}