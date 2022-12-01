# Recursive Systems General Concepts

## Terminology Related to Recursive Systems

A recursive system is a system of equations where each equation at a given timestep depends on some equations in the recursive system evaluated at the previous timestep.

A "state" within the context of recursive systems is a vector of values.

A "seed" is an initial state, system pair.

A "future" is a chronologically ordered collection of all states occuring for a given seed. The future consists of states occuring at positive timesteps only, and does not include the initial state since that is typically designated timestep zero.

Simulation is the manual derivation of states existing within the future of a given seed. (Repeated evaluation of the system on the current state, updating the current state at each timestep)

A closed recursive system is a system where the __only__ thing each equation depends on is equations within that system evaluated at the previous timestep.

A binary recursive system is a system where the domain and range of the system are binary strings of size $n$, where $n$ is the size of the system.

### Typical lingo:

Sometimes a recursive system of size $n$ will be written in shorthand as an $n$-system.

### The below definitions only apply to __closed__ recursive systems

The transience of a future is a positive integer defined as the number of timesteps it takes for the first duplicate state to appear in the future. It can more simply be described as the number of evaluations it takes for a repeated state to FIRST appear with a given seed, or even more simply, the transience of a system is equal to __the timestep at which__ the first duplicate state in the future occurs.

The period of a future is defined as the number of timesteps in between duplicate states within the future.

## Facts Related to Recursive Systems

For a given binary seed of size $n$, the maximum possible number of timesteps it will take for that future to be forced to repeat itself is $2^n$. This is because there are exactly $2^n$ binary strings of size $n$. Due to the pigeonhole principle, a repeat will be forced to occur before or at timestep $2^n$.

## Minterm Matrices

One of the most expanded forms in which we can represent a system is called a minterm matrix. This representation also prevents the existence of multiple representations for the same system.

The idea of minterm matrices starts with the idea for a minterm sum. Every binary equation can be represented as a sum of minterms (or product of maxterms.) In boolean algebra, these allign with DNF (Disjunctive Normal Form), and CNF (Conjunctive Normal Form), respectively.

Since a closed binary recursive system is really just an ordered collection of binary equations relating to substates at the previous timestep, we can express each equation in this system as a minterm sum.

Each minterm sum can be expressed as a binary vector, where each bit in the vector represents exclusion (0) of a minterm in its sum, or inclusion (1) of a minterm in its sum.

Treating these minterm vectors as rows, (with there being one for each equation in the system), we are left with a ($n$ x $2^n$) matrix. Why $2^n$ columns? It's because there exist $2^n$ minterms for a system of size $n$.

Each distinct system has only one minterm matrix representation, and each distinct minterm matrix has only one corresponding system that it represents.

Minterm matrices express an interesting behavior: the values in a given column represent the state that will follow from the input of a certain state to the system. That state is one which will satisfy the minterm associated with the selected column.

### Higher order minterm matrices and systems

Higher order minterm matrices are those which result from taking a system and composing it with itself at least once. The idea of higher order minterm matrices is that the original system itself is a binary function which maps from $n$ inputs to $n$ outputs, and SO IS the recursive system composed with itself. 

A minterm matrix of order 2 will generate states two timesteps into the future compared to another. This higher order minterm matrix will perfectly generate future states which will equal those of the original system at every other timestep. For this reason, one could assume that there could exist a higher order binary system for every binary system, and that in fact, for some binary recursive systems, they may act as a higher order system for some systems which produce timesteps at a higher frequency.

Just a thought, but it could be the case that first order systems (those which have maximum frequency and for which no higher resolution system could be generated) may be systems which express full transience.

> Minterm matrices are useful for studying the behavior of recursive systems, since they allow for trivial modification of systems and comparison of two or more systems (bitwise comparisons).

## Minterm Flow

The idea of minterm flow comes from the idea that each state in the future of a recursive system will flow to exactly one proceeding state.

The concept was originally derived by staring at a minterm matrix for long enough, after realizing that each column represents the state that will follow from another. Since each state in the system leads to exactly one proceeding state, we can create a graph which represents state flow, or when thinking about things in the sense of satisfaction of minterms, we could call it _minterm flow_.

Since each minterm flows to exactly one other minterm (the out-degree of every vertex is 1), when generating the adjacency matrix for the graph, each row of the matrix will only have a single position of a 1, and the rest will be zeroes.

### Upside:

Raising this matrix to high degrees will yield future states given initial states in $O(log_2(t))$ time. Also, each row in the matrix represents a different initial state for the system. Raising this matrix to high powers shows how EVERY single initial state will flow to future states, all at once.

### Downside:

This seems to require the construction of the entire graph (adjacency matrix). This means that every single state in the domain ($2^n$ total minterms) will have to have their own vertex, and every input state will have to be run through the system, signifying that there will have to be $O(2^n)$ computations in order to construct the graph. Funnily enough, $2^n$ is actually the number of times a system of size $n$ must to be evaluated to force a repeat of a system that expresses full transience, and so this could at BEST require the SAME number of computations to achieve as pure simulation when you are only trying to use this method for the evaluation of a future state for a single initial state.

### Final notes on pros/cons

Each evaluation of the system can be done in parallel, meaning that the overall process of evaluating future states may be able to be done faster than pure simulation, for some systems. However, for large systems (1000+ substates), this quickly becomes intractible Furthermore, this can be shown to be useless since in effect, the goal of a useful recursive system is to yield useful results (futures states). If we are (in parallel) computing all states that exist within the range of the system, then why could we not simultaneously at evaluation time check the quality of the output state and then on synchronization, compare the qualities to select the best state for operation? This would be even faster than compiling them into a matrix to then perform matrix multiplication with.

## Future Mapping

Future mapping means being able to go from a seed for which the future is known, and a different seed, and being able predict states in the future of the different seed given only information about the initial seed, initial future, and final seed, thereby avoiding much unnecessary compute which would have to be done to get from the final seed to the final future states through simulation.

Both systems involved in the seeds can be described by minterm matrices, and the initial states can be described using binary vectors.

The change in system minterm matrix can be derived by the element-wise XOR of the initial and final minterm matrices.

If you hold the initial states constant and only change the system, one could derive a minterm matrix describing the way that a select future state at a timestep of focus changes depending on initial state to the system.
