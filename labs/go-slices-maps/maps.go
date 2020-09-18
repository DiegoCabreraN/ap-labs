package main

import (
	"strings"

	"golang.org/x/tour/wc"
)

func WordCount(s string) map[string]int {
	occurences := make(map[string]int)
	splited := strings.Fields(s)

	for _, val := range splited {
		occurences[val] += 1
	}
	return occurences
}

func main() {
	wc.Test(WordCount)
}
