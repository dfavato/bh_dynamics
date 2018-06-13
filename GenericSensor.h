#ifndef GenericSensor_h
#define GenericSensor_h

#include "Arduino.h"

class GenericSensor {
  int digital; // if digital = 1, if analog = 0

  protected:
    int pin; // qual o pino do sensor
    int last_read; // guarda a posição da array que deve ser atualizada na próxima leitura
    int last_reads[10]; // guarda as últimas 10 leituras do sensor
  
  public:
    GenericSensor(int pin, int digital); // construtor
    int get_value(); // retorna a média das últimas 10 leituras
    int read(); // faz uma nova leitura, atualiza "last_reads" e retorna o valor lido
};

#endif
