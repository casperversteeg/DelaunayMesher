import matplotlib.pyplot as plt
import numpy as np
import csv
import sys

for k in range(len(sys.argv)):
    if k == 0:
        continue
    with open(str(sys.argv[k]), 'r') as mesh_file:
        mesh = csv.reader(mesh_file, delimiter=',')
        nodes = []
        elements = []
        elemFlag = False
        line_count = 0
        for row in mesh:
            if line_count > 0:
                if row[0]=="$elements":
                    elemFlag = True
                    continue

                if elemFlag == False:
                    nodes.append(row)
                else:
                    elements.append(row)

            else:
                line_count +=1

    connectivity = [x[1:] for x in elements]
    nodes = [x[1:] for x in nodes]
    numRows = np.size(connectivity) / np.size(connectivity[0])
    numCols = np.size(connectivity[0])
    Xel = [0 for i in range(numCols+1)]
    Yel = [0 for i in range(numCols+1)]

    for i in range(numRows):
        for j in range(numCols):
            node = connectivity[i][j]
            Xel[j] = float(nodes[int(node)-1][0])
            Yel[j] = float(nodes[int(node)-1][1])
        Xel[j+1] = Xel[0]
        Yel[j+1] = Yel[0]
        plt.plot(Xel,Yel, '-ko')

    plt.title(sys.argv[k])

    plt.show(block=True)
