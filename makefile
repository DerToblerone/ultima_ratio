urv2xp: main.cpp
	g++ main.cpp -o urv2xp -fconstexpr-ops-limit=100000000000 -std=c++23 -Ofast -march=native -flto -fno-signed-zeros -frename-registers -funroll-loops
