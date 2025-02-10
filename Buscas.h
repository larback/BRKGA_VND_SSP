#ifndef BUSCAS_H
#define BUSCAS_H

#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <string.h>
#include <bitset>
#include <random>
#include <ctime>
#include <ratio>
#include <string>
#include <fstream>


#include "KTNS.h"
#include "delta_avaliacao.h"
#define MAXBIT 40
#define maxsize 4501
extern std::vector < std::bitset <MAXBIT> > bitMatrix;
extern int m;
extern std::vector<unsigned>tProcessamento;
extern double tempoBuscas[4]; // IBNS, EFB, ONB
extern long execucoesBuscas[4];
extern long melhoriasBuscas[4];
extern double mediaMelhorias[4];
std::chrono::high_resolution_clock::time_point tb1;
std::chrono::high_resolution_clock::time_point tb2;
std::chrono::duration<double> time_span_b;
double IBSMakespan, EFBMakespan, ONBMakespan;




extern std::ofstream fileLKH;
extern std::ifstream fileSolutionLKH;
extern std::ifstream fileTeste;


using namespace std;
using namespace chrono;

high_resolution_clock::time_point t2_,t1_;
duration<double> time_span_;

void ONB_m(std::vector <int>& mCritica, long& cTimeCritica){
    // mCritica - solucao
    // cTimeCritica - trocas da máquina

    extern std::vector<std::vector<int>> matrixFerramentas;
  	extern unsigned t; // ferramentas
    std::pair<int, int>ONB1, ONB2;
    bool melhorou = true;
    while (melhorou){
        melhorou = false;
        std::vector<int> mAux1,mAux2;
        if (KTNS(mCritica)==0)
            return ;

        std::vector<int> linhas;
        for(int i = 0; i<t; ++i)
          linhas.push_back(i);
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle (linhas.begin(), linhas.end(), std::default_random_engine(seed));

        for (vector<int>::const_iterator i = linhas.begin(); i!= linhas.end(); ++i){
          ONB1 = std::make_pair(-1,-1);
          ONB2 = std::make_pair(-1,-1);
          for (unsigned j=0; j<mCritica.size();++j){
            if (matrixFerramentas[*i][mCritica[j]]==1){
              if (ONB1.first == -1){
                ONB1.first = j;
                while (j < mCritica.size() && matrixFerramentas[*i][mCritica[j]]==1) ++j;
                ONB1.second = j-1;
              } else {
                if (ONB2.first == -1){
                  ONB2.first = j;
                  while (j < mCritica.size() && matrixFerramentas[*i][mCritica[j]]==1) ++j;
                  ONB2.second = j-1;
                }
              }
              if (ONB2.first!=-1){
                int nMovimentos = ONB1.first - ONB1.second +1;
                int pivo = ONB1.first;
                int TPivo = 0;
                mAux1 = mCritica;
                mAux2 = mCritica;
                double c1,c2;
                for (int p=0;p<nMovimentos;++p){
                  // Insiro a esquerda do 2 ONB
                  TPivo=mAux1[pivo];
                  for (int pe=pivo;pe<ONB2.first;++pe)
                    mAux1[pe]=mAux1[pe+1];
                  mAux1[ONB2.first] = TPivo;
                  c1 = KTNS(mAux1);
                  // Insiro a direita do 2 ONB
                  TPivo=mAux2[pivo];
                  for (int pd=pivo;pd<ONB2.second;++pd)
                    mAux2[pd]=mAux2[pd+1];
                  mAux2[ONB2.second] = TPivo;
                  c2 = KTNS(mAux2);
                  if (c1 < cTimeCritica || c2 < cTimeCritica){
                    melhorou = true;
                    if (c1<c2){
                      // Fica à esquerda
                      mCritica = mAux1;
                      ONB2.first = ONB2.first -1;
                      cTimeCritica = c1;

                    } else {
                      // Fica à direita
                      mCritica = mAux2;
                      cTimeCritica = c2;
                    }
                  }
                  ++pivo;
                  mAux1=mCritica;
                  mAux2=mCritica;
                } // Fim dos movimentos ONB1->ONB2
                // Procura-se o proximo ONB
                ONB1.first = ONB2.first;
                ONB1.second = ONB2.second;
                ONB2 = make_pair(-1,-1);
              }
            }
          }
        } // fim das linhas
    } // wend

}

