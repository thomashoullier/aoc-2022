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
typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> scenic_map;

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

int right_view_dist (int krow, int kcol, const height_mat &heights) {
/* Compute the view distance to the right of the tree at position (krow, kcol)*/
  int view_dist = 0;
  int kheight = heights(krow, kcol);
  int cur_height = -1;
  for (int icol = kcol + 1; (icol < heights.cols() and cur_height < kheight);
       icol++) {
    cur_height = heights(krow, icol);
    view_dist++;
  }
  return view_dist;
}

int left_view_dist (int krow, int kcol, const height_mat &heights) {
  int view_dist = 0;
  int kheight = heights(krow, kcol);
  int cur_height = -1;
  for (int icol = kcol - 1; (icol >= 0 and cur_height < kheight); icol--) {
    cur_height = heights(krow, icol);
    view_dist++;
  }
  return view_dist;
}

int bottom_view_dist (int krow, int kcol, const height_mat &heights) {
  int view_dist = 0;
  int kheight = heights(krow, kcol);
  int cur_height = -1;
  for (int irow = krow + 1; (irow < heights.rows() and cur_height < kheight);
       irow++) {
    cur_height = heights(irow, kcol);
    view_dist++;
  }
  return view_dist;
}

int top_view_dist (int krow, int kcol, const height_mat &heights) {
  int view_dist = 0;
  int kheight = heights(krow, kcol);
  int cur_height = -1;
  for (int irow = krow - 1; (irow >= 0 and cur_height < kheight);
       irow--) {
    cur_height = heights(irow, kcol);
    view_dist++;
  }
  return view_dist;
}

scenic_map compute_scenic_scores (const height_mat &heights) {
/* Compute the scenic score for each matrix cell. */
  scenic_map scenic_scores = scenic_map::Zero(heights.rows(), heights.cols());
  for (int krow = 0; krow < heights.rows(); krow++) {
    for (int kcol = 0;  kcol < heights.cols(); kcol++) {
      scenic_scores(krow, kcol) =
        right_view_dist(krow, kcol, heights)
      * left_view_dist(krow, kcol, heights)
      * bottom_view_dist(krow, kcol, heights)
      * top_view_dist(krow, kcol, heights);
    }
  }
  return scenic_scores;
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

  /* Computing the scenic score for every tree */
  auto scenic_scores = compute_scenic_scores(heights);
  std::cout << "Maximum scenic score: " << scenic_scores.maxCoeff()
            << std::endl;
}
