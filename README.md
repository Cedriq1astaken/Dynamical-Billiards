# *Dynamical Billiards*

A C++ lightweight simulation of dynamical billiards, a classical system where a point particle moves freely inside a domain and reflects elastically off the boundaries deterministic chaos, stability, sensitivity to initial conditions, and geometric effects on trajectories.

## *Features*

- Simulation of multiple point particles moving with constant speed
- Elastic reflections using geometric reflection laws
- Multiple domain types
- Interactive visualization
- Adjustable initial position / velocity
- Stable numerical integrator using analytic intersection
- Rudementary implementation of quantum chaos using a Gaussian Wave Packet

## *Mathematics*

- Geometric reflection laws <br>
  $v_1 = v_0 - 2(v_0 \cdot n)n$ <br>
  where: <br>
  v0: Is the current velocity of the point particle <br>
  v1: Is the new velocity of the point particle <br>
  n : Normal of the intersection surface <br>
  
- Schrodinger's Equation <br>
  $i\hbar \frac{\partial}{\partial t}\Psi(\hat{x},t) = \hat{H}\Psi(\hat{x},t)$ <br>
  Solved the time dependent Schrodinger Equation (TDSE) using RK4 based of the implementation found on paper [1] 
  
- Guassian Wave Packet <br>
  $\Psi(\hat{x}, 0) = e^{-\left(\frac{|\hat{x}-\hat{r}_0|}{2\sigma}\right)^2} (\sin(xk) + i\cos(xk))$ [1]

# Libraries
- Raylib
- Dear ImGUI

## *Bibliography*

[1] "Numerical solution to the Time Dependant Schrodinger Equation" 
https://github.com/carloscerlira/Schrodinger-equation-numerical-solution/blob/master/paper.pdf