void rotacao(std::vector<int> &solution, long &trocas){
	int aux;
	long tempTrocas;
	std::vector<int>solutionTemp;
	solutionTemp.clear();
	solutionTemp = solution;
	//solution = f_solution;
	// tempTrocas = trocas;
	
	for (int i =0; i<n; ++i){
		aux = solutionTemp[0];
		solutionTemp.erase(solutionTemp.begin());
		solutionTemp.push_back(aux);
		tempTrocas = KTNS(solutionTemp);
		if (tempTrocas < trocas){
			trocas = tempTrocas;
			solution.clear();
			solution = solutionTemp;
		}
	}
}

void relocation(std::vector<int> &solution, long &trocas){
  long tempTrocas, auxTrocas;
	std::vector<int>solutionTemp;
  vector<int> order; // Ordem em que os vértices serão acessados.
  int job, tamanho;
  int destino = -1;
	solutionTemp.clear();
	solutionTemp = solution;
  for (unsigned i=0;i<solutionTemp.size();++i)
    order.push_back(i);
  random_shuffle(order.begin(),order.end());
  tamanho = solutionTemp.size();
  for (int i=0;i<tamanho;++i){
    job = solutionTemp[order[i]];
    solutionTemp.erase(solutionTemp.begin()+order[i]);
    for (int j=0;j<tamanho-1;++j){
      solutionTemp.insert(solutionTemp.begin()+j,job);
      auxTrocas = KTNS(solutionTemp);
      if (auxTrocas < tempTrocas){
        tempTrocas = auxTrocas;
        destino = j;
        break;
      }
      solutionTemp.erase(solutionTemp.begin()+j);
    }
    // Se achou um melhor lugar para este job
    if (destino!=-1){
      // solutionTemp.insert(solutionTemp.begin()+destino,job);
      solution.clear();
      solution = solutionTemp;
      trocas = tempTrocas;
      break;
    }else // volta o job para o local original
      solutionTemp.insert(solutionTemp.begin()+order[i],job);
  }
}

void inverte(std::vector<int> &solution, int a, int b){
  int aux;
  if (a>b){
    aux = a;
    a = b;
    b = aux;
  }
  for (int i=a;i<=b;++i){
    aux = solution[i];
    solution[i] = solution[b];
    solution[b] = aux;
    --b;
  }
}

void two_opt(std::vector<int> &solution, long &trocas){
  long tempTrocas, auxTrocas;
	std::vector<int>solutionTemp;
  vector<int> order; // Ordem em que os vértices serão acessados.
  bool feito = false;
  solutionTemp.clear();
  solutionTemp = solution;
  for (unsigned i=0;i<solutionTemp.size();++i){
    order.push_back(i);
  }

  // random_shuffle(order.begin(),order.end());

  for (int i=0;i<order.size()-1;++i){
    for (int j = i+1;j<order.size();++j){
      inverte(solutionTemp,order[i],order[j]);
      tempTrocas = KTNS(solutionTemp);
      if (tempTrocas < trocas){
        trocas = tempTrocas;
        solution.clear();
        solution = solutionTemp;
        feito = true;
        break;
      } else{
        solutionTemp.clear();
        solutionTemp = solution;
      }
    }
    if (feito) break;
  }
}


