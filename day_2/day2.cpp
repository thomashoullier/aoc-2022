#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <utility>

typedef std::pair<char, char> move; // Single move, eg. A Y
typedef std::vector<move> moves;

char convert_to_RPS (const char &letter) {
/* Convert characters of input to more understandable letters:
 * R: Rock, P: Paper, S: Scissors. */
  switch(letter) {
    case 'A': return 'R';
    case 'B': return 'P';
    case 'C': return 'S';
    case 'X': return 'R';
    case 'Y': return 'P';
    case 'Z': return 'S';
    default:
      std::cerr << "Incorrect move character" << std::endl;
      return '0';
  }
}

char winning_move (const char &opp_move) {
/* Return the winning move depending on opponent's move */
  switch(opp_move) {
    case 'R': return 'P';
    case 'P': return 'S';
    case 'S': return 'R';
    default:
      std::cerr << "Incorrect character" << std::endl;
      return '0';
  }
}

char losing_move (const char &opp_move) {
/* Return the losing move depending on opponent's move */
  switch(opp_move) {
    case 'R': return 'S';
    case 'P': return 'R';
    case 'S': return 'P';
    default:
      std::cerr << "Incorrect character" << std::endl;
      return '0';
  }
}

char choose_move (const char &opp_move, const char &outcome) {
/* Parse the opponent move and desired outcome, output the
 * move which needs to happen to meet the outcome */
  char out_move;
  switch(outcome) {
    case 'X': out_move = losing_move(opp_move); break;
    case 'Y': out_move = opp_move; break;
    case 'Z': out_move = winning_move(opp_move); break;
    default:
      std::cerr << "Incorrect character" << std::endl;
      out_move = '0';
  }
  return out_move;
}

bool winp (const char &us, const char &them) {
  /* In non-draw situations, determine whether we win or not.*/
  switch(us) {
    case 'R': if (them == 'S') {return true;} break;
    case 'P': if (them == 'R') {return true;} break;
    case 'S': if (them == 'P') {return true;} break;
    default:
      std::cerr << "Incorrect move character" << std::endl;
      return true;
  }
  return false; // If we did not win, then we lost.
}

char outcome (const move &move) {
/* Compute the outcome of a round.
 * L: Lose, D: Draw, W: Win. */
  auto us = move.second;
  auto them = move.first;
  if (us == them) {return 'D';}
  else if(winp(us, them)) {return 'W';}
  else {return 'L';}
}

int score_from_choice (const move &move) {
/* Compute the score given by our own choice in a move. */
  int score;
  switch(move.second) {
    case 'R':
      score = 1;
      break;
    case 'P':
      score = 2;
      break;
    case 'S':
      score = 3;
      break;
    default:
      std::cerr << "Incorrect move character" << std::endl;
      score = -1;
  }
  return score;
}

int score_from_outcome (const char &outcome) {
/* Give the score depending on the outcome: L/D/W.*/
  int score;
  switch(outcome) {
    case 'L': score = 0; break;
    case 'D': score = 3; break;
    case 'W': score = 6; break;
    default:
      std::cerr << "Incorrect outcome character" << std::endl;
      score = -1;
  }
  return score;
}

int comp_total_score (const moves &moves) {
/* Compute the total score from all the moves in rounds */
  /* Computing rounds outcomes */
  std::vector<char> outcomes;
  std::transform(moves.begin(), moves.end(), std::back_inserter(outcomes),
                 outcome);

  /* Computing scores from rounds */
  std::vector<int> outcome_scores;
  std::transform(outcomes.begin(), outcomes.end(),
                 std::back_inserter(outcome_scores),
                 score_from_outcome);

  std::vector<int> choice_scores;
  std::transform(moves.begin(), moves.end(), std::back_inserter(choice_scores),
                 score_from_choice);

  /* Total score from strategy */
  int total_score = std::reduce(choice_scores.begin(), choice_scores.end())
                   + std::reduce(outcome_scores.begin(), outcome_scores.end());
  return total_score;
}

int main (int argc, char *argv[]) {
  std::cout << "# Day 2 #" << std::endl;

  if (argc != 2) {
    std::cerr << "Please provide the input file." << std::endl;
    return 1;
  }

  /* Parsing the input text */
  std::ifstream input(argv[1]);

  moves orig_moves;
  std::string line;
  while (std::getline(input, line)) {
    char first_char = line.at(0);
    char second_char = line.at(2);
    move cur_pair = std::make_pair(first_char, second_char);
    orig_moves.push_back(cur_pair);
  }

  moves moves_p1; // Storing the moves in pairs R/P/S
  for (auto m : orig_moves) {
    auto our_move = convert_to_RPS(m.second);
    auto opp_move = convert_to_RPS(m.first);
    move cur_pair = std::make_pair(opp_move, our_move);
    moves_p1.push_back(cur_pair);
  }

  /* Total score from strategy */
  auto total_score = comp_total_score(moves_p1);
  std::cout << "Strategy total score: " << total_score << std::endl;

  /* Part2: Reading the second column differently. */
  moves moves_p2;
  for (auto m : orig_moves) {
    auto opp_move = convert_to_RPS(m.first);
    auto outcome = m.second;
    auto our_move = choose_move(opp_move, outcome);
    move cur_pair = std::make_pair(opp_move, our_move);
    moves_p2.push_back(cur_pair);
  }

  auto total_score_p2 = comp_total_score(moves_p2);
  std::cout << "New strategy total score: " << total_score_p2 << std::endl;
}
