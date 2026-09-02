Tradycyjne implementacje FreeRTOS są zazwyczaj jednowątkowe.
Ponieważ ESP32-S3 posiada procesor dwurdzeniowy, firma Espressif znacznie zmodyfikowała FreeRTOS, aby obsługiwał SMP (Symmetric Multiprocessing – symetryczne przetwarzanie wieloprocesorowe).

Oznacza to, że wersja FreeRTOS w ESP-IDF pozwala nam nie tylko podzielić program na zadania,
ale również jawnie określić, czy dane zadanie ma działać na Core 0,
który jest zwykle wykorzystywany przez zadania systemowe i komunikacyjne,
takie jak Wi-Fi/Bluetooth, czy na Core 1, który jest przeznaczony dla kodu aplikacji.

W centrum FreeRTOS znajduje się scheduler (harmonogramista) – komponent odpowiedzialny za decydowanie,
które zadanie w danym momencie otrzyma dostęp do procesora.
Scheduler działa na podstawie okresowego sygnału znanego jako przerwanie tick.
Sprzętowy timer wewnątrz mikrokontrolera generuje to przerwanie ze stałą częstotliwością,
zwykle co 1 milisekundę (1 kHz) w systemach ESP-IDF.
Za każdym razem, gdy wystąpi to przerwanie, FreeRTOS na chwilę wstrzymuje normalne wykonywanie programu
i ponownie ocenia stan wszystkich zadań w systemie.

To okresowe sprawdzanie daje FreeRTOS jego „czas rzeczywisty”.
Zamiast pozwalać jednemu przepływowi programu wykonywać się bez końca,
system operacyjny cały czas sprawdza, czy inne zadanie powinno przejąć kontrolę nad procesorem.
Zadania mogą znajdować się w różnych stanach, takich jak Running, Ready, Blocked lub Suspended.
Zadaniem schedulera jest sprawdzenie tych stanów i wybranie zadania o najwyższym priorytecie, które jest gotowe do wykonania.

FreeRTOS korzysta z modelu planowania z wywłaszczaniem, co oznacza, że priorytet zadań bezpośrednio wpływa na dostęp do procesora.
Jeśli zadanie o wyższym priorytecie stanie się gotowe, podczas gdy zadanie o niższym priorytecie jest aktualnie wykonywane,
harmonogram natychmiast przerywa wykonywanie zadania o niższym priorytecie i przełącza je na zadanie o wyższym priorytecie.
Ten proces nazywa się wywłaszczaniem.

Tasks
Zadanie (Task) jest najbardziej podstawowym elementem FreeRTOS.
Możemy myśleć o zadaniu jak o niezależnym programie z własną nieskończoną pętlą,
własnym priorytetem oraz własną przydzieloną pamięcią (nazywaną stosem – stack).

Zadania w FreeRTOS mogą znajdować się w różnych stanach:
Running: Zadanie jest aktualnie wykonywane przez rdzeń procesora.
Ready: Zadanie jest gotowe do wykonania, ale czeka, ponieważ zadanie o wyższym priorytecie aktualnie korzysta z procesora.
Blocked: Zadanie czeka na wystąpienie określonego zdarzenia (np. upłynięcie czasu timera lub otrzymanie danych z czujnika).
Podczas oczekiwania w stanie Blocked nie zużywa czasu procesora.
Suspended: Zadanie zostało jawnie wstrzymane przez programistę i nie zostanie ponownie uruchomione, dopóki nie zostanie jawnie wznowione.

Task Priorities:
Każde zadanie otrzymuje priorytet od 0 do configMAX_PRIORITIES - 1.
W ESP-IDF maksymalna wartość wynosi zazwyczaj 24.
Wyższa liczba oznacza wyższy priorytet.
Scheduler FreeRTOS ściśle przestrzega priorytetów: zawsze wstrzyma zadanie o niższym priorytecie,
jeśli zadanie o wyższym priorytecie jest gotowe do wykonania.

Teraz, wewnątrz app_main(), tworzymy te zadania za pomocą xTaskCreatePinnedToCore.
Ta funkcja wymaga:
Nazwy funkcji zadania.
Czytelnej dla człowieka nazwy zadania używanej podczas debugowania.
Rozmiaru stosu w bajtach, który określa, ile pamięci potrzebuje zadanie.
Parametrów przekazywanych do zadania; tutaj przekazujemy NULL.
Poziomu priorytetu; dla obu zadań używamy wartości 1.
Uchwytu zadania (Task Handle); przekazujemy NULL, ponieważ nie potrzebujemy później odwoływać się do zadania.
Identyfikatora rdzenia 0 lub 1 albo tskNO_AFFINITY, aby pozwolić RTOS-owi wybrać rdzeń.

void app_main(void) {
// Create Task 1 on Core 1
xTaskCreatePinnedToCore(blink_task_1, "Task 1", 2048, NULL, 1, NULL, 1);

```
// Create Task 2 on Core 1
xTaskCreatePinnedToCore(blink_task_2, "Task 2", 2048, NULL, 1, NULL, 1);
```

}

Task Management: Suspend, Resume, and Delete
Możemy zrobić więcej niż tylko tworzyć i uruchamiać zadania.
Możemy również kontrolować ich stany oraz cykl życia.
Aby to zrobić, musimy zapisać Task Handle podczas tworzenia zadania.

vTaskSuspend(TaskHandle_t xTaskToSuspend): Wstrzymuje zadanie. Przechodzi ono do stanu Suspended i ignoruje wszystkie zdarzenia RTOS do momentu wznowienia.
vTaskResume(TaskHandle_t xTaskToResume): Wyprowadza zadanie ze stanu Suspended i przywraca je do stanu Ready.
vTaskDelete(TaskHandle_t xTaskToDelete): Całkowicie usuwa zadanie i zwalnia pamięć przydzieloną na jego stos.
(Aby „zrestartować” zadanie, należy je usunąć i ponownie wywołać xTaskCreate).

TaskHandle_t myTaskHandle = NULL;

void app_main(void) {
// 1. Create the task and save its handle
xTaskCreatePinnedToCore(blink_task_1, "Task 1", 2048, NULL, 1, &myTaskHandle, 1);

```
// 2. Suspend the task (Stop it)
vTaskSuspend(myTaskHandle);
// 3. Resume the task 
vTaskResume(myTaskHandle);
// 4. Delete the task (Passing NULL deletes the task that calls it)
vTaskDelete(myTaskHandle); 
```

}
