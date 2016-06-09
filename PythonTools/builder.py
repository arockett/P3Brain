import matplotlib.pyplot as plt
import numpy as np
import math
from matplotlib.backends.backend_pdf import PdfPages
import matplotlib.font_manager as font_manager
import pandas
import sys
import getopt
#from scipy import stats
import ast
import argparse


parser = argparse.ArgumentParser()


parser.add_argument('-buildFile', type=str, metavar='FILE', default = 'buildOptions.txt',  help=' name of file with build options - default : buildOptions.txt', required=False)
#parser.add_argument('-conditions', type=str, metavar=('CONDITION'), default = [''],  help='names of condition directories - default: none (will use files in path directly)',nargs='+', required=False)
#parser.add_argument('-conditionNames', type=str, metavar=('CONDITION_NAME'), default = [''],  help='names to dispaly. must have same number of elements as conditions if defined - default: none (will use conditionNames)', nargs='+',required=False)
#parser.add_argument('-files', type=str, metavar='FILE_PREFIX', default = ['ave','dominant'], help='file prefex(s) - default: ave dominant', nargs='+', required=False)
#parser.add_argument('-repRange', type=int, metavar=('FIRST','LAST'), default = [1,0],  help='replicate range - default: none (will use files in path directly)', nargs=2, required=False)
#parser.add_argument('-repList', type=str, metavar='REP', default = [],  help='replicate list. useful if you are missing a replicat. cannot be used with repRange - default: none (will use files in path directly)', nargs='+', required=False)
#
#parser.add_argument('-save', type=str, choices=('pdf','png'), default = '',  help='save files rather then display as either pdf or png - default: none (display image)', required=False)
#
#parser.add_argument('-data', type=str, metavar='COLUMN_NAME', default = [''],  help='column names of data to be graphed - default : none (will attempt to graph all columns from all files)',nargs='+', required=False)
#parser.add_argument('-dataFromFile', type=str, metavar='FILE_NAME', default = 'ave',  help='this file will be used to determine with column names of data will be graphed. If this file is not in files, then all data will be plotted - default : ave', required=False)
#parser.add_argument('-xAxis', type=str, metavar='COLUMN_NAME', default = 'update',  help='column name of data to be used on x axis - default : update', required=False)
#parser.add_argument('-dataIndex', type=str, metavar='COLUMN_NAME', default = 'update',  help='column name of data to be used as index when generating averages - default : update', required=False)
#
#parser.add_argument('-pltWhat', type=str, metavar='{ave,error,reps}',choices=('ave','error','reps'), default = ['ave','error'], help='what should be ploted. ave (averages), error, reps (show data for all reps) - default : ave error', nargs='+', required=False)
#parser.add_argument('-pltStyle', type=str, choices=('line','point','randomLine','randomPoint'), default = 'line', help='plot style. Random is useful if plotting multiple data on the same plot - default : line', required=False)
#parser.add_argument('-errorStyle', type=str, choices=('region','bar','barX','barXY'), default = 'region', help='how error is ploted - default : region', required=False)
##parser.add_argument('-errorMethod', type=str, choices=('stderr'), default = ['stderr'], help='what error is ploted - default : region', required=False)
#
#parser.add_argument('-numCol', type=str, metavar='#', default = '3', help='if ploting a multi plot (default), how many columns in plot - default : 3', required=False)
#parser.add_argument('-combineConditions', action='store_true', default = False, help='if ploting multiple conditions, adding this flag will combine data from files with same name - default (if not set) : OFF', required=False)
#parser.add_argument('-combineData', action='store_true', default = False, help='if ploting multiple data lines, adding this flag will combine data into one plot - default (if not set) : OFF', required=False)
#
#parser.add_argument('-verbose', action='store_true', default = False, help='adding this flag will provide more text output while running (useful if you are working with a lot of data to make sure that you are not hanging) - default (if not set) : OFF', required=False)
#
#parser.add_argument('-legendLocation', type=str, choices=('ur','ul','lr','ll','cr','cl','lc','uc','c'), default = 'lr', help='if ledgends are needed this is determins placement (first letter u = upper, c = center, l = lower. second letter l = left, c = center, r = right) - default : lr (lower right)', required=False)
#parser.add_argument('-legendLinesWeight', type=int, default = -1, help='changes line thickness in legend - default : lineWeight', required=False)
#parser.add_argument('-lineWeight', type=int, default = 1, help='changes line thickness of lines in plots - default : 1', required=False)
#
#parser.add_argument('-grid', action='store_true', default = False, help='if set, this flag cause a grid to be displayed on plots - default : OFF', required=False)
#parser.add_argument('-fontSizeMajor', type=int, default = 15, help='size of "Major" fonts (main title) - default : 15', required=False)
#parser.add_argument('-fontSizeMinor', type=int, default = 10, help='size of "Minor" fonts (subplot titles and lables) - default : 10', required=False)
#parser.add_argument('-fontSizeTicks', type=int, default = 8, help='size of font for axis ticks - default : 8', required=False)
#
#parser.add_argument('-whereValue', type=str, default = 'update', help='only plot data where this column has values defined by whereRange - default : update', required=False)
#parser.add_argument('-whereRange', type=int, default = [], help='only plot data where column with name set by whereValue has values defined this range. Single value, just this value. Two values, inclusive range. Three values, inclusive range with step. - default : none', nargs='+', required=False)
#
## trick to allow "-" in an argument name! Parse it seperatly and then remove from sys.argv
#tempPltStyle = '-'
#if ('-pltStyle' in sys.argv):
#	pltStyleIndex = sys.argv.index('-pltStyle')
#	tempPltStyle = sys.argv[pltStyleIndex+1]
#	sys.argv.remove(sys.argv[pltStyleIndex+1])
#	sys.argv.remove('-pltStyle')
## end of trick	

