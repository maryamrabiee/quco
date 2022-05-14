#!/bin/bash

input=$1
outdir=$2

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )


tmp=$(mktemp)
i=0
for l in $(cat $input);do
	cat $l | sort | uniq -c > $(echo "$tmp")
	$SCRIPT_DIR/quart_freq/quart_bin fancy printQuartets $(echo "$tmp") > $outdir/$i.qf
	if [ -s $outdir/$i.qf ];then
		printf "$outdir/$i.qf\n"
	fi
	i=$((i+1))
done | awk '{print NR-1,$0}' > $outdir/quco-input.txt

echo "computing frequencies done"
python3 $SCRIPT_DIR/save_load_matrix_P.py $outdir/quco-input.txt $outdir/quco-out.tre $outdir/quco-genetrees.tre > $outdir/quco.log
echo "quartets are inferred"
cat $outdir/quco-out.tre | awk '{print $3}' > $outdir/quco-out.tre.nw
java -Xmx2G -jar $SCRIPT_DIR/Astral/astral.5.7.8.jar  -i $outdir/quco-out.tre.nw -o $outdir/quco-out.tre.astral.out 2> $outdir/quco-out.tre.astral.err
echo "You can find astral output in $outdir/quco-out.tre.astral.out"
