#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"

#define LED_RED 13    // Indica que o teto está fechado
#define LED_GREEN 11  // Indica que o teto está aberto

#define SERVER_PORT 1234 // Porta UDP que o receptor escuta

void udp_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
    if (p != NULL) {
        char msg[16];
        memcpy(msg, p->payload, p->len);
        msg[p->len] = '\0';

        printf("Recebido: %s\n", msg);

        if (strcmp(msg, "FECHAR") == 0) {
            gpio_put(LED_GREEN, false);
            gpio_put(LED_RED, true);
            printf("Teto fechado!\n");
        } else if (strcmp(msg, "ABRIR") == 0) {
            gpio_put(LED_RED, false);
            gpio_put(LED_GREEN, true);
            printf("Teto aberto!\n");
        }

        pbuf_free(p);
    }
}

int main() {
    stdio_init_all();
    cyw43_arch_init();
    cyw43_arch_enable_ap_mode("PicoW_AP", "12345678", CYW43_AUTH_WPA2_AES);

    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    struct udp_pcb *udp = udp_new();
    udp_bind(udp, IP_ADDR_ANY, SERVER_PORT);
    udp_recv(udp, udp_recv_callback, NULL);

    printf("Receptor pronto para receber comandos...\n");

    while (true) {
        sleep_ms(100);
    }

    return 0;
}
