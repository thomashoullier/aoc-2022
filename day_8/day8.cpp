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
typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> visibility_map;

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

void probe_left_side (const height_mat &heights, visibility_map &visibs) {
/* Mark the trees visible from the left side of the forest. */
  for (int irow = 0; irow < heights.rows(); irow++) {
    int max_height = -1;
    for (int icol = 0; (icol < heights.cols() and max_height < 9); icol++) {
      int cur_height = heights(irow, icol);
      if (cur_height > max_height) {
        max_height = cur_height;
        visibs(irow, icol) = 1;
      }
    }
  }
}

void probe_right_side (const height_mat &heights, visibility_map &visibs) {
  for (int irow = 0; irow < heights.rows(); irow++) {
    int max_height = -1;
    for (int icol = heights.cols() - 1;
         (icol >= 0 and max_height < 9); icol--) {
      int cur_height = heights(irow, icol);
      if (cur_height > max_height) {
        max_height = cur_height;
        visibs(irow, icol) = 1;
      }
    }
  }
}

void probe_top_side (const height_mat &heights, visibility_map &visibs) {
  for (int icol = 0; icol < heights.cols(); icol++) {
    int max_height = -1;
    for (int irow = 0; (irow < heights.rows() and max_height < 9); irow++) {
      int cur_height = heights(irow, icol);
      if (cur_height > max_height) {
        max_height = cur_height;
        visibs(irow, icol) = 1;
      }
    }
  }
}

void probe_bottom_side (const height_mat &heights, visibility_map &visibs) {
  for (int icol = 0; icol < heights.cols(); icol++) {
    int max_height = -1;
    for (int irow = heights.rows() - 1;
         (irow >= 0 and max_height < 9); irow--) {
      int cur_height = heights(irow, icol);
      if (cur_height > max_height) {
        max_height = cur_height;
        visibs(irow, icol) = 1;
      }
    }
  }
}

visibility_map compute_visibility_map (const height_mat &heights) {
/* Compute the visibility map corresponding to the tree heights matrix */
  visibility_map visibs;
  visibs = visibility_map::Constant(heights.rows(), heights.cols(), 0);
  // We probe the four sides of the matrix with an eye. We mark trees
  // as visible from the edges inward.
  probe_left_side(heights, visibs);
  probe_right_side(heights, visibs);
  probe_top_side(heights, visibs);
  probe_bottom_side(heights, visibs);
  return visibs;
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

  /* Probing for visible trees */
  auto visibs = compute_visibility_map(heights);
  
  /* Counting visible trees */
  int nvis = visibs.sum();
  std::cout << "Visible trees: " << nvis << std::endl;
}
