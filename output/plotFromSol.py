#!/usr/bin/python3

import glob, os
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from matplotlib import colors



def main():
    print("Running plotFromSol.py")
    # plotFromFile('macroPl_3_3_to_10_1.sol')
    # plotFromFile('macroPl_3_3_to_5_2.sol')
    # plotFromFile('macroPl_3_3_to_5_2.sol')

    ## Find all *.sol in the current directory
    for solFileName in glob.glob("*.sol"):
        plotFromFile(solFileName)


def plotFromFile(solFileName):

    ## Parse size from solFileName

    # print(solFileName)
    
    name = solFileName.split(".sol")
    plotFileName = name[0] + ".png"
    print('plotFromFile <{}> to <{}>'.format(solFileName, plotFileName))

    tokens = name[0].split('_')

    Y = int(tokens[1]) # size y of the array.
    X = int(tokens[2]) # size x of the array.

    targetHeight = int(tokens[4]) # height of the target device.
    targetWidth = int(tokens[5]) # width of the target device.

    relPosX = 0
    relPosY = 0
    wtX = 1
    wtY = 1
    if (len(tokens) > 6):
        # print("wtY = ", tokens[11])
        # print("len = ", len(tokens))
        # print("tokens is ", name[0])
        relPosX = int(tokens[7]) # bool, if relative position constraint is applied in X direction.
        relPosY = int(tokens[8]) # bool, if relative position constraint is applied in Y direction.
        wtX = float(tokens[10]) # weight in X direction.
        wtY = float(tokens[11]) # weight in Y direction.
        # wtX = int(tokens[10]) # weight in X direction.
        # wtY = int(tokens[11]) # weight in Y direction.

    
    x = np.zeros((Y,X))
    y = np.zeros((Y,X))


    ## Open the file, read coordinates
    with open(solFileName, 'r') as file:
        lineNum = 0
        for line in file:
            if (lineNum == 0):
                lineNum += 1
                tokens = line.split()
                obj = tokens[4]
                
            elif (lineNum < 2*Y*X + 1 ):
                lineNum += 1
                tokens = line.split()
                coordinate = tokens[1]
                indice = tokens[0].split('_')
                # print(indice)
                i = int(indice[1])
                j = int(indice[2])
                if (indice[0] == 'X'):
                    x[i][j] = coordinate
                elif (indice[0] == 'Y'):
                    y[i][j] = coordinate
            
            else:
                break
    
    # ## Print the coordinates for debug.
    # for i in range(Y):
    #     for j in range(X):
    #         print("(" + str(x[i][j]) + ", " + str(y[i][j]) + ")")
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(30,30))
    plt.subplots_adjust(top=0.72)
    height = 0.2
    width = 0.2
    ## Plot cells.
    for i in range(Y):
        for j in range(X):
            # s = "(" + str(i) + "," + str(j) + ")"
            s = "(" + str(int(y[i][j])) + "," + str(int(x[i][j])) + ")"
            plotBox(j - 0.5 * width, i - 0.5 * height, width, height, colorMap(j, i, Y), edgeColorMap(j, i), ax1, s)
            # plotBox(x[i][j] - 0.5 * width, y[i][j] - 0.5 * height, width, height, colorMap(j, i, Y), edgeColorMap(j, i), ax2, s)
            plotBox(wtX * x[i][j] - 0.5 * width, wtY * y[i][j] - 0.5 * height, width, height, colorMap(j, i, Y), edgeColorMap(j, i), ax2, s)

    ## Plot nets.
    for i in range(Y):
        for j in range(X):
            if (i < Y - 1):
                plotLine(j, i, j, i+1, 0 , ax1)
                plotLine(wtX * x[i][j], wtY * y[i][j], wtX * x[i+1][j], wtY * y[i+1][j], 0, ax2)
            if (j < X - 1):
                plotLine(j, i, j+1, i, 0, ax1)
                plotLine(wtX * x[i][j], wtY * y[i][j], wtX * x[i][j+1], wtY * y[i][j+1], 0, ax2)

    # ## Plot the target area.
    # ax2.add_patch(Rectangle((0, 0), targetWidth, targetHeight, color = 'silver'))


    ax1.set_xlim([-1, X+1])
    ax1.set_ylim([-1, Y+1])
    ax1.axis('equal')
    ax1.set_title('Array size: ' + str(Y) + " x " + str(X))
    ax2.set_xlim([-1, wtX * targetWidth+1])
    ax2.set_ylim([-1, wtY * targetHeight+1])
    ax2.set_title('Site limit: ' + str(targetHeight) + " x " + str(targetWidth))
    xticks =  list(range(targetWidth))
    # print("tick is", xticks)
    xticks = [x * wtX for x in xticks]
    # print("tick is", xticks)
    ax2.set_xticks(xticks)
    ax2.axis('equal')
    title = 'Total WL: ' + str(obj)
    title += '\nWeight(x): ' + str(wtX) + "; Weight(y): " + str(wtY) 
    title += '\nRelativePostionX: ' + str(relPosX) + "; RelativePositionY: " + str(relPosY) 
    fig.suptitle(title)

    plt.savefig(plotFileName, dpi=600)
    plt.close()


def plotBox(x, y, width, height, color, edgecolor, ax, s = ''):
    ax.add_patch(Rectangle((x, y), width, height, facecolor = color, edgecolor = edgecolor))
    ax.text(x + 0.15, y - 0.2, s)

def plotLine(x0, y0, x1, y1, color, ax):
    ax.plot([x0, x1], [y0, y1], linestyle = '--', color = 'grey')

## x must be from {0, 1, 2, 3, 4, 5}: An index selecting from the color list.
## 0 <= y <= Y: A scale from transparent to solid.
def colorMap(x, y, Y):
    color = ["red", "orange", "yellow", "green", "blue", "purple"]
    color = ["red", "orange", "green", "blue", "purple"]
    if (x >= len(color)):
        return 'red'
    else:
        c = color[x]
        return colors.to_rgba(c, 0.1 + 0.9 * y / Y)

def edgeColorMap(x, y):
    color = ["red", "orange", "green", "blue", "purple"]
    if (x >= len(color)):
        return 'black'
    else:
        return color[x]





if __name__ == "__main__":
    main()
