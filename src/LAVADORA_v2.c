#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//_______ESTADOS DE LA FMS________
typedef enum {
  INTENSIDAD,
  SUMINISTRO_AGUA ,
  LAVADO,
  ENJUAGUE ,
  CENTRIFUGADO
} estado_fsm_t;



typedef enum{
	I_BAJA,
	I_MEDIA,
	I_ALTA
} intensidad;

estado_fsm_t estado_actual = INTENSIDAD;
int Contador_interrupt=0;// Se encarga de contar los ciclos
int contador = 0;
int estado_lavadora_t = 0;
intensidad intensidad_seleccionada = 0;
int pass=0;
int t_total = 0;
int decenas = 0;
int unidades = 0;
int display = 0;
int tiempos = 0;
int flag=0;
int segundos_display;
volatile int timer_count = 0;
volatile int segundos_transcurridos = 0;
volatile int contador_aux = 0;

int rounds(float x) {
  if (x >= 0) {
    return (int)(x + 0.5);
  } else {
    return (int)(x - 0.5);
  }
}
//INTERRUPCION BOTON
//////////////// ISR //////////////////////////////////
// ISR para el boton de intensidad I_BAJA (GPIO0)
ISR(PCINT0_vect) {
	if (estado_actual==INTENSIDAD)
	{
		// Actualizar el estado de la lavadora en funcion de la intensidad I_BAJA
    	if(PB0==0){/*. i.e. boton bajo, seleccione estado bajo*/
    		intensidad_seleccionada = I_BAJA;
    	}else if(PB1==0){ /*registro del boton se enciende o se apaga, haga algo. i.e. boton medio, seleccione estado medio*/
      		intensidad_seleccionada = I_MEDIA;
    	}else if(PB2==0){/*registro del boton se enciende o se apaga, haga algo. i.e. boton alto, seleccione estado alto*/
      		intensidad_seleccionada = I_ALTA;
    }
	}
	else{
		intensidad_seleccionada=intensidad_seleccionada;
	}
}
//ISR para el boton de estado_lavadora/play]
ISR(PCINT1_vect){ 
  //boton inicio/pausa -> Contador inicia en 0,
  if (PA2==1)
  {
	pass=0;
  }
  else{
	pass=1;
  }
  
  }

//INTERRUPCION DE TIEMPO PARA MANEJAR DELAY
ISR (TIMER0_OVF_vect){      //Interrupt vector for Timer0
	static uint16_t counter_flag = 0;
	counter_flag++;
	if (estado_actual==INTENSIDAD){
		contador_aux=0;
	}
	else{
		contador_aux++;
		segundos_transcurridos =rounds(contador_aux/31);
	}
	if (estado_actual==INTENSIDAD)
	{
		timer_count=0;
	}
	else{
		if (timer_count > 0) { // Solo decrementar si no ha llegado a cero
			segundos_display = rounds(timer_count/31);
			timer_count--;	
		}
	}
	if (counter_flag == 2) {
		counter_flag = 0;
		flag = !flag; // Cambiar el valor del flag
	}
}
//###########################################################################
//############################FUNCIONES######################################
//###########################################################################

