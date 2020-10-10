package main

import (
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"strings"
)

func handler(clockName string, host string) {
	conn, err := net.Dial("tcp", host)
	if err != nil {
		log.Fatal(err)
	}
	defer conn.Close()
	mustCopy(os.Stdout, conn)
}

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}

func main() {
	if len(os.Args) == 1 {
		fmt.Printf("Check the arguments and try again.\n")
		return
	}
	for _, input := range os.Args[1:] {
		arg := strings.Split(input, "=")
		go handler(arg[0], arg[1])
	}
	for {
	}
}
