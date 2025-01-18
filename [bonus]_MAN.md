
## [bonus] Korzystanie z MAN-a (Manuala)
  
Na kolokwiach dozwolone jest korzystanie z komend:  
- man   
- whatis  
- apropos  
- wszelkich komend z wykorzystaniem parametru `--help`  

Budowa polecenia man:  
man [numer-sekcji] komenda  

> [!CAUTION]
> Niestety standardowa bazadanych man-a dołączona do systemów ubuntu / debian nie zawiera informacji o IPC.  
> Co jest **krytycznie** potrzebne do dobrego napisania kolokwium.  
>   
> **Ubuntu / Debian**:  
> W celu rozwiązania tego problemu należy doinstalować strony dedykowane deweloperą poleceniem `apt install manpages-dev`.
>   
> **Windows**:  
> Najprostszym rozwiązaniem będzie zainstalowanie WSLv2 (Windows Subsystem for Linux).  
>   
> **MacOS**:  
> W przypadku MacOS jest problem ponieważ Brew nie ma takiego packageu a zatem rozwiązania są dwa:  
> 1. Rozwiązanie zaproponowane na [stackexchange](https://apple.stackexchange.com/questions/87863/where-are-the-posix-message-functions-msgsnd-msgrcv-etc-man-pages-in-mac). Którego nie testowałem.  
> 2. Wykorzystanie dockera z ubuntu i na nim wykonać: `apt update`, `apt install man-db`, `apt install manpages-dev`.   


## Sekcje
Na kolokwium nabardziej przydaten okażą się być (1, 2, 3, 7)  
Dostępne sekcje możemy odczytać przy pomocy polecenia:
```man man```
1.   **Executable programs or shell commands:** Commands that can be executed in the shell, like ls, cd, or grep.
2.   **System calls:** Functions provided by the kernel, such as open, read, and write.
3.   **Library calls:** Functions within program libraries, like those in the C standard library, such as printf and malloc.
4.   **Games:** Documentation for games available on the system.
5.   **Special files:** Usually found in /dev, these include device files like /dev/null and /dev/sda.
6.   **File formats and conventions:** Details on file formats and conventions, e.g., /etc/passwd.
7.   **Miscellaneous:** Including macro packages and conventions, e.g., groff(7).
8.   **System administration commands:** Commands typically used by the root user for system administration tasks, like iptables or mount.
9.   **Kernel routines:** Non-standard kernel routines used within the system.

