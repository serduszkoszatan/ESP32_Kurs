//Dodać ten plik do CMakeList.txt wraz ze ścieżką z main jako folderem roboczym

//================================================
// READ ME
//================================================

/*
Sygnalizacja i synchronizacja: semafory binarne
Podczas gdy muteksy służą do ochrony zasobów (blokowania drzwi), semafor binarny służy głównie do
sygnalizacji i synchronizacji między zadaniami lub między przerwaniem a zadaniem.
W przeciwieństwie do muteksu, nie chodzi o własność zasobu, lecz o powiadomienie o wystąpieniu zdarzenia. 
Działa on jak prosta flaga, która może być „dostępna” (1) lub „niedostępna” (0). 
Jedna część systemu „daje” semafor, aby wysłać sygnał, a druga „bierze” go, aby czekać na ten sygnał.

Semafory binarne są najczęściej używane do obsługi procedur obsługi przerwań (ISR). 
W systemach wbudowanych chcemy, aby nasze procedury ISR działały jak najszybciej. 
Jeśli przycisk zostanie naciśnięty lub nadejdzie pakiet, 
nie powinniśmy wykonywać intensywnego przetwarzania wewnątrz samego przerwania, ponieważ blokuje to cały procesor.
Zamiast tego stosujemy technikę zwaną przetwarzaniem przerwań opóźnionych:

-Sprzęt wyzwala przerwanie.
-ISR wykonuje niezbędne minimum i „daje” semafor binarny.
-Zadanie, które polegało na „wykonywaniu” tego semafora, budzi się i wykonuje ciężką pracę.

Programowanie semaforów binarnych:
Aby użyć semafora binarnego, używamy następujących funkcji:
xSemaphoreCreateBinary():Ta funkcja inicjuje obiekt semafora, ale ważne jest, aby zrozumieć jego stan początkowy. 
W przeciwieństwie do prostej flagi, która może zaczynać się od stanu „dostępny”, 
nowo utworzony semafor binarny rozpoczyna działanie w stanie pustym (niedostępny). 
Oznacza to, że pierwsza Takeoperacja zostanie zablokowana, dopóki inna część systemu nie wyśle ​​sygnału.

xSemaphoreGiveFromISR():Ta funkcja wysyła sygnał z procedury obsługi przerwań (ISR). 
Jej rolą jest po prostu powiadomienie systemu o wystąpieniu zdarzenia, bez wykonywania żadnego intensywnego przetwarzania.

xSemaphoreTake():Używany przez zadanie do oczekiwania na sygnał. 
Zadanie wywołujące tę funkcję przejdzie w stan zablokowania do momentu podania semafora. 
Po wysłaniu sygnału przez ISR (lub inne zadanie), 
zadanie oczekujące zostanie natychmiast odblokowane i wznowi wykonywanie w celu obsługi zdarzenia.

*/
//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO

#include "freertos/semphr.h"

// Standardowe biblioteki C

// Własne include
#include "binary_semaphores.h"

//================================================
// DEFINITIONS
//================================================

#define BUTTON_GPIO GPIO_NUM_0
#define LED_GPIO    GPIO_NUM_7

//================================================
// STATIC VARIABLES
//================================================
//będzie używana jako semafor binarny do synchronizacji przerwania przycisku z zadaniem
SemaphoreHandle_t xGuiSemaphore;

//================================================
// INITIALIZATION FUNCTIONS
//================================================
/*
             PRZERWANIE GPIO
                   │
                   ▼
           gpio_isr_handler()
                   │
                   ▼
       xSemaphoreGiveFromISR()
                   │
                   ▼
       Czy ktoś czekał na semafor?
              │           │
             NIE          TAK
              │           │
              │           ▼
              │     Zadanie zostaje
              │       odblokowane
              │           │
              │           ▼
              │     Czy ma wyższy
              │       priorytet?
              │           │
              │           ▼
              │   xHigherPriorityTaskWoken
              │           │
              └───────┬───┘
                      ▼
             portYIELD_FROM_ISR()
                      │
                      ▼
             właściwy Task wykonuje
                właściwą pracę
*/

