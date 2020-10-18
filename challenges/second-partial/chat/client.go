// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"flag"
	"io"
	"log"
	"net"
	"os"
)

func main() {
	var server string
	var username string

	flag.StringVar(&server, "server", "localhost:8000", "<host>:<port>")
	flag.StringVar(&username, "user", "", "user to login")
	flag.Parse()

	if username == "" {
		log.Fatal("You need a username in order to login")
	}

	conn, err := net.Dial("tcp", server)
	if err != nil {
		log.Fatal(err)
	}
	done := make(chan struct{})
	go func() {
		io.WriteString(conn, username+"\n")

		io.Copy(os.Stdout, conn) // NOTE: ignoring errors
		log.Println("Connection Closed")
		os.Exit(1)
		done <- struct{}{}
	}()
	mustCopy(conn, os.Stdin)
	conn.Close()
	os.Exit(1)
	<-done
}

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}
