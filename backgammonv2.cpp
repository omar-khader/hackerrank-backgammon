#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <numeric>

using namespace std;
using VVVI = vector<vector<vector<int>>>;
using VVI = vector<vector<int>>;
using VI = vector<int>;

const double infinity = numeric_limits<double>::infinity();
int player = 0; // track player
const int MAX_DEPTH = 4; // recursion depth
vector<vector<int>> dummy_action = {{0, 0}, {0, 0}}; // dummy action vector

vector<vector<int>> all_rolls = []() { // initialize all rolls vector
    vector<vector<int>> rolls; // vector to store all rolls
    for (int i = 1; i <= 6; ++i) { // through dice values 1-6
        for (int j = i; j <= 6; ++j) { // again through dice values 1-6
            if (i != j) { // if they are not the same
                rolls.push_back({i, j}); // add them to the rolls vector
            } else { // if they are the same
                rolls.push_back({i, i, i, i}); // if they are the same and doubles
            }
        }
    }
    return rolls;
}();

class GameState {
public:
    VVI board;
    VI bar;
    int player;
    VVI all_moves;

    GameState(const VVI& board, const VI& bar, int player) // GameState constructor
        : board(board), bar(bar), player(player) {} // members initializer

    bool can_bear(const VVI& board, const VI& bar, int player) { // check if player can bear
        bool bear = (bar[player] == 0); // if bar empty
        for (int i = 1; i < 19; ++i) { // check each point for checkers
            int index = (2 == player) ? (25 - i) : i; // get index
            if (bear) { // if can bear off
                bear = (board[index].empty() || (board[index][0] != player)); // update condition
            }
        }
        return bear;
    }

    VVI bear_moves(const VVI& board, int roll, int player) {
        VVI posi_moves; // vector to store possible moves
        int index = (1 == player) ? (25 - roll) : roll;
        int end = (2 == player) ? 0 : 25;

        if (!board[index].empty() && board[index][0] == player) { // match player and index not empty
            posi_moves.push_back({index, end}); // adding moves to the vector
        }

        for (int i = 1; i <= 6; ++i) { // check home positions
            index = (1 == player) ? (18 + i) : (7 - i); // find index based on whose turn
            if (!board[index].empty() && board[index][0] == player) {
                for (int j = 1; j < 7 - i; ++j) {
                    int index2 = (2 == player) ? (index - j) : (index + j);
                    if (j == roll) {
                        if (board[index2].empty() || board[index].size() == 1 || board[index2][0] == player) {
                            posi_moves.push_back({index, index2});
                        }
                    }
                }
            }
        }

        if (!posi_moves.empty()) { // ensuring possible moves are available
            return posi_moves;
        } else { // no possible moves
            for (int i = 1; i <= 6; ++i) {
                index = (1 == player) ? (25 - i) : i;
                end = (2 == player) ? 0 : 25;
                if (!board[index].empty() && board[index][0] == player) {
                    if (i < roll) {
                        return {{index, end}}; // return bear off moves
                    }
                }
            }
        }
        return posi_moves;
    }

    VVI normal_moves(const VVI& board, int roll, int player) {
        VVI posi_moves;
        for (int i = 1; i < 24; ++i) {
            int index = (2 == player) ? (25 - i) : i;
            if (!board[index].empty() && board[index][0] == player) {
                for (int j = 1; j < min(7, 25 - i); ++j) {
                    int index2 = (2 == player) ? (index - j) : (index + j);
                    if (j == roll) {
                        if (board[index2].size() <= 1 || board[index2][0] == player) {
                            posi_moves.push_back({index, index2});
                        }
                    }
                }
            }
        }
        return posi_moves;
    }

    VI bar_moves(const VVI& board, int roll, int player) {
        int index = (player == 2) ? (25 - roll) : roll;
        if (board[index].size() <= 1 || board[index][0] == player) {
            return {-1, index};
        } else {
            return {};
        }
    }

