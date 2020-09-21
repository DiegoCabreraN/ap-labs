package main

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"
	"time"
)

type Point struct{ x, y float64 }

func (p Point) X() float64 {
	return p.x
}

func (p Point) Y() float64 {
	return p.y
}

func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.X()-p.X(), q.Y()-p.Y())
}

type Path []Point

func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			increment := path[i-1].Distance(path[i])
			fmt.Printf("%.2f + ", increment)
			sum += increment
		}
	}
	increment := path[len(path)-1].Distance(path[0])
	fmt.Printf("%.2f = ", increment)
	sum += increment
	return sum
}

func onSegment(p Point, q Point, r Point) bool {
	if q.x <= math.Max(p.x, r.x) && q.x >= math.Min(p.x, r.x) &&
		q.y <= math.Max(p.y, r.y) && q.y >= math.Min(p.y, r.y) {
		return true
	}
	return false
}

func orientation(p Point, q Point, r Point) int {
	val := ((q.y - p.y) * (r.x - q.x)) - ((q.x - p.x) * (r.y - q.y))

	if val == 0 {
		return 0
	}
	if val > 0 {
		return 1
	}
	return 2

}

func doIntersect(p1 Point, q1 Point, p2 Point, q2 Point) bool {
	o1 := orientation(p1, q1, p2)
	o2 := orientation(p1, q1, q2)
	o3 := orientation(p2, q2, p1)
	o4 := orientation(p2, q2, q1)

	if (o1 != o2) && (o3 != o4) {
		return true
	}

	if (o1 == 0) && (onSegment(p1, p2, q1)) {
		return true
	}
	if (o2 == 0) && (onSegment(p1, q2, q1)) {
		return true
	}
	if (o3 == 0) && (onSegment(p2, p1, q2)) {
		return true
	}
	if (o4 == 0) && (onSegment(p2, q1, q2)) {
		return true
	}
	return false
}

func (path Path) verify() bool {
	intersection := false
	for x := 0; x < len(path)-3; x++ {
		if doIntersect(path[x], path[x+1], path[x+2], path[x+3]) {
			intersection = true
			break
		}
	}
	return intersection
}

func generatePath(sides int) Path {
	var path Path
	for i := 0; i < sides; i++ {
		rand.Seed(time.Now().UnixNano())
		point := Point{
			x: -100 + rand.Float64()*200,
			y: -100 + rand.Float64()*200,
		}
		path = append(path, point)
		for path.verify() {
			path[i] = Point{
				x: -100 + rand.Float64()*200,
				y: -100 + rand.Float64()*200,
			}
		}
	}
	return path
}

func main() {
	if len(os.Args) > 1 {
		sides, err := strconv.Atoi(os.Args[1])
		if err != nil {
			fmt.Println(err)
			return
		}
		if sides < 3 {
			err = fmt.Errorf("Error while generating figure, please check the sides parameter")
			fmt.Println(err)
			return
		}
		fmt.Printf("- Generating a [%d] sides figure\n", sides)
		fmt.Println("- Figure's vertices")
		path := generatePath(sides)
		for v := 0; v < len(path); v++ {
			fmt.Printf("  - ( %.2f, %.2f)\n", path[v].X(), path[v].Y())
		}
		fmt.Print("- Figure's Perimeter\n  - ")
		perimeter := path.Distance()
		fmt.Printf("%.2f\n", perimeter)
	} else {
		err := fmt.Errorf("Error, function needs more arguments")
		fmt.Println(err)
		return
	}
}
