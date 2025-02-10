/*
 * SampleDecoder.h
 *
 * Any decoder must have the format below, i.e., implement the method decode(std::vector< double >&)
 * returning a double corresponding to the fitness of that vector. If parallel decoding is to be
 * used in the BRKGA framework, then the decode() method _must_ be thread-safe; the best way to
 * guarantee this is by adding 'const' to the end of decode() so that the property will be checked
 * at compile time.
 *
 * The chromosome inside the BRKGA framework can be changed if desired. To do so, just use the
 * first signature of decode() which allows for modification. Please use double values in the
 * interval [0,1) when updating, thus obeying the BRKGA guidelines.
 *
 *  Created on: Jan 14, 2011
 *      Author: rtoso
 */

#ifndef SAMPLEDECODER_H
#define SAMPLEDECODER_H

#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <chrono>
#include <cstdio>
#include "KTNS.h"
#include "Buscas.h"
#define maxsize 4501
extern bool vai;
extern unsigned n;
extern int s; // retorno de programas externos;
extern std::vector<int>solution; 
extern unsigned generation;

std::ofstream fileLKH;
std::ifstream fileSolutionLKH;
std::ifstream fileTeste;



class SampleDecoder {
public:
	SampleDecoder()  { }
	// SampleDecoder(std::vector<unsigned> _tProcessamento): tProcessamento(_tProcessamento) { }
	~SampleDecoder() { }

	double decode(std::vector< double >& chromosome) const{
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator(seed);
		std::uniform_real_distribution<double> distribution(0,1);

		std::vector<int>jobs;

		std::vector < std::pair < double, unsigned > > ranking(chromosome.size());
		for(unsigned i = 0; i < chromosome.size(); ++i){
			ranking[i]=std::pair<double,unsigned>(chromosome[i],i);
		}
		
		std::sort(ranking.begin(), ranking.end());

		jobs.clear();
		for(std::vector<std::pair<double, unsigned>>::const_iterator i = ranking.begin(); i!=ranking.end(); ++i){
			jobs.push_back(i->second);
		}
		

		long trocas = KTNS(jobs);
		return trocas;
	}

private:
	std::vector<unsigned> tProcessamento;
};

#endif
