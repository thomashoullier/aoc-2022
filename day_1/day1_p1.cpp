#include <iostream>
#include <fstream>
#include <string>
#include <vector>

typedef std::vector<std::string> block; // Block of a single elf notes.
typedef std::vector<block> pad; // All of the elves' block.

int main (int argc, char *argv[]) {
  std::cout << "Day 1, part 1." << std::endl;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);
  
  pad elves_blocks;
  block cur_block;
  std::string line;
  while (std::getline(input, line)) {
    if (!line.empty()) {
      cur_block.push_back(line);
    } else {
      elves_blocks.push_back(cur_block);
      cur_block.clear();
    }
  }
  elves_blocks.push_back(cur_block);

  /* Text to numbers */
  // TODO: Map over the vectors. Use std::transform
  
  std::cout << elves_blocks.back().back() << std::endl;
}
