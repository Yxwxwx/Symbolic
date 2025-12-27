#include "expr.hpp"
#include "wick.hpp"

using namespace Symbolic;

int main() {
  Index p1 = Index("p_1")
                 .set_vacuum(Vacuum::Physical)
                 .set_space(Space::General)
                 .build();
  Index p2{"p_2"}, p3{"p_3"}, p4{"p_4"};

  auto cp1 = fcrex(p1), cp2 = fcrex(p2);
  auto ap3 = fannx(p3), ap4 = fannx(p4);
  std::cout << "Full contractions: \n";
  std::cout << to_latex(ap3 * ap4 * cp1 * cp2) << " = "
            << to_latex(FWickTheorem{ap3 * ap4 * cp1 * cp2}
                            .full_contractions(true)
                            .compute())
            << '\n';
  // a_{p3}a_{p4}a^{p1}a^{p2} = -s^{p3}_{p1}s^{p4}_{p2}+s^{p3}_{p2}s^{p4}_{p1}

  std::cout << "No Full contractions: \n";
  std::cout << to_latex(ap3 * ap4 * cp1 * cp2) << " = "
            << to_latex(FWickTheorem{ap3 * ap4 * cp1 * cp2}
                            .full_contractions(false)
                            .compute())
            << '\n';
  // a_{p3}a_{p4}a^{p1}a^{p2} =
  // a^{p1p2}_{p4p3}-s^{p3}_{p2}a^{p1}_{p4}+s^{p4}_{p2}a^{p1}_{p3}+s^{p3}_{p1}a^{p2}_{p4}-s^{p3}_{p1}s^{p4}_{p2}-s^{p4}_{p1}a^{p2}_{p3}+s^{p4}_{p1}s^{p3}_{p2}
}
