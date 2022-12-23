#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <utility>

#include <boost/graph/adjacency_list.hpp>

struct VertexProperty {
  bool is_file; // true for files (leaf nodes), false for folders.
  int size = 0; // File size
  std::string name; // File or folder names.
};


int main (int argc, char *argv[]) {
  std::cout << "# Day 7#" << std::endl;

  /*if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }*/

  /* Parsing the input text */
  // std::ifstream input(argv[1]);
  
  typedef boost::adjacency_list<boost::listS, boost::listS,
                                boost::bidirectionalS, VertexProperty> Graph;
  typedef boost::graph_traits<Graph>::vertex_descriptor vertex_desc;

  Graph g;
  
  vertex_desc v0 = add_vertex(g);
  vertex_desc v1 = add_vertex(g);

  g[v0].is_file = false;
  g[v0].name = "/";
  
  g[v1].is_file = true;
  g[v1].name = "file1";

  add_edge(v0, v1, g);

  typedef boost::graph_traits<Graph>::vertex_iterator VItr;
  VItr vitr, vend;
  
  boost::tie(vitr, vend) = vertices(g);
  for ( ; vitr != vend ; ++vitr )
    std::cout << g[*vitr].name << std::endl;
}
