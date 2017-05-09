#include <stdlib.h>
#include <stdio.h>
 

//screen /dev/ttyACM0

int main(){
	
	FILE* fichier;

	char* port_serie = "/dev/ttyACM0";
	char q;
	int angle[2] = {0,0};
	int coeff = 1;

	printf("COUCOCUOCUCOUC : %d %d %d\n",1/2, 2/3,5/2);

	while(getchar() != 'q'){

		//Ouverture tty
		fichier = fopen(port_serie,"w");
		if(fichier==NULL){
			printf("Impossible d'ouvrir %s\n",port_serie);
			perror("fopen failed for /dev/ttyACM0" );
			exit( EXIT_FAILURE );
		}


	/*	//Lecture tty
		printf("Valeurs moteurs xxx et yyy actuelles :\n");
		fscanf(fichier,"%d %d",angle,angle+1);
		printf("\nxxx=%d; yyy=%d\n",angle[0]*coeff,angle[1]*coeff);
	*/
		//Ecriture tty
		printf("\n Valeurs moteurs xxx,yyy ? ");
		
		while(scanf(" %d %d", angle,angle+1) != 2){
			printf("Angles incorrects, mets les bonnes valeurs cette fois...\n");
			getchar();
		}
	
		fprintf(fichier,"%d\n",angle[0]*coeff);
		fprintf(fichier,"%d\n",angle[1]*coeff);
		printf("Valeur actuelles : xxx=%d yyy=%d\n", angle[0],angle[1]);

		
		//Quitter
		fclose(fichier);
		printf("\nq pour quitter\n");
		scanf(" %c",&q);
		if(q == 'q') break;
	}
	
	return 0;
}
/*fwrite et fread

	read(angle,sizeof(int),1,fichier);
	read(angle+1,sizeof(int),1,fichier);

	fwrite(angle,1,sizeof(int),fichier);
	fwrite(angle+1,1,sizeof(int),fichier);
*/

/*putc()
	char buffer[255];
	char* c;
	char* c0;
	int num;
		c=buffer;
		c0=buffer;
		for(num=0;num<2;){
		  printf("a\n");  
		    *c=getc(stdin); // echo input back to terminal
		    fputc(*c,fichier);
		    if(*c == 'q')
		        break ; 
		    if((*c == '\n') || (*c == '\r') || (*c == '\0') || (*c == ' ') || (*c == ','))
		    {
		        //
		        *c='\0'; 
		        angle[num]=atoi(c0);
		        c0=c+1;
		        num++;
		    }
		    c++;
		  }
*/
