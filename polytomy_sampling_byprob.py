import dendropy
import sys
import numpy
import random


version = "1.0v"


def assign_probabilities(node, pp):
	node.probability=pp
	degree = len(node.child_nodes())
	# if node.probability:
	# 	pp = node.probability
	for child in node.child_node_iter(): 
		child.probability = pp/degree
		if not child.is_leaf():
			assign_probabilities(child, pp/degree)

def assign_probabilities_parent(tree, node, pp):
	newroot = tree.reroot_at_edge(node.edge)
	newroot.probability = pp
	sib = node.sibling_nodes()[0]
	sib.probability=pp
	degree = len(sib.child_nodes())
	for child in sib.child_node_iter(): 
		child.probability = pp/degree
		if not child.is_leaf():
			assign_probabilities(child, pp/degree)

def compute_sample_num(degree):
	return max(int(400/(degree/3.0)),2)

import copy
def sample_around_polytomy(tree):
	allnode_samples = {}
	N = 2
	for node in tree.postorder_node_iter():
		if len(node.child_nodes())>2:
			
			degree = len(node.child_nodes()) + 1
			sample_num = compute_sample_num(degree)
			samples = []
			node.probability=1
			for child in node.child_node_iter():
				assign_probabilities(child, 1)
			if node.parent_node:
				assign_probabilities_parent(tree, node, 1)
			sides = [list(set(tree.leaf_nodes()) - set(node.leaf_nodes()))]
			for child in node.child_node_iter():
				sides.append(child.leaf_nodes())
			sides_copy = copy.deepcopy(sides)
			for x in range(sample_num):
				sample = []
				while  len(sample) <= min(9,degree) and sides:
					selected_side = random.choice(sides)
					sides.remove(selected_side)
					p = [nn.probability for nn in selected_side]
					selected_node = numpy.random.choice(selected_side, p=p)
					sample.append(selected_node.taxon.label)
				samples.append(sample)
				sides = sides_copy.copy()
			allnode_samples[node] = samples


	return allnode_samples



if __name__ == '__main__':
	print("This is version"+version)
	tree = dendropy.Tree.get(path=sys.argv[1],schema='newick')
	for n in tree.postorder_node_iter():
		if n.label and float(n.label) < 1:
			n.edge.collapse()


	print([len(n.child_nodes()) for n in tree.postorder_internal_node_iter()])
	print(tree.as_string(schema="newick"))
	samples = sample_around_polytomy(tree)

	printstr = ""
	for key,vals in samples.items():
		print(vals)
		for s in vals:
			print(" ".join(s),len(s))
			printstr+= " ".join(s)+"\n"

	with open(sys.argv[2],"w") as f:
			f.write(printstr)


def assign_all(tree):
	for node in tree.postorder_node_iter():
		if len(node.child_nodes())>2:
			node.probability=1
			for child in node.child_node_iter():
				assign_probabilities(child, 1)
			assign_probabilities_parent(tree,node, 1)
			for nn in tree.postorder_node_iter():
				print([n.taxon.label for n in nn.leaf_nodes()],end=" ")
				print(nn.probability)
