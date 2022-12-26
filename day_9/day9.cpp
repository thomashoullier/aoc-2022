#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <utility>

#include <boost/functional/hash.hpp>

struct pos {
  int row;
  int col;

  pos(int row, int col) {
    this->row = row;
    this->col = col;
  }

  bool operator==(const pos &pos2) const {
    return (this->row == pos2.row and this->col == pos2.col);
  }

  struct hash_function {
    size_t operator()(const pos &_pos) const {
      std::size_t hash = 0;
      boost::hash_combine(hash,boost::hash_value(_pos.row));
      boost::hash_combine(hash,boost::hash_value(_pos.col));
      return hash;
    }
  };
};

int main (int argc, char *argv[]) {
  std::cout << "# Day 9#" << std::endl;

  std::unordered_map<pos, int, pos::hash_function> umap;
  umap[pos(3, 2)] = 10;
  umap[pos(2, 1)] = 11;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);
}
