========================================================================
# P3Brain Project Overview
========================================================================

The P3Brain Project enables optimization of Markov Brains using
the Parameterless Population Pyramid (P3) algorithm.

P3 optimizes bit strings that represent problem solutions using hill climbing,
dynamic population sizes, and LTGA crossover.

## Compiling the project

Use the Makefile if you can otherwise the build.bat script works on Windows if
you have the Microsoft Visual C++ Build Tools installed.

Both P3 and the Markov Brain project have their own main.cpp files that need to
be ignored during compilation. The P3Brain project has its own mainP3Brain.cpp.
The Makefile ignores the other two main files automatically but the build.bat does
not so you'll have to comment them out.

## Running an experiment

The P3Brain project essentially runs the main function from P3 so refer to the P3
README located in the P3Brain/FastEfficientP3 folder for more info.

```
$ p3brain.exe P3Brain/FastEfficientP3/config/default.cfg P3Brain/world.cfg
```

## How does this work and where is the code?

The P3Brain code is in the P3Brain folder. The P3 code is also located in the P3Brain
folder.

The P3Brain project adds an Evaluator class called MarkovWorld to the P3 project
which uses a specified Decoder to translate bit strings into Markov Brains
which are then tested in a world written to test an agent's fitness.

Both the Decoder type and test World are specified in the config file passed from
the command line.

### All about Decoders

Currently 4 types of Decoders exist:
    1. Unstructured
    2. Fixed Input
    3. Fixed Logic
    4. Hypercube

The names reflect how they allow gates to connect to each other if at all and
whether or not the gates can have unique internal logic.

Different methods of Decoding put restrictions on the length of the bit string and sometimes
on the number of gates that can be used. Error messages should be explicit enough to guide
you to a valid configuration. See below for a less trial and error method of understanding
what a valid configuration is.

To decrease the number of bits needed to encode a brain, each gate is only allowed
to write to itself a.k.a. it has one fixed output which does not need to be stored
in the bit string.

Additionaly, since P3 only works on fixed length bit strings, gates can not change their
number of inputs part way through the algorithm. The brain structure must remain consistent
unless you're willing to take a significant hit to efficiency in order to permit changes
to brain structure/size. This brings up a question P3 could be used to answer: Is the "un-used"
space in a Markov Brain's genome needed to achieve high fitness?

#### Unstructured Decoder

Gates can connect to any other gate (including input nodes) and can have any internal logic.
As such, the number of bits needed for each gate is

    bits_per_gate = log2(num_nodes) * num_inputs_per_gate + 2^num_inputs_per_gate

#### Fixed Input

This is a fairly trivial low capacity network where gates are only allowed to have inputs
from the nodes immediately preceding them in the node array but are allowed to have unique
internal logic.

    bits_per_gate = 2^num_inputs_per_gate

#### Fixed Logic

The opposite of Fixed Input, Fixed Logic Decoders force every gate to have only two inputs
with an internal logic of NOR. Each gate, however, can connect to any two nodes.
NOR was chosen because it is a "univeral logic gate" which means every other logic gate can
be made from a combination of NOR gates.

    bits_per_gate = log2(num_nodes) * 2

#### Hypercube

For this Decoder, each node (input, hidden, and output) represent the vertices of a hypercube.
Nodes can have any internal logic but can only connect to their neighbors in the hypercube.

    bits_per_gate = log2(dimension_of_hypercube) * num_inputs_per_gate + 2^num_inputs_per_gate

This structure allows for quite a bit of flexibility while still cutting down the number of bits
needed to represent each input to a gate. However as with hypercube structures in distributed
computing, this option is restrictive in its scale. For example, you can't have a hypercube with
20 nodes. You are forced to have numbers of nodes that are powers of 2. Even then, to be 100%
efficient with your input representations, the number of neighbors to any one node should
also be a power of two. Examples of this are a 4d hypercube (16 nodes), 8d (256 nodes) with each vertex
having 4 and 8 neighbors respectively.

You're not required to limit your hypercube dimension to a power of 2, if you only have 5 neighbors
you need at least 3 bits to distinguish between them. But since 3 bits allows you to represent 8
different numbers, those representations would wrap around. So if 0 equaled neighbor one then 5 would
also equal neighbor one.

### Where is the code?

Relevant files:
    - Evaluation.h
        Declares MarkovWorld Evaluator and creates string mappings that allow
        the 'world' config parameter to specify a test world at run time. Go
        here to add support for more worlds.
    - Evaluation.cpp
        This is where the MarkovWorld Evaluator is defined. Go here to see where
        bit string candidates suggested by P3 are turned into Markov Brains and
        given a fitness score by a world.
    - Decoder.h
        This is where all the Decoder types are declared and where the config parameter
        strings specifying which Decoder to use at runtime are declared.
    - Decoder.cpp
        Go here to see how each Decoder type creates gates for a Markov Brain from bit strings.
