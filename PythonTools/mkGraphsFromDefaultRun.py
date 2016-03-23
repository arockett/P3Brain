# https://bespokeblog.wordpress.com/2011/07/07/basic-data-plotting-with-matplotlib-part-2-lines-points-formatting/

import matplotlib.pyplot as plt
import numpy as np
import math
from matplotlib.backends.backend_pdf import PdfPages
import matplotlib.font_manager as font_manager
import pandas
import sys
import getopt

####
#
# BuildMultiPlot(DataMap,NamesList,XCoordinateName='',Columns=1)
#   Builds a Figure with rows = # elements in Names list.
#   Each row contains a graph generated from DataMap[NamesList[row#]]
#
#   DataMap : a dictornary with data (all entires must have the same number of elements)
#   NamesList : list of names of elements from data to be graphed
#   XCoordinateName : If specified, this element from the DataMap will determin the XCoordinate scale
#   Columns : spread the graphs over this many columns, if not defined, there will be 1 column
#             # of rows is calcualted by the number of columns and the number of elements in NamesList
#
#   Return: the figure created
#
####

def BuildMultiPlotFromDict(DataMap,NamesList,XCoordinateName='',Columns=1,title = ''):
	fig = plt.figure(figsize=(20,10))                                                # create a new figure
	fig.subplots_adjust(hspace=.35)

	if (title!=''):
		plt.suptitle(title, fontsize=20, fontweight='bold')

	Rows = math.ceil(float(len(NamesList))/float(Columns))      # calcualate how many rows we need
  
	if (XCoordinateName==''):                                   # if there is no XCoordinateName
		XLimit = len(NamesList[0])
		for count in range(len(NamesList)):                       # for each name
			ax = plt.subplot(Rows,Columns,count+1)                       # go to the count-th row of in our figure (with len(NamesList) rows)
			plt.plot(DataMap[NamesList[count]],label=NamesList[count])
                                                              # plot the data for each element in name in it's own plot
			plt.title(NamesList[count], fontsize=16) 	              # set the title for this plot
			ax.title.set_position([.5, .97])
			for label in (ax.get_xticklabels() + ax.get_yticklabels()):
				#label.set_fontname('Arial')
				label.set_fontsize(10)
			ax.xaxis.set_tick_params(pad=2)
			plt.xlim([0,XLimit])
	else:                                                       # else, there is a XCoordinateName
		XLimit = DataMap[XCoordinateName][len(DataMap[XCoordinateName])-1]
		for count in range(len(NamesList)):                       # for each name
			ax = plt.subplot(Rows,Columns,count+1)                       # go to the count-th row of in our figure (with len(NamesList) rows)
			plt.plot(DataMap[XCoordinateName],DataMap[NamesList[count]],label=NamesList[count])
                                                              # plot the data for each element in name in it's own plot
			plt.title(NamesList[count], fontsize=16)                # set the title for this plot
			ax.title.set_position([.5, .97])
			for label in (ax.get_xticklabels() + ax.get_yticklabels()):
				#label.set_fontname('Arial')
				label.set_fontsize(10)
			ax.xaxis.set_tick_params(pad=2)
			plt.xlim([0,XLimit])

	return plt.gcf()                                            # gcf = get current figure - return that.

####
#
# BuildPlot(DataMap,NamesList,XCoordinateName='',AddLegend="")
#   Builds a figure with a single plot graphing the elements from DataMap named by NamesList.
#   Axis are labled if possible, and a legend is added.
#
#   DataMap : a dictornary with data (all entires must have the same number of elements)
#   NamesList : list of names of elements from data to be graphed
#   XCoordinateName : If specified, this element from the DataMap will determin the XCoordinate scale
#   AddLegend : if defined, places a legend ('upper left','upper right','lower left','lower right')
#
#   Return: the figure created
#
####

