#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv)
{
	FILE* fp;
	char *opcija;
	char *string;
	char *string2;
	char *izraz;
	char *broj;
	char komanda[200];
	size_t num_of_bytes = 100;
        char *str;

	while(opcija[0] != 'Q') 
	{

	
		printf("1: Pročitaj trenutno stanje stringa\n");
		printf("2: Upiši novi string\n");
		printf("3: Konkataniraj string na trenutni\n");
		printf("4: Izbriši čitav string\n");
		printf("5: Izbriši vodeće i prateće space karaktere\n");
		printf("6: Izbriši izraz iz stringa\n");
		printf("7: Izbriši poslednjih n karaktera iz stringa\n");
		printf("Unesite broj zeljene opcije ili Q za kraj\n");
		
		opcija = (char *)malloc(2);
		getline (&opcija, &num_of_bytes, stdin);

		if (*opcija == '2') 
		{		
		printf("Unesite string:\n");	
		string = (char *)malloc(num_of_bytes+1);
		getline (&string, &num_of_bytes, stdin);
		}

		if (*opcija == '3') 
		{		
		printf("Unesite string:\n");	
		string2 = (char *)malloc(num_of_bytes+1);
		getline (&string2, &num_of_bytes, stdin);
		}
		if (*opcija == '6') 
		{	
		printf("Unesite izraz koji zelite da izbrisete:\n");	
		izraz = (char *)malloc(num_of_bytes+1);
		getline (&izraz, &num_of_bytes, stdin);
		}

		if (*opcija == '7') 
		{
		printf("Unesite broj karaktera koje zelite da izbrisete:\n");	
		broj = (char *)malloc(num_of_bytes+1);
		getline (&broj, &num_of_bytes, stdin);
		}

 		switch(*opcija)
		{
			case '2':
				strcpy(komanda,"string=");
				strncat (komanda, string, 100);
				break;

			case '3':
				strcpy(komanda,"append=");
				strncat (komanda, string2, 100);
				break;

			case '4':
				strcpy(komanda,"clear");
				break;

	
			case '5':
				strcpy(komanda,"shrink");
				break;
			
			case '6':
				strcpy(komanda,"remove=");
				strncat (komanda, izraz, 100);
				break;

			case '7':
				strcpy(komanda,"truncate=");
				strncat (komanda, broj, 3);
				break;
		}

		if(*opcija =='1')
		{
		fp = fopen("/dev/stred", "r");
		if(fp==NULL)
		{
			puts("Problem pri otvaranju /dev/stred");
			return -1;
		}


		str = (char *)malloc(num_of_bytes+1); 
		getline(&str, &num_of_bytes, fp); 
		puts(str);
	
		if(fclose(fp))
		{
			puts("Problem pri zatvaranju /dev/stred");
			return -1;
		}

                free(str);
		}
		if(*opcija == '7' ||*opcija == '2' ||*opcija == '3' ||*opcija == '4' ||*opcija == '5' ||*opcija == '6' )
		{ 
			
		fp = fopen("/dev/stred", "w");
		if(fp == NULL)
		{
			printf("Problem pri otvaranju /dev/stred\n");
			return -1;
		}

		if(*opcija == '4' ||*opcija == '5')
		{
		strncat (komanda, "\n", 100);
		}
		fputs(komanda, fp);
		
		
		if(fclose(fp))
		{
			printf("Problem pri zatvaranju /dev/stred\n");
			return -1;
		}
		}
		if (*opcija == '2') free(string);
		if (*opcija == '3') free(string2);
		if (*opcija == '6') free(izraz);
		if (*opcija == '7') free(broj);
	}

	free(opcija);
	return 0;
}


