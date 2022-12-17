#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>

typedef std::vector<std::string> block;
typedef std::vector<block> pad;

pad parse_blocks (std::ifstream &input) {
/* Parse a stream containing successive blocks of strings separated by empty
 * lines. Strings are put in ragged vectors.*/
  pad blocks;
  block cur_block;
  std::string line;
  while (std::getline(input, line)) {
    if (!line.empty()) {
      cur_block.push_back(line);
    } else {
      blocks.push_back(cur_block);
      cur_block.clear();
    }
  }
  blocks.push_back(cur_block);
  return blocks;
}

typedef std::vector<int> iblock; // Block of ints.
typedef std::vector<iblock> ipad; // Pad of iblock.

iblock parse_to_iblock (const block &cur_block) {
/* Parse a block of strings into a block of ints */
  iblock new_block;
  std::transform(cur_block.begin(), cur_block.end(),
                 std::back_inserter(new_block),
                 [](const std::string &s){return std::stoi(s);});
  return new_block;
}

ipad parse_to_ipad (const pad &blocks) {
/* Parse a pad containing blocks of strings to a pad containing blocks
 *   of ints. */
  ipad iblocks;
  iblocks.resize(blocks.size());
  std::transform(blocks.begin(), blocks.end(), iblocks.begin(),
                 parse_to_iblock);
  return iblocks;
}

int main (int argc, char *argv[]) {
  std::cout << "Day 1, part 1." << std::endl;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);
  
  pad elves_blocks = parse_blocks(input);
  ipad elves_counts = parse_to_ipad(elves_blocks);

  /* Tallying the elves calories */
  std::vector<int> elves_calories;
  std::transform(elves_counts.begin(), elves_counts.end(),
                 std::back_inserter(elves_calories),
                 [](const iblock &lines)
                   {return std::reduce(lines.begin(), lines.end());});

  /* Finding the maximum calories among elves */
  auto max_cals = *std::max_element(elves_calories.begin(),
                                    elves_calories.end());
  
  std::cout << "Maximum calories among elves is " << max_cals << std::endl;

  /* Sorting calories counts */
  std::vector<int> sorted_calories(elves_calories);
  std::sort(sorted_calories.begin(), sorted_calories.end(),
            std::greater<int>());
  auto sum_of_topthree = std::reduce(sorted_calories.begin(),
                                     sorted_calories.begin() + 3);
  
  std::cout << "The top three calories counts amount to a total of "
            << sum_of_topthree << std::endl;
}
