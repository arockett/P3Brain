# https://bespokeblog.wordpress.com/2011/07/07/basic-data-plotting-with-matplotlib-part-2-lines-points-formatting/

import matplotlib.pyplot as plt
import numpy as np
import math
from matplotlib.backends.backend_pdf import PdfPages
#import csv
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
  plt.figure()                                                # create a new figure
  if (title!=''):
    plt.suptitle(title, fontsize=14, fontweight='bold')

  Rows = math.ceil(float(len(NamesList))/float(Columns))      # calcualate how many rows we need
  
  if (XCoordinateName==''):                                   # if there is no XCoordinateName
    for count in range(len(NamesList)):                       # for each name
      plt.subplot(Rows,Columns,count+1)                       # go to the count-th row of in our figure (with len(NamesList) rows)
      plt.plot(DataMap[NamesList[count]],label=NamesList[count])
                                                              # plot the data for each element in name in it's own plot
      plt.title(NamesList[count], fontsize=12) 	              # set the title for this plot

  else:                                                       # else, there is a XCoordinateName
    for count in range(len(NamesList)):                       # for each name
      plt.subplot(Rows,Columns,count+1)                       # go to the count-th row of in our figure (with len(NamesList) rows)
      plt.plot(DataMap[XCoordinateName],DataMap[NamesList[count]],label=NamesList[count])
                                                              # plot the data for each element in name in it's own plot
      plt.title(NamesList[count], fontsize=12)                # set the title for this plot

	
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

	  
######## LOAD DATA

def usage():
	print()
	print(sys.argv[0] + " [-h][-s pdf|png]")
	print()
	print("  -h show this help message")
	print("  -s do not display image, save images (pdf or png formatte")
	print()
	
def main(argv=None):
	if argv is None:
		argv = sys.argv
	try:
		opts, args = getopt.getopt(sys.argv[1:], "ho:v", ["help", "output="])
	except getopt.GetoptError as err:
		# print help information and exit:
		print()
		print (str(err)) # will print something like "option -a not recognized"
		usage()
		sys.exit(2)
	output = None
	verbose = False
	for o, a in opts:
		if o == "-v":
			verbose = True
		elif o in ("-h", "--help"):
			usage()
			sys.exit()
		elif o in ("-o", "--output"):
			output = a
		else:
			assert False, "unhandled option"


	ave_csv_file = pandas.read_csv(r'ave.csv')
	dominant_csv_file = pandas.read_csv(r'dominant.csv')

	aveList = list(ave_csv_file.columns.values)
	if "update" in aveList:
		aveList.remove("update")
	domList = aveList
	aveGraph = BuildMultiPlotFromDict(ave_csv_file,NamesList = aveList,XCoordinateName='update',Columns=2,title = 'Average')
	domGraph = BuildMultiPlotFromDict(dominant_csv_file,NamesList = domList,XCoordinateName='update',Columns=2,title = 'Dominant')


	#plt.show()


	######## SAVE TO A PNG FILE
	aveGraph.savefig('aveGraph.png', dpi=100)
	domGraph.savefig('domGraph.png', dpi=100)


	######## SAVE TO A PDF FILE

	#pp = PdfPages('output.pdf')
	#pp.savefig(fig1)
	#pp.savefig(fig2)
	#pp.close()

  

if __name__ == "__main__":
    main()
