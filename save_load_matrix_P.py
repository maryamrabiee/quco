import sys
import re
import numpy as np
from scipy.optimize import minimize,LinearConstraint
from numpy import savetxt,loadtxt
from scipy.stats import chi2
import os
import time

version="QCv1.1"
def read_files_for_P(file, quartets, gnum, GENE_NUM):
        topologies = []
        genes_pp = {}
        NN= GENE_NUM
        gnums = []
#        quartets = {}
        with open(os.path.expanduser(file)) as f:
                lines = f.readlines()
                lines = [l.strip() for l in lines]
                #12 Acunomia_m,Afronomia_ci,Aust,Die,| Acunomia_m Aust | Afronomia_circumnitens Dieunomia_heteropoda

                for k,line in enumerate(lines):
                        if not line:
                            continue
                        x = line.split("|")[0]
                        qtree = x.split()[1]
                        freq = int(x.split()[0])
                        tree = line.split("|")[1:]
                        tree = [" ".join(sorted(st.strip().split(" "))) for st in tree]
                        tree.sort()
                        tree = "|".join(tree)
#                        print(tree)

#                       genes_pp[tree] = [0]*GENE_NUM
                        if "- -" in tree:
                                continue
#                        if qtree=='47,62,66,84,':
#                                print(tree,(qtree in quartets.keys()),freq)
                        if qtree in quartets.keys():
                                if not tree in quartets[qtree]:

                                        quartets[qtree][tree] = [0]*GENE_NUM
                        else:
                                quartets[qtree] = {}
                                quartets[qtree][tree] = [0]*GENE_NUM

                        quartets[qtree][tree][gnum] = freq
                        if k%1000 == 0:
                                print(".",end="")
                print(file)


#         prev, l = -1, -1
#         for k,line in enumerate(lines):
# #               [&W 000 1.000000]       ((A,C),B,D);
#                 parts = line.split()
#                 tree = parts[-1]
#                 pp = float(parts[2][:-1])
#                 gnum = int(parts[1])-1
#                 if gnum != prev:
#                         l += 1
#                         prev = gnum
#                 if tree in genes_pp.keys():
#                         genes_pp[tree][l] = pp
#                 else:
#                         #genes_pp[tree] = [0]*GENE_NUM
#                         #genes_pp[tree] = [0]*GENE_NUM
#                         genes_pp[tree] = [0]*NN
#                         genes_pp[tree][l] = pp
#                 #       trees.append((tree,pp))
#                 # if trees:
#                 #       maps.append(max(trees,key=lambda x:x[1]))
#                 # else:
#                 #       maps.append(('',-1))

        return quartets


def convert_quartet_to_newick(qstr):
        parts = qstr.split("|")
        newick = "(("+",".join(parts[0].split())+"),("+",".join(parts[1].split())+"));"
        return newick

def print_tofile(quartets, files):
        nfiles = len(files)
        nq = len(quartets)
        eachf = nq/nfiles + 1
        filestr = ""
        i = 0
        plist = []
        for q,qdict in quartets.items():

                topologies = list(qdict.keys()) 
                print(topologies)
                P = convert_to_array(qdict, topologies, GENE_NUM)
                P += 10 ** -8
                P = P/P.sum(axis=0,keepdims=1)
                np.set_printoptions(suppress=True)
                np.set_printoptions(threshold=sys.maxsize)
                #pstr = np.array2string(P, precision=2, separator=',')
                filestr += " ".join([convert_quartet_to_newick(qstr) for qstr in topologies])+'\n' 
                plist.append(P)
                i += 1
                if i % int(nq/nfiles) == 0  :
                        with open(files[int(i/eachf)]+".top",'w') as f:
                                f.write(filestr)
                        np.savez(files[int(i/eachf)], *plist)
                        plist = []
                        filestr = ""

def print_toafile(quartets, file):
#        nfiles = len(files)
 #       nq = len(quartets)
  #      eachf = nq/nfiles + 1
        filestr = ""
        i = 0
        plist = []
        for q,qdict in quartets.items():

                topologies = list(qdict.keys())
                P = convert_to_array(qdict, topologies, GENE_NUM)
                P += 10 ** -8
                P = P/P.sum(axis=0,keepdims=1)
                np.set_printoptions(suppress=True)
                np.set_printoptions(threshold=sys.maxsize)
                #pstr = np.array2string(P, precision=2, separator=',')
                filestr += " ".join(topologies)+'\n'
#filestr += " ".join([convert_quartet_to_newick(qstr) for qstr in topologies])+'\n' 
                plist.append(P)
        with open(file+".top",'w') as f:
            f.write(filestr)
        np.savez(file, *plist)

def convert_to_array(genes_pp,topologies,GENE_NUM):
#       topologies = list(genes_pp.keys())
        P = np.zeros((3, GENE_NUM))

        for i,top in enumerate(topologies):
                P[i,] = np.array(genes_pp[top])
#        for j in range(GENE_NUM):
 #               if P[:,j].sum() < 0.99:
  #                      print(j, P[:,j].sum())
        return P

def f1(d, i, P):
        return -np.log(P[i,]+np.exp(-d)*(1/3.0 - P[i,])).sum()

def f2(d, i, P):
        return -np.log((1-np.exp(-d))*P[i,]+np.exp(-d-np.log(3.0))).sum()


