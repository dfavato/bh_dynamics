#ifndef LineSensor_h
#define LineSensor_h

class LineSensor {
  int state; // estado do sensor, sobre ou fora da linha
  int calibrated; // se o sensor foi calibrado ou não
  int last_reads[10]; // guarda as últimas 10 leituras do sensor
  int last_read; // guarda a posição da array que deve ser atualizada na próxima leitura
  int line_threshold; // limiar para decidir que o sensor está sobre a linha
  int off_line_threshold; // limiar para decidir que o sensor está fora da linha
  int pin; // qual o pino do sensor
  
  public:
    LineSensor(int pin); // construtor
    const static int ON_LINE = 0;
    const static int OFF_LINE = 1;
    int status(); // retorna o status atual, sobre ou fora da linha
    int get_value(); // retorna a média das últimas 10 leituras
    void read(); // faz uma nova leitura e atualiza "last_reads"
    void calibrate(int line, int offline); // calibragem, os argumentos devem ser os valores médios de leitura na linha e fora dela
};

#endif
