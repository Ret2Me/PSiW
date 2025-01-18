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
		sleep(10);
		close(file_descriptors[0]); // i tak nie uzywamy odczytu wiec zamknijmy fd;
		char message[] = "Witaj politechniko :)";
		write(file_descriptors[1], message, sizeof(message) / sizeof(message[0]));
		printf("Wyslano wiadomosc do rodzica\n");
		exit(0);
	}

	close(file_descriptors[1]); // i tak nie uzywamy zapisu jako rodzic :)
	
	printf("Czekam na wiadomosc od dziecka...\n");
	char buffer[80];
	int err = read(file_descriptors[0], buffer, 80);
	if (err == -1) {
		perror("read");
	}

	// funkcja read powinna zwrocic ilosc odczytanych byte-ow
	printf("Funkcja read zwrocila wartosc: %d\n", err);
	printf("Otrzymalem wiadomosc: \"%s\"\n", buffer);

	// funkcja read nie bedzie czekac na naplywajace dane poniewaz zaden proces
	// nie ma otwartego anonimowego pipe-a a wiec dane i tak nigdy by nie naplynely
	printf("\n\nJeszcze raz sprobuje przeczytac cos z pipe-a\n");
	memset(buffer, 0, 80);   // czyscimy buffer
	err = read(file_descriptors[0], buffer, 80);
	if (err == -1) {
		perror("read");
	}
	printf("Funkcja read zwrocila wartosc: %d\n", err);
	printf("Otrzymalem wiadomosc: \"%s\"\n", buffer);


}
