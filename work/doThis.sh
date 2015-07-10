#!/bin/sh -login
#PBS -l nodes=1:ppn=1,walltime=23:59:00,mem=8gb
#PBS -j oe
cd noisyNavigator
module swap GNU GNU/4.8.3
./NN updates 10000 epsilon "$localE" LOD LOD_"$localE"_"$localN".csv GEN GEN_"$localE"_"$localN".csv 
