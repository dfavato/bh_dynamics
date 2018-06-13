#include "GenericSensor.h"

int avg(int *arr, int size) {
  int sum = 0;
  for(int i=0; i<size; i++) {
    sum += arr[i];
  }
  return sum/size;
}

GenericSensor::GenericSensor(int pin, int digital) {
  this->pin = pin;
  this->digital = digital;
  this->last_read = 0;
  for(int i=0; i<10; i++) {
    this->read();
  }
}

int GenericSensor::get_value() {
  int value = this->read();
  if(this->digital) {
    return value;
  } else {
    return avg(last_reads, 10);  
  }
}

int GenericSensor::read() {
  int value;
  if(this->last_read >= 10)
    this->last_read = 0;
  if(this->digital) {
    value = digitalRead(this->pin);
  } else {
    value = analogRead(this->pin);
  }
  if(!this->digital) {
    this->last_reads[this->last_read++ % 10] = value;
  }
  return value;
}
