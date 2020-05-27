/* Estrutura da mensagem */
#define READ_COIL   0X01
#define READ_INPUT  0X02
#define WRITE_COIL  0X05
#define READ_ANALOG 0x06
#define WRITE_ANALOG 0X07

/* lógica negada para saídas */
#define ON  0
#define OFF 1

/* Parametros */
#define SLAVE_ADR       "03"

/* mapeamento das saídas */
//enum {OUT1=8, OUT2, OUT3, OUT4} OUTPUTS;
#define OUTPUT_OFFSET 7
#define OUT1 8
#define OUT2 9
#define OUT3 10
#define OUT4 11

/* mapeamento das entradas */
#define INPUT_OFFSET 1
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5

/* mapeamento das entradas analógicas */
#define ANALOG_INPUT_OFFSET -1
#define AIN1 A0
#define AIN2 A1
#define AIN3 A2
#define AIN4 A3

/* mapeamento da saída analógica */
#define ANALOG_OUTPUT_OFFSET 5
#define AON1 6

void msgHandler();
void msgInit();

// void static funcao
// estas nao sao visiveis para os outros modulos