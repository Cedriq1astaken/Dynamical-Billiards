# *Dynamical Billiards*

A C++ lightweight simulation of dynamical billiards, a classical system where a point particle moves freely inside a domain and reflects elastically off the boundaries deterministic chaos, stability, sensitivity to initial conditions, and geometric effects on trajectories.

## *Features*

- Simulation of multiple point particles moving with constant speed
- Elastic reflections using geometric reflection laws
- Multiple domain types
- 
- Interactive visualization
- Adjustable initial position / velocity
- Stable numerical integrator using analytic intersection
- Rudementary implementation of quantum chaos using a Gaussian Wave Packet

## *Mathematics*

- Geometric reflection laws
  v1 = v0−2(v0⋅n)n
  where:
  v0: Is the current velocity of the point particle
  v1: Is the new velocity of the point particle
  n : Normal of the intersection surface
- Schrodinger's Equation
  
  Solved the time dependent Schrodinger Equation (TDSE) using RK4 based of the implementation found on paper [1]
- Guassian Wave Packet
  \Psi(\hat{x}, 0) = e^{-\left(\frac{|\hat{x}-\hat{r}_0|}{2\sigma}\right)^2} (\sin(xk) + i\cos(xk))



## *Bibliography*

[1] "Numerical solution to the Time Dependant Schrodinger Equation" 
https://github.com/carloscerlira/Schrodinger-equation-numerical-solution/blob/master/paper.pdf

