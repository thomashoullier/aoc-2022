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
#include <boost/graph/topological_sort.hpp>

/* Graph */
struct VertexProperty {
  bool is_file; // true for files (leaf nodes), false for folders.
  int size = 0; // File size
  std::string name; // File or folder names.
};

typedef boost::adjacency_list<boost::vecS, boost::vecS,
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
                               { return g[target(e, g)].name == name; });
  cur_dir = target(*find_res, g);
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
  add_edge(cur_dir, dir, g);
}

void graph_builder::parse_cd_line (const std::string &line) {
  std::string cd_name = parse_cd_target(line);
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

/* Graph for computing and assessing folder sizes */
class graph_sizes {
public:
  Graph g;
  std::vector<vertex_desc> topo_dirs;
  
  graph_sizes (Graph _g);
  
  void sort_topo ();
  void compute_dirs_size ();
  void compute_contents_size (const vertex_desc &folder);
};

graph_sizes::graph_sizes (Graph _g) {
/* Graph Sizes constructor */
  g = _g;
}

void graph_sizes::sort_topo () {
/* Sort the graph vertices in topological order */
  std::vector<vertex_desc> topo_nodes;
  topological_sort(g, std::back_inserter(topo_nodes));
  std::copy_if(topo_nodes.begin(), topo_nodes.end(),
               std::back_inserter(topo_dirs),
               [this](vertex_desc v){return !g[v].is_file;});
}

void graph_sizes::compute_contents_size (const vertex_desc &folder) {
/* Compute the size of the current folder's contents. */
  typename GraphTraits::out_edge_iterator out_i, out_end; 
  boost::tie(out_i, out_end) = out_edges(folder, g);
  int size = std::accumulate(out_i, out_end, 0,
    [this](int cur_sum, GraphTraits::edge_descriptor e)
          {return cur_sum + g[target(e, g)].size;});
  g[folder].size = size;
}

void graph_sizes::compute_dirs_size () {
/* Compute the size of all dirs in the tree */
  for (auto folder : topo_dirs) {
    compute_contents_size(folder);
  }
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 7#" << std::endl;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Part 1 solution outline
 *   * Parse all folders and files into the graph.
 *     * Detect the type and subtype of lines: input line (cd or ls),
 *       output line (dir or file).
 *   * Perform a topological sort of the directories in the graph.
 *     Iterate from the bottom to the top, compute the size of
 *     the directory's contents each time.
 *   * Iterate over folder nodes and accumulate the size of those
 *     with size at most X. */

  /* Parsing the input text */
  graph_builder gb;

  std::ifstream input(argv[1]);
  std::string line;
  std::getline(input, line); // Skip first line (root folder)
  while(std::getline(input, line)) {
    gb.process_line(line);
  }

  /* Computing folder sizes */
  graph_sizes gs (gb.g);
  gs.sort_topo();
  gs.compute_dirs_size();
  /*
  for (auto vert : gs.topo_dirs) {
    std::cout << gs.g[vert].name << " " << gs.g[vert].size << std::endl;
  }
  */
  /* Finding all folders with size at most 100k */
  std::vector<vertex_desc> small_folders;
  std::copy_if(gs.topo_dirs.begin(), gs.topo_dirs.end(),
               std::back_inserter(small_folders),
               [gs](vertex_desc v){return gs.g[v].size <= 100000;});
  /* Summing the size of all of the small folders */
  int small_folders_sum = 
    std::accumulate(small_folders.begin(), small_folders.end(), 0,
    [gs](int cur_sum, vertex_desc v)
        {return cur_sum + gs.g[v].size;});
  std::cout << "Total size of folders of size at most 100k: "
            << small_folders_sum << std::endl;

  /* Part 2 */
  /* Total used space */
  int fs_space = 70000000;
  int used_space = gs.g[gs.topo_dirs.back()].size;
  int free_space = fs_space - used_space;
  int target_free_space = 30000000;
  int space_to_free = target_free_space - free_space;
  std::cout << "Used space: " << used_space << std::endl;
  std::cout << "Space to free: " << space_to_free << std::endl;
  /* Folders big enough to delete */
  std::vector<vertex_desc> big_folders;
  std::copy_if(gs.topo_dirs.begin(), gs.topo_dirs.end(),
               std::back_inserter(big_folders),
               [gs, space_to_free](vertex_desc v)
               {return gs.g[v].size >= space_to_free;});
  /* Find the smallest of these folders */
  std::sort(big_folders.begin(), big_folders.end(),
            [gs](vertex_desc a, vertex_desc b)
            {return gs.g[a].size < gs.g[b].size;});
  std::cout << "Smallest directory size for deletion: "
            << gs.g[big_folders.front()].size << std::endl;
}
