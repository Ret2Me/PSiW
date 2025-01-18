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
2. **msgflg** - dodatkowe parametry takie jestk IPC_CREAT oraz IPC_EXCL
  
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
`ftok(int msgid, int cmd, struct msgid_ds *buf)` - wygenerowanie unikatowego klucza kolejki  
1. **msgid** - identyfikator kolejki  
2. **cmd** - stała specyfikująca rodzaj operacji  
  **IPC_STAT** - zwraca informację o stanie kolejki  
  **IPC_SET** - zmienia ograniczenia kolejki  
  **IPC_RMID** - usuwa kolejkę z systemu (parametr buffor przyjmuje wartość **0**)  

`msgctl()` - dokonywanie operacji na samej kolejce (najczesciej uzywane do jej usuwania)  
  
#### Struktura komunikatu:  
```C
struct msgbuf {
  long mtype;  // typ komunikatu wartość > 0 
  char mtext[1];  // treść komunikatu
}
```
  
#### Uwagi dodatkowe:
> [!WARNING]
> Jeśli użyjemy funkcji `msgrcv()` bez dodatkowych flag program będzie oczekiwać tak długo aż nie napłynie komunikat **o treści krótszej niż msgs**.  
> Można odczytać wiadomość dłuższą niż wielkość zdefiniowana w parametrze **msgs** wykorzystując flagę **MSG_NOERROR**, treść komunikatu zostanie wtedy skrócona do wartości zdefiniowanej w parametrze **msgs**.
  
> [!NOTE]
> Raz odebrany komunikat nie może zostać ponownie odczytany.
  
> [!NOTE]
> Jeśli w kolejce komunikatów nie ma miejsca, proces zostaje zablokowany chyba że w funkcji `msgnd()` w parametrze `msgflg` zostanie ustawiona flaga `IPC_NOWAIT`.
  
> [!NOTE]
> Po wywołaniu `msgrcv()` jeśli w kolejce komunikatów nie ma komunikatów, proces będzie ocekiwać aż jakieś napłyną chyba że w parametrze `msgflg` zostanie ustawiona flaga `IPC_NOWAIT`.
   
> [!NOTE]
> Funkcja `msgrcv()` zwraca rozmiar odebranego komunikatu w bajtach.
> 


#### Przykład:  

