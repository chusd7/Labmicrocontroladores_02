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

// Se configuran 3 pines para interrupciones y se agregan a Pin Change Mask n
// Estos son las intensidades baja media y alta
PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2);
// Se configura el pin para el boton play/pause
PCMSK1 |= (1 << PCINT10);
// se activan las interrupciones antes creadas
PCICR |= (1 << PCIE0) | (1 << PCIE1);



// Subestados de cada estado,corresponden a la intensidad seleccionada
enum {
  I_BAJA,
  I_MEDIA,
  I_ALTA,
};

//////////////// ISR //////////////////////////////////
// ISR para el botón de intensidad I_BAJA (GPIO0)
ISR(PCINT0_vect) {
    // Actualizar el estado de la lavadora en función de la intensidad I_BAJA
    estado_actual = I_BAJA;
    // Crear ifs para poder delimitar la intensidad actual
    if (bb==0)
    {
      intensidad_seleccionada = I_BAJA;
    }
    if (bm==0)
    {
      intensidad_seleccionada = I_MEDIA;
    }
    if (ba==0)
    {
      intensidad_seleccionada = I_ALTA;
    }
    
}
ISR(PCINT1_vect) {
    //crear funcion para boton pausa
    
    
}
// Aparte de estos hay que agregar el boton para inicio pausa,este tiene una prioridad 
// Mayor a los anteriores este iría en el PCINT1
// falta inicializar el PCINT_vec

int estado_actual = SUMINISTRO_AGUA;
int intensidad_seleccionada = I_BAJA;

int main(void){


  while (1) {


  // Actualizar el estado de la lavadora según la interrupcion que se active
  switch (estado_actual) {

    case SUMINISTRO_AGUA:
      // Suministrando agua en la intensidad seleccionada
      switch (intensidad_seleccionada) {
        case I_BAJA:
          suministrar_agua_I_BAJA();
          break;
        case I_MEDIA:
          suministrar_agua_I_MEDIA();
          break;
        case I_ALTA:
          suministrar_agua_I_ALTA();
          break;
      }
      estado_actual = LAVADO;
      break;

    case LAVADO:
      // Lavando la ropa en la intensidad seleccionada
      switch (intensidad_seleccionada) {
        case I_BAJA:
          lavar_bajo();
          break;
        case I_MEDIA:
          lavar_medio();
          break;
        case I_ALTA:
          lavar_alto();
          break;
      }
      estado_actual = ENJUAGUE;
      break;

    case ENJUAGUE:
      // Enjuagando la ropa en la intensidad seleccionada
      switch (intensidad_seleccionada) {
        case I_BAJA:
          enjuagar_bajo();
          break;
        case I_MEDIA:
          enjuagar_medio();
          break;
        case I_ALTA:
          enjuagar_alto();
          break;
      }
      estado_actual = CENTRIFUGADO;
      break;

    case CENTRIFUGADO:
      // Centrifugando la ropa en la intensidad seleccionada
      switch (intensidad_seleccionada) {
        case I_BAJA:
          centrifugar_bajo();
          break;
        case I_MEDIA:
          centrifugar_medio();
          break;
        case I_ALTA:
          centrifugar_alto();
          break;
      }
      estado_actual = SUMINISTRO_AGUA;
      break;
  }
}
}
