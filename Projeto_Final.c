#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"

#define BUTTON_A_PIN 5   // Fecha o teto
#define BUTTON_B_PIN 6   // Abre o teto
#define BLUE_LED_PIN 12  // LED azul (movimento)
#define RED_LED_PIN 13   // LED vermelho (teto fechado)
#define GREEN_LED_PIN 11 // LED verde (teto aberto)

#define WIFI_SSID "ERRO404"
#define WIFI_PASS "mesquita01"

bool teto_fechado = false; // Estado inicial
struct tcp_pcb *server_pcb; // Ponteiro do servidor TCP

// Configuração dos LEDs
void set_leds(bool red, bool green, bool blue) {
    gpio_put(RED_LED_PIN, red);
    gpio_put(GREEN_LED_PIN, green);
    gpio_put(BLUE_LED_PIN, blue);
}

// Função que move o teto (LED azul pisca, mas sem exibir no display)
void move_teto(bool fechar) {
    printf("Teto está se movendo...\n");

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

    if (teto_fechado) {
        printf("Teto Fechado\n");
    } else {
        printf("Teto Aberto\n");
    }
}

// Processa requisições HTTP
static err_t http_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char *request = (char *)p->payload;
    printf("📡 Requisição HTTP recebida: %s\n", request);

    if (strstr(request, "GET /fechar")) {
        if (!teto_fechado) {
            move_teto(true);
        }
        tcp_write(tpcb, "HTTP/1.1 200 OK\r\n\r\nTeto fechado!", 35, 1);
    } else if (strstr(request, "GET /abrir")) {
        if (teto_fechado) {
            move_teto(false);
        }
        tcp_write(tpcb, "HTTP/1.1 200 OK\r\n\r\nTeto aberto!", 34, 1);
    } else {
        tcp_write(tpcb, "HTTP/1.1 404 Not Found\r\n\r\n", 26, 1);
    }

    pbuf_free(p);
    return ERR_OK;
}

// Inicia o servidor HTTP
void http_server_init() {
    server_pcb = tcp_new();
    tcp_bind(server_pcb, IP_ADDR_ANY, 80);
    server_pcb = tcp_listen(server_pcb);
    tcp_accept(server_pcb, http_server_recv);
}

// Conectar ao Wi-Fi
void connect_wifi() {
    if (cyw43_arch_init()) {
        printf("Erro ao iniciar Wi-Fi\n");
        return;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi: %s...\n", WIFI_SSID);

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_MIXED_PSK, 15000) != 0) {
        printf("Falha ao conectar ao Wi-Fi\n");
        return;
    }

    printf("Wi-Fi Conectado!\n");
}

int main() {
    stdio_init_all();
    sleep_ms(3000); // Aguarda inicialização

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
    printf("Teto Aberto\n");

    // Conectar ao Wi-Fi
    connect_wifi();

    // Inicia o servidor HTTP
    http_server_init();
    printf("🌐 Servidor HTTP rodando...\n");

    while (true) {
        if (!gpio_get(BUTTON_A_PIN) && !teto_fechado) {
            move_teto(true);
        }

        if (!gpio_get(BUTTON_B_PIN) && teto_fechado) {
            move_teto(false);
        }

        cyw43_arch_poll(); // Mantém o Wi-Fi ativo
        sleep_ms(100);
    }

    return 0;
}
