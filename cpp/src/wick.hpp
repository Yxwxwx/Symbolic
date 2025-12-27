#pragma once

#include <numeric>
#include "expr.hpp"

namespace Symbolic {
template <Statistics S>
struct WickTheorem;

using FWickTheorem = WickTheorem<Statistics::FermiDirac>;
using BWickTheorem = WickTheorem<Statistics::BoseEinstein>;

using FWickResult = std::vector<Expr<Statistics::FermiDirac>>;
using BWickResult = std::vector<Expr<Statistics::BoseEinstein>>;

template <Statistics S = Statistics::FermiDirac>
using ExprSum = std::vector<Expr<S>>;

template <Statistics S = Statistics::FermiDirac>
struct WickTheorem {
  using WickResult = std::vector<Expr<S>>;

  Expr<S> expr_;
  constexpr static bool is_fermi = S == Statistics::FermiDirac;
  bool full_contractions_ = true;

  WickTheorem(Expr<S> e) : expr_(std::move(e)) {}
  inline WickTheorem<S>& full_contractions(const bool fc) {
    full_contractions_ = fc;
    return *this;
  }
  std::vector<Expr<S>> compute() const;

 private:
  // inline Delta make_delta(const Op& a, const Op& b) const {
  //   return Delta{a.index, b.index};
  // }
  std::vector<Expr<S>> wick_expand(const Expr<S>& e) const;
  std::vector<Expr<S>> wick_expand_fc_recurse(const Expr<S>& e) const;
  std::vector<Expr<S>> wick_expand_fc(const Expr<S>& e) const;
};

inline bool can_contract(const Op& a, const Op& b) {
  return (a.action == Action::Annihilate && b.action == Action::Create);
}

template <Statistics S = Statistics::FermiDirac>
inline bool is_normal_order(const Expr<S>& e) {
  for (std::size_t i = 0; i + 1 < e.ops.size(); ++i) {
    const Op& a = e.ops[i];
    const Op& b = e.ops[i + 1];

    if (can_contract(a, b)) return false;
  }
  return true;
}

template <>
FWickResult WickTheorem<Statistics::FermiDirac>::compute() const {
  if (full_contractions_) return wick_expand_fc(expr_);

  return wick_expand(expr_);
}

using IndexList = std::vector<std::size_t>;
using Pairing = std::vector<std::pair<std::size_t, std::size_t>>;

template <Statistics S>
std::vector<Pairing> generate_pairings(const Expr<S>& e,
                                       const IndexList& free_indices) {
  if (free_indices.empty()) {
    return {{}};
  }

  std::vector<Pairing> results;

  const std::size_t i = free_indices.front();
  const Op& a = e.ops[i];

  if constexpr (S == Statistics::FermiDirac) {
    if (a.action != Action::Annihilate) return {};
  }

  for (std::size_t k = 1; k < free_indices.size(); ++k) {
    const std::size_t j = free_indices[k];
    const Op& b = e.ops[j];

    if (!can_contract(a, b)) continue;

    IndexList rest;
    rest.reserve(free_indices.size() - 2);
    for (std::size_t t = 1; t < free_indices.size(); ++t) {
      if (t != k) rest.push_back(free_indices[t]);
    }

    auto sub_pairings = generate_pairings<S>(e, rest);
    for (auto& sub : sub_pairings) {
      Pairing p;
      p.reserve(sub.size() + 1);
      p.emplace_back(i, j);
      p.insert(p.end(), sub.begin(), sub.end());
      results.emplace_back(std::move(p));
    }
  }

  return results;
}

inline std::size_t count_crossings(const Pairing& p) {
  std::size_t c = 0;
  for (std::size_t a = 0; a < p.size(); ++a) {
    auto [i, j] = p[a];
    if (i > j) std::swap(i, j);
    for (std::size_t b = a + 1; b < p.size(); ++b) {
      auto [k, l] = p[b];
      if (k > l) std::swap(k, l);
      if ((i < k && k < j && j < l) || (k < i && i < l && l < j)) {
        ++c;
      }
    }
  }
  return c;
}

template <Statistics S>
std::vector<Expr<S>> WickTheorem<S>::wick_expand_fc(const Expr<S>& e) const {
  std::size_t num_create{0}, num_annihilate{0};
  for (auto& x : e.ops) {
    if (x.action == Action::Create)
      ++num_create;
    else
      ++num_annihilate;
  }
  if (num_create != num_annihilate) return {};

  IndexList indices(e.ops.size());
  std::iota(indices.begin(), indices.end(), 0);

  auto pairings = generate_pairings<S>(e, indices);

  std::vector<Expr<S>> results;
  results.reserve(pairings.size());

  for (auto& p : pairings) {
    std::size_t crossings = 0;
    if constexpr (S == Statistics::FermiDirac) {
      crossings = count_crossings(p);
    }

    int sign = (crossings % 2 == 0) ? 1 : -1;
    Expr<S> term = Expr<S>::scalar(sign * e.coeff);
    for (auto [i, j] : p) {
      term.add_delta(Delta{e[i].index, e[j].index});
    }
    results.emplace_back(std::move(term));
  }
  return results;
}
template <Statistics S>
std::vector<Expr<S>> WickTheorem<S>::wick_expand_fc_recurse(
    const Expr<S>& e) const {
  // if e is a single operator, return it
  if (e.ops.size() <= 1) {
    return {e};
  }

  std::vector<Expr<S>> results;
  // fix the first operator
  const Op& first = e.ops.front();

  if constexpr (S == Statistics::FermiDirac) {
    if (first.action != Action::Annihilate) {
      return {};
    }
  }

  for (std::size_t i = 1; i < e.ops.size(); ++i) {
    const Op& other = e.ops[i];

    if (!can_contract(first, other)) continue;

    Expr<S> next = e;

    if constexpr (S == Statistics::FermiDirac)
      if ((i - 1) % 2 == 1) next.coeff *= -1.0;

    next.add_delta(Delta{first.index, other.index});

    next.ops.erase(next.ops.begin() + i);
    next.ops.erase(next.ops.begin());

    // recurse on the rest of the expression
    auto sub = wick_expand_fc(next);
    results.insert(results.end(), std::make_move_iterator(sub.begin()),
                   std::make_move_iterator(sub.end()));
  }

  return results;
}

template <Statistics S>
std::vector<Expr<S>> WickTheorem<S>::wick_expand(const Expr<S>& e) const {
  if (e.ops.size() <= 1 || is_normal_order(e)) return {e};

  std::vector<Expr<S>> results;

  for (std::size_t i = 0; i < e.ops.size() - 1; ++i) {
    const Op& a = e.ops[i];
    const Op& b = e.ops[i + 1];

    if (can_contract(a, b)) {
      Expr<S> swapped = e;
      std::swap(swapped.ops[i], swapped.ops[i + 1]);

      if constexpr (S == Statistics::FermiDirac) {
        swapped.coeff *= -1.0;
      }

      auto res_swap = wick_expand(swapped);
      results.insert(results.end(), std::make_move_iterator(res_swap.begin()),
                     std::make_move_iterator(res_swap.end()));

      Expr<S> contracted = e;
      contracted = contracted.add_delta(Delta{a.index, b.index});

      if (std::abs(contracted.coeff) > 1e-12) {
        contracted.ops.erase(contracted.ops.begin() + i,
                             contracted.ops.begin() + i + 2);

        auto res_contract = wick_expand(contracted);
        results.insert(results.end(),
                       std::make_move_iterator(res_contract.begin()),
                       std::make_move_iterator(res_contract.end()));
      }

      return results;
    }
  }

  return {e};
}

std::string to_latex(const FWickResult& w) {
  if (w.empty()) return "0";
  std::string s;
  if (w.begin() != w.end()) {
    s += to_tensor_notation(*w.begin());
  }
  for (auto it = w.begin() + 1; it != w.end(); ++it) {
    if (it->coeff == 1.0) s += "+";
    s += to_tensor_notation(*it);
  }
  return s;
}
}  // namespace Symbolic