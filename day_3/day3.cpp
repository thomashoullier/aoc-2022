#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <utility>

typedef std::vector<std::string> strvec;

strvec read_from_file (std::ifstream &input) {
/* Read text file into a vector of strings. */
  strvec lines;
  std::string line;
  while (std::getline(input, line)) {
    lines.push_back(line);
  }
  return lines;
}

std::pair<strvec, strvec> split_into_halves (const strvec &full) {
/* Split the full vector into two vectors containing the first and
 * second halves of the strings */
  strvec first_half, second_half;
  for (auto str : full) {
    auto str_half_len = str.length() / 2;
    auto half1 = str.substr(0, str_half_len);
    auto half2 = str.substr(str_half_len, str_half_len);
    first_half.push_back(half1);
    second_half.push_back(half2);
  }
  return std::make_pair(first_half, second_half);
}

char common_char (const std::string &str1, const std::string &str2) {
/* Find the common character between two strings */
  auto sz = str1.find_first_of(str2);
  char incommon = str1.at(sz);
  return incommon;
}

std::vector<char> find_common_chars (const std::pair<strvec, strvec> &halves) {
/* Find the common character in halves */
  std::vector<char> common_chars;
  std::transform(halves.first.begin(), halves.first.end(), halves.second.begin(),
                 std::back_inserter(common_chars),
                 common_char);
  return common_chars;
}

int char_to_priority (const char &chr) {
/* Convert a character to its priority value */
  int chr_val = int(chr);
  int prio;
  if (chr_val >= 97 && chr_val <= 122) {
    prio = chr_val - 96;
  }
  else if (chr_val >= 65 && chr_val <= 90) {
    prio = chr_val - 65 + 27;
  }
  else {std::cerr << "Invalid character" << std::endl; return -1;}
  return prio;
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 3 #" << std::endl;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);
  auto lines = read_from_file(input);
  
  /* Splitting strings in two */
  auto halves = split_into_halves(lines);

  /* Finding the common characters */
  auto common_chars = find_common_chars(halves);

  /* Computing the priorities of each common character. */
  std::vector<int> priorities;
  std::transform(common_chars.begin(), common_chars.end(),
                 std::back_inserter(priorities),
                 char_to_priority);

  /* Total priority */
  auto total_priority = std::reduce(priorities.begin(), priorities.end());

  std::cout << "Total priority: " << total_priority << std::endl;
}
