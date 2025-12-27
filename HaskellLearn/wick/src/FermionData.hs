-- src/FermionData.hs
module FermionData
  ( Op (..),
    Delta (..),
    Expr (..),
    scale,
    withDelta,
    formatPolynomial,
  )
where

import Data.List (intercalate)

-- | Fermionic operators
data Op
  = Create String
  | Annihilate String
  deriving (Eq)

instance Show Op where
  show (Create s) = "a^{" ++ s ++ "}"
  show (Annihilate s) = "a_{" ++ s ++ "}"

-- | Kronecker delta
data Delta = Delta String String
  deriving (Eq)

instance Show Delta where
  show (Delta i j) = "delta^{" ++ i ++ "}_{" ++ j ++ "}"

-- | Fermionic expression
data Expr
  = Term Double [Op] [Delta]
  deriving (Eq)

-- | Semigroup instance for Expr
instance Semigroup Expr where
  Term c1 ops1 ds1 <> Term c2 ops2 ds2 =
    Term (c1 * c2) (ops1 ++ ops2) (ds1 ++ ds2)

-- | Monoid instance for Expr
instance Monoid Expr where
  mempty = Term 1.0 [] []

instance Show Expr where
  show (Term c ops ds) =
    let coeffStr =
          if c == 1.0 && (not (null ds) || not (null ops))
            then ""
            else show c

        deltaStrs = map show ds
        opStrs = map show ops

        components = filter (not . null) ([coeffStr] ++ deltaStrs ++ opStrs)
     in if null components
          then "0"
          else unwords components

-- | Format a list of Expr (a polynomial) into a single string
formatPolynomial :: [Expr] -> String
formatPolynomial [] = "0"
formatPolynomial (x : xs) = show x ++ concatMap formatNext xs
  where
    formatNext (Term c ops ds)
      | c < 0 = " - " ++ show (Term (abs c) ops ds)
      | otherwise = " + " ++ show (Term c ops ds)

-- | Multiply coefficient
scale :: Double -> Expr -> Expr
scale a (Term c ops ds) = Term (a * c) ops ds

-- | Add a delta factor
withDelta :: Delta -> Expr -> Expr
withDelta d (Term c ops ds) = Term c ops (d : ds)