#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "inc/ssd1306.h"

#define I2C_SDA 14
#define I2C_SCL 15

// Definição dos pinos
const uint BUTTON_A_PIN = 5;  // Fecha o teto
const uint BUTTON_B_PIN = 6;  // Abre o teto
const uint BLUE_LED_PIN = 12; // LED azul (movimento)
const uint RED_LED_PIN = 13;  // LED vermelho (teto fechado)
const uint GREEN_LED_PIN = 11; // LED verde (teto aberto)

bool teto_fechado = false; // Estado inicial

// Inicializa o display OLED
void init_oled() {
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init();
}

// Atualiza o display OLED com o estado do teto (sem "Movendo...")
void update_display(const char *status) {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length); // Limpa o display antes de atualizar

    // Centraliza o texto corretamente
    ssd1306_draw_string(ssd, 20, 20, status);

    render_on_display(ssd, &frame_area);
}

// Define o estado dos LEDs
void set_leds(bool red, bool green, bool blue) {
    gpio_put(RED_LED_PIN, red);
    gpio_put(GREEN_LED_PIN, green);
    gpio_put(BLUE_LED_PIN, blue);
}

// Move o teto (LED azul pisca, mas display só muda no final)
void move_teto(bool fechar) {
    // Pisca o LED azul por 3 segundos
    for (int i = 0; i < 6; i++) {
        set_leds(0, 0, 1); // Liga azul
        sleep_ms(500);
        set_leds(0, 0, 0); // Desliga azul
        sleep_ms(500);
    }

    // Define o estado final
    teto_fechado = fechar;
    set_leds(teto_fechado, !teto_fechado, 0); // Vermelho = fechado, Verde = aberto

    // Atualiza o display somente quando o teto terminar de se mover
    if (teto_fechado) {
        update_display("Teto Fechado");
    } else {
        update_display("Teto Aberto");
    }
}

int main() {
    stdio_init_all();
    sleep_ms(3000); // Aguarda inicialização

    init_oled();
    update_display("Teto Aberto"); // Inicia com teto aberto

    // Configuração dos botões como entrada
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Configuração dos LEDs como saída
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);
    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);

    // Inicia com o teto aberto (LED verde aceso)
    set_leds(0, 1, 0); // Verde aceso, vermelho apagado

    while (true) {
        if (!gpio_get(BUTTON_A_PIN) && !teto_fechado) {
            move_teto(true);
        }

        if (!gpio_get(BUTTON_B_PIN) && teto_fechado) {
            move_teto(false);
        }

        sleep_ms(100);
    }

    return 0;
}
