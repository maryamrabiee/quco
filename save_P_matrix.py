import sys
import re
import numpy as np
from scipy.optimize import minimize,LinearConstraint
from numpy import savetxt,loadtxt
from scipy.stats import chi2


version="QCv1.1"
def read_files_for_P(file):
	topologies = []
	genes_pp = {}
	NN= GENE_NUM
	gnums = []
	with open(file) as f:
		lines = f.readlines()
		for line in lines:
			n = line.split()[1]
			if not gnums or n != gnums[-1] :
				gnums.append(n)
		NN = len(gnums)
		print(NN)
		for k,line in enumerate(lines[:10]):
			tree = line.split()[-1]
#			genes_pp[tree] = [0]*GENE_NUM
			genes_pp[tree] = [0]*NN
			if tree not in topologies:
				topologies.append(tree)
#	print(topologies)
	if len(topologies)>3:
		print("Error in finding topologies",file)
		topologies= topologies[:3]

		#trees = []
	prev, l = -1, -1
	for k,line in enumerate(lines):
#		[&W 000 1.000000]	((A,C),B,D);
		parts = line.split()
		tree = parts[-1]
		pp = float(parts[2][:-1])
		gnum = int(parts[1])-1
		if gnum != prev: 
			l += 1
			prev = gnum
		if tree in genes_pp.keys():
			genes_pp[tree][l] = pp
		else:
			#genes_pp[tree] = [0]*GENE_NUM
			genes_pp[tree] = [0]*NN
			genes_pp[tree][l] = pp
		#	trees.append((tree,pp))
		# if trees:
		# 	maps.append(max(trees,key=lambda x:x[1]))
		# else:
		# 	maps.append(('',-1))

	return genes_pp,topologies,NN






def convert_to_array(genes_pp,topologies,GENE_NUM):
#	topologies = list(genes_pp.keys())
	P = np.zeros((3, GENE_NUM))
	
	for i,top in enumerate(topologies):
		P[i,] = np.array(genes_pp[top])
	for j in range(GENE_NUM):
		if P[:,j].sum() < 0.99:
			print(j, P[:,j].sum())
	return P

def f(d, i, P):
	return -np.log(P[i,]+np.exp(-d)*(1/3.0 - P[i,])).sum()

def f2(d, i, P):
	return -np.log((1-np.exp(-d))*P[i,]+np.exp(-d-np.log(3.0))).sum()


def jacobian(d, i, P):
	return -((3*P[i,]-1)/(1+3*P[i,]*(np.exp(d)-1))).sum()

def hessian(d, i, P):
	return -(( 3 * np.exp(d) * P[i,] * (1 - 3 * P[i,]) )/(1+3*P[i,]*(np.exp(d)-1))**2).sum()



if __name__ == "__main__":
	GENE_NUM = 500
	file = sys.argv[1]
	print(version)
	N = 3
	genes = []
	genes_pp,topologies,NN = read_files_for_P(file)
#	print(topologies)

	P = convert_to_array(genes_pp,topologies,NN)

#####	savetxt(sys.argv[3], P, delimiter=',')
#	P = load(sys.argv[3])
	np.set_printoptions(suppress=True)
	np.set_printoptions(threshold=sys.maxsize)
#	print(np.transpose(P))
	print("All sums to 1:", end=" ")
	print((P.sum(axis=0) > 0.99).all())
	P += 10 ** -8
	P = P/P.sum(axis=0,keepdims=1)
	results = []
	for i in range(3):
		res = minimize(f, [0.01], method='trust-constr', jac=jacobian, hess=hessian,bounds=[(0,np.inf)],args=(i,P))
		results.append(res)


	best_ind = np.argmin([r.fun for r in results])
	best2 = np.argpartition([r.fun for r in results], 2)[1]
	for i,r in enumerate(results):
		print("top"+str(i)+": "+str(r.x[0]),str(r.fun),topologies[i])

	sortedres = sorted(results,key=lambda x: x.fun)
	chis = 2*(-sortedres[0].fun + sortedres[1].fun)
	p_value = 1 - chi2.cdf(chis,1)
	print("LRT 2*(best-best2), p-value:",str(chis),str(p_value))
	print("worst:",end=" ")
	print(sortedres[-1].x[0],sortedres[-1].fun)
	print("2nd best:",end=" ")
	print(results[best2].x[0], results[best2].fun, topologies[best2])
	print("best:",end=" ")
	print(results[best_ind].x[0], results[best_ind].fun, topologies[best_ind])
#	print(results[best_ind].x, results[best_ind].fun, topologies[best_ind])