void two_optFull(std::vector<int> &solution, long &trocas){
  long tempTrocas, auxTrocas;
	std::vector<int>solutionTemp;
  vector<int> order; // Ordem em que os vértices serão acessados.
  bool feito = false;
  solutionTemp.clear();
  solutionTemp = solution;
  for (unsigned i=0;i<solutionTemp.size();++i){
    order.push_back(i);
  }
  // random_shuffle(order.begin(),order.end());
 
  while(true){
    feito = false;
    for (int i=0;i<order.size()-1;++i){
      for (int j = i+1;j<order.size();++j){
        inverte(solutionTemp,order[i],order[j]);
        tempTrocas = KTNS(solutionTemp);
        if (tempTrocas < trocas){
          trocas = tempTrocas;
          solution.clear();
          solution = solutionTemp;
          feito = true;
          i=0;
        } else{
          solutionTemp.clear();
          solutionTemp = solution;
        }
      }
    }
    if (!feito) break;
  }
}

void two_optBI(std::vector<int> &solution, long &trocas){
  long tempTrocas, auxTrocas;
	std::vector<int>solutionTemp;
  vector<int> order; // Ordem em que os vértices serão acessados.
  bool feito = false;
  int a,b;
  solutionTemp.clear();
  solutionTemp = solution;
  for (unsigned i=0;i<solutionTemp.size();++i){
    order.push_back(i);
  }

  random_shuffle(order.begin(),order.end());
  
  for (int i=0;i<order.size()-1;++i){
    for (int j = i+1;j<order.size();++j){
      inverte(solutionTemp,order[i],order[j]);
      tempTrocas = KTNS(solutionTemp);
      if (tempTrocas < trocas){
        trocas = tempTrocas;
        a = order[i];
        b = order[j];
        feito = true;
      } 
      solutionTemp.clear();
      solutionTemp = solution;
    }
  }
  if (feito){
    inverte(solution,a,b);
  }
}


// Conferir nome
void two_swap(std::vector<int> &solution, long &trocas){
  long tempTrocas, auxTrocas;
	std::vector<int>solutionTemp;
  vector<int> order,order2; // Ordem em que os vértices serão acessados.
  int job1, job2, tamanho;
  bool feito = false;
  solutionTemp.clear();
  solutionTemp = solution;
  for (unsigned i=0;i<solutionTemp.size();++i){
    order.push_back(i);
    order2.push_back(i);
  }
  random_shuffle(order.begin(),order.end());
  random_shuffle(order2.begin(),order2.end());

  for (int i=0;i<solutionTemp.size();++i){
    job1 = solutionTemp[order[i]];
    for (int j=0;j<solutionTemp.size();++j){
      job2 = solutionTemp[order2[j]];
      if (job1 == job2)
        continue;
      solutionTemp[order[i]] = job2;
      solutionTemp[order2[j]] = job1;
      tempTrocas = KTNS(solutionTemp);
      if (tempTrocas<trocas){
        feito = true;
        solution.clear();
        solution = solutionTemp;
        trocas = tempTrocas;
        break;
      } else{
        solutionTemp[order[i]] = job1;
        solutionTemp[order2[j]] = job2;
      }
    }
    if (feito)
      break;
  }

}


