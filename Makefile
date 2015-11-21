all:
	gcc -o out analisemamografia.c -lm
	./out mdb028.pgm mdb028l.pgm 338 314 56
