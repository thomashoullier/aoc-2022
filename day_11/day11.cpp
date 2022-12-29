#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <utility>

#include <boost/algorithm/string.hpp>

/* Text file parsing */
std::string parse_file_to_string (std::ifstream &input) {
    std::ostringstream string_stream;
    string_stream << input.rdbuf();
    return string_stream.str();
}

std::vector<std::string> text_to_monkey_blocks (std::string &text) {
/* Parse the text string to individual monkey paragraphs */
  std::string delim = "\n\n";
  std::string cur_block;
  std::string::size_type block_beg = 0;
  std::string::size_type block_end = -2;
  std::vector<std::string> monkey_blocks;
  while (block_end != std::string::npos) {
    block_beg = block_end+2;
    block_end = text.find(delim, block_beg+1);
    std::cout << block_end << std::endl;
    cur_block = text.substr(block_beg, block_end);
    boost::trim(cur_block);
    monkey_blocks.push_back(cur_block);
  }
  return monkey_blocks;
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 11#" << std::endl;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);
  std::string text = parse_file_to_string(input);
  std::cout << text << std::endl;
  std::vector<std::string> monkey_blocks = text_to_monkey_blocks(text);
  
  std::cout << "First monkey block: " << std::endl << monkey_blocks.front()
            << std::endl << "Last monkey block: " << std::endl
            << monkey_blocks.back() << std::endl;
}
