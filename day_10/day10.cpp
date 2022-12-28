#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <utility>

struct op {
  int duration;
  int value;

  op(int duration, int value) {
    this->duration = duration;
    this->value = value;
  }
};

op make_noop () {
/* Create a noop operation */
  return op(1, 0);
}

op make_addx (const int value) {
/* Make an addx operation of given value */
  return op(2, value);
}

op parse_op (const std::string &line) {
/* Parse an operation string into an op */
  std::string op_name = line.substr(0, 4);
  if (op_name == "noop") {
    return make_noop();
  } else if (op_name == "addx") {
    std::string op_value = line.substr(5);
    return make_addx(std::stoi(op_value));
  } else {
    std::cerr << "Unrecognized op string" << std::endl;
    return op(0, 0);
  }
}

class cycle_sim {
  public:
  int cycles = 0;
  int value = 1; // CRT sprite position.
  std::vector<int> value_at_cycle; // Value during cycle index.
  int cur_pix = 0; // Current CRT pixel position.
  std::string cur_crtline;
  std::vector<std::string> crt_lines;

  cycle_sim ();
  void do_op (const op &next_op);
  private:
  char draw_char();
  void update_cur_pix();
};

cycle_sim::cycle_sim () {
/* Initialize the value_at_cycle vector */
  value_at_cycle.push_back(value); // Value during cycle zero.
}

char cycle_sim::draw_char() {
/* Draw the current CRT character */
  if ((cur_pix >= value - 1) and (cur_pix <= value + 1)) {
    return '#';
  } else {
    return '.';
  }
}

void cycle_sim::update_cur_pix () {
/* Update the current pixel position (horizontal) */
  cur_pix++;
  if (cur_pix >= 40) {
    cur_pix = 0;
    crt_lines.push_back(cur_crtline);
    cur_crtline.clear();
  }
}

void cycle_sim::do_op (const op &next_op) {
/* Apply the next operation */
  for (int icyc = 0; icyc < next_op.duration; icyc++) {
    value_at_cycle.push_back(value);
    cur_crtline.push_back(draw_char());
    update_cur_pix();
    cycles++;
  }
  value += next_op.value;
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 10#" << std::endl;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  std::vector<op> ops;
  /* Parsing the input text */
  std::ifstream input(argv[1]);
  std::string line;
  while(std::getline(input, line)) {
    ops.push_back(parse_op(line));
  }

  /* Part 1: running the simulation */
  cycle_sim csim;
  for (auto o : ops) {
    csim.do_op(o);
  }

  std::vector<int> indices = {20, 60, 100, 140, 180, 220};
  std::cout << "Register value during cycles: " << std::endl;
  int sigsum = 0;
  for (auto i : indices) {
    int value = csim.value_at_cycle.at(i);
    std::cout << "Reg# " << i << ": " << value << std::endl;
    sigsum += i * value;
  }
  std::cout << "Total signal strength: " << sigsum << std::endl;

  /* Part2: printing the CRT scree */
  std::cout << "CRT screen:" << std::endl;
  for (auto s : csim.crt_lines) {
    std::cout << s << std::endl;
  }
}
