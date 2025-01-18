#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main() {
	int file_descriptors[2];
	
	if (pipe(file_descriptors) == -1) {
		perror("pipe");
	}
	
	// tworzy dziecko
	int pid = fork();
	if (pid == -1) {
		perror("fork");
	}

	if (pid == 0) {
		// kod zostanie wykonany przez dziecko

		// zamykamy oba file_descriptory 
		// juz tylko rodzic probuje zapisac do pipe
		// nikt nie nasluchuje jego danych
		close(file_descriptors[0]); 
		close(file_descriptors[1]);
		sleep(100);
		exit(0);
	}
	
	sleep(5);	
	close(file_descriptors[0]); // i tak nie uzywamy odczytu jako rodzic :)
	printf("Probuje wyslac wiadomosc do dziecka...\n");
	char message[] = "Witaj politechniko :)";
	int err = write(file_descriptors[1], message, sizeof(message));
	printf("Nawet nie wyswietli tego tekstu bo proces sie otrzyma sygnal Broken Pipe");
}
