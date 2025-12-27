#pragma once

#include "attr.hpp"

#include <stdexcept>
#include <string>
namespace Symbolic {

struct Index {
  std::string name;
  Space space = Space::General;
  Vacuum vacuum = Vacuum::Physical;

  Index() = default;
  explicit Index(std::string n) : name(std::move(n)) {}

  Index& set_space(Space s) {
    space = s;
    return *this;
  }
  Index& set_vacuum(Vacuum v) {
    vacuum = v;
    return *this;
  }

  Index build() const {
    if (!space_allowed(vacuum, space))
      throw std::runtime_error("Illegal Space for this Vacuum");
    return *this;
  }

  bool operator==(const Index& other) const {
    return name == other.name && space == other.space && vacuum == other.vacuum;
  }
  bool operator!=(const Index& other) const { return !(*this == other); }
};

}  // namespace Symbolic
