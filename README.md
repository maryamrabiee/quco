# QuCo
## Quartet Coestimation of gene trees and species tree 
# Installation
## Prerequisites

* Java, version 1.6 or later 
* Dendropy, version > 4.0.0; Dendropy package which is easy to install with ***pip***. You can find installation instructions [here](https://dendropy.org).
* Numpy
* Newick utilities, can be downloaded from [here](https://github.com/tjunier/newick_utils)

## Steps:

1. Clone the code from this github page or download the zip file.
2. Unzip the package if you downloaded the zip file.
3. `cd` into the directory where QuCo code is placed. 
4. `cd` into quart_freq and compile the code using:

```
make clean
make QUARTETS=1
```


# EXECUTION:
The input required to run QuCo, is a set of posterior trees that are explored through MCMC sampling, for all the genes. If you have run multiple chains, you need to combine them into one file for each gene, and don't forget to remove trees explored during burn-ins.

In one file, provide the list of paths to the posterior trees for each gene, and this is the first input to run Quco. You can now run Quco with the following command:

```
./run_quco.sh [paths_to_pp_trees] [output_folder]
```
Each of the posterior files should have a frequency in column one and the tree topology in column two. If you don't have the frequencies, you can run the following command to add them:

```
cat <posterior_file> | sort | uniq -c > <posterior_file_with_freq>
```
Make sure that the write permissions for the output folder are correct.

## Interpreting the output
The output folder contains the quartet frequencies for each gene and also inferred quartets with branch lengths and likelihoods.

The `quco-out.tre` has the following format, if it was needed for some analysis:

```
#Branch_length -best_lh best_topology -2nd_best_lh 2nd_best_topology -3rd_lh 3rd_topology
1.0949076826832758 168.9191119151238 ((A,B),(C,D)); 219.72646034989606 ((A,C),(B,D)); 219.72646185380768 ((A,D),(B,C));
1.0064805441823255 175.00172401706408 ((A,B),(C,E)); 219.72646032774875 ((A,C),(B,E)); 219.72646377537126 ((A,E),(B,C));
0.9685868730517407 178.2975624082731 ((A,B),(C,F)); 219.72646188616702 ((A,F),(B,C)); 219.72646252857652 ((A,C),(B,F));
.
.
.
```
Astral tree run on best quartets can be found in the output directory as `quco-out.tre.astral.out `. It is in newick format, as well as all other output trees.

#Data Availability
Datasets used in the paper can be accessed via [this Drive link](https://drive.google.com/drive/folders/1l3IoZhDHo8cdq_apDshfoAvwa-qJlD1f?usp=sharing).





