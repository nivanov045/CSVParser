#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <tuple>
#include <sstream>

template <typename... TArgs>
class CSVParser
{
public:
  CSVParser(std::ifstream& iFile, uint32_t iSkipCount, char iSeparator = ',',
            char iEscapeSymbol = '"');
  std::vector<std::tuple<TArgs...>> Parse();

private:
  template <typename T>
  T getNextCell(std::stringstream& stream);

  static std::pair<uint32_t, uint32_t> _curCell;
  std::ifstream& _fileStream;
  char _separator;
  char _escapeSymbol;
};

template <typename... TArgs>
std::pair<uint32_t, uint32_t> CSVParser<TArgs...>::_curCell = std::pair<uint32_t, uint32_t>(1, 1);

template <typename... TArgs>
CSVParser<TArgs...>::
CSVParser(std::ifstream& iFileStream, uint32_t iSkipCount, char iSeparator, char iEscapeSymbol):
  _fileStream(iFileStream), _separator(iSeparator), _escapeSymbol(iEscapeSymbol)
{
  CSVParser<TArgs...>::_curCell = std::pair<uint32_t, uint32_t>(1, 1);
  std::string line;
  while (_curCell.first <= iSkipCount && std::getline(iFileStream, line)) {
    ++_curCell.first;
  }
}

template<typename ...TArgs>
std::vector<std::tuple<TArgs...>> CSVParser<TArgs...>::Parse()
{
  std::vector<std::tuple<TArgs...>> result;
  std::string line;
  while (std::getline(_fileStream, line)) {
    std::stringstream ss(line);
    try {
      std::tuple<TArgs...> curTuple = { getNextCell<TArgs>(ss)... };
      result.emplace_back(curTuple);
    }
    catch (std::invalid_argument& e) {
      std::cout << "[ERROR]: " << e.what() << " in line " << _curCell.first << ", column " << _curCell.second <<
        std::endl;
      throw std::invalid_argument("");
    }
    _curCell.second = 1;
    ++_curCell.first;
  }
  return result;
}

template <typename ... TArgs>
template <typename T>
T CSVParser<TArgs...>::getNextCell(std::stringstream& stream)
{
  T result;
  if (stream.peek() == _escapeSymbol) {
    //Escaped text processing
    result = stream.get();
    bool finishedParseCell = false;
    while (!finishedParseCell) {
      while ((stream.peek() != _escapeSymbol) && (stream.peek() != EOF)) {
        result += stream.get();
      }
       if (stream.peek() == EOF) {
         throw std::invalid_argument("Unclosed escaped symbols at line end");
       }
      stream.ignore(1);
      if (stream.peek() == '"') { //escaped quote
        result += stream.get();
      }
      else {
        result += _escapeSymbol;
        finishedParseCell = true;
      }
    }
  }
  else {
    stream >> result;
  }
  if (stream.peek() != _separator && _curCell.second < sizeof...(TArgs)) {
    throw std::invalid_argument("Unexpected symbol after value");
  }
  stream.ignore(1);
  while (stream.peek() == ' ') //ignore spaces after separator
    stream.ignore(1);
  ++_curCell.second;
  return result;
}

template <class Tuple, std::size_t... Is>
void printTuple(std::ostream& stream, const Tuple& t, std::index_sequence<Is...>)
{
  ((stream << (Is == 0 ? "" : " ") << std::get<Is>(t)), ...);
}

template <typename... TArgs>
std::ostream& operator<<(std::ostream& stream, const std::tuple<TArgs...> t)
{
  printTuple(stream, t, std::index_sequence_for<TArgs...>{});
  return stream;
}
