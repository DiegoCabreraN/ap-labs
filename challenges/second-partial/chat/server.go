// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"net"
	"strings"
	"time"
)

// User is a structure to save user data
type User struct {
	name      string
	isAdmin   bool
	kicked    bool
	conn      net.Conn
	loginTime string
	client    chan string
}

//!+eventHandler
type client chan<- string // an outgoing message channel

var (
	adminSet = false
	allUsers = make(chan *User)
	entering = make(chan *User)
	leaving  = make(chan *User)
	process  = make(chan string)
	clients  = make(map[string]*User)
)

func setAdmin(user *User) {
	log.Printf("irc-server> [%s] was promoted as the channel ADMIN", user.name)
	user.client <- "irc-server> Congrats, you were the first user."
	user.client <- "irc-server> You're the new IRC Server ADMIN"
	user.isAdmin = true
	adminSet = true
}

func closeHandler() {
	for {
		select {
		case user := <-leaving:
			delete(clients, user.name)
			user.conn.Close()
			if user.isAdmin && len(clients) > 0 {
				for cli := range clients {
					clients[cli].isAdmin = true
					clients[cli].client <- "You are the new IRC Server ADMIN"
					log.Printf("irc-server> [%s] was promoted as the channel ADMIN\n", clients[cli].name)
					break
				}
			} else if user.isAdmin {
				adminSet = false
			}
		}
	}
}

func eventHandler() {
	for {
		select {
		case p := <-process:
			if len(p) > 0 {
				line := strings.Split(p, "> ")
				if line[1][0] == '/' {
					if line[1] == "/users" {
						for user := range clients {
							clients[line[0]].client <- "irc-server> " + clients[user].name + " - Connected since " + clients[user].loginTime
						}
					} else if line[1][0:4] == "/msg" {
						data := strings.SplitN(line[1], " ", 3)
						if clients[data[1]] == nil {
							clients[line[0]].client <- "irc-server> This user does not exist"
						} else {
							clients[data[1]].client <- clients[line[0]].name + " #private channel > " + data[2]
						}
					} else if line[1] == "/time" {
						t := time.Now()
						zone := t.Location().String()
						clients[line[0]].client <- "Local time: " + zone + " " + t.Format("15:04")
					} else if line[1][0:5] == "/user" {
						data := strings.SplitN(line[1], " ", 3)
						if clients[data[1]] == nil {
							clients[line[0]].client <- "irc-server> This user does not exist"
						} else {
							username := clients[data[1]].name
							ip := clients[data[1]].conn.RemoteAddr().String()
							loginTime := clients[data[1]].loginTime
							clients[line[0]].client <- "irc-server> username: " + username + ", IP: " + ip + " Connected since: " + loginTime
						}
					} else if line[1][0:5] == "/kick" {
						data := strings.SplitN(line[1], " ", 3)
						if clients[data[1]] == nil {
							clients[line[0]].client <- "irc-server> This user does not exist"
						} else if !clients[line[0]].isAdmin {
							clients[line[0]].client <- "irc-server> Only the admin can kick users out of the channel"
						} else {
							clients[data[1]].client <- "irc-server> You're kicked from this channel"
							clients[data[1]].client <- "irc-server> Bad language is not allowed on this channel"
							for user := range clients {
								if user != data[1] {
									clients[user].client <- "irc-server> [" + clients[data[1]].name + "] was kicked from this channel for bad language policy violation"
								} else {
									clients[user].kicked = true
									leaving <- clients[user]
								}
							}
						}
					} else {
						for user := range clients {
							clients[user].client <- p
						}
					}
				} else {
					for user := range clients {
						if user != line[0] {
							clients[user].client <- p
						}
					}
				}
			}
		case user := <-entering:
			log.Printf("irc-server> New connected user [%s]", user.name)
			clients[user.name] = user
		}
	}
}

//!-eventHandler

//!+handleConn
func handleConn(conn net.Conn) {
	ch := make(chan string)
	go clientWriter(conn, ch)
	input := bufio.NewScanner(conn)
	input.Scan()

	name := input.Text()

	handleUser := User{
		name:      name,
		isAdmin:   false,
		kicked:    false,
		conn:      conn,
		loginTime: time.Now().String()[0:19],
		client:    ch,
	}

	handleUser.client <- "irc-server> Welcome to the Simple IRC Server"
	handleUser.client <- "irc-server> Your user [" + handleUser.name + "] is successfully logged"
	process <- "irc-server> New connected user [" + handleUser.name + "]"
	entering <- &handleUser

	if !adminSet {
		setAdmin(&handleUser)
	}

	for input.Scan() {
		process <- handleUser.name + "> " + input.Text()
	}
	// NOTE: ignoring potential errors from input.Err()
	if !handleUser.kicked {
		leaving <- &handleUser
		process <- "irc-server> [" + handleUser.name + "] has left"
	}
	conn.Close()
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

//!-handleConn

func main() {
	var host string
	var port int

	flag.StringVar(&host, "host", "localhost", "Host to serve")
	flag.IntVar(&port, "port", 8000, "Port to use")
	flag.Parse()

	server := fmt.Sprintf("%s:%d", host, port)

	listener, err := net.Listen("tcp", server)
	if err != nil {
		log.Fatal(err)
	}

	log.Printf("irc-server> Simple IRC Server started at %s", server)
	log.Printf("irc-server> Ready for recieving new clients")

	go eventHandler()
	go closeHandler()
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}
