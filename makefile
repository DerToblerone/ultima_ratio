main: position.h bitboard.h types.h display.h main.cpp utility.h movegen.h perft.h search.h test.h table.h
	g++ -g -o  main position.h bitboard.h types.h display.h main.cpp test.h utility.h table.h movegen.h perft.h search.h -Ofast -fconstexpr-ops-limit=335544320 -march=native -flto -fno-signed-zeros -frename-registers -funroll-loops
