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
typedef boost::graph_traits<Graph> GraphTraits;

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

std::string parse_cd_target (const std::string &line) {
/* Return the folder name targetted by cd */
  return line.substr(5);
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

class graph_builder {
  public:
    Graph g;
    vertex_desc cur_dir;
    
    graph_builder ();
    void cd_toparent ();
    void cd_tochild (const std::string &name);
    void add_file (const std::string name, const int size);
    void add_dir (const std::string name);

    void parse_cd_line (const std::string &line);
    void parse_input_line (const std::string &line);
    void parse_output_line (const std::string &line);
    void process_line (const std::string &line);
};

graph_builder::graph_builder () {
/* Graph Builder constructor. */
  vertex_desc root = add_vertex(g);
  g[root].is_file = false;
  g[root].name = "/";
  cur_dir = root;
}

void graph_builder::cd_toparent () {
/* Move the current directory to the parent */
  typename GraphTraits::in_edge_iterator in_i, in_end;
  boost::tie(in_i, in_end) = in_edges(cur_dir, g);
  cur_dir = source(*in_i, g);
}

void graph_builder::cd_tochild (const std::string &name) {
/* Move to the child directory with given name. */
  typename GraphTraits::out_edge_iterator out_i, out_end; 
  boost::tie(out_i, out_end) = out_edges(cur_dir, g);
  auto find_res = std::find_if(out_i, out_end,
                               [this, name](GraphTraits::edge_descriptor e)
                               { 
                                std::cout << "Search: " << g[target(e, g)].name
                                << " vs. " << name << std::endl;
                                return g[target(e, g)].name == name; });
  // TODO: Bug, we reach the end of the vector. Try printing all the out_edges.
  std::cout << "Result distance to end: " << std::distance(out_end, find_res) << std::endl;
  vertex_desc cd = target(*find_res, g);
  std::cout << "cd to : " << g[cd].name << std::endl;
  cur_dir = cd;
}

void graph_builder::add_file (const std::string name, const int size) {
/* Add a file in the current directory. */
  vertex_desc file = add_vertex(g);
  g[file].is_file = true;
  g[file].name = name;
  g[file].size = size;
  add_edge(cur_dir, file, g);
}

void graph_builder::add_dir (const std::string name) {
/* Add a directory to the current directory */
  vertex_desc dir = add_vertex(g);
  g[dir].is_file = false;
  g[dir].name = name;
  std::cout << "Add dir: " << name << std::endl;
  std::cout << "Cur dir: " << g[cur_dir].name << std::endl;
  add_edge(cur_dir, dir, g);
  std::cout << "Dir added." << std::endl;
}

void graph_builder::parse_cd_line (const std::string &line) {
  std::string cd_name = parse_cd_target(line);
  std::cout << "cd target: " << cd_name << std::endl;
  if (cd_name == "..") { graph_builder::cd_toparent(); }
  else { graph_builder::cd_tochild(cd_name); }
}

void graph_builder::parse_input_line (const std::string &line) {
  if (input_is_cd(line)) { graph_builder::parse_cd_line(line); }
  else if (input_is_ls(line)) { return; /*Do nothing for ls lines.*/ }
  else { std::cerr << "Line not recognized." << std::endl; }
}

void graph_builder::parse_output_line (const std::string &line) {
  if (output_is_dir(line)) {
    std::string dir_name = parse_dir_name(line);
    graph_builder::add_dir(dir_name);
  }
  else if (output_is_file(line)) {
    auto file_data = parse_filedata(line);
    int filesize = file_data.first;
    std::string filename = file_data.second;
    graph_builder::add_file(filename, filesize);
  }
  else { std::cerr << "Line not recognized." << std::endl; }
}

void graph_builder::process_line (const std::string &line) {
/* Process one input line */
  if (is_input(line)) { graph_builder::parse_input_line(line); }
  else if (is_output(line)) { graph_builder::parse_output_line(line); }
  else { std::cerr << "Line not recognized." << std::endl; }
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 7#" << std::endl;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  graph_builder gb;
  std::cout << "Graph builder created." << std::endl;

  /* Parsing the input text */
  std::ifstream input(argv[1]);
  std::string line;
  std::getline(input, line); // Skip first line (root folder)
  while(std::getline(input, line)) {
    std::cout << line << std::endl;
    gb.process_line(line);
  }

  /* Minimal example for the graph usage.
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

  std::cout << "Find child file1 in v0 children" << std::endl;
  boost::tie(out_i, out_end) = out_edges(v0, g);
  auto find_res = std::find_if(out_i, out_end,
                               [g](GraphTraits::edge_descriptor e)
                               { return g[target(e, g)].name == "file1"; });
  vertex_desc found_vert = target(*find_res, g);
  std::cout << g[found_vert].name << std::endl;
  */

  /* Part 1 solution outline
 *   * Parse all folders and files into the graph.
 *     * Detect the type and subtype of lines: input line (cd or ls),
 *       output line (dir or file).
 *   * Iterate over all files (leaf nodes) and add their size to all of their
 *     parent nodes.
 *   * Iterate over folder nodes and accumulate the size of those
 *     with size at most X. */
}