//Definiujemy procedurę obsługi przerwań (ISR), która uruchamia się automatycznie po każdym przerwaniu na przycisku. 
//Funkcja jest umieszczana w pamięci IRAM, IRAM_ATTR dzięki czemu może być wykonywana szybko i niezawodnie podczas przerwań.
static void IRAM_ATTR gpio_isr_handler(void* arg){
//Wewnątrz ISR deklarujemy zmienną o nazwie xHigherPriorityTaskWokeni inicjalizujemy ją na pdFALSE. 
//Zmienna ta jest używana przez FreeRTOS do wskazania,
//czy podanie semafora spowodowało opuszczenie stanu zablokowanego przez zadanie o wyższym priorytecie i gotowość do uruchomienia.    
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//Funkcja xSemaphoreGiveFromISR()zwraca semafor w sposób bezpieczny dla ISR. 
//Jeśli zadanie o wyższym priorytecie czekało na ten semafor, funkcja aktualizuje się xHigherPriorityTaskWokendo pdTRUE.
    xSemaphoreGiveFromISR(xGuiSemaphore, &xHigherPriorityTaskWoken);
//Sprawdza tę flagę i żąda natychmiastowej zmiany kontekstu przed wyjściem z przerwania. 
//Pozwala to na natychmiastowe uruchomienie uruchomionego zadania o wyższym priorytecie, 
//bez czekania na kolejny sygnał harmonogramu.
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

//================================================
// MAIN FUNCTIONS
//================================================

//To zadanie odpowiada za obsługę faktycznej pracy, która powinna zostać wykonana po naciśnięciu przycisku. 
//Najpierw konfiguruje ono GPIO LED jako wyjście, a następnie wchodzi w nieskończoną pętlę, 
//w której czeka na sygnał z semafora. Gdy ISR przekaże sygnał z semafora, zadanie się wybudza i przełącza stan diody LED.
void led_toggle_task(void *pvParameters) {
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    int state = 0;

    while (1) {
        if (xSemaphoreTake(xGuiSemaphore, portMAX_DELAY) == pdPASS) {
            state = !state;
            gpio_set_level(LED_GPIO, state);
        }
    }
}

/*
Na koniec, w app_main():  

void app_main(void) {
//najpierw tworzymy semafor binarny za pomocą xSemaphoreCreateBinary(). 
//Ten semafor będzie używany do synchronizacji między procedurą obsługi przerwań (ISR) a zadaniem.
    xGuiSemaphore = xSemaphoreCreateBinary();

//Po utworzeniu sprawdzamy, czy semafor został pomyślnie przydzielony, weryfikując, czy zwrócony uchwyt nie jest NULL. 
//Jeśli jest NULL, oznacza to, że system nie przydzielił wymaganych zasobów.
    if (xGuiSemaphore != NULL) {  
    
//Następnie konfigurujemy GPIO za pomocą gpio_config_t struktury. Struktura ta definiuje wszystkie ustawienia dla wybranego pinu:
//intr_type = GPIO_INTR_NEGEDGE konfiguruje przerwanie do wyzwolenia na opadającym zboczu 
//    (tj. gdy sygnał zmienia się z wysokiego na niski), co jest zwykle wykorzystywane do wykrywania naciśnięcia przycisku, 
//    gdy pin jest podciągnięty do góry i dociśnięty do masy.
//mode = GPIO_MODE_INPUT ustawia pin jako wejście, umożliwiając odczyt sygnałów zewnętrznych, np. stanu przycisku.
//pin_bit_mask = (1ULL << BUTTON_GPIO) wybiera, który pin GPIO jest konfigurowany 
//    poprzez ustawienie odpowiedniego bitu w masce 64-bitowej.
//pull_up_en = 1 włącza wewnętrzny rezystor podciągający, zapewniając, że pin pozostaje na stabilnym poziomie WYSOKIM, 
//    gdy przycisk nie jest wciśnięty.
//Po wypełnieniu struktury konfiguracji dokonujemy jej wdrożenia za pomocą polecenia gpio_config(&io_conf), 
//które zapisuje te ustawienia do rejestrów sprzętowych i aktywuje konfigurację GPIO.
        gpio_config_t io_conf = {
            .intr_type = GPIO_INTR_NEGEDGE, // Trigger on press (High to Low)
            .mode = GPIO_MODE_INPUT,
            .pin_bit_mask = (1ULL << BUTTON_GPIO),
            .pull_up_en = 1,
        };
        gpio_config(&io_conf);

//Następnie inicjujemy system przerwań GPIO, wywołując gpio_install_isr_service(0). 
//Ta funkcja konfiguruje strukturę obsługi przerwań, aby umożliwić rejestrację procedur obsługi przerwań GPIO. 
//Argument 0 wskazuje, że używane są domyślne flagi konfiguracyjne.
        gpio_install_isr_service(0);

//Na koniec dołączamy obsługę przerwań do wybranego pinu GPIO za pomocą gpio_isr_handler_add(BUTTON_GPIO, gpio_isr_handler, NULL).
// Łączy to wskazany pin GPIO z funkcją ISR gpio_isr_handler, 
// która będzie uruchamiana automatycznie po wystąpieniu skonfigurowanego zdarzenia przerwania. 
//Ostatni parametr ( NULL) to zdefiniowany przez użytkownika argument przekazywany do ISR, 
//który w tym przypadku nie jest używany.

        gpio_isr_handler_add(BUTTON_GPIO, gpio_isr_handler, NULL);
        xTaskCreatePinnedToCore(led_toggle_task, "LED_Task", 2048, NULL, 10, NULL, 1);
    }
}
*/