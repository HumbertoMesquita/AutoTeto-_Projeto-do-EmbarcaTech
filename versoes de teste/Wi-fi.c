#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"

#define WIFI_SSID "ERRO404"   // Nome da rede Wi-Fi no Wokwi
#define WIFI_PASS "mesquita01"              // Wokwi-GUEST não exige senha

// Definição dos botões e sensor
#define BTN_A 5   // Botão para fechar o teto
#define BTN_B 6   // Botão para abrir o teto
#define SENSOR 4  // Botão representando o sensor

// Definição dos pinos do LED RGB na BitDogLab
#define LED_RED 13   // LED vermelho - Teto fechado
#define LED_GREEN 11 // LED verde - Teto aberto
#define LED_BLUE 12  // LED azul - Movimento do teto

#define DEBOUNCE_TIME 200  // Tempo de debounce (ms)
#define TEMPO_MOVIMENTO 3000 // Tempo que o LED azul pisca (3 segundos)

bool teto_fechado = false; // Estado inicial (teto aberto)

// Simula movimento do teto com LED azul piscando
void movimentar_teto() {
    printf("🔵 Movendo o teto...\n");

    for (int i = 0; i < 6; i++) { // Pisca o LED azul por 3 segundos
        gpio_put(LED_BLUE, 1);
        sleep_ms(250);
        gpio_put(LED_BLUE, 0);
        sleep_ms(250);
    }
}

// Atualiza o estado do teto e os LEDs
void atualizar_teto(bool fechar) {
    if (fechar == teto_fechado) return; // Se já estiver no estado desejado, não faz nada

    movimentar_teto(); // Pisca o LED azul antes de mudar o estado

    teto_fechado = fechar;

    if (teto_fechado) {
        gpio_put(LED_GREEN, 0);
        gpio_put(LED_RED, 1);
        printf("🔴 Teto fechado!\n");
    } else {
        gpio_put(LED_RED, 0);
        gpio_put(LED_GREEN, 1);
        printf("🟢 Teto aberto!\n");
    }
}

// Função para processar requisições HTTP
static err_t http_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char *request = (char *)p->payload;
    printf("📡 Requisição HTTP recebida: %s\n", request);

    if (strstr(request, "GET /fechar")) {
        atualizar_teto(true);
        tcp_write(tpcb, "HTTP/1.1 200 OK\r\n\r\nTeto fechado!", 35, 1);
    } else if (strstr(request, "GET /abrir")) {
        atualizar_teto(false);
        tcp_write(tpcb, "HTTP/1.1 200 OK\r\n\r\nTeto aberto!", 34, 1);
    } else {
        tcp_write(tpcb, "HTTP/1.1 404 Not Found\r\n\r\n", 26, 1);
    }

    pbuf_free(p);
    return ERR_OK;
}

// Inicializa o servidor HTTP
void iniciar_servidor_http() {
    struct tcp_pcb *server_pcb = tcp_new();
    tcp_bind(server_pcb, IP_ADDR_ANY, 80);
    server_pcb = tcp_listen(server_pcb);
    tcp_accept(server_pcb, http_server_recv);
}

// Conectar ao Wi-Fi e exibir o IP do Pico W
void conectar_wifi() {
    printf("🔄 Conectando ao Wi-Fi: %s...\n", WIFI_SSID);

    if (cyw43_arch_init()) {
        printf("⚠️ Erro ao inicializar Wi-Fi!\n");
        return;
    }

    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_MIXED_PSK, 10000)) {
        printf("❌ Falha ao conectar ao Wi-Fi!\n");
        return;
    }

    printf("✅ Conectado ao Wi-Fi!\n");

    // Obtém e exibe o IP local do Pico W
    printf("🌍 IP Local: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
}

// Configuração inicial
void setup() {
    stdio_init_all();

    // Inicializa botões e sensor como entrada com pull-up
    gpio_init(BTN_A);
    gpio_set_dir(BTN_A, GPIO_IN);
    gpio_pull_up(BTN_A);

    gpio_init(BTN_B);
    gpio_set_dir(BTN_B, GPIO_IN);
    gpio_pull_up(BTN_B);

    gpio_init(SENSOR);
    gpio_set_dir(SENSOR, GPIO_IN);
    gpio_pull_up(SENSOR);

    // Inicializa LEDs RGB como saída
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_put(LED_RED, 0);

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, 1); // Inicia com o teto aberto (LED verde ligado)

    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_put(LED_BLUE, 0);

    printf("\n🚀 Programa iniciado! Pressione os botões A, B ou ative o sensor.\n");

    conectar_wifi();
    iniciar_servidor_http();
}

// Loop principal
void loop() {
    if (!gpio_get(BTN_A)) { // Botão A pressionado (Fecha o teto)
        atualizar_teto(true);
        sleep_ms(DEBOUNCE_TIME);
    }

    if (!gpio_get(BTN_B)) { // Botão B pressionado (Abre o teto)
        atualizar_teto(false);
        sleep_ms(DEBOUNCE_TIME);
    }

    if (!gpio_get(SENSOR)) { // Sensor ativado
        printf("⚠️ Sensor ativado!\n");
        atualizar_teto(true); // Fecha o teto automaticamente ao detectar chuva
        sleep_ms(DEBOUNCE_TIME);
    }

    cyw43_arch_poll(); // Mantém a conexão Wi-Fi ativa
    sleep_ms(100);
}

int main() {
    setup();
    while (true) {
        loop();
    }
}
