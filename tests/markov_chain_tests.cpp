#include <gtest/gtest.h>

#include "markov.h"

std::vector<int> BruteFindStatesChain(const double transition_probs[],
                                      int state,
                                      size_t chain_length,
                                      size_t states_count,
                                      double &chain_prob);

double CalculateProbability(const double transition_probs[],
                            int state,
                            size_t states_count,
                            const std::vector<int> &chain);

std::unique_ptr<double[]> GenerateTransitionsMatrix(size_t size);

TEST(Markov, CalculateProbability) {
    const double transition[] = {0.0, 0.6, 0.4,
                                 0.5, 0.5, 0.0,
                                 0.0, 0.99, 0.01};

    ASSERT_EQ(0.3, CalculateProbability(transition, 0, 3, {1, 0}));
}

TEST(Markov, CheckBruteforce) {
    const double transition[] = {0.0, 0.6, 0.4,
                                 0.5, 0.5, 0.0,
                                 0.0, 0.99, 0.01};

    double prob;

    ASSERT_EQ(std::vector<int>({2, 1}), BruteFindStatesChain(transition, 0, 2, 3, prob));
    ASSERT_EQ(0.4 * 0.99, prob);
}

SparseTable<double> GetSparseTable(const double *source, size_t size) {
    SparseTable<double> table(size, std::numeric_limits<double>::lowest());

    for (size_t row = 0; row < size; ++row) {
        for (size_t col = 0; col < size; ++col) {
            if (*source != 0.0) {
                table.Set(row, col, std::log(*source));
            }
            ++source;
        }
    }

    return table;
}

TEST(Markov, FindStatesChain1) {
    const double transition[] = {0.0, 0.6, 0.4,
                                 0.5, 0.5, 0.0,
                                 0.0, 0.99, 0.01};

    const auto& table = GetSparseTable(transition, 3);
    ASSERT_EQ(std::vector<int>({2, 1}), FindStatesChain(table, 0, 2));
}

TEST(Markov, FindStatesChain2) {
    const double transition[] = {0.0, 0.6, 0.4,
                                 0.5, 0.5, 0.0,
                                 0.0, 0.99, 0.01};

    const auto& table = GetSparseTable(transition, 3);
    double prob;
    ASSERT_EQ(BruteFindStatesChain(transition, 0, 3, 3, prob), FindStatesChain(table, 0, 3));
}

TEST(Markov, FindStatesChain3) {
    const double transition[] = {0.0, 0.6, 0.4,
                                 0.5, 0.5, 0.0,
                                 0.0, 0.99, 0.01};

    const auto& table = GetSparseTable(transition, 3);
    ASSERT_EQ(std::vector<int>({1}), FindStatesChain(table, 0, 1));
}

// TODO Print

TEST(Markov, FindStatesChain4) {
    const size_t transitions_count = 5;
    const size_t chain_size = 2;

    double prob;
    for (size_t attempts = 0; attempts < 100; ++attempts) {
        const auto& transitions = GenerateTransitionsMatrix(transitions_count);
        const auto& table = GetSparseTable(transitions.get(), transitions_count);

        const auto& expected = BruteFindStatesChain(transitions.get(), 0, chain_size, transitions_count, prob);
        const auto& actual = FindStatesChain(table, 0, chain_size);

        ASSERT_EQ(CalculateProbability(transitions.get(), 0, transitions_count, expected),
                  CalculateProbability(transitions.get(), 0, transitions_count, actual));
    }
}



std::vector<int> RecursiveBruteFindStatesChain(const double transition_probs[],
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
        auto chain = RecursiveBruteFindStatesChain(transition_probs,
                                                   next_state,
                                                   chain_length - 1,
                                                   states_count,
                                                   chain_prob);

        if (chain_prob * transition_prob > max_prob) {
            max_prob_chain = std::move(chain);
            max_prob_chain.push_back(next_state);
            max_prob = chain_prob * transition_prob;
        }
    }

    chain_prob = max_prob;
    return max_prob_chain;
}

std::vector<int> BruteFindStatesChain(const double transition_probs[],
                                      int state,
                                      size_t chain_length,
                                      size_t states_count,
                                      double &chain_prob) {
    auto chain = RecursiveBruteFindStatesChain(transition_probs,
                                               state,
                                               chain_length,
                                               states_count,
                                               chain_prob);

    std::reverse(chain.begin(), chain.end());
    return chain;
}

double CalculateProbability(const double transition_probs[],
                            int state,
                            size_t states_count,
                            const std::vector<int> &chain) {
    int prev_state = state;
    double probability = 1.0;

    for (const auto &node : chain) {
        probability *= transition_probs[prev_state * states_count + node];
        prev_state = node;
    }

    return probability;
}

std::unique_ptr<double[]> GenerateTransitionsMatrix(size_t size) {
    std::unique_ptr<double[]> matrix(new double[size * size]);
    std::mt19937 gen(3312444);
    std::uniform_real_distribution<double> distr(0.0, 1.0);
    auto matrix_ptr = matrix.get();

    for (size_t row = 0; row < size; ++row) {
        double prob_sum = 0.0;
        for (size_t col = 0; col < size; ++col) {
            double el = distr(gen);
            prob_sum += el;
            *(matrix_ptr++) = el;
        }

        for (size_t col = 1; col <= size; ++col) {
            *(matrix_ptr - col) /= prob_sum;
        }
    }

    return matrix;
}