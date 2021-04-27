# This is a sample Python script.

# Press ⌃R to execute it or replace it with your code.
# Press Double ⇧ to search everywhere for classes, files, tool windows, actions, and settings.

import dendropy
from dendropy.simulate import treesim
import numpy as np
import sys


class Simulation:
    def __init__(self, internal_coal, term_coal, short_r, long_r, felsenstein):
        self.x = internal_coal/2     # internal coal unit bl
        self.y = term_coal           # terminal coal unit bl
        if not self.y:
            self.y = np.exp(-2 * self.x) / 3   # This will make sure internal bl = short terminal bl
        self.term_bl = 1 - np.exp(-2 * self.x)/3 + self.y
        
        print(self.term_bl)
        self.felsenstein = felsenstein
        self.sr = short_r/self.term_bl   # low rate
        self.lr = (long_r - (self.term_bl - self.y) * self.sr) / self.y   # high rate


        self.st = dendropy.Tree.get(data="((A:%f,B:%f):%f,(C:%f,D:%f):%f):1000;"
                                  %(self.y, self.y, self.x, self.y, self.y, self.x), schema="newick")
        print(self.st, self.sr, self.lr)
        self.t_map = dendropy.TaxonNamespaceMapping.\
            create_contained_taxon_mapping(containing_taxon_namespace=self.st.taxon_namespace,
                                           num_contained=1, contained_taxon_label_fn=lambda s, i: s)

    def unroot(self,gt):
        gt.reroot_at_node(gt.find_node_with_taxon_label("D").parent_node)
        gt.reroot_at_node(gt.mrca(taxon_labels=["A", "B"]))


    def turn_to_mut(self,gt):
        assert type(gt) == dendropy.Tree
        for e in gt.postorder_edge_iter():
            if e.head_node.taxon and e.head_node.taxon.label in \
                    (["A", "C"] if self.felsenstein else ["C", "D"]):
                e.length = self.y * self.lr + (e.length - self.y) * self.sr
            else:
                e.length = e.length * self.sr
        return gt

    def simulate_coalgenetree(self, k):
        self.genetrees = [treesim.contained_coalescent_tree(
            containing_tree=self.st, gene_to_containing_taxon_map=self.t_map) for i in range(0, k)]
        for gt in self.genetrees:
            self.unroot(gt)
        return self.genetrees

    def multiply_rates(self):
        self.genetrees = [self.turn_to_mut(g) for g in self.genetrees]
        return self.genetrees

if __name__ == '__main__':
    short_r = float(sys.argv[2])
    long_r = float(sys.argv[3])
    internal_coal = float(sys.argv[4])
    f = True
    if sys.argv[5]=='false':
        f = False
    #sim = Simulation(internal_coal=0.6, term_coal=None, short_r=0.02, long_r=0.4, felsenstein=False)
    sim = Simulation(internal_coal=internal_coal, term_coal=None, short_r=short_r, long_r=long_r, felsenstein=f)
    N = int(sys.argv[6])
    gts = sim.simulate_coalgenetree(N)
    gts = sim.multiply_rates()
    a = sum(x.find_node_with_taxon_label("A").edge.length for x in gts)
    b = sum(x.find_node_with_taxon_label("B").edge.length for x in gts)
    c = sum(x.find_node_with_taxon_label("C").edge.length for x in gts)
    d = sum(x.find_node_with_taxon_label("D").edge.length for x in gts)
    sb = sum(sum(e.length for e in gt.postorder_edge_iter() if e.tail_node) for gt in gts)
    kk = len(gts)
    print(a/kk, b/kk, c/kk, d/kk, (sb-a-b-c-d)/kk)
    with open(sys.argv[1],"w") as f:
        for g in gts:
            f.write(g.as_string(schema='newick',real_value_format_specifier='.10f'))