args = parser.parse_args()
#args.pltStyle = tempPltStyle

# load all lines from buildFile into lines, ignore blank lines
file = open(args.buildFile, 'r')
lines = [line.rstrip('\n').split() for line in file if line.rstrip('\n').split() not in [[],['EOF']] ]
file.close()

options = {'World':[],'Genome':[],'Brain':[],'Optimizer':[],'Archivist':[],}
currentOption = ""

for line in lines:
	if len(line) != 2:
		exit()
	if line[0] == '%':
		currentOption = line[1]
	if line[0] == '*':
		options[currentOption].append(line[1])
		options[currentOption][0], options[currentOption][len(options[currentOption])-1] = options[currentOption][len(options[currentOption])-1], options[currentOption][0]
	if line[0] == "+":
		options[currentOption].append(line[1])


print("Building MABE with:")
print()
for option in options:
	print(option)
	for o in options[option]:
		print("  " + o)
	print()

outFile = open("modules.h", 'w')

outFile.write('// This file has been auto-generated by builder.py\n\n')

outFile.write('#ifndef __AutoBuild__Modules__\n')
outFile.write('#define __AutoBuild__Modules__\n')
outFile.write('\n#include "World/AbstractWorld.h"\n')
for option in options["World"]:
	outFile.write('#include "World/'+option+'World/'+option+'World.h"\n')
outFile.write('\n#include "Genome/AbstractGenome.h"\n')
for option in options["Genome"]:
	outFile.write('#include "Genome/'+option+'Genome/'+option+'Genome.h"\n')
outFile.write('\n#include "Brain/AbstractBrain.h"\n')
for option in options["Brain"]:
	outFile.write('#include "Brain/'+option+'Brain/'+option+'Brain.h"\n')
outFile.write('\n#include "Optimizer/AbstractOptimizer.h"\n')
for option in options["Optimizer"]:
	outFile.write('#include "Optimizer/'+option+'Optimizer/'+option+'Optimizer.h"\n')
outFile.write('\n#include "Archivist/DefaultArchivist.h"\n')
for option in options["Archivist"]:
	outFile.write('#include "Archivist/'+option+'Archivist.h"\n')

outFile.write('\n\n//create a world\n')
outFile.write('shared_ptr<AbstractWorld> makeWorld(shared_ptr<ParametersTable> PT = Parameters::root){\n')
outFile.write('  shared_ptr<AbstractWorld> newWorld;\n')
outFile.write('  bool found = false;\n')
outFile.write('  string worldType = (PT == nullptr) ? Global::worldTypePL->lookup() : PT->lookupString("GLOBAL-worldType");\n')
for option in options["World"]:
	outFile.write('  if (worldType == "'+option+'") {\n')
	outFile.write('    newWorld = make_shared<'+option+'World>(PT);\n')
	outFile.write('    found = true;\n')	
	outFile.write('    }\n')
outFile.write('  if (!found){\n')
outFile.write('    cout << "  ERROR! could not find GLOBAL-worldType \\"" << PT->lookupString("GLOBAL-worldType") << "\\".\\n  Exiting." << endl;\n')
outFile.write('    exit(1);\n')
outFile.write('    }\n')
outFile.write('  return newWorld;\n')
outFile.write('}\n')

outFile.write('\n\n//create a template genome\n')
outFile.write('shared_ptr<AbstractGenome> makeTemplateGenome(shared_ptr<ParametersTable> PT = Parameters::root){\n')
outFile.write('  shared_ptr<AbstractGenome> newGenome;\n')
outFile.write('  bool found = false;\n')
outFile.write('  string genomeType = (PT == nullptr) ? AbstractGenome::genomeTypeStrPL->lookup() : PT->lookupString("GENOME-genomeType");\n')
for option in options["Genome"]:
	outFile.write('  if (genomeType == "'+option+'") {\n')
	outFile.write('    newGenome = '+option+'Genome::genomeFactory(PT);\n')
	outFile.write('    found = true;\n')	
	outFile.write('    }\n')
outFile.write('  if (found == false){\n')
outFile.write('    cout << "  ERROR! could not find GENOME-genomeType \\"" << genomeType << "\\".\\n  Exiting." << endl;\n')
outFile.write('    exit(1);\n')
outFile.write('    }\n')
outFile.write('  return newGenome;\n')
outFile.write('}\n')

outFile.write('\n\n//create a template brain\n')
outFile.write('shared_ptr<AbstractBrain> makeTemplateBrain(shared_ptr<AbstractWorld> world, shared_ptr<ParametersTable> PT = Parameters::root){\n')
outFile.write('  shared_ptr<AbstractBrain> newBrain;\n')
outFile.write('  bool found = false;\n')
outFile.write('  string brainType = (PT == nullptr) ? AbstractBrain::brainTypeStrPL->lookup() : PT->lookupString("BRAIN-brainType");\n')
for option in options["Brain"]:
	outFile.write('  if (brainType == "'+option+'") {\n')
	outFile.write('    newBrain = '+option+'Brain::brainFactory(world->requiredInputs(), world->requiredOutputs(), PT->lookupInt("BRAIN-hiddenNodes"),PT);\n')
	outFile.write('    found = true;\n')	
	outFile.write('    }\n')
outFile.write('  if (found == false){\n')
outFile.write('    cout << "  ERROR! could not find BRAIN-brainType \\"" << brainType << "\\".\\n  Exiting." << endl;\n')
outFile.write('    exit(1);\n')
outFile.write('    }\n')
outFile.write('  return newBrain;\n')
outFile.write('}\n')

outFile.write('\n\n#endif /* __AutoBuild__Modules__ */\n')

outFile.close()
