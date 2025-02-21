#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>

#define RAIN_SENSOR_DIGITAL 15  // Sensor de chuva (simulado com botão)

/*sensor de chuva como o YL-83, FC-37 ou similares.
Esses sensores funcionam detectando a presença de água na superfície de uma placa (Placa Sensora para Módulo Sensor de Chuva).
Enviando um sinal digital para a BitDogLab.
*/

int main() {
    stdio_init_all();

    // Configuração do sensor digital (simulado com botão)
    gpio_init(RAIN_SENSOR_DIGITAL);
    gpio_set_dir(RAIN_SENSOR_DIGITAL, GPIO_IN);
    gpio_pull_up(RAIN_SENSOR_DIGITAL);

    while (true) {
        // Lendo valor digital do sensor (simulado com botão)
        if (gpio_get(RAIN_SENSOR_DIGITAL) == 0) {
            printf("Tempo chuvoso!\n");
        } else {
            printf("Tempo ensolarado!.\n");
        }
        sleep_ms(1000);
    }
}
