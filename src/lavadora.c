#include<avr/io.h>
#include <avr/interrupt.h>
#include<util/delay.h>

// struct que define los miembros que componen cada estado de la máquina
typedef struct lavadora{
  void (*state_func_ptr)(void); // Puntero a las funciones de salida de cada estado
  int time; // Tiempo de duración de cada estado
} Lavadora;
/*
Funcionamiento
1.se habilitan las interrupciones para los pines
alto medio y bajo,para poner asignarles un botón y elegir la intensidad
2. se hacen las isr de cada boton 
// ISR para el botón de intensidad baja (PB0)
ISR(PCINT0_vect) {
    // Actualizar el estado de la lavadora en función de la intensidad baja
    current_state = LOW_INTENSITY_STATE;
}

// ISR para el botón de intensidad media (PB1)
ISR(PCINT0_vect) {
    // Actualizar el estado de la lavadora en función de la intensidad media
    current_state = MEDIUM_INTENSITY_STATE;
}

// ISR para el botón de intensidad alta (PB2)
ISR(PCINT0_vect) {
    // Actualizar el estado de la lavadora en función de la intensidad alta
    current_state = HIGH_INTENSITY_STATE;
}
cuando ya se tienen las ISR se configuran los distintos timers para que entren dependiendo del estado que se este´
*/

#define SUMINISTRO_AGUA 0 // Se suministra agua a la lavadora
#define LAVADO 1 // Se lava la ropa 
#define ENJUAGUE 2 // Se enjuaga
#define CENTRIFUGADO 3 // Se seca la ropa 

void A_out(void){
  PORTB = 0x09;
}

void B_out(void){
  PORTB ^= 0x01;
}

void C_out(void){
  PORTB = 0x0A;
}

void D_out(void){
  PORTB = 0x06;
}

void timer_setup() {
  TCCR0A=0x00; // Modo normal
  TCCR0B=0x00; 
  TCCR0B |= (1<<CS00)|(1<<CS02); // Prescaling de 1024
  sei(); // Se llama a la función sei() para habilitar las interrupciones globales
  TCNT0=0;
  TIMSK|=(1<<TOIE0); // Se habilita la interrupción del timer1
}

// Subestados de cada estado,corresponden a la intensidad seleccionada
enum {
  BAJA,
  MEDIA,
  ALTA,
};

int estado_actual = SUMINISTRO_AGUA;
int intensidad_seleccionada = BAJA;

int main(void){


  while (1) {
  // Leer los botones y actualizar la intensidad seleccionada
  // ...

  // Actualizar el estado de la lavadora según el botón presionado
  switch (estado_actual) {

    case SUMINISTRO_AGUA:
      // Suministrando agua en la intensidad seleccionada
      switch (intensidad_seleccionada) {
        case BAJA:
          suministrar_agua_baja();
          break;
        case MEDIA:
          suministrar_agua_media();
          break;
        case ALTA:
          suministrar_agua_alta();
          break;
        default 
      }
      estado_actual = LAVADO;
      break;

    case LAVADO:
      // Lavando la ropa en la intensidad seleccionada
      switch (intensidad_seleccionada) {
        case BAJA:
          lavar_bajo();
          break;
        case MEDIA:
          lavar_medio();
          break;
        case ALTA:
          lavar_alto();
          break;
      }
      estado_actual = ENJUAGUE;
      break;

    case ENJUAGUE:
      // Enjuagando la ropa en la intensidad seleccionada
      switch (intensidad_seleccionada) {
        case BAJA:
          enjuagar_bajo();
          break;
        case MEDIA:
          enjuagar_medio();
          break;
        case ALTA:
          enjuagar_alto();
          break;
      }
      estado_actual = CENTRIFUGADO;
      break;

    case CENTRIFUGADO:
      // Centrifugando la ropa en la intensidad seleccionada
      switch (intensidad_seleccionada) {
        case BAJA:
          centrifugar_bajo();
          break;
        case MEDIA:
          centrifugar_medio();
          break;
        case ALTA:
          centrifugar_alto();
          break;
      }
      estado_actual = SUMINISTRO_AGUA;
      break;
  }
}
}
