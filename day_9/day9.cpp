#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <utility>

#include <boost/functional/hash.hpp>

/* Position structure */
struct pos {
  int row;
  int col;

  pos(int row, int col) {
    this->row = row;
    this->col = col;
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

/* Move struct */
struct move {
  char dir;
  int steps;

  move(char dir, int steps) {
    this->dir = dir;
    this->steps = steps;
  }
};

move parse_moveline (const std::string &line) {
/* Parse a move string */
  char dir = line.at(0);
  int steps = std::stoi(line.substr(2));
  return move(dir, steps);
}

/* Head and Tail knots state class */
typedef std::unordered_map<pos, int, pos::hash_function> pos_counts_map;

class knots_state {
  public:
  pos head = pos(0, 0);
  pos tail = pos(0, 0);
  pos_counts_map visits;

  knots_state ();
  void process_move (const move &_move);
  
  private:
  pos move_to_pos (const move &_move); // Convert the move direction to a delta pos.
  bool tail_mustmove ();
  void update_head (const pos &dpos); // Update the head position.
  void update_tail (); // Update the tail position.
  void note_visited (); // Update the tail visit map.
};

knots_state::knots_state () {
  note_visited();
}

pos knots_state::move_to_pos (const move &_move) {
  switch (_move.dir) {
    case 'L': return pos(0, -1); break;
    case 'R': return pos(0, 1); break;
    case 'U': return pos(1, 0); break;
    case 'D': return pos(-1, 0); break;
    default:
      std::cerr << "Move direction unrecognized." << std::endl;
      return pos(0,0);
  }
}

void knots_state::update_head (const pos &dpos) {
/* Update the head position by a single step */
  head.row += dpos.row;
  head.col += dpos.col;
}

bool knots_state::tail_mustmove () {
/* Indicate whether the tail position must move */
  int drow = std::abs(head.row - tail.row);
  int dcol = std::abs(head.col - tail.col);
  int dmax = std::max(drow, dcol);
  return (dmax >= 2);
}

void knots_state::update_tail () {
/* Update the tail position in relation with the head position */
  if (tail_mustmove()) {
    int drow = head.row - tail.row;
    int dcol = head.col - tail.col;
    if (drow != 0) { tail.row += drow / std::abs(drow); }
    if (dcol != 0) { tail.col += dcol / std::abs(dcol); }
  }
}

void knots_state::note_visited () {
/* Update the visits map with the current tail position */
  visits[tail]++;
}

void knots_state::process_move (const move &_move) {
/* Process a single move line */
  pos dpos = move_to_pos(_move); // Single update step.
  for (int istep = 0; istep < _move.steps; istep++) {
    update_head(dpos);
    update_tail();
    note_visited();
  }
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 9#" << std::endl;

  knots_state ks;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);
  std::string line;
  while (std::getline(input, line)) {
    move cur_move = parse_moveline(line);
    ks.process_move(cur_move);
  }

  std::cout << "Number of locations visited by tail: "
            << ks.visits.size() << std::endl;
}
