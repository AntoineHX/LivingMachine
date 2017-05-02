#include <stdlib.h>
#include <stdio.h>

int main(){
	
	FILE* fichier;

	char* port_serie = "/dev/ttyACM0";
	char buffer[30];

	char c = '1';
	double angle[2];

		fichier = fopen(port_serie,"r+");
		if(fichier==NULL){
			printf("Impossible d'ouvrir %s\n",port_serie);
			return 0; 
		}

	while(c != 'q'){

		printf("Valeurs moteurs xxx et yyy actuelles :\n");


		read(angle,sizeof(double),1,fichier);
		read(angle+1,sizeof(double),1,fichier);
		//fscanf(fichier,"%d %d",angle,angle+1);

		printf("\nxxx=%lf; yyy=%lf\n",angle[0]*0.001,angle[1]*0.001);



/*
		fichier = fopen(port_serie,"w+");
		if(fichier==NULL){
			printf("Impossible d'ouvrir %s\n",port_serie);
			return 0; 
		}*/
		printf("\n Valeurs moteurs xxx,yyy ?\n");
		scanf("%lf %lf",angle,angle+1);

		fprintf(fichier,"%lf %lf",angle[0]*0.001,angle[1]*0.001);
/*
		fwrite(angle,1,sizeof(int),fichier);
		fwrite(angle+1,1,sizeof(int),fichier);*/

		printf("\nq pour quitter\n");

		c = getchar();
		c = getchar();


		

	}
	fclose(fichier);
	return 0;
}
