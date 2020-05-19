# Estimating Pi using Monte Carlo Simulations

## Chase Toyofuku-Souza
> 2296478

> toyofukusouza@chapman.edu

----
### About
A C program that creates a separate thread to generate a number of random points. The thread will count the number of points that occur within the circle and store that result in a global variable. When this thread has exited, the parent thread will calculate and output the estimated value of π. As a general rule, the greater the number of points, the closer the approximation to π.

### Files
- mcarlo.c

### Instructions
- gcc mcarlo.c -o mcarlo -pthread -lm
- ./mcarlo <number of points>

### Notes
- Estimates pi based on the Monte Carlo technique, which uses a circle inscribed in a 2x2 square, with the origin at (0,0).
- Randomly generate a number of points inside the bounds of the square
- Pi is estimated with the following formula:
	- π = 4 x (number of points in circle)/(total number of points)