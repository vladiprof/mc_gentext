#pragma once

#include <unordered_map>

template<typename T>
class SparseTable {
private:
    using RowIterator = typename std::unordered_map<size_t, T>::const_iterator;

    class RangeIterator {
    public:
        RangeIterator(const std::unordered_map<size_t, T> &row)
            : begin_(row.cbegin()), end_(row.cend()) {}

        RowIterator begin() const {
            return begin_;
        }

        RowIterator end() const {
            return end_;
        }
    private:
        RowIterator begin_;
        RowIterator end_;
    };
public:
    SparseTable(size_t rows_count, const T &default_value = T{}) : rows_(rows_count),
                                                                   default_value_(default_value) {
    }

    T Get(size_t row, size_t col) const {
        const auto &row_val = rows_[row];
        auto it = row_val.find(col);

        if (it != row_val.end()) {
            return it->second;
        } else {
            return default_value_;
        }
    }

    size_t GetRowsCount() const { return rows_.size(); }

    void Set(size_t row, size_t col, const T &value) {
        rows_[row][col] = value;
    }

    RangeIterator GetRow(size_t row) const {
        return RangeIterator(rows_.at(row));
    }

    size_t MaxValueCol(size_t row) const {
        const auto &row_value = rows_[row];

        if (!row_value.empty()) {
            return std::max_element(row_value.begin(), row_value.end(),
                                    [](const std::pair<int, double> &lhs,
                                       const std::pair<int, double> &rhs) {
                                        return lhs.second < rhs.second;
                                    })->first;
        } else {
            return default_value_;
        }
    }

private:
    std::vector<std::unordered_map<size_t, T>> rows_;
    T default_value_;
};

std::vector<int> FindStatesChain(const SparseTable<double> &log_transition_probs,
                                 int current_state,
                                 size_t chain_length) {
    SparseTable<double> dyntbl(chain_length, std::numeric_limits<double>::lowest());
    SparseTable<double> dyntbl_states(chain_length);

    for (const auto &next_state: log_transition_probs.GetRow(current_state)) {
        dyntbl.Set(0, next_state.first, next_state.second);
        dyntbl_states.Set(0, next_state.first, current_state);
    }

    for (size_t state = 1; state < chain_length; ++state) {
        for (const auto &inter_state : dyntbl.GetRow(state - 1)) {
            for (const auto &next_state : log_transition_probs.GetRow(inter_state.first)) {
                double prob = inter_state.second + next_state.second;
                if (prob > dyntbl.Get(state, next_state.second)) {
                    dyntbl.Set(state, next_state.first, prob);
                    dyntbl_states.Set(state, next_state.first, inter_state.first);
                }
            }
        }
    }

    // Reconstruct chain

    std::vector<int> states_chain(chain_length);
    auto it_out = states_chain.rbegin();

    int state = dyntbl.MaxValueCol(chain_length - 1);
    *(it_out++) = state;
    --chain_length;

    for (; chain_length > 0; --chain_length) {
        state = dyntbl_states.Get(chain_length, state);
        *(it_out++) = state;
    }


    return states_chain;
}
