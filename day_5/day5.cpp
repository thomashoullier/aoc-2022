#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <utility>
#include <regex>

/* Initial crate piles parsing */
typedef char crate;
typedef std::vector<crate> pile;
typedef std::vector<pile> piles;

std::regex int_regex("[0-9]+");

std::vector<crate> parse_crate_line (const std::string &line) {
/* Parse a text string describing a line of crates */
  std::vector<crate> crate_line;
  for (int char_pos = 1; char_pos < int(line.size()); char_pos += 4) {
    crate_line.push_back(line.at(char_pos));
  }
  return crate_line;
}

std::vector<std::string> to_crate_lines (std::ifstream &input) {
/* Return the vector of strings describing the crate lines.
 * From top to bottom. */
  std::string line;
  std::getline(input, line);
  std::vector<std::string> crate_lines;
  while (not line.empty()) {
    crate_lines.push_back(line);
    std::getline(input, line);
  }
  return crate_lines;
}

int count_piles (const std::string &index_line) {
/* Count the piles described by the pile indices text line.*/
  auto indices = std::sregex_iterator(index_line.begin(), index_line.end(),
                                      int_regex);
  auto indices_end = std::sregex_iterator();
  int npiles = std::distance(indices, indices_end);
  return npiles;
}

piles parse_init_piles (std::vector<std::string> &crate_lines, int npiles) {
/* Parse the crate piles as text lines from top to bottom.
 * Return piles from bottom to top. */
  std::reverse(crate_lines.begin(), crate_lines.end());
  piles init_piles;
  for (int ipile = 0; ipile < npiles; ipile++) {
    pile add_pile;
    init_piles.push_back(add_pile);
  }

  for (auto cur_line : crate_lines) {
    auto crate_line = parse_crate_line(cur_line);
    for (int ipile = 0; ipile < npiles; ipile++) {
      crate cur_crate = crate_line.at(ipile);
      if (not std::isspace(cur_crate)) {
        init_piles.at(ipile).push_back(cur_crate);
      }
    }
  }
  return init_piles;
}

piles parse_crate_piles (std::ifstream &input) {
/* Parse the initial crate piles. Leaves the input stream at the beginning
 * of the move lines.*/
  // Read the piles in consecutive strings vector.
  auto crate_lines = to_crate_lines(input);

  // Count the piles
  std::string index_line = crate_lines.back();
  crate_lines.pop_back();
  int npiles = count_piles(index_line);

  // Parse the piles
  piles init_piles = parse_init_piles(crate_lines, npiles);
  return init_piles;
}

/* Move lines parsing */
struct move {
  int n; // Number of crates to move.
  int from; // Pile to take the crates from.
  int to; // Pile to which the crates go.
};

typedef std::vector<move> moves;

int regex_match_to_int (const std::smatch &match) {
/* Convert a regex string match to an integer. */
  return std::stoi(match.str());
}

move parse_move (const std::string &line) {
/* Parse a move string to a move */
  move line_move;
  auto nums = std::sregex_iterator(line.begin(), line.end(), int_regex);
  line_move.n = regex_match_to_int(*nums); nums++;
  line_move.from = regex_match_to_int(*nums); nums++;
  line_move.to = regex_match_to_int(*nums);
  return line_move;
}

moves parse_moves (std::ifstream &input) {
/* Parse the moves text lines into moves. */
  moves moves_vec;
  std::string line;
  while(std::getline(input, line)) {
    moves_vec.push_back(parse_move(line));
  }
  return moves_vec;
}

/* Class for managing piles */
class crane {
  public:
    piles crate_piles; //Crate piles to process.

    crane (const piles init_piles);

    void do_move (const move &_move);

    int tallest_pile ();
    std::string print_at_height (int height);
    std::string print_indices ();
    std::string print_piles ();
    std::string report_top ();
};

crane::crane (piles init_piles) {
/* Crane constructor */
  crate_piles = init_piles;
}

void crane::do_move (const move &_move) {
/* Perform the provided move */
  int from_pile = _move.from - 1;
  int to_pile = _move.to - 1;
  for (int i = 0; i < _move.n; i++) {
    crate cur_crate = crate_piles.at(from_pile).back();
    crate_piles.at(from_pile).pop_back();
    crate_piles.at(to_pile).push_back(cur_crate);
  }
}

int crane::tallest_pile () {
/* Report the tallest pile height */
  auto max_index = std::max_element(crate_piles.begin(), crate_piles.end(),
                        [](pile pile1, pile pile2) {
                        return pile1.size() < pile2.size();});
  int max_height = crate_piles.at(std::distance(crate_piles.begin(), max_index))
                   .size();
  return max_height;
}

std::string crane::print_at_height (int height) {
/* Print the crate piles at given height as a string. */
  std::string piles_line;
  for (auto pile_i : crate_piles) {
    if (int(pile_i.size()) > height) {
      piles_line += (std::string(" [")
                   + pile_i.at(height)
                   + std::string("]"));
    } else {
      piles_line += "    ";
    }
  }
  return piles_line + "\n";
}

std::string crane::print_indices () {
/* Pile indices line which goes below the piles printing */
  std::string indices_line = "  ";
  int npiles = crate_piles.size();
  for (int i = 1; i <= npiles; i++) {
    indices_line += (std::to_string(i) + std::string("   "));
  }
  return indices_line + "\n";
}

std::string crane::print_piles () {
/* Print the current piles state as a string */
  int max_height = tallest_pile();
  std::string piles_str;
  for (int height = max_height-1; height>=0; height--) {
    piles_str += print_at_height(height);
  }
  piles_str += print_indices();
  return piles_str;
}

std::string crane::report_top () {
/* Report the crates that are on top of each pile. 
 * It is assumed there is at least a crate in each pile. */
  std::string top_crates;
  for (auto pile_i : crate_piles) {
    top_crates.push_back(pile_i.back());
  }
  return top_crates;
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 5 #" << std::endl;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);
  piles init_piles = parse_crate_piles(input);
  moves moves_vec = parse_moves(input);
  
  /* Instantiate the crane */
  crane crane_mover (init_piles);
  std::cout << "Initial piles:\n" << crane_mover.print_piles() << std::endl;

  for (auto m : moves_vec) { crane_mover.do_move(m); }

  std::cout << "Final piles:\n" << crane_mover.print_piles() << std::endl;
  std::cout << "Top crates: " << crane_mover.report_top() << std::endl;
}
