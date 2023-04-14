rm musicianAPP
rm /home/ssq/workdir/nfs/*.ms
arm-linux-gnueabihf-gcc main.c cJSON.c -o musicianAPP
cp musicianAPP /home/ssq/workdir/nfs
cp ms/*.ms /home/ssq/workdir/nfs
cp testms/*.ms /home/ssq/workdir/nfs