#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <utility>

typedef std::pair<int, int> range; // Individual range assigned to an elf.
typedef std::pair<range, range> elf_pair; // Ranges assigned to a pair of elves.
typedef std::vector<elf_pair> pad; // The complete input list.


/*** File parsing ***/
range parse_range (const std::string &range_str) {
/* Parse a single elf's range */
  // Split the string around the hyphen
  auto hyphen_pos = range_str.find_first_of('-');
  auto beg_str = range_str.substr(0, hyphen_pos);
  auto end_str = range_str.substr(hyphen_pos + 1);
  range sections = std::make_pair(std::stoi(beg_str), std::stoi(end_str));
  return sections;
}

elf_pair parse_pair (const std::string &line) {
/* Parse a single line: the ranges given to an elf pair */
  // Split the string around the comma.
  auto comma_pos = line.find_first_of(',');
  auto first_str = line.substr(0, comma_pos);
  auto second_str = line.substr(comma_pos + 1);
  range first_range = parse_range(first_str);
  range second_range = parse_range(second_str);
  elf_pair ranges = std::make_pair(first_range, second_range);
  return ranges;
}

pad read_from_file (std::ifstream &input) {
/* Parse the text file input */
  pad range_pairs;
  std::string line;  
  while (std::getline(input, line)) {
  elf_pair cur_elves = parse_pair(line);
  range_pairs.push_back(cur_elves);
  }
  return range_pairs;
}

/*** Solving part 1 ***/
bool range_contains_p (const range &container, const range &included) {
/* Is the provided range included in the container range? */
  return ((container.first <= included.first) and
          (container.second >= included.second));
}

bool inter_contain_p (const range &range1, const range &range2) {
/* Does range1 contain range2 or range2 contain range1? */
  return (range_contains_p(range1, range2) or range_contains_p(range2, range1));
}

bool pair_has_contain (const elf_pair &pair) {
/* Does the provided pair contain section ID ranges overlap? */
  return inter_contain_p(pair.first, pair.second);
}

/*** Solving part 2 ***/
bool pair_has_overlap (const elf_pair &pair) {
  auto range1 = pair.first;
  auto range2 = pair.second;
  return ((range1.first <= range2.second) and (range1.second >= range2.first));
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 4 #" << std::endl;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);

  pad range_pairs = read_from_file(input);

  /* Count the number of occurences of full containment */
  auto contain_count = std::count_if(range_pairs.begin(), range_pairs.end(),
                                     pair_has_contain);
  std::cout << "Number of pairs with full containment: "
            << contain_count << std::endl;

  /* Count the number of occurences of overlap */
  auto overlap_count = std::count_if(range_pairs.begin(), range_pairs.end(),
                                     pair_has_overlap);
  std::cout << "Number of pairs with overlap: "
            << overlap_count << std::endl;
}
