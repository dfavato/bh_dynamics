#include "Arduino.h"
#include "LineSensor.h"

int avg(int *arr, int size) {
  int sum = 0;
  for(int i=0; i<size; i++) {
    sum += arr[i];
  }
  return sum/size;
}

LineSensor::LineSensor(int pin) {
  this->pin = pin;
  pinMode(this->pin, INPUT);
  this->calibrated = 0;
  this->state = this->ON_LINE;
  this->line_threshold = 900;
  this->off_line_threshold = 700;
  this->last_read = 0;
}

int LineSensor::status() {
  int mean = avg(last_reads, 10);
  if(this->state == this->ON_LINE) {
    if(mean < this->off_line_threshold) {
      return this->OFF_LINE;
    }
  } else {
    if(mean > this->line_threshold) {
      return this->ON_LINE;
    }
  }
  return this->state;
}

void LineSensor::read() {
  this->last_reads[this->last_read++ % 10] = analogRead(this->pin);
}

int LineSensor::get_value() {
  return avg(last_reads, 10);
}


void LineSensor::calibrate(int line, int offline) {
  int diff = line - offline;
  this->line_threshold = line - (diff / 3);
  this->off_line_threshold = offline + (diff / 3);
  this->calibrated = 1;
}