void swap_blocks(std::vector<int> &solution, long &trocas){

  vector<int> orderA, orderB, solutionTemp; // Ordem em que os vértices serão acessados.
  int pivo1, pivo2;
  bool jaFoi = false;
  long trocasTemp = trocas;
  for (unsigned i=0;i<solution.size();++i){
    orderA.push_back(i);
    orderB.push_back(i);
  }

  random_shuffle(orderA.begin(),orderA.end());
  random_shuffle(orderB.begin(),orderB.end());

  solutionTemp.clear();
  solutionTemp = solution;

  for (int i=0;i<orderA.size();++i){
    if ((orderA[i] == 0) || (orderA[i]==orderA.size()-1))
      continue;
    pivo1 = orderA[i];
    for (int j=0;j<orderB.size();++j){
      int aux;
      if ((orderB[j]>0) && (orderB[j]<orderB.size()-1) &&  ((orderB[j]<pivo1-2) || (orderB[j]>pivo1+2))){
        pivo2 = orderB[j];
        
        aux = solutionTemp[pivo1];
        solutionTemp[pivo1] = solutionTemp[pivo2];
        solutionTemp[pivo2] = aux;
        aux = solutionTemp[pivo1-1];
        solutionTemp[pivo1-1] = solutionTemp[pivo2-1];
        solutionTemp[pivo2-1] = aux;
        aux = solutionTemp[pivo1+1];
        solutionTemp[pivo1+1] = solutionTemp[pivo2+1];
        solutionTemp[pivo2+1] = aux;
        
        trocasTemp =  KTNS(solutionTemp);
        if (trocasTemp < trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = solutionTemp;
          jaFoi = true;
          break;
        } else {
          // Desfaz a troca;
          aux = solutionTemp[pivo1];
          solutionTemp[pivo1] = solutionTemp[pivo2];
          solutionTemp[pivo2] = aux;
          aux = solutionTemp[pivo1-1];
          solutionTemp[pivo1-1] = solutionTemp[pivo2-1];
          solutionTemp[pivo2-1] = aux;
          aux = solutionTemp[pivo1+1];
          solutionTemp[pivo1+1] = solutionTemp[pivo2+1];
          solutionTemp[pivo2+1] = aux;
        }
      }else
        continue;
    }
    if (jaFoi)
      break;
  }
}

void movimento1(std::vector<int> solution, std::vector<int> &temp, int p1, int p2, int p3){
  temp.clear();
  // Em ordem até o ponto 1
  for (int j=0;j<=p1;++j)
    temp.push_back(solution[j]);
  
  // Pega do ponto 2 até ponto 1 +1
  for (int j=p2;j>p1;--j)
    temp.push_back(solution[j]);
  
  // Pega do ponto 3 até o ponto 2 +1
  for (int j=p3;j>p2;--j)
    temp.push_back(solution[j]);
  
  // Pega do ponto 3 + 1 até o final
  for (int j=p3+1;j<solution.size();++j)
    temp.push_back(solution[j]);
  
}

void movimento2(std::vector<int> solution, std::vector<int> &temp, int p1, int p2, int p3){
  temp.clear();
  // Em ordem até o ponto 1
  for (int j=0;j<=p1;++j){
    temp.push_back(solution[j]);
  }
  // Ponto 1 segue para Ponto 2+1 até Ponto 3
  for (int j=p2+1;j<=p3;++j){
    temp.push_back(solution[j]);
  }
  // Pega do ponto 1+1 até o ponto 2
  for (int j=p1+1;j<=p2;++j){
    temp.push_back(solution[j]);
  }
  // Pega do ponto 3 + 1 até o final
  for (int j=p3+1;j<solution.size();++j){
    temp.push_back(solution[j]);
  }
}

void movimento3(std::vector<int> solution, std::vector<int> &temp, int p1, int p2, int p3){
  temp.clear();
  // Em ordem até o ponto 1
  for (int j=0;j<=p1;++j){
    temp.push_back(solution[j]);
  }
  // Ponto 1 segue para Ponto 3, inverte até Ponto 2 +1
  for (int j=p3;j>p2;--j){
    temp.push_back(solution[j]);
  }
  // Pega do Ponto 1+1 até Ponto 2
  for (int j=p1+1;j<=p2;++j){
    temp.push_back(solution[j]);
  }
  // Pega do ponto 3 + 1 até o final
  for (int j=p3+1;j<solution.size();++j){
    temp.push_back(solution[j]);
  }
}

