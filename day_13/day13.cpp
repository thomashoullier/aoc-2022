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
#include <limits>

/* Parsing input text */
typedef std::pair<std::string, std::string> pair_str;
typedef std::vector<pair_str> pairs_str;

pair_str parse_pair_str (const std::string line1, const std::string line2) {
  // Parse two strings to a pair of strings.
  pair_str p{line1, line2};
  return p;
}

pairs_str parse_pairs_str (std::ifstream &input) {
  // Read the input file to pairs_str
  pairs_str pairs;
  std::string line1, line2, dum;
  for (std::string line1, line2, dum ;
       std::getline(input, line1) && std::getline(input, line2)
       && std::getline(input, dum) ; ) {
    pair_str p = parse_pair_str(line1, line2);
    pairs.push_back(p);
  }
  return pairs;
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 13#" << std::endl;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);
  auto pstrs = parse_pairs_str(input);
  
  std::cout << pstrs.at(10).first << std::endl << pstrs.at(10).second
            << std::endl;

  // [[4,3],[2]]
  
  // We can probably do a recursive function to parse these strings.
  // We also need:
  //   * Find closing bracket.
  
}
