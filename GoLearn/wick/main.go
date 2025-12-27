package main

import (
	"fmt"
)

func main() {
	// 定义算符 (使用刚才定义的辅助函数)
	ap3 := Fannx("p3")
	ap4 := Fannx("p4")
	cp1 := Fcrex("p1")
	cp2 := Fcrex("p2")

	// 构造表达式: a_{p3} a_{p4} a^{p1} a^{p2}
	// Go 没有 Rust 的 .into()，我们写一个显式的转换函数
	expr := ExprFromOps([]Op{ap3, ap4, cp1, cp2})

	fmt.Println("Original Expression:")
	fmt.Printf("  %s\n", expr)

	// 使用链式调用计算 Wick 全收缩
	// 注意：Go 的方法如果返回指针，可以直接链式调用
	wt := NewWickTheorem(expr)
	resultStr := wt.
		SetFullContractions(true).
		Compute().
		ToString()

	fmt.Println("\nWick Full Contraction Result:")
	fmt.Printf("  %s\n", resultStr)
}
