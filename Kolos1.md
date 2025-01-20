# PSiW opracowanie

### Deskryptory
Kazdy program na linuxie tworzy 3 podstawowe deskryptory umozliwiajace   
uzytkownikowi interakcje z programem:  
0 - STDIN (Input)  
1 - STDOUT (Output)  
2 - STDERR (Komunikaty o błędach)  


### Fork
Fork uruchamia "kopię" programu który kontynuuje wykonywanie kodu w miejscu 
w którym została wywołana funkcja `int pid = fork()`. Funkcja fork w dziecku naszego procesu zwróci 0
natomiast proces rodzic (eng. parent) w zmiennej pid będzie mieć dostęp do ProcessID nowo utworzonego dziecka.



### Pipe-y
Pipey (pl. potoki) umoliwiają komunikację między procesami.   
Kazdy pipe składa się z dwóch deskryptorów pierwszy umozliwiający zapis do naszego pipe-u
oraz drugi umozliwiający odczyt napływających danych.
  
Warto zwrócić uwagę ze jesli uzywamy forka zarowno rodzic jak i dziecko dysponuje niezaleznym od siebie kompletem deskryptorow. Zamkniecie deskryptora dedykowanego zapisowi przez rodzica nie skutkuje zamknieciem go u dziecka.  
  
Dane raz odczytane z pipe-u zostają natychmiastowo usunięte.

> [!CAUTION]  
> Zwróć uwagę że potoki zwracają deskryptory w odwrotnej kolejności niż STDIN / STDOUT 

> [!CAUTION]  
> Jeśli spróbujemy dokonać zapisu do pipe-a którego nic nie nasłuchuje nasz proces otrzyma sygnał SIGPIPE który powoduje natychmiastowe zamknięcie programu. 

> [!NOTE]  
> Nalezy zwrocic tez uwage ze nie jestesmy w stanie przesuwac sie po danych wysylanych przez pipe z wykorzystaniem
lseeka (umozliwia on przesuwanie kursora jedynie w przypadku danych odczytywanych z "dysku").  




```C
// pipes.c
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
		sleep(5);
		close(file_descriptors[0]); // i tak nie uzywamy odczytu wiec zamknijmy fd;
		char message[] = "Witaj politechniko :)";
		write(file_descriptors[1], message, sizeof(message));
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
    // funkcja read zwróci wartość 0
	printf("\n\nJeszcze raz sprobuje przeczytac cos z pipe-a\n");
	memset(buffer, 0, 80);   // czyscimy buffer
	err = read(file_descriptors[0], buffer, 80);
	if (err == -1) {
		perror("read");
	}
	printf("Funkcja read zwrocila wartosc: %d\n", err);
	printf("Otrzymalem wiadomosc: \"%s\"\n", buffer);
}
```

```C
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
		// nikt nie nasluchuje jego komunikatow
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
```

Output:
```BASH
❯ gcc pipe2.c && ./a.out
Probuje wyslac wiadomosc do dziecka...
❯ echo $?
141
```
Jak mozemy zauwazyc nasz proces niespodziewanie zakonczyl się z kodem 141.  
Sygnał 141 oznacz zakończenie działania spowodowane przerwaniem potoku "Broken Pipe" (SIGPIPE)

### FIFO (ak. Names Pipe)
Kolejka nazwana różni się od pipeu tym ze procesy komunikujace się przez nią nie muszą być spokrewnione (nie muszą być forkami).


Dostępne flagi przy otwieraniu FIFO:
```
-O_WRONLY - otwieranie lacza tylko do pisania
-O_RDONLY - otwieranie lacza tylko do czytania
-O_RDWR - otwieranie lacza do pisania i czytania
-O_NDELAY - okresla czy operacje, ktorych wykonanie nie moze byc natychmiast wykonane maja byc blokowane. Flaga ustawiona - brak blokowania.
```

> [!CAUTION]  
> Jeśli spróbujemy dokonać zapisu do pipe-a którego nic nie nasłuchuje nasz proces otrzyma sygnał SIGPIPE który powoduje natychmiastowe zamknięcie programu.

> [!NOTE]  
> Jeśli spróbujemy utworzyć kolejkę w ścieżce pod którą juz istnieje kolejka funkcja mkfifo() zwroci blad `[EEXIST] The named file exists.`


