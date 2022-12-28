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

class rope {
  public:
  // The front knot is the head. The back knot is the tail.
  std::vector<pos> knots;
  pos_counts_map visits;

  std::vector<pos> init_knots ();

  rope ();
  void process_move (const move &_move);
  
  private:
  pos move_to_pos (const move &_move); // Convert the move direction to a delta pos.
  bool knot_mustmove (const int iknot);
  void update_head (const pos &dpos); // Update the head position.
  void update_knot (const int iknot); // Update the tail position.
  void note_visited (); // Update the tail visit map.
};

std::vector<pos> rope::init_knots () {
/* Initial set of knots */
  std::vector<pos> initial_knots;
  for (int iknot = 0; iknot < 10; iknot++) {
    initial_knots.push_back(pos());
  }
  return initial_knots;
}

rope::rope () {
  knots = init_knots();
  note_visited();
}

pos rope::move_to_pos (const move &_move) {
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

void rope::update_head (const pos &dpos) {
/* Update the head position by a single step */
  knots.front().row += dpos.row;
  knots.front().col += dpos.col;
}

bool rope::knot_mustmove (const int iknot) {
/* Indicate whether the knot at position iknot must move. */
  int drow = std::abs(knots.at(iknot - 1).row - knots.at(iknot).row);
  int dcol = std::abs(knots.at(iknot - 1).col - knots.at(iknot).col);
  int dmax = std::max(drow, dcol);
  return (dmax >= 2);
}

void rope::update_knot (const int iknot) {
/* Update the knot at position iknot in relation with the
 * preceding knot. */
  if (knot_mustmove(iknot)) {
    int drow = knots.at(iknot - 1).row - knots.at(iknot).row;
    int dcol = knots.at(iknot - 1).col - knots.at(iknot).col;
    if (drow != 0) { knots.at(iknot).row += drow / std::abs(drow); }
    if (dcol != 0) { knots.at(iknot).col += dcol / std::abs(dcol); }
  }
}

void rope::note_visited () {
/* Update the visits map with the current tail position */
  visits[knots.back()]++;
}

void rope::process_move (const move &_move) {
/* Process a single move line */
  pos dpos = move_to_pos(_move); // Single update step.
  for (int istep = 0; istep < _move.steps; istep++) {
    update_head(dpos);
    for (int iknot = 1; iknot < int(knots.size()); iknot ++) {
      update_knot(iknot);
    }
    note_visited();
  }
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 9#" << std::endl;


  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);
  std::string line;
  std::vector<move> moves;
  while (std::getline(input, line)) {
    moves.push_back(parse_moveline(line));
  }

  /* Part 1 */
  knots_state ks;

  for(auto m : moves) {
    ks.process_move(m);
  }

  std::cout << "Number of locations visited by tail: "
            << ks.visits.size() << std::endl;

  /* Part 2 */
  rope r;

  for(auto m : moves) {
    r.process_move(m);
  }

  std::cout << "Rope case: number of locations visited by the tail: "
            << r.visits.size() << std::endl;
}