//###########################################################################
//###########################################################################
//Esta funcion configura los relojes e habilita las interrupciones generales y timer0, ademas de hacer el prescaling de TCCR0B
void Timer_config() {
	TCCR0A = 0; // Modo normal
	TCCR0B = (1 << CS02) | (1 << CS00); // Prescaler de 1024
	TCNT0 = 0; // Inicializar contador en 0
	TIMSK = (1 << TOIE0); // Habilitar interrupción de overflow
	sei();
}
//###########################################################################
//CONFIGURACION DE INTERRUPCION EXTERNA: Basado en link de referencia
void interruptor_externo(){
  //DDRB |= (1 << PB7)|(1 << PB4); //Valores de salida PB
  //DDRD |= (1 << PD4) |(1 << PD5); //Valores de salida PD
  // MCUCR |= (1<<ISC01)|(1<<ISC11); // SOLO SIRVE PARA LOS INT Configurando como flanco = Falling edge genera interrupt request
	GIMSK |= (1 << PCIE0) | (1 << PCIE1);
  // Se configuran 3 pines para interrupciones y se agregan a Pin Change Mask n
  // Estos son las intensidades baja media y alta
  PCMSK |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2);
  // Se configura el pin para el boton play/pause
  PCMSK1 |= (1 << PCINT10);

  // Habilitar las interrupciones por cambio de estado en los pines PCINT y las interrupciones generales
  GIMSK |= (1 << PCIE0) | (1 << PCIE1);

  // se activan las interrupciones antes creadas
  //PCICR |= (1 << PCIE0) | (1 << PCIE1); //PCICR creo que no existe en el ATtiny, me parece que el adecuado es INTCON
}
//###########################################################################
//Esta funcion se encarga de llamar a las funciones que permiten configurar las interrupciones usadas en el laboratorio
void Setup_INTERRUPCIONES(){
    interruptor_externo();
    Timer_config();
}
//se puede poner dentro de la fsm
void cuenta_regresiva(int tiempos){
	display = tiempos;
	decenas = display / 10;
	unidades = display % 10;
	if (flag==0){
		PORTB |= (1 << PB3); // Invertir el valor del bit 5 de PORTB
		if (decenas==0) // Si decenas=0
			{	
				PORTB &= ~(1 << PB4);
				PORTB &= ~(1 << PB5);
				PORTB &= ~(1 << PB6);
				PORTB &= ~(1 << PB7);
			}
			else if ( decenas==1) //  Si  decenas=1
			{
				PORTB &= ~(1 << PB4);
				PORTB &= ~(1 << PB5);
				PORTB &= ~(1 << PB6);
				PORTB |= (1 << PB7);
			}
			else if ( decenas==2) //  Si  decenas=2
			{
				PORTB &= ~(1 << PB4);
				PORTB &= ~(1 << PB5);
				PORTB |= (1 << PB6);
				PORTB &= ~(1 << PB7);
			}
			else if ( decenas==3) //  Si  decenas=3
			{
				PORTB &= ~(1 << PB4);
				PORTB &= ~(1 << PB5);
				PORTB |= (1 << PB6);
				PORTB |= (1 << PB7);
			}
			else if ( decenas==4) //  Si  decenas=4
			{
				PORTB &= ~(1 << PB4);
				PORTB |= (1 << PB5);
				PORTB &= ~(1 << PB6);
				PORTB &= ~(1 << PB7);
			}
			else if ( decenas==5) //  Si  decenas=5
			{
				PORTB &= ~(1 << PB4);
				PORTB |= (1 << PB5);
				PORTB &= ~(1 << PB6);
				PORTB |= (1 << PB7);
			}
			else if ( decenas==6) //  Si  decenas=6
			{
				PORTB &= ~(1 << PB4);
				PORTB |= (1 << PB5);
				PORTB |= (1 << PB6);
				PORTB &= ~(1 << PB7);
			}
			else if (decenas==7) //  Si  decenas=7
			{
				PORTB &= ~(1 << PB4);
				PORTB |= (1 << PB5);
				PORTB |= (1 << PB6);
				PORTB |= (1 << PB7);
			}
			else if ( decenas==8) //  Si  decenas=8
			{
				PORTB |= (1 << PB4);
				PORTB &= ~(1 << PB5);
				PORTB &= ~(1 << PB6);
				PORTB &= ~(1 << PB7);
			}
			else if ( decenas==9) //  Si  decenas=9
			{
				PORTB |= (1 << PB4);
				PORTB &= ~(1 << PB5);
				PORTB &= ~(1 << PB6);
				PORTB |= (1 << PB7);
			}
					//delay(1);
					
					//delay(100);
					//flag = !flag;
	}
	 
				
	else{
		PORTB |= (1 << PB3); // Invertir el valor del bit 5 de PORTB
		if (unidades==0) // Si unidades=0
		{	
			PORTB &= ~(1 << PB4);
			PORTB &= ~(1 << PB5);
			PORTB &= ~(1 << PB6);
			PORTB &= ~(1 << PB7);
		}
		else if ( unidades==1) //  Si  unidades=1
		{
			PORTB &= ~(1 << PB4);
			PORTB &= ~(1 << PB5);
			PORTB &= ~(1 << PB6);
			PORTB &= ~(1 << PB7);
			PORTB |= (1 << PB7);
		}
		else if ( unidades==2) //  Si  unidades=2
		{
			PORTB &= ~(1 << PB4);
			PORTB &= ~(1 << PB5);
			PORTB |= (1 << PB6);
			PORTB &= ~(1 << PB7);
		}
		else if ( unidades==3) //  Si  unidades=3
		{
			PORTB &= ~(1 << PB4);
			PORTB &= ~(1 << PB5);
			PORTB |= (1 << PB6);
			PORTB |= (1 << PB7);
		}
		else if ( unidades==4) //  Si  unidades=4
		{
			PORTB &= ~(1 << PB4);
			PORTB |= (1 << PB5);
			PORTB &= ~(1 << PB6);
			PORTB &= ~(1 << PB7);
			PORTB &= ~(1 << PB7);
		}
		else if ( unidades==5) //  Si  unidades=5
		{
			PORTB &= ~(1 << PB4);
			PORTB |= (1 << PB5);
			PORTB &= ~(1 << PB6);
			PORTB &= ~(1 << PB7);
			PORTB |= (1 << PB7);
		}
		else if ( unidades==6) //  Si  unidades=6
		{
			PORTB &= ~(1 << PB4);
			PORTB |= (1 << PB5);
			PORTB |= (1 << PB6);
			PORTB &= ~(1 << PB7);
		}
		else if ( unidades==7) //  Si  unidades=7
		{
			PORTB &= ~(1 << PB4);
			PORTB |= (1 << PB5);
			PORTB |= (1 << PB6);
			PORTB |= (1 << PB7);
		}
		else if ( unidades==8) //  Si  unidades=8
		{
			PORTB |= (1 << PB4);
			PORTB &= ~(1 << PB5);
			PORTB &= ~(1 << PB6);
			PORTB &= ~(1 << PB7);
		}
		else if ( unidades==9) //  Si  unidades=9
		{
			PORTB |= (1 << PB4);
			PORTB &= ~(1 << PB5);
			PORTB &= ~(1 << PB6);
			PORTB |= (1 << PB7);
		}
					//delay(1);
					//delay(100);
					//flag = !flag;
	}	
				
}	
//###########################################################################
void FMS_lavadora(){

  // Actualizar el estado de la lavadora según la interrupcion que se active
  switch (estado_actual) {
//usar el counter
    case INTENSIDAD:	
      	switch (intensidad_seleccionada) {
        	case I_BAJA:
				t_total = 279; //31*9 = 279 ciclos == 9 segundos
				timer_count=t_total;
				//Meter timer del display 7 segmentos
				PORTD |= (1 << PD3); //Enciende LED (B0 creo que esta malo
				estado_actual = SUMINISTRO_AGUA;
				
			//cuenta_regresiva();
			break;
			case I_MEDIA:
				t_total = 589; //31*19 = 589 ciclos == 19 segundos
				timer_count=t_total;
				//Meter timer del display 7 segmentos
				PORTD |= (1 << PD4); //Enciende LED 
				estado_actual = SUMINISTRO_AGUA;
				
			break;
			case I_ALTA:
				t_total = 837; //31*27 = 837 ciclos == 27 segundos
				timer_count=t_total;
				//Meter timer del display 7 segmentos
				PORTD |= (1 << PD5); //Enciende LED

				estado_actual = SUMINISTRO_AGUA;
				
			break;
      }
    case SUMINISTRO_AGUA:
/*
      if(estado_lavadora_t==0){
        //pause timers
      }
      else if(estado_lavadora_t==1){ // si esta lavando
        // Suministrando agua en la intensidad seleccionada 
        if(31>t_total>0){
          PD4 = 1;
          estado_actual = SUMINISTRO_AGUA;
        }else if(t_total==32){
        estado_actual = LAVADO;
        }
      }*/
	  // Lavando la ropa en la intensidad seleccionada
      switch (intensidad_seleccionada) {
        case I_BAJA:
          while (contador_aux<31)
		  {
			PORTD |= (1 << PD0);
			PORTD |= (1 << PD3);
			cuenta_regresiva(segundos_display);
		  }
		  estado_actual = LAVADO;
          break;
        case I_MEDIA:
          while (contador_aux<62) 
		  {
			PORTD |= (1 << PD0);
			PORTD |= (1 << PD4);
			cuenta_regresiva(segundos_display);
		  }
		  estado_actual = LAVADO;
          break;
        case I_ALTA:
          while (contador_aux<93) 
		  {
			PORTD |= (1 << PD0);
			PORTD |= (1 << PD5);
			cuenta_regresiva(segundos_display);
		  }
		  estado_actual = LAVADO;
          break;
      }
	  PORTD &= ~(1 << PD0);
      break;

    case LAVADO:
      // Lavando la ropa en la intensidad seleccionada
      switch (intensidad_seleccionada) {
        case I_BAJA:
          while (contador_aux >= 31 && contador_aux <= 124)
		  {
			PORTD |= (1 << PD1);
			PORTD |= (1 << PD3);
			cuenta_regresiva(segundos_display);
		  }
		  estado_actual = ENJUAGUE;
          break;
        case I_MEDIA:
          while (contador_aux >= 62 && contador_aux <= 279) 
		  {
			PORTD |= (1 << PD1);
			PORTD |= (1 << PD4);
			cuenta_regresiva(segundos_display);
		  }
		  estado_actual = ENJUAGUE;
          break;
        case I_ALTA:
          while (contador_aux >= 62 && contador_aux <= 403) 
		  {
			PORTD |= (1 << PD1);
			PORTD |= (1 << PD5);
			cuenta_regresiva(segundos_display);
		  }
		  estado_actual = ENJUAGUE;
          break;
      }
	  PORTD &= ~(1 << PD1);
      break;

    case ENJUAGUE:
      // Enjuagando la ropa en la intensidad seleccionada
      switch (intensidad_seleccionada) {
        case I_BAJA:
          while (contador_aux >= 124 && contador_aux <= 186)
		  {
			PORTA |= (1 << PA1);
			PORTD |= (1 << PD3);
			cuenta_regresiva(segundos_display);
		  }
		  estado_actual = CENTRIFUGADO;
          break;
        case I_MEDIA:
          while (contador_aux >= 279 && contador_aux <= 403) 
		  {
			PORTA |= (1 << PA1);
			PORTD |= (1 << PD4);
			cuenta_regresiva(segundos_display);
		  }
		  estado_actual = CENTRIFUGADO;
          break;
        case I_ALTA:
          while (contador_aux >= 403 && contador_aux <= 558) 
		  {
			PORTA |= (1 << PA1);
			PORTD |= (1 << PD5);
			cuenta_regresiva(segundos_display);
		  }
		  estado_actual = CENTRIFUGADO;
          break;
      }
	  PORTA &= ~(1 << PA1);
      break;

    case CENTRIFUGADO:
      // Centrifugando la ropa en la intensidad seleccionada
      switch (intensidad_seleccionada) {
        case I_BAJA:
          while (contador_aux >= 186 && contador_aux <= 279)
		  {
			PORTA |= (1 << PA0);
			PORTD |= (1 << PD3);
			cuenta_regresiva(segundos_display);
		  }
		  estado_actual = INTENSIDAD;
          break;
        case I_MEDIA:
          while (contador_aux >= 403 && contador_aux <= 589) 
		  {
			PORTA |= (1 << PA0);
			PORTD |= (1 << PD4);
			cuenta_regresiva(segundos_display);
		  }
		  estado_actual = INTENSIDAD;
          break;
        case I_ALTA:
          while (contador_aux >= 558 && contador_aux <= 837) 
		  {
			PORTA |= (1 << PA0);
			PORTD |= (1 << PD5);
			cuenta_regresiva(segundos_display);
		  }
		  estado_actual = INTENSIDAD;
          break;
      }
	  PORTA &= ~(1 << PA0);
      break;
  }
}

