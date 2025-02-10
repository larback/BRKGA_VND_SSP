#include <dirent.h>
#include <cstdlib>
#include <string>
#include <fstream>

#include <iostream>
#include <vector>
#include <ctime>
#include <ratio>
#include <chrono>

#include "SampleDecoder.h"
#include "MTRand.h"
#include "BRKGA.h"
#include "KTNS.h"
#include "Buscas.h"
#define MAXBIT 40
#define maxsize 4501
typedef uint8_t u8;

std::vector < std::bitset <MAXBIT> > bitMatrix(202);


std::vector<std::vector<int>> matrixFerramentas;
unsigned n = 0; // tarefas
int m = 0; // máquinas
unsigned t = 0; // ferramentas
int c = 0; // tool magazine
unsigned tempoTroca = 0; // tempo de troca entre duas ferramentas
unsigned contadorUniversal = 0;
unsigned processamento;
std::vector<unsigned>tProcessamento; // tempo de processamento das tarefas


//4626
unsigned p = 50;	// size of population
int chromosome_size; // size of chromosome
const double pe = 0.30;	 //0.30 antes	// fraction of population to be the elite-set
const double pm = 0.25;	 // 0.25 antes	// fraction of population to be replaced by mutants
const double rhoe = 0.85;	// probability that offspring inherit an allele from elite parent
const unsigned K = 1;		// number of independent populations
const unsigned MAXT = 4;	// number of threads for parallel decoding
unsigned generation = 1;		// current generation
const unsigned X_INTVL = 500;	// exchange best individuals at every 100 generations
const unsigned X_NUMBER = 2;	// exchange top 2 best
const unsigned MAX_GENS = 200;	// run for 1000 gens
const long unsigned rngSeed = 0;	// seed to the random number generator
MTRand rng(rngSeed);				// initialize the random number generator
const unsigned MAX_VOID = 1000; //  gerações sem melhoria permitidos

unsigned nVoid = 0; // numero de gerações sem melhorias


double tempoBuscas[4]; // VND, SWITCH, US, 2OPT
long execucoesBuscas[4];
long melhoriasBuscas[4];
double mediaMelhorias[4];
double primeiroMakespan;

bool vai = true;


int s; // Variável para retorno de programas externos

std::vector<int>solution;

std::ofstream fileR;
std::ifstream file;
std::ofstream fileSolution;