Aby utworzyć kolejkę wystarczy wykorzystać funkcję `int mkfifo(const char *pathname, mode_t mode);`
```C
#include <stdlib.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

int main() {

	// tworzy nowe fifo w folderze /tmp/ z permisjami do odczytu i zapisu
	char path[] = "/tmp/put_fifo";
    if (mkfifo(path, 0666) == -1)
        if (errno == EEXIST)
            printf("FIFO already exists: %s\n", path);

/*

Output:
❯ gcc fifo.c && ./a.out     <- utworzono prawidlowo FIFO 
❯ gcc fifo.c && ./a.out     <- FIFO juz istnieje 
FIFO already exists: /tmp/put_fifo

*/
}
```

---
> [!NOTE]  
> Jeśli spróbujemy otworzyć kolejkę tylko do odczytu / zapisu gdy nie ma procesów z otwartą kolejką do komplementarnej operacji w zaleności od tego czy została ustawiona flaga `O_NONBLOCK` proces będzie oczekiwać na otwarcie kolejki, lub zwróci błąd przy otwieraniu `ENXIO` i będzie kontynuować wykonywanie.

```C
#include <fcntl.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

int main() {

	// tworzy nowe fifo w folderze /tmp/ z permisjami do odczytu i zapisu
	char path[] = "/tmp/put_fifo";
    if (mkfifo(path, 0666) == -1)
        if (errno == EEXIST)
            printf("FIFO already exists: %s\n", path);

	if (open(path, O_RDONLY) == -1) {
		perror("open");
	}
}
/*

Output:
❯ gcc fifo2.c && ./a.out
FIFO already exists: /tmp/put_fifo
...

*/
```

### ExecXXX

- `execl(const char *path, const char *arg0, ..., (char *)0)`  
   Uruchamia nowy program, wskazany przez ścieżkę path, przekazując argumenty w postaci listy argumentów zakończonej NULL-em.
   Uzywa zmiennych środowiskowych procesu.  
   ```
   execl("/bin/ls", "ls", "-l", "/home", (char *)0);
   ```
   ---
- `execle(const char *path, const char *arg0, ..., (char *)0, char *const envp[])`  
   Podobne do execl, ale pozwala dodatkowo przekazać tablicę zmiennych środowiskowych envp, które nadpisze bieżące zmienne środowiskowe procesu.
   ```C
   char *env[] = {"PATH=/usr/bin", "USER=myuser", NULL};
   execle("/bin/ls", "ls", "-l", "/home", (char *)0, env);
   ```
   ---
- `execlp(const char *file, const char *arg0, ..., (char *)0)`  
  Działa jak execl, ale file jest wyszukiwane w ścieżce systemowej PATH, jeśli nie podano pełnej ścieżki.  
  ```C
  execlp("ls", "ls", "-l", "/home", (char *)0);
  ```
  ---
- `execv(const char *path, char *const argv[])`
  Uruchamia nowy program, wskazany przez pełną ścieżkę path, przekazując argumenty w postaci tablicy wskaźników argv[].  
  ```
  char *args[] = {"ls", "-l", "/home", NULL};
  execv("/bin/ls", args);
  ```
  ---
- `execvp(const char *file, char *const argv[])`
  Działa jak execv, ale file jest wyszukiwane w ścieżce systemowej PATH, jeśli nie podano pełnej ścieżki.  
  ```C
  char *args[] = {"ls", "-l", "/home", NULL};
  execvp("ls", args);
  ```
  ---
- `execvP(const char *file, const char *search_path, char *const argv[])`
  Działa jak execvp, ale pozwala przekazać niestandardową ścieżkę wyszukiwania programów poprzez parametr search_path.  
  ```C
  char *args[] = {"ls", "-l", "/home", NULL};
  execvP("ls", "/bin:/usr/bin", args);
  ```




"Jeden z execXXX("ls", "kat", ">", "plik", 0) - jeden z nich ma że odpala komendę w shellu więc to zapisze odp ls'a w pliku, inne nie"  
Przykład:  
```C
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main() {
        execl("/bin/sh", "sh", "-c", "ls /tmp > /tmp/output.txt", (char *)NULL);
}
```

### Open
Funkcja open odpowiada za utworzenie file_descriptora na podstawie ścieki do pliku.  
  
`open(const char *path, int oflag, ...)`  
ew. jeśli `O_CREAT` zostało ustawione:  
`open(const char *path, int oflag, mode_t mode)`  
  