def BuildPlotFromDict(DataMap,NamesList,XCoordinateName='',AddLegend='',title = ''):
  plt.figure()                                                # create a new figure

  if XCoordinateName=='':                                     # if there is no XCoordinateName
    for count in range(len(NamesList)):                       # for each name
      plt.plot(DataMap[NamesList[count]],label=NamesList[count])
                                                              # plot the data for each element in name in it's own plot

  else:                                                       # else, there is a XCoordinateName
    for count in range(len(NamesList)):                       # for each name
      plt.plot(DataMap[XCoordinateName],DataMap[NamesList[count]],label=NamesList[count])
                                                              # plot the data for each element in name in it's own plot
      plt.xlabel(XCoordinateName)                             # add a X axis label

  if (AddLegend!=""):
    plt.legend(loc=AddLegend, shadow=True)                    # add a legend
  
  if (title!=''):
    plt.title(title)
	
  return plt.gcf()                                            # gcf = get current figure - return that.

	  
def usage():
	print()
	print(sys.argv[0] + ' [-h][-s pdf|png][-l][-c "source,x-axis,(legend location,)data1,data2,..."]')
	print()
	print('  -h show this help message')
	print('  -s do not display graph(s), save image(s) instead ("pdf" or "png" format)')
	print('  -l also load and graph Line of Decent (data.csv)')
	print('  -c make a custom graph')
	print('       source = ave | dom | LOD (for LOD, must also use -l flag)')
	print('       x-axis = which column to use for x-axis')
	print('       legend location = (optional - default "lower right")')
	print('                         upper right | upper left | lower left | lower right | right,center | left,center | right | lower center | upper center | center')
	print('       data* = columns to graph')
	print()
	
def main(argv=None):
	if argv is None:
		argv = sys.argv
	try:
		opts, args = getopt.getopt(sys.argv[1:], "hs:vc:vl")
	except getopt.GetoptError as err:
		# print help information and exit:
		print()
		print (str(err)) # will print something like "option -a not recognized"
		usage()
		sys.exit(2)
	output = None
	customOptions = None
	useLOD = False
	for option, value in opts:
		if option == "-v":
			verbose = True
		elif option in ("-h"):
			usage()
			sys.exit()
		elif option == "-c":
			customOptions = value
		elif option in ("-s"):
			output = value
		elif option in ("-l"):
			useLOD = True
		else:
			assert False, "unhandled option"

	ave_csv_file = pandas.read_csv(r'ave.csv')
	dominant_csv_file = pandas.read_csv(r'dominant.csv')

	if useLOD:
		LOD_csv_file = pandas.read_csv(r'data.csv')
		
	aveList = list(ave_csv_file.columns.values)
	if "update" in aveList:
		aveList.remove("update")
	domList = aveList
	if useLOD:
		LODList = aveList
	
	if customOptions == None:
		aveGraph = BuildMultiPlotFromDict(ave_csv_file,NamesList = aveList,XCoordinateName='update',Columns=2,title = 'Average')
		domGraph = BuildMultiPlotFromDict(dominant_csv_file,NamesList = domList,XCoordinateName='update',Columns=2,title = 'Dominant')
		if useLOD:
			LODGraph = BuildMultiPlotFromDict(LOD_csv_file,NamesList = LODList,XCoordinateName='update',Columns=2,title = 'Line of Descent')
	else :
		customList = customOptions.split(",")
		if customList[0] == "ave":
			source = ave_csv_file;
		elif customList[0] == "dom":
			source = dominant_csv_file
		elif customList[0] == "LOD":
			source = LOD_csv_file
		else:
			print ('options for custom must start with "ave", "dom" or, "lod"')
			sys.exit()
		if customList[2] in ['upper right','upper left','lower left','lower right','right,center','left,center','right','lower center','upper center','center']:
			legLoc = customList[2]
			names = customList[3:]
		else :
			legLoc = 'lower right'
			names = customList[2:]
		testGraph = BuildPlotFromDict(source,NamesList = names,XCoordinateName=customList[1],AddLegend=legLoc,title = 'foods')
	
	
	if output == None:
		plt.show()


	######## SAVE TO A PNG FILE
	if output == 'png':
		aveGraph.savefig('AveGraph.png', dpi=100)
		domGraph.savefig('DomGraph.png', dpi=100)
		if useLOD:
			LODGraph.savefig('LODGraph.png', dpi=100)


	######## SAVE TO A PDF FILE

	if output == 'pdf':
		pp = PdfPages('MABE_GRAPHS.pdf')
		pp.savefig(aveGraph)
		pp.savefig(domGraph)
		if useLOD:
			pp.savefig(LODGraph)

		pp.close()

  

if __name__ == "__main__":
    main()
