# PSiW opracowanie Kolos 2  

## Wstęp  
Mechanizmy IPC (Inter Process Communication) słóżą do komunikacji między programami w ramach tego samego  
systemu operacyjnego (dokładniej namespace-u w ramach danego systemu).  
  
Przykładami takich mechanizmów są:  
- Kolejki Komunikatów  
- Pamięć współdzielona  
- Semafory  
---

## Kolejki komunikatów  
#### Podstawowe komponenty:  
- **Kolejka**:  
  - Każda kolejka ma swój unikalny klucz (identyfikator) którym zawsze jest liczba.  
  - Możliwym jest wysyłanie komunikatów do kolejki której nikt nie nasłuchuje (wiadomości w takiej sytuacji są buforowane i oczekują na odbiór). Dzięki temu nazywamy ją asynchroniczną.
  - Komunikaty są przetrzymywane w kolejce nawet po zakończeniu procesu nadawcy i znajdują się w kolejce tak długo aż ktoś ich nie odczyta bądź kolejka nie zostanie usunięta.  
  - Odbiorca może oczekiwać na: dowolny komunikat bądź komunikat o określonym typie.
  - Procesy nie muszą być ze sobą spokrewnione.  
- **Komunikat**:
  - Każdy komunikat ma 3 atrybuty: typ, długość oraz zawartość.  
  
#### Podstawowe funkcje:  
`int msgget(key_t key, int msgflg);` - utworzenie nowej / otworzenie istniejącej kolejki  
1. **key** - klucz kolejki
2. **msgflg** - dodatkowe parametry takie jestk **IPC_CREAT** (niezbędny do utworzenia kolejki) oraz **IPC_EXCL**
  
`int msgsnd(int msgid, struct msgbuf *msgp, int msgs, int msgflg)` - dodatnie komunikatu na koniec kolejki   
  
1. **msgid** - identyfikator kolejki  
2. **msgbuf** - wskaźnik do obszaru pamięci zawierającego treść komunikatu  
3. ***msgs** - rozmiar właściwej treści komunikatu  
4. **msgflg** - flaga specyfikująca zachowanie się funkcji w warunkach nietypowych:  
   0  / IPC_NOWAI - jeśli kolejka jest pełna wiadomość nie zostanie zapisana, proces kontynuuje działanie.  
  
`int msgrcv(int msgid, struct msgbuf *msgp, int msgs, long msgtyp, int msgflg)` - odczyt komunikatów z kojeki
1. **msgid** - klucz (identyfikator) naszej kolejki  
2. ***msgp** - wskaźnik do struktury zawierającej kreść komunikatu  
3. **msgs** -rozmiar właściwej treści komunikatu  
4. **msgtyp** - typ komunikatu jaki ma być odebrany  
   msgtyp = 0 typ nie ma znaczenia  
   msgtyp > 0 konkretny typ komunikatu  
   msgtyp < 0 wybieramy komunikat o wartości (najbardziej zbliżonej) mniejszej lub równej |msgtyp|   
5. flagi specyfikjąc zachowanie się funkcji w warunkach nietypowych:  
   - 0 / MSG_NOERROR - ucięcie komunikatu przy odbiorze  
   - IPC_NOWAIT - jeśli w kolejce nie ma komunikatów program kontynuuje działanie   
  


#### Funkcje dodatkowe:  
`key_t ftok(const char *pathname, int proj_id);` - wygenerowanie unikatowego klucza kolejki na podstawie sciezki do pliku w projekcie i id projektu
1. `pathname` ścieżka do pliku (najlepiej w folderze naszego projektu), plik musi istnieć i być "dostępny"
2. `proj_id` identyfikator projektu dowolna liczba lub pojedyńcza literka 
`msgctl(int msgid, int cmd, struct msgid_ds *buf)` - dokonywanie operacji na samej kolejce (najczesciej uzywane do jej usuwania)  
1. **msgid** - identyfikator kolejki  
2. **cmd** - stała specyfikująca rodzaj operacji  
  **IPC_STAT** - zwraca informację o stanie kolejki  
  **IPC_SET** - zmienia ograniczenia kolejki  
  **IPC_RMID** - usuwa kolejkę z systemu (parametr buffor przyjmuje wartość **0**)  

#### Struktura komunikatu:  
```C
struct msgbuf {
  long mtype;  // typ komunikatu wartość > 0 
  char mtext[1];  // treść komunikatu
}
```
  
#### Uwagi dodatkowe:

> [!WARNING]
> Kolejka z kluczem o wartości `0` lub `IPC_PRIVATE` dostępna jest tylko dla procesów potomnych. W przeciwieństwie do standardowej kolejki o dowolnym innym kluczu, ta **nie** jest globalnie dostępna. Aby z niej coś odczytać lub do niej zapisać proces musi być spokrewniony (np. dziedzi utworzeone przy pomocy `fork()`).   

> [!WARNING]
> Jeśli użyjemy funkcji `msgrcv()` bez dodatkowych flag program będzie oczekiwać tak długo aż nie napłynie komunikat **o treści krótszej niż msgs**.  
> Można odczytać wiadomość dłuższą niż wielkość zdefiniowana w parametrze **msgs** wykorzystując flagę **MSG_NOERROR**, treść komunikatu zostanie wtedy skrócona do wartości zdefiniowanej w parametrze **msgs**.

> [!NOTE]
> Raz odebrany komunikat nie może zostać ponownie odczytany.

> [!NOTE]
> Jeśli spróbujemy podłączyć się do kolejki do której uprawnień nie mamy funkcja `msgget()` zakończy swoje działanie z blędem `EACCES` a program będzie konynuować działanie.  
> Jeśli kolejka istnieje ale zdefiniowaliśmy flagi `IPC_CREAT` oraz `IPC_EXCL` funkcja `msgget()` zakończy się z błędem `EEXIST`.

> [!NOTE]
> Jeśli w kolejce komunikatów nie ma miejsca, proces zostaje zablokowany chyba że w funkcji `msgnd()` w parametrze `msgflg` zostanie ustawiona flaga `IPC_NOWAIT`.
  
> [!NOTE]
> Po wywołaniu `msgrcv()` jeśli w kolejce komunikatów nie ma komunikatów, proces będzie ocekiwać aż jakieś napłyną chyba że w parametrze `msgflg` zostanie ustawiona flaga `IPC_NOWAIT`.
   
> [!NOTE]
> Funkcja `msgrcv()` zwraca rozmiar odebranego komunikatu w bajtach.
> 


#### Przykład:  
```C
#include <stdlib.h>
#include <sys/msg.h>
#include <stdio.h>

int main() {
        // klucz zdefiniowany manualnie
        // int err = msgget(9999, IPC_CREAT | 0666);

        // klucz zdefiniowany na podstawie sciezki oraz nazwy projektu z ftok
        key_t key = ftok("/Users/prywatne/Documents/PSiW/src2/ipc", 99);
        int err = msgget(key, IPC_CREAT | 0666);
        if (err != -1) {
                printf("IPC Key: %i", err);
        } else {
                perror("msgget");
        }

}
```


