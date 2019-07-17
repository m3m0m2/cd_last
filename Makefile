proto: proto.o
	gcc -o proto proto.o -ltermcap

#proto.o : proto.c
#        gcc -c proto.c
#
clean :
	rm proto *.o
