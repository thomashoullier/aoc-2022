#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <utility>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

/* Graph */
struct VertexProperty {
  bool is_file; // true for files (leaf nodes), false for folders.
  int size = 0; // File size
  std::string name; // File or folder names.
};

typedef boost::adjacency_list<boost::listS, boost::listS,
                              boost::bidirectionalS, VertexProperty> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_desc;
typedef boost::graph_traits<Graph>::vertex_iterator VItr;

/* Input line types detection */
bool is_input (const std::string &line) {
/* Is the line an input line? Output line otherwise. */
  return line.front() == '$';
}

bool is_output (const std::string &line) {
  return !is_input(line);
}

bool input_is_cd (const std::string &line) {
/* Is the input line a cd line? ls line otherwise.*/
  return line.at(2) == 'c';
}

bool input_is_ls (const std::string &line) {
  return !input_is_cd(line);
}

bool output_is_dir (const std::string &line) {
/* Is the output line a dir line? */
  return line.front() == 'd';
}

bool output_is_file (const std::string &line) {
  return !output_is_dir(line);
}

/* Parsing lines */
void parse_input_line (const std::string &line) {
  std::cout << "Input line";
}

void parse_output_line (const std::string &line) {
  std::cout << "Output line";
}

std::string parse_cd_target (const std::string &line) {
/* Return the folder name targetted by cd */
  return line.substr(3);
}

std::string parse_dir_name (const std::string &line) {
/* Return the folder name contained in the dir line. */
  return line.substr(4);
}

std::pair<int, std::string> parse_filedata (const std::string &line) {
/* Parse the filesize and filename of the file in line */
  std::pair<int, std::string> filedata;
  std::string::size_type delim_pos = line.find_first_of(' ');
  std::string filesize_str = line.substr(0, delim_pos);
  int filesize = std::stoi(filesize_str);
  std::string filename = line.substr(delim_pos + 1);
  filedata.first = filesize;
  filedata.second = filename;
  return filedata;
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 7#" << std::endl;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);
  /*std::string line;
  while(std::getline(input, line)) {
    if (is_input(line)) { parse_input_line(line); }
    else if (is_output(line)) { parse_output_line(line); }
    else { std::cerr << "Line not recognized." << std::endl; }
  }*/

  Graph g;
  
  vertex_desc v0 = add_vertex(g);
  vertex_desc v1 = add_vertex(g);
  vertex_desc v2 = add_vertex(g);

  g[v0].is_file = false;
  g[v0].name = "/";
  
  g[v1].is_file = true;
  g[v1].name = "file1";

  g[v2].is_file = true;
  g[v2].name = "file2";

  add_edge(v0, v1, g);
  add_edge(v0, v2, g);

  VItr vitr, vend;
  
  boost::tie(vitr, vend) = vertices(g);
  for ( ; vitr != vend ; ++vitr)
    std::cout << g[*vitr].name << std::endl;

  typedef boost::graph_traits<Graph> GraphTraits;

  typename GraphTraits::in_edge_iterator in_i, in_end;
  std::cout << "v2 in_edges: ";
  for (boost::tie(in_i, in_end) = in_edges(v2,g); in_i != in_end; ++in_i) {
    GraphTraits::edge_descriptor e = *in_i;
    vertex_desc src = source(e, g), targ = target(e, g);
    std::cout << "(" << g[src].name << ", " << g[targ].name << ") ";
  }
  std::cout << std::endl;

  typename GraphTraits::out_edge_iterator out_i, out_end; 
  std::cout << "v0 out_edges: ";
  for (boost::tie(out_i, out_end) = out_edges(v0, g);
       out_i != out_end; ++out_i){
    GraphTraits::edge_descriptor e = *out_i;
    vertex_desc src = source(e, g), targ = target(e, g);
    std::cout << "(" << g[src].name << ", " << g[targ].name << ") ";
  }
  std::cout << std::endl;

  //TODO: * Find the vertex with given name in the v0 out_edges.

  /* Part 1 solution outline
 *   * Parse all folders and files into the graph.
 *     * Detect the type and subtype of lines: input line (cd or ls),
 *       output line (dir or file).
 *   * Iterate over all files (leaf nodes) and add their size to all of their
 *     parent nodes.
 *   * Iterate over folder nodes and accumulate the size of those
 *     with size at most X. */
}