int main(void)
{
	DDRB |= (1 << PB7) | (1 << PB4) | (1 << PB5) | (1 << PB6) | (1 << PB7);
	DDRD |= (1 << PD5) | (1 << PD4) | (1 << PD3) | (1 << PD1) | (1 << PD0);
  	DDRA |= (1 << PA0) | (1 << PA1);
	PORTB &= (1<<PB7)|(1<<PB4)|(1<<PB5)|(1<<PB6)|(1<<PB7); // Se entablecen los pines B3 y B4 como salidas
	PORTD &= ~(1 << PD5);
	PORTD &= ~(1 << PD4);
	PORTD &= ~(1 << PD3);
	PORTD &= ~(1 << PD1);
	PORTD &= ~(1 << PD0);
	PORTA &= ~(1 << PA0);
	PORTA &= ~(1 << PA1);
	int num = 310; // Número a contar
  	timer_count = num; // Inicializar el contador con el valor del número
	Setup_INTERRUPCIONES();
	sei();
	PORTB &= (1<<PB7); // Poner en bajo el pin 5
	estado_actual = INTENSIDAD; // Se inicializa en estado paso de vehiculos
	estado_lavadora_t = 1; //Lavadora inicia en play
	contador = 0;
	intensidad_seleccionada= I_BAJA;
	
	while (1) {
		FMS_lavadora(); //Maquina de estados de lavadora empieza a funcionar
	}
	return 0;
}