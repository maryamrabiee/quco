import dendropy
import sys


version = "1.1v"

def find_closest_leaf_under(node):
	close_candidates = []
	if node.is_leaf():
		close_candidates.append((node,0))
		return [(node,0)], []
#		return node,0
	for n in node.levelorder_iter():
		if n.is_leaf():
			close_candidates.append((n, n.level()-node.level()))
		#if len(close_candidates) == 2:
		
	sortedl = sorted(close_candidates, key = lambda t: t[1])
	ties = [c for c in close_candidates if c[1]<=sortedl[0][1]+1 and c not in sortedl[:2]]
	return sortedl[:2], ties


def find_closest_leaf_above(node,tree):
	close_candidates = []
	n = node
	while n:
		
		for sibl in n.sibling_nodes():
			ch, a = find_closest_leaf_under(sibl)
			close_candidates+=[(c[0],c[1]+2) for c in ch]
		n = n.parent_node
	
	if len(close_candidates) > 1:
		sortedl = sorted(close_candidates, key = lambda t: t[1])
		ties = [c for c in close_candidates if c[1]<=sortedl[0][1]+1 and c not in sortedl[:2]]
		return sortedl[:2], ties
#		return [l[0] for l in sorted(close_candidates,key = lambda t: t[1])[:2]]
	else:
		return close_candidates,[]



def sample_around_polytomy(tree):
	allnode_samples = []
	for node in tree.postorder_node_iter():

		if len(node.child_nodes()) > 2:
			samples = []
			sample1,sample2 = [],[]
			cc = []
			if node.parent_node:
				cc.append(find_closest_leaf_above(node,tree)[0])

			for child in node.child_node_iter():
				cc.append(find_closest_leaf_under(child)[0])
			for c in cc:
				sample1.append(c[0][0])
				i = int(len(c)>1)
				sample2.append(c[i][0])
			samples.append(sample1)
			samples.append(sample2)
			# for c in cc:
			# 	for j in range(len(c)):
			# 		if len(samples) <= j:
			# 			samples.append([])
			# 		samples[j].append(c[j][0])

			allnode_samples += samples

	return allnode_samples

def sample_around_polytomy_ties(tree):

	allnode_samples = []
	for node in tree.postorder_node_iter():
		if len(node.child_nodes()) > 2:
#			print(len(node.child_nodes()))
			samples = []
			cc = []
			leaves = []
			if node.parent_node:
				a, ties = find_closest_leaf_above(node,tree)
#				print(ties)
				if ties:
					cc.append(ties)
				else:
					cc.append(a)

			for child in node.child_node_iter():
				a, ties = find_closest_leaf_under(child)

				cc.append(ties)
				if child.is_leaf():
					leaves.append(child)

			cc.append([leaves])
			for c in cc:
				for j in range(len(c)):
					if len(samples) <= j:
						samples.append([])
					samples[j].append(c[j][0])

			allnode_samples += [s for s in samples if len(s) >= 4]
#	print(allnode_samples)			
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

	samples += sample_around_polytomy_ties(tree)
	printstr = ""
	for s in samples:
		print(" ".join([n.taxon.label for n in s]),len(s))
		printstr+= " ".join([n.taxon.label for n in s])+"\n"

	with open(sys.argv[2],"w") as f:
        	f.write(printstr)
