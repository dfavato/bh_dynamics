#ifndef LineSensor_h
#define LineSensor_h

#include "GenericSensor.h"

class LineSensor : public GenericSensor {
  int state; // estado do sensor, sobre ou fora da linha
  int line_threshold; // limiar para decidir que o sensor está sobre a linha
  int offline_threshold; // limiar para decidir que o sensor está fora da linha
  int persistent_address_line;
  int persistent_address_offline;
  void store_values();
  void load_values();
  
  public:
    LineSensor(int pin); // construtor
    const static int ON_LINE = 0;
    const static int OFF_LINE = 1;
    int status(); // retorna o status atual, sobre ou fora da linha
    void calibrate(int line, int offline); // calibragem, os argumentos devem ser os valores médios de leitura na linha e fora dela
};

#endif
