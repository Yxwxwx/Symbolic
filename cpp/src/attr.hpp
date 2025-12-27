#pragma once

#include <cstdint>
#include <string_view>

namespace Symbolic {

// Vacumm
enum class Vacuum : uint8_t {
  Physical,       // |0|
  Fermi,          // |HF|
  MultiReference  // I dont know
};
inline constexpr std::string_view vacuum_name(Vacuum v) {
  switch (v) {
    case Vacuum::Physical:
      return "PhysicalVacuum";
    case Vacuum::Fermi:
      return "FermiVacuum";
    case Vacuum::MultiReference:
      return "MultiReferenceVacuum";
  }
  return "";
}

// Space
enum struct Space : std::uint8_t {
  General,        // p, q, r, s
  Occupied,       // i, j, k
  Virtual,        // a, b, c
  DoublyOccupied  // core / frozen core
};
inline constexpr bool space_allowed(Vacuum v, Space s) {
  switch (v) {
    case Vacuum::Physical:
      return s == Space::General;
    case Vacuum::Fermi:
      return s != Space::General;
    case Vacuum::MultiReference:
      return true;
  }
  return false;
}

inline constexpr std::string_view space_name(Space s) {
  switch (s) {
    case Space::General:
      return "GeneralSpace";
    case Space::Occupied:
      return "OccupiedSpace";
    case Space::Virtual:
      return "VirtualSpace";
    case Space::DoublyOccupied:
      return "DoublyOccupiedSpace";
  }
  return "";
}

// Action
enum struct Action : std::uint8_t { Create, Annihilate };
inline Action adjoint(Action action) {
  return action == Action::Create ? Action::Annihilate : Action::Create;
}

inline constexpr std::string_view action_name(Action a) {
  switch (a) {
    case Action::Create:
      return "Create";
    case Action::Annihilate:
      return "Annihilate";
  }
  return "";
}

// Statistics
enum struct Statistics : std::uint8_t {
  FermiDirac,
  BoseEinstein,
  Arbitrary,
};

inline constexpr std::string_view statistics_name(Statistics s) {
  switch (s) {
    case Statistics::FermiDirac:
      return "FermiDirac";
    case Statistics::BoseEinstein:
      return "BoseEinstein";
    case Statistics::Arbitrary:
      return "Arbitrary";
  }
  return "";
}

}  // namespace Symbolic
