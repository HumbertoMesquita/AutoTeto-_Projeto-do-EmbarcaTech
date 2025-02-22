#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>

#define SWITCH_PIN 15   // Chave de controle do teto
#define LED_RED 6       // LED vermelho - Teto fechado
#define LED_GREEN 7     // LED verde - Teto aberto
#define LED_BLUE 8      // LED azul - Movimento do Teto

// Função para definir a cor do LED RGB
void set_led_color(bool red, bool green, bool blue) {
    gpio_put(LED_RED, red);
    gpio_put(LED_GREEN, green);
    gpio_put(LED_BLUE, blue);
}

// Função para simular o movimento do teto (piscando azul)
void move_gate(bool closing) {
    printf(" O teto está %s...\n", closing ? "fechando" : "abrindo");

    for (int i = 0; i < 10; i++) { // Pisca por 10 segundos
        set_led_color(0, 0, 1);  // Liga o led azul
        sleep_ms(500);
        set_led_color(0, 0, 0);  // Desliga o led azul
        sleep_ms(500);
    }

    if (closing) {
        printf("Teto fechado!\n");
        set_led_color(1, 0, 0);  // Ativa o led vermelho (fechado)
    } else {
        printf("Teto aberto!\n");
        set_led_color(0, 1, 0);  // Ativa o led verde (aberto)
    }
}

int main() {
    stdio_init_all();

    // Configuração da chave
    gpio_init(SWITCH_PIN);
    gpio_set_dir(SWITCH_PIN, GPIO_IN);
    gpio_pull_up(SWITCH_PIN);

    // Configuração dos LEDs
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);

    // Começa com o teto aberto (LED verde aceso)
    set_led_color(0, 1, 0);
    printf("Aguardando comando do switch...\n");

    int last_state = gpio_get(SWITCH_PIN);  // Estado anterior do switch

    while (true) {
        int current_state = gpio_get(SWITCH_PIN);  // Estado atual do switch

        if (current_state != last_state) {  // Executa apenas se houve mudança no estado
            if (current_state == 0) {
                move_gate(true);  // Fechar teto
            } else {
                move_gate(false); // Abrir teto
            }

            last_state = current_state;  // Atualiza o estado anterior para evitar repetições
        }

        printf("Aguardando comando do switch...\n"); // Mensagem contínua enquanto aguarda
        sleep_ms(100);
    }
}
