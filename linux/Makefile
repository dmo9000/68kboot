all: md5sum.linux fletcher16

md5sum.linux: md5sum.c 
	gcc -o md5sum.linux md5sum.c
	./md5sum.linux  ../hello1.txt 

fletcher16: fletcher16.c
	gcc -D__LINUX__=1 -o fletcher16 fletcher16.c

clean:
	rm -f md5sum.linux