void movimento4(std::vector<int> solution, std::vector<int> &temp, int p1, int p2, int p3){
  temp.clear();
  // Em ordem até o ponto 1
  for (int j=0;j<=p1;++j){
    temp.push_back(solution[j]);
  }
  // Ponto 1 segue para Ponto 3, inverte até Ponto 1 +1
  for (int j=p3;j>p1;--j){
    temp.push_back(solution[j]);
  }
  // Pega do ponto 3 + 1 até o final
  for (int j=p3+1;j<solution.size();++j){
    temp.push_back(solution[j]);
  }
}

void movimento5(std::vector<int> solution, std::vector<int> &temp, int p1, int p2, int p3){
  temp.clear();
  // Em ordem até o ponto 1
  for (int j=0;j<=p1;++j){
    temp.push_back(solution[j]);
  }
  // Ponto 1 segue para último, inverte até Ponto 3 + 1
  for (int j=solution.size()-1;j>p3;--j){
    temp.push_back(solution[j]);
  }
  // Pega do ponto 2 + 1 até o Ponto 3
  for (int j=p2+1;j<=p3;++j){
    temp.push_back(solution[j]);
  }
  // Pega do ponto 2, inverte até Ponto 1-1
  for (int j=p2;j>p1;--j){
    temp.push_back(solution[j]);
  }
}

void movimento6(std::vector<int> solution, std::vector<int> &temp, int p1, int p2, int p3){
  temp.clear();
  // Em ordem até o ponto 1
  for (int j=0;j<=p1;++j){
    temp.push_back(solution[j]);
  }
  // Ponto 2, inverte até Ponto 2+1
  for (int j=p2;j>=p1+1;--j){
    temp.push_back(solution[j]);
  }
  // Pega do Ponto 2+1 até o final
  for (int j=p2+1;j<solution.size();++j){
    temp.push_back(solution[j]);
  }
}

