-- src/WickTheorem.hs
module WickTheorem
  ( canContract,
    isNormalOrder,
    fullWickContract,
  )
where

import Data.List (delete)
import FermionData

--------------------------------------------------
-- Contraction & normal ordering rules
--------------------------------------------------

-- | Whether two fermionic operators can contract
canContract :: Op -> Op -> Bool
canContract (Annihilate _) (Create _) = True
canContract _ _ = False

--------------------------------------------------
-- Normal ordering check
--------------------------------------------------

isNormalOrder :: Expr -> Bool
isNormalOrder (Term _ ops _) = ordered ops
  where
    ordered [] = True
    ordered [_] = True
    ordered (a : b : xs) =
      not (canContract a b) && ordered (b : xs)

-- Stores indices of operators that are contracted together.
type Pairing = [(Int, Int)]

-- | Helper to get pairings directly from an Expr
allFullContractions :: Expr -> [Pairing]
allFullContractions (Term _ ops _) =
  let indices = [0 .. length ops - 1]
   in generatePairings ops indices

-- | Core logic to find all possible complete pairings
generatePairings :: [Op] -> [Int] -> [Pairing]
generatePairings _ [] = [[]]
generatePairings ops (i : restIndices) =
  case ops !! i of
    Create _ -> []
    Annihilate _ ->
      [ (i, j) : sub
      | (k, j) <- zip [0 ..] restIndices,
        canContract (ops !! i) (ops !! j),
        let nextRest = delete j restIndices,
        sub <- generatePairings ops nextRest
      ]

-- | Count the number of crossings in a pairing to determine the phase factor.
-- Each crossing represents a permutation that contributes a (-1) factor.
countCrossings :: Pairing -> Int
countCrossings p =
  let -- Normalize each pair (i, j) so that i < j, equivalent to std::swap(i, j)
      normalized = [(min i j, max i j) | (i, j) <- p]

      -- Check if two pairs (i, j) and (k, l) cross each other
      -- Condition: (i < k < j < l) OR (k < i < l < j)
      isCrossing (i, j) (k, l) =
        (i < k && k < j && j < l) || (k < i && i < l && l < j)
   in -- Double loop equivalent: pick all unique pairs of (pairA, pairB) from the list
      length
        [ ()
        | (idxA, pairA) <- zip [0 ..] normalized,
          (idxB, pairB) <- zip [0 ..] normalized,
          idxA < idxB, -- Ensure we only count each combination once (a < b)
          isCrossing pairA pairB
        ]

-- | Perform full Wick contraction on a single Term.
fullWickContract :: Expr -> [Expr]
fullWickContract (Term coeff ops _) =
  let -- Count operators
      numCreate = length [() | Create _ <- ops]
      numAnnihilate = length [() | Annihilate _ <- ops]

      -- In full contraction, creation and annihilation counts must match
      indices = [0 .. length ops - 1]
   in if numCreate /= numAnnihilate
        then []
        else
          [ let -- Calculate sign based on crossings
                crossings = countCrossings p
                sign = if even crossings then 1.0 else -1.0

                -- Extract delta indices from the pairing
                -- Equivalent to Delta{e[i].index, e[j].index}
                toDelta (i, j) =
                  let (idxI, idxJ) = (getIdx (ops !! i), getIdx (ops !! j))
                   in Delta idxI idxJ

                -- Helper to extract string index from Op
                getIdx (Create s) = s
                getIdx (Annihilate s) = s

                -- Build the new term starting with (sign * original_coeff)
                -- Then fold with all delta functions
                newCoeff = sign * coeff
                deltas = map toDelta p
             in Term newCoeff [] deltas
          | p <- generatePairings ops indices
          ]