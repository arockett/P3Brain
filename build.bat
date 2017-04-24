@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

if not exist ".\build\" mkdir build

set compilerflags=/Ox /Zi /EHsc /W2 /Fo".\build\\"

set linkerflags=/OUT:p3brain.exe

set sourcefiles=.\P3Brain\*.cpp^
 .\P3Brain\FastEfficientP3\src\*.cpp^
 .\Analyze\*.cpp^
 .\Archivist\*.cpp^
 .\Archivist\LODwAPArchivist\*.cpp^
 .\Archivist\SSwDArchivist\*.cpp^
 .\Brain\*.cpp^
 .\Brain\ConstantValuesBrain\*.cpp^
 .\Brain\HumanBrain\*.cpp^
 .\Brain\MarkovBrain\*.cpp^
 .\Brain\MarkovBrain\Gate\*.cpp^
 .\Brain\MarkovBrain\GateBuilder\*.cpp^
 .\Brain\MarkovBrain\GateListBuilder\*.cpp^
 .\Brain\WireBrain\*.cpp^
 .\Genome\*.cpp^
 .\Genome\CircularGenome\*.cpp^
 .\Genome\MultiGenome\*.cpp^
 .\Genome\MultiGenome\Chromosome\*.cpp^
 .\Group\*.cpp^
 .\Optimizer\*.cpp^
 .\Optimizer\GAOptimizer\*.cpp^
 .\Optimizer\Tournament2Optimizer\*.cpp^
 .\Optimizer\TournamentOptimizer\*.cpp^
 .\Organism\*.cpp^
 .\Utilities\*.cpp^
 .\World\*.cpp^
 .\World\LogicalWorld\*.cpp^
 .\World\BerryWorld\*.cpp^
 .\World\UtilityWorld\*.cpp

set includedirs=/I .\P3Brain^
 /I .\P3Brain\FastEfficientP3\src^
 /I .\Analyze^
 /I .\Archivist^
 /I .\Archivist\LODwAPArchivist^
 /I .\Archivist\SSwDArchivist^
 /I .\Brain^
 /I .\Brain\ConstantValuesBrain^
 /I .\Brain\HumanBrain^
 /I .\Brain\MarkovBrain^
 /I .\Brain\MarkovBrain\Gate^
 /I .\Brain\MarkovBrain\GateBuilder^
 /I .\Brain\MarkovBrain\GateListBuilder^
 /I .\Brain\WireBrain^
 /I .\Genome^
 /I .\Genome\CircularGenome^
 /I .\Genome\MultiGenome^
 /I .\Genome\MultiGenome\Chromosome^
 /I .\Group^
 /I .\Optimizer^
 /I .\Optimizer\GAOptimizer^
 /I .\Optimizer\Tournament2Optimizer^
 /I .\Optimizer\TournamentOptimizer^
 /I .\Organism^
 /I .\Utilities^
 /I .\World^
 /I .\World\LogicalWorld^
 /I .\World\BerryWorld^
 /I .\World\UtilityWorld

cl.exe %compilerflags% %includedirs% %sourcefiles% /link %linkerflags%