void movimento7(std::vector<int> solution, std::vector<int> &temp, int p1, int p2, int p3){
  temp.clear();
  // Em ordem até o ponto 1
  for (int j=0;j<=p1;++j){
    temp.push_back(solution[j]);
  }
  // Ponto 2+1, segue até Ponto 3
  for (int j=p2+1;j<=p3;++j){
    temp.push_back(solution[j]);
  }
  // Pega do Ponto 2, inverte até P1+1
  for (int j=p2;j>p1;--j){
    temp.push_back(solution[j]);
  }
  // Pega do Ponto 3+1, segue até o final
  for (int j=p3+1;j<solution.size();++j){
    temp.push_back(solution[j]);
  }
}
void three_opt(std::vector<int> &solution, long &trocas){
  std::vector<int> temp;
  long trocasTemp = 0;
  bool melhorou = false;

  for (int i=0;i<solution.size();++i){
    for (int j=i+1;j<solution.size();++j){
      for (int k=j+1;k<solution.size();++k){
        movimento1(solution, temp, i,j,k);
        trocasTemp = KTNS(temp); 
        if (trocasTemp<trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = temp;
          melhorou = true;
          break;
        }
        movimento2(solution, temp, i,j,k);
        trocasTemp = KTNS(temp); 
        if (trocasTemp<trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = temp;
          melhorou = true;
          break;
        }  

        movimento3(solution, temp, i,j,k);
        trocasTemp = KTNS(temp); 
        if (trocasTemp<trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = temp;
          melhorou = true;
          break;
        }  

        movimento4(solution, temp, i,j,k);
        trocasTemp = KTNS(temp); 
        if (trocasTemp<trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = temp;
          melhorou = true;
          break;
        }  

        movimento5(solution, temp, i,j,k);
        trocasTemp = KTNS(temp); 
        if (trocasTemp<trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = temp;
          melhorou = true;
          break;
        }  

        movimento6(solution, temp, i,j,k);
        trocasTemp = KTNS(temp); 
        if (trocasTemp<trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = temp;
          melhorou = true;
          break;
        }  

        movimento7(solution, temp, i,j,k);
        trocasTemp = KTNS(temp); 
        if (trocasTemp<trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = temp;
          melhorou = true;
          break;
        }  
      }
      if (melhorou) break;
    }    
    if (melhorou) break;
  }  

}
void three_optFull(std::vector<int> &solution, long &trocas){
  std::vector<int> temp;
  long trocasTemp = 0;
  bool melhorou = true;
  while (melhorou){
    melhorou = false;
    for (int i=0;i<solution.size();++i){
      for (int j=i+1;j<solution.size();++j){
        for (int k=j+1;k<solution.size();++k){
          movimento1(solution, temp, i,j,k);
          trocasTemp = KTNS(temp); 
          if (trocasTemp<trocas){
            trocas = trocasTemp;
            solution.clear();
            solution = temp;
            melhorou = true;
            i=0;
            // break;
          }
          movimento2(solution, temp, i,j,k);
          trocasTemp = KTNS(temp); 
          if (trocasTemp<trocas){
            trocas = trocasTemp;
            solution.clear();
            solution = temp;
            melhorou = true;
            i=0;
            // break;
          }  

          movimento3(solution, temp, i,j,k);
          trocasTemp = KTNS(temp); 
          if (trocasTemp<trocas){
            trocas = trocasTemp;
            solution.clear();
            solution = temp;
            melhorou = true;
            i=0;
            // break;
          }  

          movimento4(solution, temp, i,j,k);
          trocasTemp = KTNS(temp); 
          if (trocasTemp<trocas){
            trocas = trocasTemp;
            solution.clear();
            solution = temp;
            melhorou = true;
            i=0;
            //break;
          }  

          movimento5(solution, temp, i,j,k);
          trocasTemp = KTNS(temp); 
          if (trocasTemp<trocas){
            trocas = trocasTemp;
            solution.clear();
            solution = temp;
            melhorou = true;
            i=0;
            //break;
          }  

          movimento6(solution, temp, i,j,k);
          trocasTemp = KTNS(temp); 
          if (trocasTemp<trocas){
            trocas = trocasTemp;
            solution.clear();
            solution = temp;
            melhorou = true;
            i=0;
            //break;
          }  

          movimento7(solution, temp, i,j,k);
          trocasTemp = KTNS(temp); 
          if (trocasTemp<trocas){
            trocas = trocasTemp;
            solution.clear();
            solution = temp;
            melhorou = true;
            i=0;
            // break;
          }  
        }
      }    
    }  
  }
}

vector<int> usados;
bool jaFoi(int job){
  for(int i=0;i<usados.size();++i){
    if(usados[i]==job) {
     
      return true;

    } 
  }
  return false;
}

void US (std::vector<int> &solution, long &trocas) {
  long tempTrocas, auxTrocas;
  std::vector<int>solutionTemp;
  int job,p;
  tempTrocas = trocas;
  auxTrocas = trocas;
  solutionTemp = solution;
  usados.clear();

  for (int k=0;k<solution.size();++k){
    for (int i=0; i<solution.size();++i){
      job = solutionTemp[i];

      if (jaFoi(job) != true){
        solutionTemp.erase(solutionTemp.begin()+i);
        tempTrocas = KTNS(solutionTemp);
        if (tempTrocas <= auxTrocas){
          auxTrocas = tempTrocas;
          p = i;
        }
        // Volta o job para o lugar
        solutionTemp.insert(solutionTemp.begin()+i,job);
      }
    }

    job = solutionTemp[p];
    solutionTemp.erase(solutionTemp.begin()+p);
    usados.push_back(job);
   
    for (int i=0;i<=solutionTemp.size();++i){
      if (i==p) continue;
      solutionTemp.insert(solutionTemp.begin()+i, job);
      
      tempTrocas = KTNS(solutionTemp);
      if (tempTrocas < trocas){
        trocas = tempTrocas;
        solution.clear();
        solution = solutionTemp;
      }
      solutionTemp.erase(solutionTemp.begin()+i);
    }
    solutionTemp.insert(solutionTemp.begin()+p,job);
    auxTrocas = trocas;
  }
}