def jacobian(d, i, P):
        return -((3*P[i,]-1)/(1+3*P[i,]*(np.exp(d)-1))).sum()

def hessian(d, i, P):
        return -(( 3 * np.exp(d) * P[i,] * (1 - 3 * P[i,]) )/(1+3*P[i,]*(np.exp(d)-1))**2).sum()


def find_genetrees(P, best_ind, d, topologies):
	P[best_ind,] *= (1 - 2/3*np.exp(-d))
#	print(list(range(3)),best_ind, list(range(3)).remove(best_ind))
	lst = list(range(3))
	lst.remove(best_ind)
	for i in lst:
		P[i,] *= (1/3*np.exp(-d))
	gene_indices = np.argmax(P,axis=0)
	genetrees = [topologies[i] for i in gene_indices]
	return genetrees

if __name__ == "__main__":
        start_time = time.time()
        file = sys.argv[1]
        print(version)
        N = 3
        genes = []
        with open(file) as f:
                lines = f.readlines()
        lines = [l.strip() for l in lines]
        GENE_NUM = len(lines)
        quartets = {}
        for l in lines:
                fo = l.split(" ")[1]
                #print(l)
                #print(l.split('\t'))
                gnum = int(l.split()[0])
                quartets = read_files_for_P(fo, quartets, gnum,GENE_NUM)
        #print(topologies)
        print(len(quartets))
#        files = [sys.argv[2]+str(j)+".tre" for j in range(int(sys.argv[3])) ]
#        print_toafile(quartets,sys.argv[2])
#        exit()
#        print(quartets)
        printstr = ""
        for q,qdict in quartets.items():
#                print(q+":")
                topologies = list(qdict.keys())
                print(topologies)
                xx = 3 - len(topologies)
                for i in range(xx):
                        qdict['- - | - -'+str(i)] = [0]*GENE_NUM
                topologies =  list(qdict.keys())
               # topologies = [convert_quartet_to_newick(qstr) for qstr in list(qdict.keys())]                
 #               print(str(topologies)+":")
                P = convert_to_array(qdict, topologies, GENE_NUM)
#                print(P)

                np.set_printoptions(suppress=True)
                np.set_printoptions(threshold=sys.maxsize)
        #       print(np.transpose(P))
         #       print("All sums to 1:", end=" ")
                print((P.sum(axis=0) > 0.99).all())
                print(P)
                P += 10 ** -8
                P = P/P.sum(axis=0,keepdims=1)
                print(P)
                results = []
                for i in range(3):
                        res = minimize(f1, [0.01], method='trust-constr', jac=jacobian, hess=hessian,bounds=[(0,np.inf)],args=(i,P))
                        results.append(res)
                topologies = [convert_quartet_to_newick(qstr) for qstr in list(qdict.keys())]

                best_ind = np.argmin([r.fun for r in results])
                best2 = np.argpartition([r.fun for r in results], 2)[1]
                sortedres = sorted(results,key=lambda x: x.fun)
                print("best:",end=" ")
                print(results[best_ind].x[0], results[best_ind].fun, topologies[best_ind])
                worst_ind = np.argmax([r.fun for r in results])
                if len(topologies) <3:
                    worst_ind = best2
                if len(topologies) < 2:
                    worst_ind = best_ind
                    best2 = best_ind
                #print(results)
                printstr += " ".join([str(x) for x in [results[best_ind].x[0], results[best_ind].fun, topologies[best_ind], sortedres[1].fun, topologies[best2],sortedres[-1].fun, topologies[worst_ind]]])+"\n"
        print("----- %s seconds -----" % (time.time() - start_time))
    #    with open(sys.argv[2],"w") as f:
    #            f.write(printstr)
        out_file = sys.argv[3]
	
        with open(sys.argv[2],"w") as f:
            f.write(printstr)
#        with open(out_file,"w") as f:
 #           f.write("\n".join(find_genetrees(P, best_ind, results[best_ind].fun, topologies)))
        print("----- %s seconds -----" % (time.time() - start_time))

#	print(results[best_ind].x, results[best_ind].fun, topologies[best_ind])
 #               print("best:",end=" ")
 #               print(results[best_ind].x[0], results[best_ind].fun, topologies[best_ind])
 #               with open(sys.argv[2],"w") as f:
 #                       f.write(" ".join([str(x) for x in [q,results[best_ind].x[0], results[best_ind].fun, topologies[best_ind]]]))
                # for i,r in enumerate(results):
                #         print("top"+str(i)+": "+str(r.x[0]),str(r.fun),topologies[i])

                # sortedres = sorted(results,key=lambda x: x.fun)
                # chis = 2*(-sortedres[0].fun + sortedres[1].fun)
                # p_value = 1 - chi2.cdf(chis,1)
                # print("LRT 2*(best-best2), p-value:",str(chis),str(p_value))
                # print("worst:",end=" ")
                # print(sortedres[-1].x[0],sortedres[-1].fun)
                # print("2nd best:",end=" ")
                # print(results[best2].x[0], results[best2].fun, topologies[best2])
                # print("best:",end=" ")
                # print(results[best_ind].x[0], results[best_ind].fun, topologies[best_ind])
#       print(results[best_ind].x, results[best_ind].fun, topologies[best_ind])
