package main

import "fmt"

func main() {
	// fmt.Printf("Ray tracing in One Weekend\n")

	imageWidth := 250
	imageHeight := 250

	fmt.Printf("P3\n%v %v\n255\n", imageWidth, imageHeight)

	for j := 0; j < imageHeight; j++ {
		for i := 0; i < imageWidth; i++ {
			r := float64(i) / float64(imageWidth-1)
			g := float64(j) / float64(imageHeight-1)
			b := float64(0)

			ir := int(255.99 * r)
			ig := int(255.99 * g)
			ib := int(255.99 * b)

			fmt.Printf("%v %v %v\n", ir, ig, ib)
		}
	}
}
