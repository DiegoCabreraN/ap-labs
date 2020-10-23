// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 241.

// Crawl2 crawls web links starting with the command-line arguments.
//
// This version uses a buffered channel as a counting semaphore
// to limit the number of concurrent calls to links.Extract.
//
// Crawl3 adds support for depth limiting.
//
package main

import (
	"flag"
	"fmt"
	"log"
	"os"

	"gopl.io/ch5/links"
)

//!+sema
// tokens is a counting semaphore used to
// enforce a limit of 20 concurrent requests.
var tokens = make(chan struct{}, 20)

func crawl(lastPage Page) []Page {
	fmt.Printf("Page: %s Depth: %d\n", lastPage.link, lastPage.depth)
	tokens <- struct{}{} // acquire a token
	list, err := links.Extract(lastPage.link)
	<-tokens // release the token

	var pages []Page
	for i := 0; i < len(list); i++ {
		page := Page{
			link:  list[i],
			depth: lastPage.depth + 1,
		}
		pages = append(pages, page)
	}

	if err != nil {
		log.Print(err)
	}
	return pages
}

func listen(pagelist chan Page, maxDepth *int, file *string) {
	var n int
	n++
	f, err := os.Create(*file)
	if err != nil {
		fmt.Println(err)
		return
	}
	defer f.Close()

	seen := make(map[string]bool)
	for ; n > 0; n-- {
		page := <-pagelist
		if !seen[page.link] && page.depth <= *maxDepth {
			_, err := f.WriteString(page.link + "\n")
			if err != nil {
				f.Close()
				log.Fatal("Error while writing file")
			}
			seen[page.link] = true
			n++
			go func(page Page) {
				pages := crawl(page)
				for _, page := range pages {
					pagelist <- page
				}
			}(page)
		} else {
			f.Close()
			break
		}
	}
}

//!-sema

//Page is a structure to save page
type Page struct {
	link  string
	depth int
}

//!+
func main() {
	maxDepth := flag.Int("depth", 0, "How many layers deep you want to explore?")
	file := flag.String("results", "results.txt", "Wher do you want to save?")
	flag.Parse()

	pagelist := make(chan Page)

	if len(os.Args) > 3 {

		args := os.Args[3:]

		go listen(pagelist, maxDepth, file)

		for i := 0; i < len(args); i++ {
			page := Page{
				link:  args[i],
				depth: 0,
			}
			pagelist <- page
		}
		for {
		}
	} else {
		log.Panicf("Error on startup, please add the results and depth flags")
		return
	}

}

//!-
