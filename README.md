# Systemy Operacyjne 2 - Projekt 1
**Autor:** [Filip Tymiński]  
**Data:** Marzec 2025  

## Temat zadania projektowego
Tematem projektu jest rozwiązanie problemu jedzących filozofów (ang. *Dining Philosophers Problem*), klasycznego zagadnienia w informatyce związanego z synchronizacją wątków, zarządzaniem sekcjami krytycznymi i zapobieganiem zakleszczeniom.  

W problemie mamy określoną liczbę filozofów siedzących przy okrągłym stole, między którymi znajduje się taka sama liczba widelców. Każdy filozof może być w jednym z trzech stanów: myślenie, głód lub jedzenie. Aby jeść, filozof potrzebuje jednocześnie dwóch widelców – lewego i prawego. Widelce są współdzielone między sąsiadami, co wymaga odpowiedniej synchronizacji.

## Opis czynności filozofów
- **Myślenie**: Filozof nie korzysta z widelców i "odpoczywa".  
- **Głód**: Filozof chce jeść i czeka na dostępność obu widelców.  
- **Jedzenie**: Filozof trzyma oba widelce, blokując je dla innych, po czym wraca do myślenia.  

## Opis kluczowych elementów programu
- **Wątki**: Każdy filozof jest reprezentowany przez osobny wątek (`std::thread`), który niezależnie wykonuje swoje stany: myślenie, głód i jedzenie.  
- **Sekcje krytyczne**: Widelce (`std::mutex` w `std::unique_ptr`) to sekcje krytyczne. Każdy widelec może być używany przez dwóch sąsiadujących filozofów, ale tylko jeden może go trzymać w danym momencie.  
- **Synchronizacja wypisywania**: Dodano mutex (`cout_mutex`), aby komunikaty w konsoli były czytelne i nie nakładały się na siebie.  

## Problematyka z perspektywy programistycznej
Główne wyzwania to:  
1. **Zapobieganie zakleszczeniu (deadlock)**: Sytuacja, w której wszyscy filozofowie trzymają jeden widelec i czekają na drugi, którego nigdy nie dostaną.  
2. **Synchronizacja wątków**: Zapewnienie, że tylko jeden filozof na raz może używać danego widelca.  
3. **Płynność działania**: Każdy filozof powinien mieć szansę jeść, bez blokowania się nawzajem na stałe.  

## Napotkane problemy i zastosowane rozwiązania
- **Problem z kopiowaniem mutexów**: Początkowo użyto `std::vector<mutex>`, ale `std::mutex` nie może być kopiowany. Rozwiązano to poprzez użycie `std::vector<unique_ptr<mutex>>`, co pozwala dynamicznie tworzyć mutexy na stercie i unika problemu kopiowania.  
- **Zakleszczenie**: Bez odpowiedniej strategii filozofowie mogli utknąć w stanie głodu. Wprowadzono zasadę blokowania widelców w kolejności (niższy indeks najpierw), co eliminuje cykle czekania i zapobiega deadlockowi.  
- **Chaos w konsoli**: Wątki wypisywały komunikaty równolegle, co powodowało ich nakładanie się. Dodano `cout_mutex` z `lock_guard`, aby synchronizować dostęp do `cout` i zapewnić czytelność outputu.  

## Uruchamianie programu
Program wymaga podania liczby filozofów jako argumentu w linii poleceń. Przykłady uruchomienia:  
- **Windows**: `SO2.exe 5`  
- **Linux**: `./SO2 5`  

### Wymagania
- Kompilator C++17 lub nowszy (np. `g++`).
- Biblioteka standardowa C++ z obsługą wątków (`<thread>`, `<mutex>`).

### Kompilacja
1. Windows (MinGW): `g++ main.cpp -o SO2 -std=c++17`
2. Linux: `g++ main.cpp -o SO2 -std=c++17 -pthread`

### Przykład użycia
```bash
./SO2 5
