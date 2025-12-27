#pragma once

#include "op.hpp"

#include <vector>

namespace Symbolic {
template <Statistics S = Statistics::FermiDirac>
struct Expr {
  double coeff = 1.0;
  std::vector<Delta> deltas;
  std::vector<Op> ops;
  constexpr static bool is_fermi = S == Statistics::FermiDirac;
  Expr() = default;
  Expr(const double c, const Op& op) : coeff(c) { ops.emplace_back(op); }
  Expr(const double c, const Op&& op) : coeff(c) { ops.emplace_back(op); }
  static Expr scalar(const double c) { return Expr(c, Op{}); }
  const std::size_t size() const { return ops.size(); }
  const Op& operator[](const std::size_t i) const { return ops[i]; }
  void append(const Op& op) { ops.emplace_back(op); }
  void append(const Expr& e) {
    ops.insert(ops.end(), e.ops.begin(), e.ops.end());
    coeff *= e.coeff;
  }
  void add_delta_impl(Delta&& delta) {
    auto& a = delta.a;
    auto& b = delta.b;

    if (a == b) return;

    for (auto& d : deltas) {
      if (d.a == b) {
        d.a = std::move(a);
        return;
      }
    }

    deltas.emplace_back(std::move(delta));
  }
  Expr<S>& add_delta(Delta delta) {
    add_delta_impl(std::move(delta));
    return *this;
  }
  Expr<S>& add_delta(const Index& a, const Index& b) {
    add_delta_impl(Delta{a, b});
    return *this;
  }

  Expr<S>& add_delta(Index&& a, Index&& b) {
    add_delta_impl(Delta{std::move(a), std::move(b)});
    return *this;
  }
  Expr<S>& mul_coeff(const double c) {
    coeff *= c;
    return *this;
  }
};

template <Statistics S>
inline std::string to_string(const Expr<S>& e) {
  std::string s;
  if (e.coeff == 1.0)
    s = "";
  else if (e.coeff == -1.0)
    s = "-";
  else
    s = std::to_string(e.coeff);
  for (auto& x : e.ops) s += to_string(x);
  return s;
}

template <Statistics S>
inline std::string to_latex(const Expr<S>& e) {
  std::string s;
  if (e.coeff == 1.0)
    s = "";
  else if (e.coeff == -1.0)
    s = "-";
  else
    s = std::to_string(e.coeff);
  for (auto& d : e.deltas) s += to_latex(d);
  for (auto& x : e.ops) s += to_latex(x, e.is_fermi);
  return s;
}
template <Statistics S>
inline std::string to_tensor_notation(const Expr<S>& e) {
  std::string s;
  if (e.coeff == 1.0)
    s = "";
  else if (e.coeff == -1.0)
    s = "-";
  else
    s = std::to_string(e.coeff);
  for (auto& d : e.deltas) s += to_latex(d);
  std::vector<Op> creates;
  std::vector<Op> annihilates;
  for (auto& x : e.ops) {
    if (x.action == Action::Create)
      creates.emplace_back(x);
    else
      annihilates.emplace_back(x);
  }
  std::string ups = "";
  std::string downs = "";
  for (auto& x : creates) ups += index_string(x);
  for (auto it = annihilates.rbegin(); it != annihilates.rend(); ++it) {
    downs += index_string(*it);
  }
  if (!ups.empty() || !downs.empty()) {
    if constexpr (S == Statistics::FermiDirac)
      s += "a";
    else
      s += "b";

    if (!ups.empty()) s += "^{" + ups + "}";
    if (!downs.empty()) s += "_{" + downs + "}";
  }
  return s;
}

// Statistics::FermiDirac
// coeff * Op -> Expr<S>
template <Statistics S = Statistics::FermiDirac>
inline Expr<S> operator*(const double c, const Op& rhs);
// Op * Op → Expr<S>
template <Statistics S = Statistics::FermiDirac>
inline Expr<S> operator*(const Op& lhs, const Op& rhs);
// Expr<S> * Op
template <Statistics S = Statistics::FermiDirac>
inline Expr<S> operator*(Expr<S> lhs, const Op& rhs);
// Op * Expr<S>
template <Statistics S = Statistics::FermiDirac>
inline Expr<S> operator*(const Op& lhs, Expr<S> rhs);
// Expr<S> * Expr<S>
template <Statistics S = Statistics::FermiDirac>
inline Expr<S> operator*(Expr<S> lhs, const Expr<S>& rhs);

template <>
inline Expr<Statistics::FermiDirac> operator*(const double c, const Op& rhs) {
  Expr<Statistics::FermiDirac> e{c, rhs};
  return e;
}

template <>
inline Expr<Statistics::FermiDirac> operator*(const Op& lhs, const Op& rhs) {
  Expr<Statistics::FermiDirac> e;
  e.append(lhs);
  e.append(rhs);
  return e;
}
template <>
inline Expr<Statistics::FermiDirac> operator*(Expr<Statistics::FermiDirac> lhs,
                                              const Op& rhs) {
  lhs.append(rhs);
  return lhs;
}

template <>
inline Expr<Statistics::FermiDirac> operator*(
    Expr<Statistics::FermiDirac> lhs, const Expr<Statistics::FermiDirac>& rhs) {
  lhs.append(rhs);
  return lhs;
}

}  // namespace Symbolic
