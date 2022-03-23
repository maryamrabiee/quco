# This is a sample Python script.


import dendropy
from dendropy.simulate import treesim
import numpy as np
import sys
from random import Random

class Simulation:
    def __init__(self, term_coal, short_r, long_r):
        self.l = long_r
        self.s = short_r

        self.st = dendropy.Tree.get(data="(((((A:%f,B:%f):%f,C:%f):%f,D:%f):%f,E:%f):%f,F:%f):1000;"
                                  %(self.l, self.l, self.l, self.l*2, self.s, self.l*2 + self.s, self.s, self.l*2 + self.s*2,self.l ,self.l*3 + self.s*2), schema="newick")
        print(self.st, self.s, self.l)
        self.t_map = dendropy.TaxonNamespaceMapping.\
            create_contained_taxon_mapping(containing_taxon_namespace=self.st.taxon_namespace,
                                           num_contained=1, contained_taxon_label_fn=lambda s, i: s)

    def unroot(self,gt):
        gt.reroot_at_node(gt.find_node_with_taxon_label("D").parent_node)
        gt.reroot_at_node(gt.mrca(taxon_labels=["A", "B"]))


    def turn_to_mut(self,gt, global_rate):
        assert type(gt) == dendropy.Tree
        for e in gt.postorder_edge_iter():
#    variance = 1/5, mean =1    => shape=5, scale=1/5 
            rate = np.random.gamma(1, scale=1.0)
            e.length = e.length * rate * global_rate
        return gt

    def simulate_coalgenetree(self, k, seed):
        rng = Random(1234567000+seed)
        print("Seed: ",1234567000+seed)
        self.genetrees = [treesim.contained_coalescent_tree(
            containing_tree=self.st, gene_to_containing_taxon_map=self.t_map, rng=rng) for i in range(0, k)]
#        for gt in self.genetrees:
#            self.unroot(gt)
        return self.genetrees

#print coal gts  before here
    def multiply_rates(self):
        global_rate = 0.02
        self.genetrees = [self.turn_to_mut(g, global_rate) for g in self.genetrees]
        return self.genetrees

if __name__ == '__main__':
    version = "1.1"
    short_r = float(sys.argv[2])
    long_r = float(sys.argv[3])
    print("Simulator version "+version)
    np.random.seed(0)
    #sim = Simulation(internal_coal=0.6, term_coal=None, short_r=0.02, long_r=0.4, felsenstein=False)
    sim = Simulation( term_coal=None, short_r=short_r, long_r=long_r)
    N = int(sys.argv[4])
    replicate = int(sys.argv[5])
    gts = sim.simulate_coalgenetree(N, replicate)
#    gts = sim.simulate_coalgenetree(N)
    gts = sim.multiply_rates()
    kk = len(gts)
    # print(a/kk, b/kk, c/kk, d/kk, (sb-a-b-c-d)/kk)
    with open(sys.argv[1],"w") as f:
        for g in gts:
            f.write(g.as_string(schema='newick',real_value_format_specifier='.10f'))

 
