#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int check(char*);

int main(int var, char** args){
	if (var != 3){
		fputs("Error: invalid number of arguments\n", stderr);
		exit(1);
	}
	if (strlen(args[2]) != 7){
		fputs("Error: invalid length of number\n", stderr);
		exit(1);
	}
	int variants = atoi(args[1]);
	int number = atoi(args[2]);
	if (variants <= 0){
		if (variants < 0){
                	fputs("Error: variants lower than zero\n", stderr);
                	exit(1);
        	}
		if (!check(args[1])){
			fputs("Error: invalid argument - variants\n", stderr);
			exit(1);
		}
	}
	if (number <= 0){
		if (number < 0){
                	fputs("Error: number lower than zero\n", stderr);
                	exit(1);
        	}
		if (!check(args[2])){
			fputs("Error: invalid argument - number\n", stderr);
			exit(1);
		}
	}
	int result = number % variants + 1;
	char buf[256];
	sprintf(buf, "Your variant is: %d\n", result);
	fputs(buf, stdout);
}

int check(char* number){
	for (int i = 0; i < strlen(number); i++){
		if (number[i] != '0'){
			return 0;
		}
	}
	return 1;
}
