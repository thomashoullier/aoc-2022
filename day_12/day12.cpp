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

#include <Eigen/Core>
#include <boost/functional/hash.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

/* Position struct: position on the map. */
struct pos {
  int row;
  int col;

  pos(int row, int col) {
    this->row = row;
    this->col = col;
  }

  pos() {
    this->row = 0;
    this->col = 0;
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

/* Elevation matrix map. */
typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> elevations;

struct elevation_map {
  elevations alts;
  pos start;
  pos end;
};

int parse_char_to_elevation (const char &chr) {
  // Parse a char to an elevation value.
  // a is 0, b is 1, z is 25.
  // S is 0, E is 25.
  switch(chr) {
  case 'S':
    return 0; break;
  case 'E':
    return 25; break;
  default:
    int val = chr - 'a'; // a-z is consecutive in char type.
    return val;
  }
}

typedef std::vector<std::string> lines_vec;

lines_vec parse_input (std::ifstream &input) {
  // Parse input text into a vector of string lines.
  std::string line;
  lines_vec lines;
  while (std::getline(input, line)) {
    lines.push_back(line);
  }
  return lines;
}

elevation_map parse_toemap (const lines_vec &lines) {
  // Parse the input text into an elevation_map.
  elevation_map emap;
  /* Determine the matrix dimensions */
  int nrows = lines.size();
  int ncols = lines.front().size();
  emap.alts = elevations::Zero(nrows, ncols);
  /* Filling the emap */
  int irow = 0;
  for (auto line : lines) {
    int icol = 0;
    for (char chr : line) {
      if (chr == 'S') { emap.start = pos(irow, icol); }
      if (chr == 'E') { emap.end = pos(irow, icol); }
      emap.alts(irow, icol) = parse_char_to_elevation(chr);
      icol++;
    }
    irow++;
  }
  return emap;
}

/* Moves graph */
struct VertexProperty {
  int alt; // Altitude of the vertex.
  pos position; // Position of the vertex on the map.
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                              VertexProperty> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_desc;

class elevation_graph {
  public:
  Graph g;
  vertex_desc start;
  vertex_desc end;
  
  void add_vertices (const elevation_map &emap);
  elevation_graph (const elevation_map &emap);
};

void elevation_graph::add_vertices (const elevation_map &emap) {
  // Add vertices to the graph (no edges yet).
  for (int irow = 0; irow < emap.alts.rows(); irow++) {
    for (int icol = 0; icol < emap.alts.cols(); icol++) {
      vertex_desc node = add_vertex(g);
      g[node].alt = emap.alts(irow, icol);
      g[node].position = pos(irow, icol);
      // Mark the start and end nodes.
      if (g[node].position == emap.start) { start = node; }
      if (g[node].position == emap.end) { end = node; }
    }
  }
}

//void elevation_graph::add_edges () {
  // Add the graph edges from the elevation rules.
  // TODO: Use the existing vertices indexing.
//}

elevation_graph::elevation_graph (const elevation_map &emap) {
  // elevation_graph constructor.
  /* Add all vertices from the elevation matrix */
  add_vertices(emap);
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 12#" << std::endl;

  /* Part 1 outline:
 * Parse the input text into a numerical elevation matrix.
 * Create a graph of possible moves between cells. This can
   be done by iterating through every cell in the matrix.
   The graph nodes are identified by their indexing in the
   matrix.
 * Use a graph algorithm to find the minimal path length between
   start and finish nodes. */

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);
  auto lines = parse_input(input);
  auto emap = parse_toemap(lines);

  std::cout << "Altitude map: " << std::endl
            << "Start: " << emap.start.row << ", " << emap.start.col << std::endl
            << "End: " << emap.end.row << ", " << emap.end.col << std::endl;
  for (int irow = 0; irow < emap.alts.rows(); irow++) {
    for (int icol = 0; icol < emap.alts.cols(); icol++) {
      std::cout << emap.alts(irow, icol);
    }
    std::cout << std::endl;
  }

  /* Creating the graph of possible moves between grid cells */
  auto gmap = elevation_graph(emap);
}
