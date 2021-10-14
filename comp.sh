## autoreconf --force --install
rm -rf work.tmp
#./configure > nul
make clean
make | grep erro 
# ./masa-opencl --no-flush --stage-1 --no-block-pruning AE002160.2.fasta CP000051.1.fasta
#gdb masa-opencl
#./masa-opencl --no-flush --stage-1 BA000035.2.fasta BX927147.1.fasta
