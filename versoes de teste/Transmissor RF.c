#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include <stdio.h>

#define RAIN_SENSOR_PIN 15  // // Pino do sensor de chuva (simulado com botão)
#define RF_TRANSMITTER_PIN 2  // Pino do transmissor RF

static struct tcp_pcb *server_pcb;

// Simula a transmissão de um código RF
void send_rf_signal() {
    printf("Enviando o sinal RF para fechar o teto retratil automatico\n");

    for (int i = 0; i < 10; i++) {  // Repetição para garantir recepção
        gpio_put(RF_TRANSMITTER_PIN, 1);
        sleep_us(350);
        gpio_put(RF_TRANSMITTER_PIN, 0);
        sleep_us(350);
    }
}

// Função para processar requisições HTTP da Alexa
static err_t http_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char *request = (char *)p->payload;
    printf("Requisição HTTP recebida: %s\n", request);

    if (strstr(request, "GET /fechar")) {
        send_rf_signal();
        tcp_write(tpcb, "HTTP/1.1 200 OK\r\n\r\nTeto fechado via RF!", 42, 1);
    } else {
        tcp_write(tpcb, "HTTP/1.1 404 Not Found\r\n\r\n", 26, 1);
    }

    pbuf_free(p);
    return ERR_OK;
}

// Função para iniciar o servidor HTTP
void http_server_init() {
    server_pcb = tcp_new();
    tcp_bind(server_pcb, IP_ADDR_ANY, 80);
    server_pcb = tcp_listen(server_pcb);
    tcp_accept(server_pcb, http_server_recv);
}

int main() {
    stdio_init_all();
    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();

    // Conectar ao Wi-Fi
    cyw43_arch_wifi_connect_timeout_ms("SSID", "SENHA", CYW43_AUTH_WPA2_MIXED_PSK, 10000);

    gpio_init(RAIN_SENSOR_PIN);
    gpio_set_dir(RAIN_SENSOR_PIN, GPIO_IN);
    gpio_pull_up(RAIN_SENSOR_PIN);

    gpio_init(RF_TRANSMITTER_PIN);
    gpio_set_dir(RF_TRANSMITTER_PIN, GPIO_OUT);

    http_server_init();
    printf("Servidor HTTP rodando...\n");

    while (true) {
        if (gpio_get(RAIN_SENSOR_PIN) == 0) {  // Sensor detecta chuva
            printf("Tempo chuvoso!\n")
            printf("Enviando o sinal RF...\n");")
            send_rf_signal();
        } else {
            printf("Tempo ensolarado!.\n");
        }
        sleep_ms(1000);
    }
}