    void get_moves(VVVI& moves, VVI board, VI bar, VVI mv, VI dice_rolls, int player) { // collect all possible moves
        if (dice_rolls.empty()) {
            moves.push_back(mv);
            return;
        }
        int roll = dice_rolls[0];
        if (bar[player] != 0) {
            VI move = bar_moves(board, roll, player);
            if (!move.empty()) {
                bar[player] -= 1; // decrement bar count
                if (board[move[1]].size() == 1 && board[move[1]][0] != player) { // make sure no opponent checker
                    board[move[1]].pop_back();
                }
                board[move[1]].push_back(player);
                VVI new_mv = mv;
                new_mv.push_back(move);
                get_moves(moves, board, bar, new_mv, vector<int>(dice_rolls.begin() + 1, dice_rolls.end()), player);
            } else {
                get_moves(moves, board, bar, mv, vector<int>(dice_rolls.begin() + 1, dice_rolls.end()), player);
            }
        } else {
            VVI move;
            if (can_bear(board, bar, player)) {
                move = bear_moves(board, roll, player);
            } else {
                move = normal_moves(board, roll, player);
            }
            if (!move.empty()) {
                for (const auto& m : move) {
                    VVI temp_board = board;
                    temp_board[m[0]].pop_back();
                    if (temp_board[m[1]].size() == 1 && temp_board[m[1]][0] != player) {
                        temp_board[m[1]].pop_back();
                    }
                    temp_board[m[1]].push_back(player);
                    VVI new_mv = mv;
                    new_mv.push_back(m);
                    get_moves(moves, temp_board, bar, new_mv, vector<int>(dice_rolls.begin() + 1, dice_rolls.end()), player);
                }
            } else {
                get_moves(moves, board, bar, mv, vector<int>(dice_rolls.begin() + 1, dice_rolls.end()), player);
            }
        }
    }
};

class Backgammon {
public:
    int opponent(int player) {
        return player == 1 ? 2 : 1;
    }

    VVVI actions(GameState& state, const VI& dice_rolls) {
        VVI board = state.board;
        VI bar = state.bar;
        int player = state.player;
        VVVI moves;
        if (!dice_rolls.empty()) {
            if (dice_rolls.size() == 4) {
                state.get_moves(moves, board, bar, {}, dice_rolls, player);
            } else {
                VVVI m1, m2;
                state.get_moves(m1, board, bar, {}, dice_rolls, player);
                state.get_moves(m2, board, bar, {}, VI(dice_rolls.rbegin(), dice_rolls.rend()), player);
                moves.insert(moves.end(), m1.begin(), m1.end());
                moves.insert(moves.end(), m2.begin(), m2.end());
            }
        }
        return moves;
    }

    GameState result(const GameState& state, const VVI& move) {
        VVI board = state.board;
        VI bar = state.bar;
        int player = state.player;
        if (move.empty()) {
            return GameState(board, bar, opponent(player));
        }
        for (const auto& m : move) {
            if (m[0] == -1) {
                bar[player] -= 1;
                if (board[m[1]].size() == 1 && board[m[1]][0] != player) {
                    bar[board[m[1]][0]] += 1;
                    board[m[1]].pop_back();
                }
                board[m[1]].push_back(player);
            } else {
                board[m[0]].pop_back();
                if (board[m[1]].size() == 1 && board[m[1]][0] != player) {
                    bar[board[m[1]][0]] += 1;
                    board[m[1]].pop_back();
                }
                board[m[1]].push_back(player);
            }
        }
        return GameState(board, bar, opponent(player));
    }

    double utility(const GameState& , int ) {
        return 0.0; // Placeholder, implement the actual utility function if needed
    }
};

double eval_fn(const GameState& state) {
    const auto& board = state.board;
    int m_player = player;
    const auto& bar = state.bar;
    double v = 0;
    int distance = 0;

    for (int i = 1; i < 25; ++i) {
        if (!board[i].empty() && board[i][0] == m_player) {
            distance += board[i].size() * (m_player == 1 ? abs(25 - i) : abs(0 - i));
        }
    }
    v = -1 * distance;

    int home_checkers = 0;
    for (int i = 1; i < 6; ++i) {
        int index = (1 == m_player) ? (18 + i) : (7 - i);
        if (!board[index].empty()) {
            home_checkers += board[index].size();
        }
    }
    v += home_checkers;
    v += 10 * (m_player == 1 ? board[25].size() : board[0].size());
    v -= 10 * bar[m_player];

    int opponent_checkers = 0;
    for (int i = 1; i < 6; ++i) {
        int index = (2 == m_player) ? (18 + i) : (7 - i);
        if (!board[index].empty()) {
            opponent_checkers += board[index].size();
        }
    }
    v -= opponent_checkers;
    return v;
}