int main(int argc, char* argv[]) {

	using namespace std::chrono;
	std::srand ( unsigned ( std::time(0) ) );

	tProcessamento.clear();
	matrixFerramentas.clear();
	cin >> n;
	cin >> t;
	cin >> c;
	// Configurações do algoritmo
	chromosome_size = n;
	p = n; 


	for (unsigned j = 0; j<t; j++){
		std::vector<int> tmpF;
		for (unsigned i = 0; i<n; ++i){
			int tooli = 0;
			cin >> tooli;
			tmpF.push_back(tooli);
		}
		matrixFerramentas.push_back(tmpF);
	}
	for (int i=0;i<4;++i){
		tempoBuscas[i] = 0; // VND, SWITCH, US, 2OPT
		execucoesBuscas[i] = 0;
		melhoriasBuscas[i] = 0;
		mediaMelhorias[i] = 0;
	}

	double teste_makespan = 9999999999;
	double solucaoInicial = -1;
	primeiroMakespan = solucaoInicial;

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	SampleDecoder decoder;			// initialize the decoder

	// initialize the BRKGA-based heuristic
	BRKGA< SampleDecoder, MTRand > algorithm(chromosome_size, p, pe, pm, rhoe, decoder, rng, K, MAXT);

	long teste_geracao = 0;
	high_resolution_clock::time_point t2;
	duration<double> time_span;
	cout << "Iniciando processo evolutivo do BRKGA...\n";
	do {
		algorithm.evolve();// evolve the population for one generation
		if((++generation) % X_INTVL == 0) {
			algorithm.exchangeElite(X_NUMBER);	// exchange top individuals
		}
    	if (solucaoInicial==-1) solucaoInicial = algorithm.getBestFitness();

		if (algorithm.getBestFitness()  < teste_makespan){
			nVoid = 0;
			teste_geracao = generation;
			teste_makespan = algorithm.getBestFitness();
		} else {
			++nVoid;
		}

		t2 = high_resolution_clock::now();
		time_span = duration_cast<duration<double>>(t2 - t1);
		cout << "Geração: " << generation << endl;
	} while (generation < MAX_GENS && nVoid < MAX_VOID && time_span.count()<7200);
	//} while (generation < MAX_GENS);

	t2 = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(t2 - t1);
 
	vector<double>ch =algorithm.getBestChromosome();

	std::vector < std::pair < double, unsigned > > ranking(ch.size());
	for(unsigned i = 0; i < ch.size(); ++i){
		ranking[i]=std::pair<double,unsigned>(ch[i],i);
	}
	
	std::sort(ranking.begin(), ranking.end());

	solution.clear();
	for(std::vector<std::pair<double, unsigned>>::const_iterator i = ranking.begin(); i!=ranking.end(); ++i){
		solution.push_back(i->second);
	}


	cout << "Makespan: " << teste_makespan << endl;
	cout << "Solução:\n";
	// Decodificar ch
	for (int i=0;i<solution.size();++i){
		cout << solution[i] << " ";
	}
	cout << endl;
	cout << KTNS(solution) << endl;

	double makespan = 00; // Trocar para o número de trocas

	fileSolution.open(argv[1]);
	// Corrigir cabecalho da solucao
	// inicial makespan tempo geracaoConverencia melhoriaVND TempoVND SW SW US US 2OPT 2OPT 
	fileSolution << solucaoInicial << " " << teste_makespan <<  " " << time_span.count() << " " << teste_geracao << " ";
	fileSolution << melhoriasBuscas[0]/execucoesBuscas[0] << " " << tempoBuscas[0]/execucoesBuscas[0] << " ";
	fileSolution << melhoriasBuscas[1]/execucoesBuscas[1] << " " << tempoBuscas[1]/execucoesBuscas[1] << " ";
	fileSolution << melhoriasBuscas[2]/execucoesBuscas[2] << " " << tempoBuscas[2]/execucoesBuscas[2] << " ";
	fileSolution << melhoriasBuscas[3]/execucoesBuscas[3] << " " << tempoBuscas[3]/execucoesBuscas[3] << " " << endl;

	
	// cout << makespan << " " << time_span.count() << " " << teste_geracao << " " << primeiroMakespan << " " << solucaoInicial << " " << tempoBuscas[0]/execucoesBuscas[0] << " " << tempoBuscas[1]/execucoesBuscas[1] << " " << tempoBuscas[2]/execucoesBuscas[2] << " " << mediaMelhorias[0]/melhoriasBuscas[0] << " " << mediaMelhorias[1]/melhoriasBuscas[1] << " " << mediaMelhorias[2]/melhoriasBuscas[2] << endl;
    fileSolution << "Parametros do Algoritmo\n";
  	fileSolution << "Tamanho da população: " << p << "\n";
  	fileSolution << "Fração da população para elite: " << pe << "\n";
  	fileSolution << "Fração da população trocada por mutantes: " << pm << "\n";
  	fileSolution << "Probabilidade de se herdar um alelo da elite: " << rhoe << "\n";
  	fileSolution << "Populações independentes: " << K << "\n";
  	fileSolution << "Threads para decodificacao paralela: " << MAXT << "\n";
  	fileSolution << "Quantidade máxima de gerações: " << MAX_GENS << "\n";

  	fileSolution << "\n\nParâmetros da instância\n";
  	fileSolution << "Tarefas: " << n << "\n";
  	// cout << "Máquinas: " << m << "\n";
  	fileSolution << "Ferramentas: " << t << "\n";
  	fileSolution << "Capacidade Magazine: " << c << "\n";
  	// cout << "Tempo de troca: " << tempoTroca << "\n";


    fileSolution << "\n\nMakespan: " << teste_makespan << "\n\n";
    fileSolution << "Tempo de execução: " << time_span.count() << " segundos \n\n";
    fileSolution <<endl<< "Melhor geração: " << teste_geracao << endl;

    fileSolution << "\nMakespan ao final da 1ª geração: " << solucaoInicial << "\n\n";

    fileSolution <<"\nBuscas Locais\n";
	fileSolution << "VND" << endl;
    fileSolution << "Tempo médio: " << tempoBuscas[0]/execucoesBuscas[0] << endl;
    fileSolution << "Melhora média: " << melhoriasBuscas[0]/execucoesBuscas[0] << endl;
    fileSolution << "Execuções: " << execucoesBuscas[0]<<endl;
   
   
    fileSolution << "Switch_pair" << endl;
    fileSolution << "Tempo médio: " << tempoBuscas[1]/execucoesBuscas[1] << endl;
    fileSolution << "Melhora média: " << melhoriasBuscas[1]/execucoesBuscas[1] << endl;
    fileSolution << "Execuções: " << execucoesBuscas[1]<<endl;

    fileSolution << "US" << endl;
    fileSolution << "Tempo médio: " << tempoBuscas[2]/execucoesBuscas[2] << endl;
    fileSolution << "Melhora média: " << melhoriasBuscas[2]/execucoesBuscas[2] << endl;
    fileSolution << "Execuções: " << execucoesBuscas[2]<<endl;

    fileSolution << "2OPT" << endl;
    fileSolution << "Tempo médio: " << tempoBuscas[3]/execucoesBuscas[3] << endl;
    fileSolution << "Melhora média: " << melhoriasBuscas[3]/execucoesBuscas[3] << endl;
    fileSolution << "Execuções: " << execucoesBuscas[3]<<endl;

    fileSolution << "Solução:\n";
	// Decodificar ch
	for (int i=0;i<solution.size();++i){
		fileSolution << solution[i] << " ";
	}
    fileSolution << "Total de gerações: " << generation << "\n";
    // ******************************

	fileSolution.close();
	return 0;
}
