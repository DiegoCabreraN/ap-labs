package main

import (
	"fmt"
	"sync"
	"time"
)

var (
	startTime     time.Time
	comunications int
	message1      chan int
	message2      chan int
)

func pong(id int, wg *sync.WaitGroup) {
	defer wg.Done()

	for {

		if id == 1 {
			<-message2
			message1 <- 1
			comunications++
		} else {
			<-message1
			message2 <- 2
			comunications++
		}

		transcurredTime := time.Now().Sub(startTime).Seconds()

		if transcurredTime > 1.0 {
			if id == 1 {
				<-message2
				comunications++
			} else {
				<-message1
				comunications++
			}
			break
		}

	}
	wg.Done()

}

func main() {
	var commsPerSecond int
	var wg sync.WaitGroup

	startTime = time.Now()

	message1 = make(chan int)
	message2 = make(chan int)

	wg.Add(2)
	go pong(1, &wg)
	go pong(2, &wg)

	message1 <- 1

	wg.Wait()

	commsPerSecond = comunications
	fmt.Println("Communications Per Second : ", commsPerSecond)
}
