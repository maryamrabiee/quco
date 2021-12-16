# quart_freq
Computes all quartet frequencies of a set of input trees

> quart_bin fancy printQuartets input\_file


The format of input file should be:

```
frequency_1		tree_1
frequency_2		tree_2
...
```
To compute the frequencies newick utilities is needed:
>cat input\_trees.tre | nw_reroot - "root" |  nw\_order - | sort | uniq -c | sort -k1n > formatted\_input.tre


To compile these codes to get quart_bin simply run:

```
make QUARTETS=1
```