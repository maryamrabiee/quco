import dendropy
import sys




def find_closest_leaf_under(node):
	close_candidates = []
	if node.is_leaf():
		close_candidates.append((node,0))
		return [(node,0)]
#		return node,0
	for n in node.levelorder_iter():
		if n.is_leaf():
			close_candidates.append((n, n.level()-node.level()))
		if len(close_candidates) == 2:
			return close_candidates


def find_closest_leaf_above(node,tree):
	close_candidates = []
	n = node
	while n:
		
		for sibl in n.sibling_nodes():
			ch = find_closest_leaf_under(sibl)
			close_candidates+=[(c[0],c[1]+2) for c in ch]
		n = n.parent_node
	
	if len(close_candidates) > 1:
		return sorted(close_candidates,key = lambda t: t[1])[:2]
#		return [l[0] for l in sorted(close_candidates,key = lambda t: t[1])[:2]]
	else:
		return close_candidates



def sample_around_polytomy(tree):
	samples = []
	for node in tree.postorder_node_iter():
		if len(node.child_nodes()) > 2:
			sample1,sample2 = [],[]
			cc = []
			if node.parent_node:
				cc.append(find_closest_leaf_above(node,tree))

			for child in node.child_node_iter():
				cc.append(find_closest_leaf_under(child))
			for c in cc:
				sample1.append(c[0][0])
				i=int(len(c)>1)
				sample2.append(c[i][0])
			samples.append((sample1,sample2))
	return samples

if __name__ == '__main__':
	tree = dendropy.Tree.get(path=sys.argv[1],schema='newick')
	for n in tree.postorder_node_iter():
		if n.label and float(n.label) < 1:
			n.edge.collapse()


	print([len(n.child_nodes()) for n in tree.postorder_internal_node_iter()])
	print(tree.as_string(schema="newick"))
	samples = sample_around_polytomy(tree)
	printstr = ""
	for s in samples:
		print(" ".join([n.taxon.label for n in s[0]]),len(s[0]))
		printstr+= " ".join([n.taxon.label for n in s[0]])+"\n"
		print(" ".join([n.taxon.label for n in s[1]]),len(s[1]))
		printstr+= " ".join([n.taxon.label for n in s[1]])+"\n"

	with open(sys.argv[2],"w") as f:
        	f.write(printstr)
