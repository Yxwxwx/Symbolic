#pragma once

#include "index.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

namespace Symbolic {

// static constexpr char8_t adjoint_label[] = u8"⁺";
static constexpr char const* adjoint_label = "\xE2\x81\xBA";

struct Op {
  Index index;
  Action action;

  Op() = default;
  Op(Index idx, Action act) : index(std::move(idx)), action(act) {}

  Op dagger() const { return Op{index, adjoint(action)}; }
};

inline Op fcrex(Index idx) { return Op(std::move(idx), Action::Create); }
inline Op fannx(Index idx) { return Op(std::move(idx), Action::Annihilate); }
inline std::string to_string(const Op& op) {
  std::string s = op.index.name;
  if (op.action == Action::Create) {
    s += adjoint_label;
  }
  return s;
}

inline std::string to_latex(const Op& op, bool fermi = true) {
  std::string idx;
  idx.reserve(op.index.name.size());

  std::copy_if(op.index.name.begin(), op.index.name.end(),
               std::back_inserter(idx),
               [](unsigned char c) { return std::isalnum(c); });

  auto elem = fermi ? "a" : "b";
  std::ostringstream os;
  os << elem << (op.action == Action::Create ? "^{" : "_{") << idx << "}";
  return os.str();
}
inline std::string index_string(const Op& op) {
  std::string idx;
  idx.reserve(op.index.name.size());

  std::copy_if(op.index.name.begin(), op.index.name.end(),
               std::back_inserter(idx),
               [](unsigned char c) { return std::isalnum(c); });
  return idx;
}

std::ostream& operator<<(std::ostream& os, const Op& op) {
  return os << to_string(op) << " "
            << "[Vacuum: " << vacuum_name(op.index.vacuum) << ", "
            << "Space: " << space_name(op.index.space) << "]"
            << action_name(op.action);
}

//
struct Delta {
  Index a, b;  // delta(a,b)
};
std::string to_latex(const Delta& d) {
  std::string idxa, idxb;
  idxa.reserve(d.a.name.size());
  std::copy_if(d.a.name.begin(), d.a.name.end(), std::back_inserter(idxa),
               [](unsigned char c) { return std::isalnum(c); });
  idxb.reserve(d.b.name.size());
  std::copy_if(d.b.name.begin(), d.b.name.end(), std::back_inserter(idxb),
               [](unsigned char c) { return std::isalnum(c); });
  if (d.a == d.b) return "";

  std::ostringstream os;
  os << "s" << "^{" << idxa << "}" << "_{" << idxb << "}";
  return os.str();
}

}  // namespace Symbolic