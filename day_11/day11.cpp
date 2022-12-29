#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <utility>
#include <regex>

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
  std::string::size_type block_end = 0;
  std::vector<std::string> monkey_blocks;
  while (block_end != std::string::npos) {
    block_beg = block_end;
    block_end = text.find(delim, block_beg+1);
    cur_block = text.substr(block_beg, block_end - block_beg);
    boost::trim(cur_block);
    monkey_blocks.push_back(cur_block);
  }
  return monkey_blocks;
}

/* Describing a monkey */
struct monkey_desc {
  std::vector<int> items;
  std::string op;
  int div_num; // Number for divisibility test.
  int true_monkey; // Index of the monkey to throw to in case of true.
  int false_monkey; // false case.
};

std::vector<std::string> monkey_block_tolines
                         (const std::string &monkey_block) {
/* Cut the monkey block into individual lines */
  char delim = '\n';
  std::string::size_type line_beg = 0;
  std::string::size_type line_end = 0;
  std::vector<std::string> lines;
  std::string line;
  while (line_end != std::string::npos) {
    line_beg = line_end;
    line_end = monkey_block.find(delim, line_beg+1);
    line = monkey_block.substr(line_beg, line_end - line_beg);
    boost::trim(line);
    lines.push_back(line);
  }
  return lines;
}

std::vector<int> parse_items (const std::string &items_line) {
/* Return the items integers contained in the items line */
  std::regex items_regex("[0-9]+");
  auto items_it = std::sregex_iterator(items_line.begin(),
                                       items_line.end(),
                                       items_regex);
  auto it_end = std::sregex_iterator();
  std::vector<int> items;
  for (auto i = items_it; i != it_end; ++i) {
    items.push_back(std::stoi((*i).str()));
  }
  return items;
}

//std::string parse_op (const std::string &op_line) {
/* Parse the op string in the op line */

//}

//int parse_div (const std::string &div_line) {
/* Parse the div number in the div line. */

//}

//int parse_true_monkey (const std::string &true_line) {
/* Parse the true monkey index */

//}

//int parse_false_monkey (const std::string &false_line) {
/* Parse the false monkey index */

//}

/*monkey_desc parse_monkey_block (const std::string &monkey_block) {
/ Parse a monkey block into a monkey_desc struct /
  monkey_desc monkey_d;
  std::vector<string> lines = monkey_block_to_lines(monkey_block);
  std::string items_line = lines.at(1);
  std::string op_line = lines.at(2);
  std::string div_line = lines.at(3);
  std::string true_line = lines.at(4);
  std::string false_line = lines.at(5);

  monkey_d.items = parse_items(items_line);
  monkey_d.op = parse_op(op_line);
  monkey_d.div_num = parse_div(div_line);
  monkey_d.true_monkey = parse_true_monkey(true_line);
  monkey_d.false_monkey = parse_false_monkey(false_line);
  return monkey_d;
}*/

int main (int argc, char *argv[]) {
  std::cout << "# Day 11#" << std::endl;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);
  std::string text = parse_file_to_string(input);
  std::vector<std::string> monkey_blocks = text_to_monkey_blocks(text);
  
  std::cout << "First monkey block: " << std::endl << monkey_blocks.front()
            << std::endl << "Last monkey block: " << std::endl
            << monkey_blocks.back() << std::endl;

  /* Monkey block to lines */
  std::vector<std::string> monkey_lines = 
    monkey_block_tolines(monkey_blocks.at(2));
  for (auto l : monkey_lines) {
    std::cout << "Line #" << std::endl;
    std::cout << l << std::endl;
  }
  std::string items_line = monkey_lines.at(1);
  auto items = parse_items(items_line);
  for (auto it : items) {
    std::cout << it << " ";
  } std::cout << std::endl;
}
