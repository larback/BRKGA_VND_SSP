# Flags:
CC = g++-14
CFLAGS = -fopenmp -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include
#Targets
all:
		$(CC) $(CFLAGS) -O3 -fopenmp -march=native -std=c++11 -o "Population" Population.h
		$(CC) $(CFLAGS) -O3 -fopenmp -march=native -std=c++11 -o "BRKGA" BRKGA.h
		$(CC) $(CFLAGS) -O3 -fopenmp -march=native -std=c++11 -o "KTNS" KTNS.h
		$(CC) $(CFLAGS) -O3 -fopenmp -march=native -std=c++11 -o "MTRand" MTRand.h
	 	$(CC) $(CFLAGS) -O3 -fopenmp -march=native -std=c++11 -o "delta_avaliacao" delta_avaliacao.h
		$(CC) $(CFLAGS) -O3 -fopenmp -march=native -std=c++11 -o "Buscas" Buscas.h
		$(CC) $(CFLAGS) -O3 -fopenmp -march=native -std=c++11 -o "SampleDecoder" SampleDecoder.h
		$(CC) $(CFLAGS) -O3 -fopenmp -march=native -std=c++11 -o "samplecode" samplecode.cpp
		$(CC) $(CFLAGS) -O3 -fopenmp -march=native -std=c++11 -o "run" run.cpp
