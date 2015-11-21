all:
	rm -f saida.txt
	gcc -o out analisemamografia.c -lm
	#./out mdb028.pgm mdb028l.pgm 338 314 56
	./out mamografias/mdb005.pgm mdb005l.pgm 500 168 26
	#./out mdb213.pgm mdb213l.pgm 547 520 45
	#./out mdb256.pgm mdb256l.pgm 400 484 37
