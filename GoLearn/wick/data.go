package main

import (
	"fmt"
	"strings"
)

// OpType identifies if an operator is Create or Annihilate
type OpType int

const (
	Create OpType = iota
	Annihilate
)

// Op represents a Fermionic operator
type Op struct {
	Type  OpType
	Index string
}

func (o Op) String() string {
	if o.Type == Create {
		return fmt.Sprintf("a^{%s}", o.Index)
	}
	return fmt.Sprintf("a_{%s}", o.Index)
}

// Delta represents a Kronecker delta
type Delta struct {
	I, J string
}

func (d Delta) String() string {
	return fmt.Sprintf("delta^{%s}_{%s}", d.I, d.J)
}

// Expr represents a term in the expansion
type Expr struct {
	Coeff  float64
	Ops    []Op
	Deltas []Delta
}

func (e Expr) String() string {
	if e.Coeff == 0 {
		return "0"
	}
	var res []string
	// Only show coefficient if it's not 1.0 or if the term is just a scalar
	if e.Coeff != 1.0 || (len(e.Ops) == 0 && len(e.Deltas) == 0) {
		res = append(res, fmt.Sprintf("%.1f", e.Coeff))
	}
	for _, d := range e.Deltas {
		res = append(res, d.String())
	}
	for _, o := range e.Ops {
		res = append(res, o.String())
	}
	return strings.Join(res, " ")
}

func Fannx(index string) Op {
	return Op{Type: Annihilate, Index: index}
}

func Fcrex(index string) Op {
	return Op{Type: Create, Index: index}
}

func ExprFromOps(ops []Op) Expr {
	return Expr{
		Coeff:  1.0,
		Ops:    ops,
		Deltas: []Delta{},
	}
}