> [!CAUTION]  
> Jeśli zdecydujemy się uzyc open z flaga O_CREAT trzeba zwrocic uwagę ze program jest uruchomiony z `umask-iem` (umask – set file creation mode mask).  
>    
> Aby sprawdzić aktualne ustawienie umaska nalezy wpisac w terminalu komende `umask`. Przekazane przez parametr uprawnienia zostaną "pomniejszone" o zawartość umaska.  
  
  
Dostępne flagi:
```
     The flags specified for the oflag argument must include exactly one of the following file access modes:

           O_RDONLY        open for reading only
           O_WRONLY        open for writing only
           O_RDWR          open for reading and writing
           O_SEARCH        open directory for searching
           O_EXEC          open for execute only

     In addition any combination of the following values can be or'ed in oflag:

           O_NONBLOCK      do not block on open or for data to become available
           O_APPEND        append on each write
           O_CREAT         create file if it does not exist
           O_TRUNC         truncate size to 0
           O_EXCL          error if O_CREAT and the file exists
           O_SHLOCK        atomically obtain a shared lock
           O_EXLOCK        atomically obtain an exclusive lock
           O_DIRECTORY     restrict open to a directory
           O_NOFOLLOW      do not follow symlinks
           O_SYMLINK       allow open of symlinks
           O_EVTONLY       descriptor requested for event notifications only
           O_CLOEXEC       mark as close-on-exec
           O_NOFOLLOW_ANY  do not follow symlinks in the entire path.
```

```C
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main() {

    char pathname[] = "/tmp/test.txt";
    int fd = open(pathname, O_RDWR | O_CREAT, 0777);
    execlp("ls", "ls", "-l", "/tmp/", NULL);
    return 0;
}
/*
Output:

sh-3.2# gcc open.c && ./a.out
total 8
srwx------  1 prywatne  wheel    0 16 sty 10:23 2f093fe9-a235-5d1c-9a62-3fae2cdf7eb1
srwx------  1 prywatne  wheel    0 16 sty 10:23 7c3338c5-8eec-50ae-bc4a-1f6969419936
-rwxr-xr-x@ 1 root      wheel    0 16 sty 10:45 test.txt

*/
```

### Wait
Funkcja wait wywoływana jest zazwyczaj przez rodzica, aby poczekał on na zakończenie pracy swojego dziecka.
Funkcja wait zwraca PID dziecka które zakończyło działanie oraz zmienną state do której zapisywany jest exit code programu,



???
- kod z execXXX i pytanie która komenda da taki sam efekt np. ls | tr -abc, ls > tr -abc, tr -abc | ls, tr -abc > ls



- krótki kod w stylu
open(..., O_CREAT | O_RDWR | O_APPEND)
write()
lseek()
write()
close()

I jest powiedziane że powyższy kod został wykonany dwa razy i pytanie jest jaka jest zawartość pliku. Ogółem to w przypadku jak jest O_APPEND to każdy write idzie na koniec pliku więc lseek tu nic nie robi. Mega podchwytliwe, w MANie jest napisane że APPEND tak się zachowuje.




- Pytanie w stylu z jakimi uprawnieniami użytkownik stworzy plik jeśli typowo nie podał ich w funkcji?  
Iirc to jest odp że plik będzie miał uprawnienia takie jak umask'a użytkownika w systemie?? Czy coś takiego  
`Prawidłową odpowiedzią powinno być umask z jakim został uruchomiony proces przez uzytkownika`

- ogarnij kiedy if zwraca (true czy false jak masz if (fork()) {}

- był krótki kod który wyglądał jakoś tak i było pytanie ile procesów odpali się podczas istnienia tego procesu ( główny procent, rodzic też się wlicza, ale jak się o to zapytałem czy tak jest to na luzie odpowiedział że tak więc to nie jest wiedza tajemna)
int main() {

if (fork())
if (fork())
...

fork()
}
- Było kilka pytań z kodem i pytanie czy taki kod zadziała, głównie jakiś open - są zasady czy możesz pominąć 3 parametr w open w zależności od tego jakie flagi dałaś 

- był kod z jednym forkiem i waitem i odpowiedzi jak będzie wyglądał output.

- były pytania o sygnały. Jedno z nich to było pytanie których sygnałów nie można złapać i faktycznie są dwa których nie można- jest info w MANie, ale nie pamiętam gdzie, pewnie MAN signal, albo MAN 2 signal

- były pytania o jakieś funkcje np. getpid, getppid
- były zadania w stylu 
close(1)
dup(0)
...
...
...
I chodziło w nich o to że taki dup duplikuje wskazany deskryptor na inny najniższy dostępny więc stdin duplikujesz na stdout i jest pytanie np jaki jest tego rezultat, albo jakieś wykonanie execXXX i co się stanie

```C
for (int i = 0; i < 100; i++) {
	int fd = open(…);
}
