package main

import (
	"fmt"
	"runtime"
	"time"
)

var (
	stages int64
)

func pipeTest() (int, int64) {
	var executionTime int
	genericChannel := make(chan int64)
	startTime := time.Now()

	defer func(startTime time.Time) {
		if goroutine := recover(); goroutine != nil {
			fmt.Println("Maximum number of pipeline stages   : ", stages)
			startTime := time.Now()
			for ; stages > 0; stages-- {
				genericChannel <- stages
			}
			executionTime = int(time.Now().Sub(startTime).Seconds())
			fmt.Println("Time to transit trough the pipeline : ", executionTime)
		}
	}(startTime)

	for {
		go func() {
			stages++
			<-genericChannel
		}()
		printMemUsage()
	}

}

func printMemUsage() { //Limit Process Memory Usage
	var m runtime.MemStats
	runtime.ReadMemStats(&m)
	if m.TotalAlloc/1024/1024 > 1024 {
		panic(fmt.Sprintf("MAX MEMORY REACHED"))
	}
}

func main() {
	runtime.GOMAXPROCS(1)
	pipeTest()
	runtime.GC()
}
