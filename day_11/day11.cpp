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
#include <queue>
#include <cmath>

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

void print_monkey_desc (const monkey_desc &mk) {
/* Print monkey_desc instance */
  std::cout << "<monkey_desc " << std::endl;
  std::cout << "Items: ";
  for (auto item : mk.items) { std::cout << item << " "; }
  std::cout << std::endl;
  std::cout << "Operation: " << mk.op << std::endl
            << "div_num: " << mk.div_num << std::endl
            << "true_monkey: " << mk.true_monkey << std::endl
            << "false_monkey: " << mk.false_monkey;
  std::cout << ">" << std::endl;
}

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

std::string parse_op (const std::string &op_line) {
/* Parse the op string in the op line */
  std::string::size_type pos = op_line.find("=");
  return op_line.substr(pos + 2);
}

int parse_integer (const std::string &line) {
/* Parse a single integer contained in the line. Helper method. */
  std::regex int_regex("[0-9]+");
  std::smatch match;
  if (std::regex_search(line, match, int_regex)) {
    std::string int_str = match.str();
    return std::stoi(int_str);
  } else {
    return -1;
  }
}

int parse_div (const std::string &div_line) {
/* Parse the div number in the div line. */
  return parse_integer(div_line);
}

int parse_true_monkey (const std::string &true_line) {
/* Parse the true monkey index */
  return parse_integer(true_line);
}

int parse_false_monkey (const std::string &false_line) {
/* Parse the false monkey index */
  return parse_integer(false_line);
}

monkey_desc parse_monkey_block (const std::string &monkey_block) {
/* Parse a monkey block into a monkey_desc struct */
  monkey_desc monkey_d;
  std::vector<std::string> lines = monkey_block_tolines(monkey_block);
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
}

/* Operation */
struct op {
  std::function<int (int, int)> fun; // Function to apply.
  std::vector<int> arg; // Argument, can be empty.
};

op parse_op_string (const std::string &op_str) {
/* Parse an operation string to an op struct */
  op operation;
  /* Operator matching */
  std::regex operator_regex("\\*|\\+");
  std::smatch match;
  std::regex_search(op_str, match, operator_regex);
  std::string fun_str = match.str();
  if (fun_str == "+") {
    operation.fun = std::plus<int>();
  } else if (fun_str == "*") {
    operation.fun = std::multiplies<int>();
  }
  else {
    std::cerr << "Operator not recognized" << std::endl;
  }
  /* Argument matching */
  int found_integer = parse_integer(op_str);
  if (found_integer != -1) {
    operation.arg.push_back(found_integer);
  }
  return operation;
}

class monkey { // Single monkey instance
  public:
  std::queue<int> items;
  op operation;
  int div_num;
  int true_monkey;
  int false_monkey;
  int inspect_count = 0; // Total number of inspections performed.

  monkey (const monkey_desc &mk);
  int inspect_next_item (); // Inspect the first item, changes its value
                            // and indicate to which monkey to throw it to.
  private:
  int do_op (const int &num);
  int test_item (const int &num);
};

monkey::monkey (const monkey_desc &mk) {
  for (auto item : mk.items) {
    items.push(item);
  }
  operation = parse_op_string(mk.op);
  div_num = mk.div_num;
  true_monkey = mk.true_monkey;
  false_monkey = mk.false_monkey;
}

int monkey::do_op (const int &num) {
// Apply the operation on item number.
  int second_arg;
  if (operation.arg.empty()) {
    second_arg = num;
  } else {
    second_arg = operation.arg.front();
  }
  int new_num = operation.fun(num, second_arg);
  return new_num;
}

int monkey::test_item (const int &num) {
// Test the current item value num and give the monkey target
  int monkey_target;
  if (num % div_num == 0) {
    monkey_target = true_monkey;
  } else {
    monkey_target = false_monkey;
  }
  return monkey_target;
}

int monkey::inspect_next_item () {
  items.front() = do_op(items.front());
  // The score is divided by 3 and floored.
  items.front() = int(std::floor(items.front() / 3.0));
  inspect_count++;
  return test_item(items.front());
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
  std::vector<std::string> monkey_blocks = text_to_monkey_blocks(text);
 
  /* Parse monkey descriptors */
  std::vector<monkey_desc> monkeys_desc;
  for (auto block : monkey_blocks) {
    monkeys_desc.push_back(parse_monkey_block(block));
  }

  /* Declare monkey instances */
  std::vector<monkey> monkeys;
  for (auto mk : monkeys_desc) {
    monkeys.push_back(monkey(mk));
  }

  /* Monkey inspects an item */
  std::cout << "First monkey throws first item to: "
            << monkeys.front().inspect_next_item() << std::endl;
}
