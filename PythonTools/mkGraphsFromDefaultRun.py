# https://bespokeblog.wordpress.com/2011/07/07/basic-data-plotting-with-matplotlib-part-2-lines-points-formatting/

import matplotlib.pyplot as plt
import numpy as np
import math
from matplotlib.backends.backend_pdf import PdfPages
#import csv
import pandas

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

ave_csv_file = pandas.read_csv(r'ave.csv')
dominant_csv_file = pandas.read_csv(r'dominant.csv')
#data_csv_file = pandas.read_csv(r'data_9750.csv')

BuildMultiPlotFromDict(ave_csv_file,NamesList = ['score','food1','switches','food2','consumptionRatio'],XCoordinateName='update',Columns=1,title = 'Average')
BuildMultiPlotFromDict(dominant_csv_file,NamesList = ['score','food1','switches','food2','consumptionRatio'],XCoordinateName='update',Columns=1,title = 'Dominant')

## tookout food 2
#BuildMultiPlotFromDict(ave_csv_file,NamesList = ['score','gates','genomeLength'],XCoordinateName='update',Columns=2,title = 'Average')
#BuildMultiPlotFromDict(dominant_csv_file,NamesList = ['score','food1','switches','food2','gates','genomeLength'],XCoordinateName='update',Columns=2,title = 'Dominant')


#BuildMultiPlotFromDict(data_csv_file,NamesList = ['score','food1','switches','food2','gates','genomeLength','total'],Columns=2,title = 'Dominant')

#BuildPlotFromDict(DataMap = Data, NamesList = ['food1','food2','switches'],XCoordinateName = 'update',AddLegend='lower right')
#BuildPlotFromDict(DataMap = Data, NamesList = ['food1','food2','switches'],XCoordinateName = 'update',AddLegend='lower right')

plt.show()


######## SAVE TO A PNG FILE
#plt.savefig('testGraph.png', dpi=100)


######## SAVE TO A PDF FILE

#pp = PdfPages('foo.pdf')
#pp.savefig(fig1)
#pp.savefig(fig2)
#pp.savefig(DisplaysFig)
#pp.close()

  

