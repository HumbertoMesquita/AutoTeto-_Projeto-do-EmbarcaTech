# AutoTeto - Automação para Telhados Moveis

## Sobre o Projeto
O **AutoTeto** é um sistema de automação para **abertura e fechamento de telhados móveis**, utilizando a **BitDogLab** e o **Raspberry Pi Pico W**. Ele permite o controle via **botões físicos, Wi-Fi e sensor de chuva**, garantindo praticidade e proteção contra intempéries.

## Funcionalidades
- **Controle Manual**: Botão A para **fechar** e Botão B para **abrir** o teto.
- **Indicação por LED RGB**:
  - 🟢 **Verde**: Teto Aberto
  - 🔴 **Vermelho**: Teto Fechado
  - 🔵 **Azul piscando (3s)**: Teto em Movimento
- **Sensor de Chuva**: Fecha automaticamente ao detectar chuva.
- **Controle Remoto via Wi-Fi**: Abre/fecha o teto via navegador.
- **Comunicação RF 433MHz**: Envia comandos sem fio ao motor.

## Hardware Utilizado
- BitDogLab (Raspberry Pi Pico W)
- LED RGB (GPIO 11, 12, 13)
- Botões físicos (GPIO 5, 6)
- Sensor de Chuva YL-83 (GPIO 4)
- Módulo RF 433MHz (GPIO 17)

## Como Funciona
1. **Inicialização**: Configura os GPIOs, LEDs e Wi-Fi.
2. **Modo Padrão**: O teto inicia **aberto** (LED verde).
3. **Ações Possíveis**:
   - **Botão A**: Fecha o teto (LED vermelho).
   - **Botão B**: Abre o teto (LED verde).
   - **Sensor de Chuva ativado**: Fecha automaticamente o teto.
   - **Wi-Fi**: Comandos via navegador.
4. **Movimento**: LED azul pisca por **3 segundos** durante a movimentação.


## Estrutura do Projeto
```
/AutoTeto
├── src/
│   ├── main.c           # Código principal
│   ├── init_i2c.c       # Inicialização do I2C
│   ├── init_GPIO.c      # Configuração dos GPIOs
│   ├── ssd1306_i2c.c    # Controle do display OLED (opcional)
├── inc/
│   ├── init_i2c.h
│   ├── init_GPIO.h
│   ├── ssd1306.h
│   ├── font.h
├── CMakeLists.txt       # Configuração para compilação
├── README.md            # Documentação do projeto
```

## Controle via Wi-Fi
1. Conecte-se à mesma rede Wi-Fi do **Raspberry Pi Pico W**.
2. Descubra o **IP do Pico W** (exibido no terminal).
3. Acesse pelo navegador:
   - **Abrir o teto**: `http://<IP_DO_PICO>/abrir`
   - **Fechar o teto**: `http://<IP_DO_PICO>/fechar`

## Tecnologias Utilizadas
- Linguagem C
- Raspberry Pi Pico SDK
- CMake
- Protocolo Wi-Fi (lwIP)
- Comunicação RF 433MHz
- BitDogLab

## Licença
Este projeto está licenciado sob a **MIT License**.
