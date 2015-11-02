# BasicMarkovBrainTemplate
basic template for Markov Brain evolution of intelligence



This project is broken up into the following:

main.cpp - includes everything and runs either Evolve Loop or Analyse

BaseCode - the core files used to run Markov Brains - These files should only be pulling fro Utilities
	Agent.* - creates and runs agents (agents are generated from genomes)
	Data.* - stores experiment data and handles output And maintains global variables
		will be broken up into (Utilities/Data.* and BaseCode/Global.*)
	Evolve.* - (WIP) runs the Evolution Loop
	Gate.* - creates and runs gates (gates are generated from genomes when agents are created)
	Genome.* - creates, mutates, etc. genomes
	Optimizer.* - methods for picking offspring for new generations
	World.* - a blank template world. Worlds will be included from the World Directory

Worlds - definitions of worlds (test environments)
	RedBlueBerryWorld (will change to something like GridWorld) - A world defined as a grid which
	may be bound by walls. The world may have random walls, resources with benefit and poison with
	cost. A Change cost can be charged when switching between different resources.

Analyse - methods used to Analyse output from Markov Brain runs
	Analyse.* - contains methods to analyse output data
	MutationAnalyse* - contains methods to test single point mutations to generate Mutation
	Distribution Effect (can be used to search for mutational robustness)

Utilities - Standalone tools.
	Utilities.h - simple tools (mostly inline functions)
	Random.* - methods for generating random values.


Contributors to this project include
Initial Design and Managment: Arend Hintze
Development and Maintenance: Cliff Bohm
Other: Brian Goldman (parameter configuration, advice)

