#!/bin/bash

input=$1
outdir=$2

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
echo $SCRIPT_DIR

if [ ! -s $outdir ];then
	mkdir $outdir
fi

if  [ ! -z "$(ls -A $outdir)" ]; then
	echo "Output directory is not empty!"
	exit 0
fi

tmp=$(mktemp)
i=0
re='^[0-9]+$'


for l in $(cat $input);do

	first=$(cat $l | head -1 | awk '{print $1}')
	#check if the posterior files have frequencies
	if ! [[ $first =~ $re ]] ; then
		cat $l | sort | uniq -c > $(echo "$tmp")
		$SCRIPT_DIR/quart_freq/quart_bin fancy printQuartets $(echo "$tmp") > $outdir/$i.qf
	else
		$SCRIPT_DIR/quart_freq/quart_bin fancy printQuartets  $l > $outdir/$i.qf
	fi
	if [ -s $outdir/$i.qf ];then
		printf "$outdir/$i.qf\n"
	fi
	i=$((i+1))
done | awk '{print NR-1,$0}' > $outdir/quco-input.txt


echo "computing frequencies done"
python3 $SCRIPT_DIR/save_load_matrix_P.py $outdir/quco-input.txt $outdir/quco-out.tre $outdir/quco-genetrees.tre > $outdir/quco.log
if [ ! -s $outdir/quco-out.tre ];then
	echo "Error in frequency files"
	exit 0
fi
echo "quartets are inferred"
cat $outdir/quco-out.tre | awk '{print $3}' > $outdir/quco-out.tre.nw
java -Xmx2G -jar $SCRIPT_DIR/Astral/astral.5.7.8.jar  -i $outdir/quco-out.tre.nw -o $outdir/quco-out.tre.astral.out 2> $outdir/quco-out.tre.astral.err
echo "You can find astral output in $outdir/quco-out.tre.astral.out"
