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
#include <boost/graph/dijkstra_shortest_paths.hpp>

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

typedef boost::property<boost::edge_weight_t, float> EdgeWeightProperty;
EdgeWeightProperty unit_weight(1);
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                              VertexProperty, EdgeWeightProperty> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_desc;
// Matrix of handles to graph vertices. (I don't know if there is a smarter
// way).
typedef std::vector<std::vector<vertex_desc>> vert_mat;

class elevation_graph {
  public:
  Graph g;
  vert_mat vertex_handles;
  vertex_desc start;
  vertex_desc end;
  
  void add_vertices (const elevation_map &emap);
  void add_endpoints (const elevation_map &emap);
  void add_edges (const elevation_map &emap);
  elevation_graph (const elevation_map &emap);
};

void elevation_graph::add_vertices (const elevation_map &emap) {
  // Add vertices to the graph (no edges yet).
  std::vector<vertex_desc> cur_vertex_row;
  for (int irow = 0; irow < emap.alts.rows(); irow++) {
    cur_vertex_row.clear();
    for (int icol = 0; icol < emap.alts.cols(); icol++) {
      vertex_desc node = add_vertex(g);
      cur_vertex_row.push_back(node);
      g[node].alt = emap.alts(irow, icol);
      g[node].position = pos(irow, icol);
    }
    vertex_handles.push_back(cur_vertex_row);
  }
}

void elevation_graph::add_endpoints (const elevation_map &emap) {
  // Add the start and end of the emap to the elevation_graph.
  start = vertex_handles.at(emap.start.row).at(emap.start.col);
  end = vertex_handles.at(emap.end.row).at(emap.end.col);
}

void elevation_graph::add_edges (const elevation_map &emap) {
  // Add the graph edges from the elevation rules.
  // Scan every cell and add edges to every neighbouring cell which
  // can be reached.
  // We could pad the emap.alts matrix with large ints on all sides
  // in order to avoid checking for bounds.
  for (int irow = 0; irow < emap.alts.rows(); irow++) {
    for (int icol = 0; icol < emap.alts.cols(); icol++) {
      int cur_alt = emap.alts(irow, icol);
      if (irow >= 1) {
        auto up_alt = emap.alts(irow - 1, icol);
        if (up_alt <= cur_alt + 1) {
          add_edge(vertex_handles.at(irow).at(icol),
                   vertex_handles.at(irow - 1).at(icol),
                   unit_weight, g);
        }
      }

      if (irow < emap.alts.rows() - 1) {
        auto down_alt = emap.alts(irow + 1, icol);
        if (down_alt <= cur_alt + 1) {
          add_edge(vertex_handles.at(irow).at(icol),
                   vertex_handles.at(irow + 1).at(icol),
                   unit_weight, g);
        }
      }

      if (icol >= 1) {
        auto left_alt = emap.alts(irow, icol - 1);
        if (left_alt <= cur_alt + 1) {
          add_edge(vertex_handles.at(irow).at(icol),
                   vertex_handles.at(irow).at(icol - 1),
                   unit_weight, g);
        }
      }

      if (icol < emap.alts.cols() - 1) {
        auto right_alt = emap.alts(irow, icol + 1);
        if (right_alt <= cur_alt + 1) {
          add_edge(vertex_handles.at(irow).at(icol),
                   vertex_handles.at(irow).at(icol + 1),
                   unit_weight, g);
        }
      }
    }
  }
}

elevation_graph::elevation_graph (const elevation_map &emap) {
  // elevation_graph constructor.
  /* Add all vertices from the elevation matrix */
  add_vertices(emap);
  add_endpoints(emap);
  add_edges(emap);
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

  /* Computing the shortest distance between start and all cells */
  std::vector<vertex_desc> parents(num_vertices(gmap.g));
  std::vector<int> distances(num_vertices(gmap.g));
  boost::dijkstra_shortest_paths(gmap.g, gmap.start,
    boost::predecessor_map(&parents[0]).distance_map(&distances[0]));
  std::cout << "Shortest path from start to end: "
            << distances[gmap.end] << std::endl;
}