VVVI forward_pruning(const GameState& state, VVVI actions, Backgammon& game, int k = 4) {
    if (actions.size() < k) {
        return actions;
    }
    vector<int> score_list;
    VVVI new_action_list;
    for (const auto& a : actions) {
        GameState new_state = game.result(state, a);
        score_list.push_back(eval_fn(new_state));
    }
    vector<int> indices(score_list.size());
    iota(indices.begin(), indices.end(), 0);
    partial_sort(indices.begin(), indices.begin() + k, indices.end(), [&score_list](int i1, int i2) {
        return score_list[i1] > score_list[i2];
    });
    for (int i = 0; i < k; ++i) {
        new_action_list.push_back(actions[indices[i]]);
    }
    return new_action_list;
}

double max_value(GameState state, Backgammon& game, const VI& roll, double alpha, double beta, int depth);
double min_value(GameState state, Backgammon& game, const VI& roll, double alpha, double beta, int depth);

double expectinode(GameState state, Backgammon& game, double alpha, double beta, int depth) {
    if (depth == 0) {
        return eval_fn(state);
    }
    double v = 0;
    for (const auto& roll : all_rolls) {
        double prob = (roll.size() == 4) ? (1.0 / 36) : (1.0 / 18);
        v += prob * ((player == state.player) ? max_value(state, game, roll, alpha, beta, depth) : min_value(state, game, roll, alpha, beta, depth));
    }
    return v;
}

double max_value(GameState state, Backgammon& game, const VI& roll, double alpha, double beta, int depth) {
    auto actions = game.actions(state, roll);
    auto pruned_actions = forward_pruning(state, actions, game);
    double v = -infinity;
    if (pruned_actions.empty()) {
        return expectinode(game.result(state, {}), game, alpha, beta, depth);
    }
    for (const auto& a : pruned_actions) {
        v = max(v, expectinode(game.result(state, a), game, alpha, beta, depth));
        if (v >= beta) {
            return v;
        }
        alpha = max(alpha, v);
    }
    return v;
}

double min_value(GameState state, Backgammon& game, const VI& roll, double alpha, double beta, int depth) {
    auto actions = game.actions(state, roll);
    auto pruned_actions = forward_pruning(state, actions, game);
    double v = infinity;
    depth -= 1;
    if (pruned_actions.empty()) {
        return expectinode(game.result(state, {}), game, alpha, beta, depth);
    }
    for (const auto& a : pruned_actions) {
        v = min(v, expectinode(game.result(state, a), game, alpha, beta, depth));
        if (v <= alpha) {
            return v;
        }
        beta = min(beta, v);
    }
    return v;
}

VVI get_best_move(GameState state, Backgammon& game, const VI& dice_rolls) {
    double best_score = -infinity;
    VVI best_action;
    double beta = infinity;
    int depth = 1;
    auto actions = game.actions(state, dice_rolls);
    auto pruned_actions = forward_pruning(state, actions, game);
    for (const auto& a : pruned_actions) {
        double v = expectinode(game.result(state, a), game, best_score, beta, depth);
        if (v > best_score) {
            best_score = v;
            best_action = a;
        }
    }
    return best_action;
}

int main() {
    VVI board(26);
    VI bar(3, 0);
    cin >> player;

    for (int i = 0; i < 26; ++i) {
        int n;
        cin >> n;
        if (n > 0) {
            int value;
            cin >> value;
            board[i] = vector<int>(n, value);
        }
    }

    cin >> bar[1] >> bar[2];

    int rolls;
    cin >> rolls;
    VI dice_rolls(rolls);
    for (int i = 0; i < rolls; ++i) {
        cin >> dice_rolls[i];
    }
    sort(dice_rolls.begin(), dice_rolls.end());

    GameState game_state(board, bar, player);
    Backgammon game;

    auto moves = get_best_move(game_state, game, dice_rolls);
    if (!moves.empty()) {
        for (const auto& move : moves) {
            cout << move[0] << " " << move[1] << "\n";
        }
    }

    return 0;
}
