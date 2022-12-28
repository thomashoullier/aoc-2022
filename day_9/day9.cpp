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
  int steps = line.at(2) - '0';
  return move(dir, steps);
}

/* Head and Tail knots state class */
typedef std::unordered_map<pos, int, pos::hash_function> pos_counts_map;

class knots_state {
  public:
  pos head;
  pos tail;
  pos_counts_map visits;

  knots_state ();
  void process_move (const move &_move);
  
  private:
  pos move_to_pos (const move &_move); // Convert the move direction to a delta pos.
  void update_head (const pos &dpos); // Update the head position.
  void update_tail (); // Update the tail position.
  void note_visited (); // Update the tail visit map.
};

knots_state::knots_state () {
  head = pos(0, 0);
  tail = pos(0, 0);
  //TODO: tail visited 0,0
}

pos knots_state::move_to_pos (const move &_move) {
  switch (_move.dir) {
    case 'L': return pos(0, -1);
    case 'R': return pos(0, 1);
    case 'U': return pos(1, 0);
    case 'D': return pos(-1, 0);
    default: std::cerr << "Move direction unrecognized." << std::endl;
  }
}

knots_state::update_head (const pos &dpos) {
/* Update the head position by a single step */
  head.row += dpos.row;
  head.col += dpos.col;
}

knots_state::update_tail () {
/* Update the tail position in relation with the head position */

}

knots_state::note_visited () {
/* Update the visits map with the current tail position */

}

knots_state::process_move (const move &_move) {
/* Process a single move line */
  pos dpos = move_to_pos(_move); // Single update step.
  for (istep = 0; istep < _move.steps; istep++) {
    update_head(dpos);
    update_tail();
    note_visited();
  }
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 9#" << std::endl;

  std::unordered_map<pos, int, pos::hash_function> umap;
  umap[pos(3, 2)] = 10;
  umap[pos(2, 1)] = 11;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);
}
