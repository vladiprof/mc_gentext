#pragma once

// TODO Int?
template<Table>
std::vector<int> FindStatesChain(const Table &log_transition_probs,
                                 int state,
                                 size_t chain_length) {
  // TODO
  Table s(log_transition_probs.rows());

  //tr.first -> col
  for (const auto &tr : log_transition_probs[state]) {
    s[0][tr.first] = tr.second;
  }

  for (size_t id = 1; id < chain_length; ++id) {
    for (const auto &s_tr : s[id - 1]) {
      for (const auto& ) {
        s[
      }
    }
  }

}


// Brute force
// T = [[0.5, 0.3, 0.0]  -> 0
//      [0.0, 0.4, 0.3]  -> 1
//      [0.1, 0.2, 0.4]] -> 3

std::vector<int> BruteFindStatesChain(double transition_probs[],
                                      int state,
                                      size_t chain_length,
                                      size_t states_count,
                                      double &chain_prob) {
  if (chain_length == 0) {
    chain_prob = 1.0;
    return {};
  }

  double max_prob = 0;
  std::vector<int> max_prob_chain{};

  for (int next_state = 0; next_state < states_count; next_state++) {
    double chain_prob = 0.0;
    double
        transition_prob = transition_probs[state * states_count + next_state];
    auto &chain = FindStatesChain(transition_probs,
                                  next_state,
                                  chain_length - 1,
                                  states_count,
                                  chain_prob);

    if (chain_prob * transition_prob > max_prob) {
      max_prob_chain = std::move(chain);
      max_prob = chain_prob * transition_prob;
    }
  }

  chain_prob = max_prob;
  return max_prob_chain;
}