ratio: main.cpp
	g++ main.cpp -o ratio -fconstexpr-ops-limit=100000000000 -std=c++23 -Ofast -march=native -flto -fno-signed-zeros -frename-registers -funroll-loops
