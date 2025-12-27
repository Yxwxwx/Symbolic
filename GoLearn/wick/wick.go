package main

import (
	"strings"
)

// Type Aliases
type IndexList []int
type Pair [2]int
type Pairing []Pair
type ExprSum []Expr

// WickTheorem
type WickTheorem struct {
	expr             Expr
	fullContractions bool
	wickResult       ExprSum
}

func NewWickTheorem(expr Expr) *WickTheorem {
	return &WickTheorem{
		expr:             expr,
		fullContractions: false,
		wickResult:       ExprSum{},
	}
}

func (wt *WickTheorem) SetFullContractions(fc bool) *WickTheorem {
	wt.fullContractions = fc
	return wt
}

func (wt *WickTheorem) Compute() *WickTheorem {
	if wt.fullContractions {
		wt.wickResult = wt.wickExpandFC()
	} else {
		// TODO:
		wt.wickResult = ExprSum{wt.expr}
	}
	return wt
}

func (wt *WickTheorem) ToString() string {
	if len(wt.wickResult) == 0 {
		return "0"
	}
	var strs []string
	for _, e := range wt.wickResult {
		strs = append(strs, e.String())
	}

	res := strings.Join(strs, " + ")
	return strings.ReplaceAll(res, "+ -", "- ")
}

func (wt *WickTheorem) wickExpandFC() ExprSum {
	ops := wt.expr.Ops
	if len(ops) == 0 {
		return ExprSum{wt.expr}
	}

	numCreate := 0
	for _, o := range ops {
		if o.Type == Create {
			numCreate++
		}
	}
	numAnnihilate := len(ops) - numCreate

	if numCreate != numAnnihilate {
		return ExprSum{}
	}

	indices := make(IndexList, len(ops))
	for i := range indices {
		indices[i] = i
	}

	pairings := generatePairings(wt.expr, indices)
	results := make(ExprSum, 0, len(pairings))

	for _, p := range pairings {
		crossings := countCrossings(p)
		sign := 1.0
		if crossings%2 != 0 {
			sign = -1.0
		}

		term := Expr{
			Coeff:  sign * wt.expr.Coeff,
			Ops:    []Op{},
			Deltas: make([]Delta, 0, len(p)),
		}

		for _, pair := range p {
			idxI := ops[pair[0]].Index
			idxJ := ops[pair[1]].Index
			term.Deltas = append(term.Deltas, Delta{I: idxI, J: idxJ})
		}
		results = append(results, term)
	}

	return results
}

func generatePairings(e Expr, freeIndices IndexList) []Pairing {
	if len(freeIndices) == 0 {
		return []Pairing{{}}
	}

	results := []Pairing{}
	i := freeIndices[0]
	opI := e.Ops[i]

	if opI.Type == Create {
		return nil
	}

	for k := 1; k < len(freeIndices); k++ {
		j := freeIndices[k]
		opJ := e.Ops[j]

		if !canContract(opI, opJ) {
			continue
		}

		rest := make(IndexList, 0, len(freeIndices)-2)
		for t := 1; t < len(freeIndices); t++ {
			if t != k {
				rest = append(rest, freeIndices[t])
			}
		}

		subPairings := generatePairings(e, rest)
		for _, sub := range subPairings {

			p := make(Pairing, 0, len(sub)+1)
			p = append(p, Pair{i, j})
			p = append(p, sub...)
			results = append(results, p)
		}
	}
	return results
}

func countCrossings(p Pairing) int {
	count := 0
	normalized := make(Pairing, len(p))
	for idx, pair := range p {
		if pair[0] < pair[1] {
			normalized[idx] = pair
		} else {
			normalized[idx] = Pair{pair[1], pair[0]}
		}
	}

	for a := 0; a < len(normalized); a++ {
		for b := a + 1; b < len(normalized); b++ {
			i, j := normalized[a][0], normalized[a][1]
			k, l := normalized[b][0], normalized[b][1]

			if (i < k && k < j && j < l) || (k < i && i < l && l < j) {
				count++
			}
		}
	}
	return count
}

func canContract(a, b Op) bool {
	return a.Type == Annihilate && b.Type == Create
}
