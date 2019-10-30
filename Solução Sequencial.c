#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "timer.h"


double retornaseno(double entrada){
  return sin(entrada);
}

double Media(double comprimento, double altura) {
  return (retornaseno((altura+comprimento)/2))*(altura-comprimento);
}

double Integral(double comprimento,double altura,double erro) {
  double area1=Media(comprimento,altura);
  double area2=Media(comprimento,(comprimento+altura)/2)+Media((comprimento+altura)/2, altura);
  if(fabs(area1-area2) <= erro)
    return area1;
  else
    return Integral(comprimento,(comprimento+altura)/2, erro)+Integral((comprimento+altura)/2, altura, erro);
}

int Ehvazio(char *c) {
  int i=1;
  if(c[0]!='0'&&c[0]!='+'&&c[0]!='-')
    return 0;
  int partedecimal=0;
  while(c[i]!='\0') {
    if(c[i]=='.')
      partedecimal++;
    else if(c[i]!= '0')
      return 0;
    i++;
  }
  if(partedecimal>1)
    return 0;
  else
    return 1;
}


int main(int argc, char *argv[]) {

  double comprimento=atof(argv[1]);
  double altura=atof(argv[2]);
  double erro=atof(argv[3]);

  if( (comprimento==0 && !(Ehvazio(argv[1]))) || (altura == 0 && !(Ehvazio(argv[2]))) || (erro==0 && !(Ehvazio(argv[3])))){
    printf("Erro na passagem de argumentos\n");
    return -1;}

  double t1,t2; //calcula tempo
	GET_TIME(t1);

  double areafinal=Integral(comprimento, altura, erro);
  printf("Area final = %lf\n", areafinal);
	printf("Tempo = %lf\n", t2-t1);
  GET_TIME(t2);
  return 0;
}






