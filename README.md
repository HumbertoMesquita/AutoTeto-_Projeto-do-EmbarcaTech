# AutoTeto - Automacao Inteligente para Telhados Moveis

## 🔧 Sobre o Projeto
O **AutoTeto** é um sistema de automação para **abertura e fechamento de telhados móveis**, utilizando a **BitDogLab** e o **Raspberry Pi Pico W**. Ele permite o controle via **botões físicos, Wi-Fi e sensor de chuva**, garantindo praticidade e proteção contra intempéries.

## 📄 Funcionalidades
- **Controle Manual**: Botão A para **fechar** e Botão B para **abrir** o teto.
- **Indicação por LED RGB**:
  - 🟢 **Verde**: Teto Aberto
  - 🔴 **Vermelho**: Teto Fechado
  - 🟦 **Azul piscando (3s)**: Teto em Movimento
- **Sensor de Chuva**: Fecha automaticamente ao detectar chuva.
- **Controle Remoto via Wi-Fi**: Abre/fecha o teto via navegador.
- **Comunicação RF 433MHz**: Envia comandos sem fio ao motor.

## 🛠️ Hardware Utilizado
- Raspberry Pi Pico W
- BitDogLab
- LED RGB (GPIO 11, 12, 13)
- Botões físicos (GPIO 5, 6)
- Sensor de Chuva YL-83 (GPIO 4)
- Módulo RF 433MHz (GPIO 17)
- Fonte de Alimentação 5V

## 🔬 Como Funciona
1. **Inicialização**: Configura os GPIOs, LEDs e Wi-Fi.
2. **Modo Padrão**: O teto inicia **aberto** (LED verde).
3. **Ações Possíveis**:
   - **Botão A**: Fecha o teto (LED vermelho).
   - **Botão B**: Abre o teto (LED verde).
   - **Sensor de Chuva ativado**: Fecha automaticamente o teto.
   - **Wi-Fi**: Comandos via navegador.
4. **Movimento**: LED azul pisca por **3 segundos** durante a movimentação.

## 🛠️ Como Instalar e Rodar
1. Clone o repositório:
   ```bash
   git clone https://github.com/seu-usuario/AutoTeto.git
   cd AutoTeto
   ```
2. Compile o código para o Raspberry Pi Pico W.
3. Grave o firmware `.uf2` no Pico W.
4. Conecte-se ao Wi-Fi e acesse pelo navegador o **IP do Pico W**.
5. Teste os botões e o sensor de chuva para verificação.

## 📂 Estrutura do Projeto
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

## 🛡️ Controle via Wi-Fi
1. Conecte-se à mesma rede Wi-Fi do **Raspberry Pi Pico W**.
2. Descubra o **IP do Pico W** (exibido no terminal).
3. Acesse pelo navegador:
   - **Abrir o teto**: `http://<IP_DO_PICO_W>/abrir`
   - **Fechar o teto**: `http://<IP_DO_PICO_W>/fechar`

## 💻 Tecnologias Utilizadas
- Linguagem C
- Raspberry Pi Pico SDK
- CMake
- Protocolo Wi-Fi (lwIP)
- Comunicação RF 433MHz
- BitDogLab

## 📚 Licença
Este projeto está licenciado sob a **MIT License**.

## 💪 Contribuições
Contribuições são bem-vindas! Caso encontre bugs ou tenha sugestões, abra uma **issue** ou faça um **pull request**.

