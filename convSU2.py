import os
import sys
import numpy as np
from copy import copy

def GetMesh(filename):
   
    print "Reading SU2 file... ",

    lines = [line.rstrip('\n') for line in open(filename)]
    
    counter = 1
    
    nelem = int(lines[counter].split()[1])
    elems = np.zeros((nelem, 4))
    for i in range(nelem):
        counter += 1
        elems[i,:] = [int(s) for s in lines[counter].split()[1:5]]
    
    counter+=1
    
    npoin = int(lines[counter].split()[1])
    points = np.zeros((npoin, 3))
    for i in range(npoin):
        counter+=1
        points[i,:] = [float(s) for s in lines[counter].split()[0:3]]
    
    counter += 1
    
    nmark = int(lines[counter].split()[1])
    
    marker_elems = []

    for i in range(nmark):
        
        counter +=2

        marker_npoin = int(lines[counter].split()[1])

        marker_elems.append(np.zeros((marker_npoin, 3)))

        for j in range(marker_npoin):
            
            counter+=1

            marker_elems[i][j] = [int(s) for s in lines[counter].split()[1:4]]

    print "Done"

    return [nelem, npoin, nmark], [elems, points, marker_elems]

def createCAS(filename, outfile):
    
    print "Creating CAS file: "

    num, dat = GetMesh(filename)

    body_faces = [];
    surface_faces = [[] for i in range(num[2])]

    a = [[] for i in range(num[1])]
    
    for i in range(num[0]):

        sys.stdout.write("\t"+str(i+1)+"/"+str(num[0])+" cells mapped\r")
        sys.stdout.flush()

        nodes = np.sort(dat[0][i])
        
        for j in range(4):

            if j==0:
                
                nd1 = int(nodes[0]);            nd2 = int(nodes[1]);            nd3 = int(nodes[2]);            nd4 = int(nodes[3]);

            if j==1:
                
                nd1 = int(nodes[0]);            nd2 = int(nodes[1]);            nd3 = int(nodes[3]);            nd4 = int(nodes[2]);

            if j==2:
                
                nd1 = int(nodes[0]);            nd2 = int(nodes[2]);            nd3 = int(nodes[3]);            nd4 = int(nodes[1]);

            if j==3:
                
                nd1 = int(nodes[1]);            nd2 = int(nodes[2]);            nd3 = int(nodes[3]);            nd4 = int(nodes[0]);

            pt1 = dat[1][nd1];    pt2 = dat[1][nd2];
            pt3 = dat[1][nd3];    pt4 = dat[1][nd4];

            flag1 = False
            flag2 = False
        
            if np.dot((pt4-pt1),np.cross((pt2-pt1),(pt3-pt2)))>0:
                    
                    for k in range(len(a[nd1])):
             
                        if a[nd1][k][0]==nd2:

                            flag1 = True

                            for l in range(1,len(a[nd1][k])):
                            
                                if a[nd1][k][l][0]==nd3:
                    
                                    a[nd1][k][l][1] = i+1
                                    flag2 = True
                                    body_faces.append([nd1+1, nd2+1, nd3+1, a[nd1][k][l][1], a[nd1][k][l][2]])

                                    break

                            if flag2==False:
                            
                                a[nd1][k].append([nd3, i+1, None])

                            break

                    if flag1==False:
                    
                        a[nd1].append([ nd2, [nd3, i+1, None] ])

            else:

                    for k in range(len(a[nd1])):
                    
                        if a[nd1][k][0]==nd2:

                            flag1 = True

                            for l in range(1,len(a[nd1][k])):
                                
                                if a[nd1][k][l][0]==nd3:
                    
                                    a[nd1][k][l][2] = i+1
                                    flag2 = True
                                    body_faces.append([nd1+1, nd2+1, nd3+1, a[nd1][k][l][1], a[nd1][k][l][2]])
                                    
                                    break

                            if flag2==False:
                                
                                a[nd1][k].append([nd3, None, i+1])
                            
                            break

                    if flag1==False:
                    
                        a[nd1].append([ nd2, [nd3, None, i+1] ])

    sys.stdout.write('\n')
    sys.stdout.flush()

    for i in range(num[2]):

        for j in range(len(dat[2][i])):

            sys.stdout.write("\t"+str(j+1)+"/"+str(len(dat[2][i]))+" faces mapped from surface "+str(i+1)+"\r")
            sys.stdout.flush()
        
            nodes = np.sort(dat[2][i][j])
            
            nd1 = int(nodes[0]) 
            nd2 = int(nodes[1])
            nd3 = int(nodes[2])

            for k in range(len(a[nd1])):
                
                if a[nd1][k][0]==nd2:

                    for l in range(1,len(a[nd1][k])):
                    
                        if a[nd1][k][l][0]==nd3:
            
                            if a[nd1][k][l][1]==None:
                            
                                surface_faces[i].append([nd1+1, nd2+1, nd3+1, int(0), a[nd1][k][l][2]])

                            else:
                                
                                surface_faces[i].append([nd1+1, nd2+1, nd3+1, a[nd1][k][l][1], int(0)])
        
        sys.stdout.write('\n')
        sys.stdout.flush()

    print "Writing mesh file... ",

    n_surface_faces = sum([len(dat[2][s]) for s in range(num[2])])

    f = open(outfile,"w+")
    f.write("(2 3)\r\n")
    f.write("\r\n")
    f.write("(12 (0 1 "+h(num[0])+" 0))\r\n")
    f.write("(13 (0 1 "+h(int(2*num[0]+0.5*n_surface_faces))+" 0))\r\n")
    f.write("(10 (0 1 "+h(num[1])+" 0))\r\n")
    f.write("\r\n")
    f.write("(12 (2 1 "+h(num[0])+" 1 2))\r\n")
    f.write("\r\n")
    f.write("(13 (3 1 "+h(int(2*num[0]-0.5*n_surface_faces))+" 2 3)(\r\n")
    
    counter = int(2*num[0]-0.5*n_surface_faces)
    
    for i in range(len(body_faces)):
        f.write(h(body_faces[i][0])+"\t"+
                h(body_faces[i][1])+"\t"+
                h(body_faces[i][2])+"\t"+
                h(body_faces[i][3])+"\t"+
                h(body_faces[i][4])+"\r\n")

    f.write("))\r\n\r\n")
    
    for i in range(num[2]):

        f.write("(13 ("+h(i+4)+" "+h(counter+1)+" "+h(counter+len(dat[2][i]))+" 3 3)(\r\n")

        counter += len(dat[2][i])

        for j in range(len(surface_faces[i])):

            f.write(h(surface_faces[i][j][0])+"\t"+
                    h(surface_faces[i][j][1])+"\t"+
                    h(surface_faces[i][j][2])+"\t"+
                    h(surface_faces[i][j][3])+"\t"+
                    h(surface_faces[i][j][4])+"\r\n")
 
        f.write("))\r\n\r\n")

    f.write("(10 (1 1 "+h(num[1])+" 1 3)(\r\n")
    
    for i in range(num[1]):
        
        f.write(str(dat[1][i,0])+"\t"+str(dat[1][i,1])+"\t"+str(dat[1][i,2])+"\r\n")

    f.write("))\r\n\r\n")
    
    f.close()

    print "Done"
    
def h(num):

    if num==0:
        return '0'
    else:
        return hex(num).lstrip('0x')

if __name__=="__main__":

    if sys.argv[1]=="cas":
        createCAS(sys.argv[2], sys.argv[3])
