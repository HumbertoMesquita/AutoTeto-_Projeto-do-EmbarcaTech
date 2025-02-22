#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "inc/ssd1306.h"

#define I2C_SDA 14
#define I2C_SCL 15

// Botões
const uint BUTTON_A_PIN = 5;  // Fecha o teto
const uint BUTTON_B_PIN = 6;  // Abre o teto

// LEDs
const uint BLUE_LED_PIN = 12;   // Movimento do teto
const uint RED_LED_PIN = 13;    // Teto fechado
const uint GREEN_LED_PIN = 11;  // Teto aberto

bool teto_fechado = false;  // Estado do teto
struct tcp_pcb *server_pcb;  // Servidor TCP

// Credenciais Wi-Fi
#define WIFI_SSID "ERRO404"
#define WIFI_PASS "mesquita01"

// Inicializa o display OLED



// Define o estado dos LEDs
void set_leds(bool red, bool green, bool blue) {
    gpio_put(RED_LED_PIN, red);
    gpio_put(GREEN_LED_PIN, green);
    gpio_put(BLUE_LED_PIN, blue);
}

// Simula o movimento do teto com LED azul piscando
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
        move_teto(true);
        tcp_write(tpcb, "HTTP/1.1 200 OK\r\n\r\nTeto fechado!", 35, 1);
    } else if (strstr(request, "GET /abrir")) {
        move_teto(false);
        tcp_write(tpcb, "HTTP/1.1 200 OK\r\n\r\nTeto aberto!", 34, 1);
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

void connect_wifi() {
    printf("🌐 Iniciando Wi-Fi...\n");

    if (cyw43_arch_init()) {
        printf("❌ Erro ao inicializar o Wi-Fi!\n");
        return;
    }

    cyw43_arch_enable_sta_mode();
    printf("🌐 Conectando ao Wi-Fi: %s...\n", WIFI_SSID);

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_MIXED_PSK, 15000) != 0) {
        printf("❌ Falha ao conectar no Wi-Fi!\n");
        return;
    }

    printf("✅ Wi-Fi conectado!\n");

    // Exibir IP no OLED
    uint8_t *ip = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
    char ip_display[20];
    sprintf(ip_display, "IP: %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    
    printf("🌍 Endereço IP: %s\n", ip_display);
}

int main() {
    stdio_init_all();

    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init();

    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    // zera o display inteiro
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

restart:

        char *text[] = {
            "  Teto Aberto   "};
    
        int y = 0;
        for (uint i = 0; i < count_of(text); i++)
        {
            ssd1306_draw_string(ssd, 5, y, text[i]);
            y += 8;
        }
        render_on_display(ssd, &frame_area);


    sleep_ms(3000); // Aguarde alguns segundos para evitar falhas iniciais

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

        cyw43_arch_poll();  // Mantém o Wi-Fi ativo
        sleep_ms(100);
    }

    return 0;
}
