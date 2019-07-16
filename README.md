# SGCP via Cliques
Solving the (Weighted) Selective Graph Colouring Problem via Maximum Cliques

## Instances
Instances are distributed in folder `generator/graphs`, which contains both the original VCP instances and the derived SGCP and WSGCP instances.
The programe used to generate the instance is also available, in folder `generator`.

## Results
The table below synthetically compares a Mixed-Integer model (see the paper) and the line-graph method.
Columns under "MIP" refer to the model, while columns under "LG" refer to the line-graph method.
Columns "Opt" give the number of optimal solutions found by each method, while column "Gap%" reports the average gap, computed as UB-LB/UB.
UB is the value of the incumbent solutionand LB is the best dual bound.
Columns "Time" report the average running time in seconds.
We ran both algorithms with a time limit of 1 hour and include the unsolved instances in the computation of the average time.
For the line-graph algorithm, we report under columns "nV" and "nE",respectively, the average number of vertices and edges of the final graphs on which we solve the Maximum Weighted Stable Set Problem.
The line-graph algorithm is able to solve all instances very quickly, whereas using the Mixed-Integer model leads to having unsolved instances and larger average times.
Our approach solves all the 531 instances in less than 12 seconds in average, whereas the direct use of Cplex only solves 509 instances in less than 1 hour.

| #V | MIP Opt | MIP Gap% | MIP Time | LG nV | LG nE | LG Opt | LG Gap% | LG Time |
|---:|     ---:|      ---:|      ---:|   ---:|   ---:|    ---:|     ---:|     ---:|
|10       | 45  | 0     | 0.05                | 21    | 132        | 45  | 0     | 0.00    |
|20 	    | 54  | 0     | 0.20                | 82 	  | 2'115      | 54  | 0     | 0.00    |
|30 	    | 54  | 0     | 0.79                | 189 	| 10'795     | 54  | 0     | 0.01    |
|40 	    | 54  | 0     | 3.02                | 335 	| 37'078     | 54  | 0     | 0.02    |
|50 	    | 53  | 0.8   | 90.22               | 523 	| 87'929     | 54  | 0     | 0.24    |
|60 	    | 51  | 2.1   | 204.63              | 750 	| 188'612    | 54  | 0     | 0.28    |
|70 	    | 49  | 3.5   | 336.07              | 1021  | 342'170    | 54  | 0     | 2.45    |
|80 	    | 50  | 3.8   | 271.67              | 1324  | 591'088    | 54  | 0     | 1.48    |
|90 	    | 49  | 5.1   | 363.18              | 1688  | 942'813    | 54  | 0     | 12.37   |
|100 	    | 50  | 3.7   | 281.82              | 2073  | 1'453'197  | 54  | 0     | 5.62    |

## Paper
A working draft of the paper is available on [my website](https://santini.in/).
