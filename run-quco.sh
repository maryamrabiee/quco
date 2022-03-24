#!/bin/bash

input=$1
outdir=$2


if [[ $3=="quartet" ]]; then



else
	for l in $(cat $input);do
		./quart_freq/quart_bin fancy printQuartets $l > $l.qf
		if [ -s $l.qf ];then
			printf "$l.qf\n"
		fi
	done | awk '{print NR-1,$0}' > $outdir/quco-input.txt
fi
echo "computing frequencies done"
python3 save_load_matrix_P.py $outdir/quco-input.txt $outdir/quco-out.tre $outdir/quco-genetrees.tre > $outdir/quco.log
echo "quartets are inferred"
cat $outdir/quco-out.tre | awk '{print $3}' > $outdir/quco-out.tre.nw
java -Xmx2G -jar ./Astral/astral.5.7.8.jar  -i $outdir/quco-out.tre.nw -o $outdir/quco-out.tre.astral.out 2> $outdir/quco-out.tre.astral.err
echo "You can find astral output in $outdir/quco-out.tre.astral.out"
