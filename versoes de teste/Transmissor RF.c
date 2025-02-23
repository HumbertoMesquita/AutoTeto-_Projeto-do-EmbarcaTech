#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"

#define SERVER_IP "192.168.4.2" // IP do receptor
#define SERVER_PORT 1234        // Porta UDP do receptor

void send_udp_message(const char *message) {
    struct udp_pcb *udp;
    struct pbuf *p;

    udp = udp_new();
    ip_addr_t dest;
    ip4addr_aton(SERVER_IP, &dest);

    p = pbuf_alloc(PBUF_TRANSPORT, strlen(message), PBUF_RAM);
    if (p != NULL) {
        memcpy(p->payload, message, strlen(message));
        udp_sendto(udp, p, &dest, SERVER_PORT);
        pbuf_free(p);
    }
    udp_remove(udp);
}

int main() {
    stdio_init_all();
    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();

    printf("Transmissor iniciado\n");

    while (true) {
        send_udp_message("FECHAR"); // Simula o envio do comando
        printf("Enviado: FECHAR\n");
        sleep_ms(5000); // Espera 5 segundos antes de enviar o próximo comando
    }

    return 0;
}
