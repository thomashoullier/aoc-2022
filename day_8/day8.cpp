#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <utility>

#include <Eigen/Core>

typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> height_mat;

std::vector<std::string> parse_input (std::ifstream &input) {
/* Parse the input file to a vector of line strings */
  std::vector<std::string> str_vec;
  std::string line;
  while(std::getline(input, line)) {
    str_vec.push_back(line);
  }
  return str_vec;
}

std::pair<int, int> input_dims (const std::vector<std::string> &str_vec) {
/* Assess the dimensions of the matrix described by the input file.
 * <rows, cols> */
  std::pair<int, int> dims;
  dims.first = str_vec.size();
  dims.second = str_vec.front().size();
  return dims;
}

height_mat parse_height_map (const std::vector<std::string> &str_vec) {
/* Parse height map from input strings vector. */
  height_mat heights;
  auto dims = input_dims(str_vec);
  heights = height_mat::Zero(dims.first, dims.second);
  int irow = 0;
  int icol = 0;
  for (auto line : str_vec) {
    icol = 0;
    for (auto chr : line) {
      int dig = chr - '0';
      heights(irow, icol) = dig;
      icol++;
    }
    irow++;
  }
  return heights;
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 8#" << std::endl;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);
  auto str_vec = parse_input(input);
  auto heights = parse_height_map(str_vec);
}
