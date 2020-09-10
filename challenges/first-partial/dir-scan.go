package main

import (
	"fmt"
	"os"
	"path/filepath"
	"text/tabwriter"
)

// scanDir stands for the directory scanning implementation
func scanDir(dir string) error {
	directoryCount := 0
	symlinkCount := 0
	deviceCount := 0
	socketCount := 0
	otherCount := 0

	err := filepath.Walk(dir, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		switch fileInfo := info.Mode(); {
		case fileInfo.IsDir():
			directoryCount++
		case fileInfo&os.ModeSymlink != 0:
			symlinkCount++
		case fileInfo&os.ModeDevice != 0:
			deviceCount++
		case fileInfo&os.ModeSocket != 0:
			socketCount++
		default:
			otherCount++
		}
		return nil
	})

	if err != nil {
		panic(err)
	}

	w := tabwriter.NewWriter(os.Stdout, 8, 8, 1, '-', 0)
	fmt.Printf("  Directory Scanner Tool\n")
	fmt.Fprintf(w, "+\t+\t+\n")
	fmt.Fprintf(w, "| Path \t| %s \t|\n", dir)
	fmt.Fprintf(w, "+\t+\t+\n")
	fmt.Fprintf(w, "| Directories \t| %d \t|\n", directoryCount)
	fmt.Fprintf(w, "| Symbolic Links \t| %d \t|\n", symlinkCount)
	fmt.Fprintf(w, "| Devices \t| %d \t|\n", deviceCount)
	fmt.Fprintf(w, "| Sockets \t| %d \t|\n", socketCount)
	fmt.Fprintf(w, "| Other files \t| %d \t|\n", otherCount)
	fmt.Fprintf(w, "+\t+\t+\n")
	w.Flush()
	return nil
}

func main() {

	if len(os.Args) < 2 {
		fmt.Println("Usage: ./dir-scan <directory>")
		os.Exit(1)
	}

	scanDir(os.Args[1])
}
