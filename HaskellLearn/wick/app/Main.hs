-- app/Main.hs
import FermionData
import WickTheorem

main :: IO ()
main = do
  let cp1 = Create "p1"
      cp2 = Create "p2"
      ap3 = Annihilate "p3"
      ap4 = Annihilate "p4"

      op :: Op -> Expr
      op o = Term 1.0 [o] []

      expr :: Expr
      expr =
        scale 1.0 $
          foldMap op [ap3, ap4, cp1, cp2]

  print expr

  let results = fullWickContract expr

  putStrLn "\nWick Full Contraction (Expanded Form):"
  putStrLn $ "  = " ++ formatPolynomial results