void switch_pair(std::vector<int> &solution, long &trocas){
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  srand(seed);
  std::vector<int> temp,p;
  int aux;
  long trocasTemp;
  p.clear();
  for (unsigned i=1;i<solution.size();++i)
    p.push_back(i);
  // random_shuffle(p.begin(),p.end());

  temp.clear();
  temp = solution;
  trocasTemp = trocas;
  for (int i=0;i<p.size();++i){
    aux = temp[p[i]-1];
    temp[p[i]-1] = temp[p[i]];
    temp[p[i]] = aux;
    trocasTemp = KTNS(temp);
    if (trocasTemp<trocas){
      solution.clear();
      solution=temp;
      trocas = trocasTemp;
    } else {
      aux = temp[p[i]-1];
      temp[p[i]-1] = temp[p[i]];
      temp[p[i]] = aux;
    }
  }
}

long VND(std::vector< double >& chromosome){
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator(seed);
  std::uniform_real_distribution<double> distribution(0,1);
  std::vector<int>jobs;
  long trocas;
  long trocas_temp, trocas_old;

  std::vector < std::pair < double, unsigned > > ranking(chromosome.size());
  for(unsigned i = 0; i < chromosome.size(); ++i){
    ranking[i]=std::pair<double,unsigned>(chromosome[i],i);
  }
  
  std::sort(ranking.begin(), ranking.end());

  jobs.clear();
  for(std::vector<std::pair<double, unsigned>>::const_iterator i = ranking.begin(); i!=ranking.end(); ++i){
    jobs.push_back(i->second);
  }
  trocas = KTNS(jobs);
  trocas_old = trocas;
  int idx = 1;
  while (idx < 4){
    if (idx == 1){
      trocas_temp = trocas;
      t1_ = high_resolution_clock::now();
      switch_pair(jobs, trocas);
      t2_ = high_resolution_clock::now();
	  	time_span_ = duration_cast<duration<double>>(t2_ - t1_);
      melhoriasBuscas[1] += (trocas_temp-trocas);
      execucoesBuscas[1]++;
		  tempoBuscas[1]+=time_span_.count();
      if (trocas < trocas_temp){
        idx = 1;
        trocas_temp = trocas;
      } else{
        idx++;
      }
    }
    if (idx == 2){
      trocas_temp = trocas;
      t1_ = high_resolution_clock::now();
      US(jobs, trocas);
      t2_ = high_resolution_clock::now();
	  	time_span_ = duration_cast<duration<double>>(t2_ - t1_);
      melhoriasBuscas[2] += (trocas_temp - trocas);
		  tempoBuscas[2]+=time_span_.count();
      execucoesBuscas[2]++;
      if (trocas < trocas_temp){
        idx = 1;
        trocas_temp = trocas;
      } else{
        idx++;
      }
    }
    if (idx == 3){
      trocas_temp = trocas;
      t1_ = high_resolution_clock::now();
      two_optFull(jobs, trocas);
      t2_ = high_resolution_clock::now();
	  	time_span_ = duration_cast<duration<double>>(t2_ - t1_);
      melhoriasBuscas[3] += (trocas_temp - trocas);
      execucoesBuscas[3]++;
		  tempoBuscas[3]+=time_span_.count();
      if (trocas < trocas_temp){
        idx = 1;
        trocas_temp = trocas;
      } else{
        idx++;
      }
    }
  }
  // Fix the chromosome
  if (trocas < trocas_old){
    for (int i=0;i<jobs.size();++i){
      ranking[i].second = jobs[i];
    }
    
    // std::vector<double> tempKey;
    // for (int b=0;b<chromosome.size();++b)
    //   tempKey.push_back(distribution(generator));

    // std::sort(tempKey.begin(), tempKey.end());
  }

  return trocas;
}





#endif
