//Dodać ten plik do CMakeList.txt wraz ze ścieżką z main jako folderem roboczym

//================================================
// READ ME
//================================================
/*

Attentuation - tłumienie:
| Tłumienie | Zakres napięcia (około) |
| --------- | ----------------------: |
| 0 dB      |                 0–1.1 V |
| 2.5 dB    |             około 1.5 V |
| 6 dB      |             około 2.2 V |
| 12 dB     |         około 3.3–3.9 V |

*/

/*
PHOTORESISTOR:

 Configuration and reading the same as standard analog_read

In darkness, its resistance is very high.
In bright light, its resistance becomes very low.
To read its changing resistance, we pair it with a fixed resistor (usually 10k Ω) to create a voltage divider. 
Let’s build an automatic night light that turns on an LED when it gets dark. Connect one leg of the LDR to 3.3V. 
Connect the other leg to GPIO 1 (ADC1_CH0) and to one end of the 10k Ω resistor.
Connect the other end of the 10k Ω resistor to GND. Finally, connect an LED to GPIO 9 through a 220 Ω resistor.
*/

/*
FLAME DETECTOR: 

Configuration and reading the same as standard analog_read

The flame sensor detects fire by sensing infrared (IR) light emitted by flames, Since Flames emit radiation across visible
and infrared wavelengths. Can also detect sunlight. The flame sensor is designed to detect infrared light, typically in the 760–1100 nm range. 
Unlike an LDR circuit that usually requires an external voltage divider, the flame sensor module
already includes the necessary resistors and signal conditioning circuitry. 
The module usually has four pins: VCC, GND, DO (Digital Output), and AO (Analog Output).
DO pin: Provides a HIGH or LOW signal to indicate whether a flame is detected, based on a built-in comparator threshold adjusted by a potentiometer on the module.
AO pin: Outputs a continuous voltage depending on the intensity of the flame.
*/

/*
SOIL MOISTURE SENSOR:

Configuration and reading the same as standard analog_read

Resistance-based: Uses two exposed electrodes. As moisture increases, conductivity increases, lowering resistance.
Capacitance-based: Measures the dielectric permittivity of the soil. Less prone to corrosion.

The resistance-based sensor has two exposed electrodes that are inserted into the soil.
When the soil contains more water, it becomes more conductive, which means the resistance between the electrodes decreases.
When the soil is dry, the resistance increases, this type of sensor is the most common and inexpensive option.
However, the probe itself usually has only two pins, so it cannot be connected directly to the Esp32s3.
Instead, it requires an external module that processes the signal, the module acts as an interface
between the probe and the ESP32s3, It typically has four pins: VCC, GND, AO (Analog Output), and DO (Digital Output).
The AO pin provides a continuous analog voltage corresponding to the measured resistance.

When the soil is very wet we get low resistance, the sensor outputs a lower voltage.
When the soil becomes drier get High resistance, the sensor outputs a higher voltage.

The DO pin outputs a digital signal (HIGH or LOW) based on a preset threshold, which can be adjusted using the onboard potentiometer;
 it goes HIGH when the measured value exceeds the set threshold and LOW otherwise.
*/
//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "esp_adc/adc_oneshot.h"    //Obsługa ADC/DAC

// Standardowe biblioteki C

// Własne include
#include "analog_read.h"

//================================================
// DEFINITIONS
//================================================

//HERE WE ADD ANOTHER PINS
#define READ_PIN_1 ADC_CHANNEL_0
#define READ_PIN_2 ADC_CHANNEL_3

//================================================
// STATIC VARIABLES
//================================================



//================================================
// INITIALIZATION FUNCTIONS
//================================================


//store a handle (a reference) to the ADC unit after it is initialized.
static adc_oneshot_unit_handle_t adc1_handle;

void analog_read_init (void) 
{
    //This structure configures which ADC unit we want to use. The ESP32 has multiple ADC units, and here we select ADC Unit 1
    adc_oneshot_unit_init_cfg_t init_config1 = { .unit_id = ADC_UNIT_1 };

    //This structure defines how a specific ADC channel will behave. 
    adc_oneshot_chan_cfg_t config = {
        //This sets the resolution of the ADC. By default, it is 12-bit, meaning the output values range from 0 to 4095.
        .bitwidth = ADC_BITWIDTH_DEFAULT, 
        //Attenuation (tłumienie) - This determines the measurable voltage range. With 12 dB attenuation, 
        //the ADC can read voltages approximately from 0 V to 3.3 V, which matches typical ESP32 input levels.
        .atten = ADC_ATTEN_DB_12 
    };
    // sets up the ADC hardware and stores the resulting handle in adc1_handle
    adc_oneshot_new_unit(&init_config1, &adc1_handle);

    // Configure all ADC channels used in the application, we select ADC_CHANNEL_0 (which maps to GPIO 1)
    //HERE WE ADD ANOTHER PINS
    adc_oneshot_config_channel(adc1_handle, READ_PIN_1, &config);
    adc_oneshot_config_channel(adc1_handle, READ_PIN_2, &config);
}


//================================================
// MAIN FUNCTIONS
//================================================

int analog_read(adc_channel_t channel)
{
    int value;

    adc_oneshot_read(adc1_handle,
                     channel,
                     &value);

    return value;
}