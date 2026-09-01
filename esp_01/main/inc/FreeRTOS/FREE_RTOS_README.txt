Traditional FreeRTOS deployments are commonly single-core.
Because the ESP32-S3 has a dual-core processor, Espressif heavily modified FreeRTOS to support SMP (Symmetric Multiprocessing).

This means the ESP-IDF version of FreeRTOS allows us to not only split our program into tasks
but also explicitly dictate whether a task runs on Core 0,
which is commonly used by system and communication tasks
such as Wi-Fi/Bluetooth or Core 1 which is reserved for application code.

At the center of FreeRTOS lies the scheduler, the component responsible for deciding which task gets access to the CPU
at any given moment. The scheduler operates using a periodic signal known as the tick interrupt.
A hardware timer inside the microcontroller generates this interrupt at a fixed frequency,
commonly every 1 millisecond (1 kHz) in ESP-IDF systems. Each time this interrupt occurs,
FreeRTOS briefly pauses normal execution and reevaluates the state of all tasks in the system.

This periodic reevaluation is what gives FreeRTOS its “real-time” behavior. 
Instead of allowing a single program flow to run endlessly, 
the operating system constantly checks whether another task should take control of the processor. 
Tasks can be in different states such as Running, Ready, Blocked, or Suspended. 
The scheduler’s job is to examine these states and choose the highest-priority task that is ready to execute.

FreeRTOS korzysta z modelu planowania z wywłaszczaniem, co oznacza, że ​​priorytet zadań bezpośrednio wpływa na dostęp do procesora.
Jeśli zadanie o wyższym priorytecie stanie się gotowe, podczas gdy zadanie o niższym priorytecie jest aktualnie wykonywane,
harmonogram natychmiast przerywa wykonywanie zadania o niższym priorytecie i przełącza je na zadanie o wyższym priorytecie.
Ten proces nazywa się wywłaszczaniem.

Tasks
A task is the most fundamental building block of FreeRTOS. We can think of a task as an independent program with its own infinite loop, its own priority, and its own allocated memory (called a stack).

Tasks in FreeRTOS operate in different states:
Running: The task is currently executing on the CPU core.
Ready: The task is ready to run but is waiting because a higher-priority task is currently using the CPU.
Blocked: The task is waiting for something to happen (like a timer to expire, or waiting for data from a sensor).
    While blocked, it consumes zero CPU time.
Suspended: The task is explicitly paused by the programmer and will not run again until explicitly resumed.

Task Priorities:
Every task is assigned a priority from 0 to configMAX_PRIORITIES - 1. In ESP-IDF, 
the maximum value is usually 24. Higher numbers represent higher priority. 
The FreeRTOS scheduler strictly obeys priority: it will always pause a lower-priority task 
if a higher-priority task is ready to run.


Now, inside app_main(), we create these tasks using xTaskCreatePinnedToCore. This function requires:
The function name of the task.
A human-readable string name for debugging.
The stack size in bytes, which determines how much memory the task needs.
Parameters to pass to the task; here, we pass NULL.
The priority level; we use 1 for both tasks.
A task handle; we pass NULL because we do not need to reference it later..
The core ID 0 or 1, or tskNO_AFFINITY to let the RTOS choose.

void app_main(void) {
    // Create Task 1 on Core 1
    xTaskCreatePinnedToCore(blink_task_1, "Task 1", 2048, NULL, 1, NULL, 1);
    
    // Create Task 2 on Core 1
    xTaskCreatePinnedToCore(blink_task_2, "Task 2", 2048, NULL, 1, NULL, 1);
}


Task Management: Suspend, Resume, and Delete
We can do more than just create and run tasks. We can also control their states and lifecycle. 
To do this, we need to capture the Task Handle when we create it.

vTaskSuspend(TaskHandle_t xTaskToSuspend): Pauses task. It goes into the Suspended and ignores all RTOS events until resumed.
vTaskResume(TaskHandle_t xTaskToResume): Brings a task out of the Suspended state and back to Ready.
vTaskDelete(TaskHandle_t xTaskToDelete): Completely destroys a task and frees its allocated stack memory. 
(To "restart" a task, you must delete it and call xTaskCreate again).

TaskHandle_t myTaskHandle = NULL;

void app_main(void) {
    // 1. Create the task and save its handle
    xTaskCreatePinnedToCore(blink_task_1, "Task 1", 2048, NULL, 1, &myTaskHandle, 1);
    
    // 2. Suspend the task (Stop it)
    vTaskSuspend(myTaskHandle);
    // 3. Resume the task 
    vTaskResume(myTaskHandle);
    // 4. Delete the task (Passing NULL deletes the task that calls it)
    vTaskDelete(myTaskHandle); 
}

Komunikacja między zadaniami: kolejki
Gdy uruchamiamy wiele zadań, nieuchronnie muszą one udostępniać sobie dane. 
Na przykład, „Zadanie czujnika” odczytuje dane o temperaturze, a „Zadanie wyświetlacza” wyświetla je na ekranie OLED.

Możemy przekazać te dane za pomocą zmiennych globalnych. 
W systemie czasu rzeczywistego (RTOS) zmienne globalne są niebezpieczne. 
Jeśli zadanie czujnika jest w trakcie aktualizacji zmiennej globalnej, 
a system czasu rzeczywistego (RTOS) nagle przełączy się na zadanie wyświetlania, 
zadanie wyświetlania może odczytać uszkodzone lub niekompletne dane.

Aby rozwiązać ten problem, FreeRTOS oferuje kolejki. 
Kolejka to bezpieczny potok FIFO (First-In-First-Out) między zadaniami.

Zadanie wysyłające przesuwa dane na koniec kolejki.
Zadanie odbierające pobiera dane z początku kolejki.
Jeśli kolejka jest pusta, zadanie odbiorcze automatycznie przechodzi w stan zablokowany, 
aż do momentu nadejścia danych, nie marnując czasu procesora.

Kolejki programowania
Aby pracować z kolejkami, musimy najpierw utworzyć zmienną globalną typu QueueHandle_t. 
Następnie możemy manipulować kolejką za pomocą trzech głównych funkcji:

xQueueCreate()Tworzy obiekt kolejki. Przyjmuje dwa argumenty: długość kolejki (maksymalną liczbę elementów, które może pomieścić)
    oraz rozmiar każdego elementu przechowywanego w kolejce.
xQueueSend()Wysyła nowe dane do kolejki. Przyjmuje trzy argumenty: globalną zmienną kolejki, 
    dodawane dane oraz wartość limitu czasu, która określa, jak długo funkcja powinna czekać, jeśli kolejka jest pełna.
xQueueReceive()Odczytuje i pobiera dane z kolejki. Podobnie jak poprzednia funkcja, przyjmuje trzy argumenty: 
uchwy(handle) kolejki, wskaźnik do miejsca, w którym powinny zostać zapisane odebrane dane, oraz wartość limitu czasu, 
która określa, jak długo funkcja powinna czekać na dane.