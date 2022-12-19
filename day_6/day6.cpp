#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <utility>
#include <regex>
#include <deque>

bool buffer_ismarker (const std::deque<char> &buf) {
/* Check whether the buffer has non-repeating characters */
  auto test_buf(buf);
  std::sort(test_buf.begin(), test_buf.end());
  auto dups = std::unique(test_buf.begin(), test_buf.end());
  test_buf.erase(dups, test_buf.end());
  return (test_buf.size() == buf.size());
}

int find_marker (const std::string &input_line) {
/* Find the first sequence of 4 characters without repetition. */
  char first_char = input_line.front();
  std::deque<char> buf = {first_char, first_char, first_char, first_char};
  int marker_pos = 0;
  for (char c : input_line) {
    buf.push_back(c);
    buf.pop_front();
    marker_pos++;
    if (buffer_ismarker(buf)) { return marker_pos; }
  }
  return marker_pos;
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 6 Part 1#" << std::endl;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);
  std::string input_line;
  std::getline(input, input_line);

  /* Finding first marker */
  int marker_pos = find_marker(input_line);
  std::cout << "Input length: " << input_line.size() << std::endl;
  std::cout << "Marker position: " << marker_pos << std::endl;
}
