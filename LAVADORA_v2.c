// Cambios agregados
// Agregar el GIMSK
// falta agregar logica para ir bajando el t_total en la cuenta regresiva
// Se cambio t_total en I_baja (estaba malo)
// se agregaron los tiempos a las intensidades en el primer estado
// se agrego el estado intensidad
// se agrego la logica de cuando intervienen los botones de intensidad

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
void Delay_tiempos();

//_______ESTADOS DE LA FMS________
#define INTENSIDAD 0 //Estado de selección
#define SUMINISTRO_AGUA 1 // Se suministra agua a la lavadora PD4 - LDPV
#define LAVADO 2 // Se lava la ropa PD5 - LDVD
#define ENJUAGUE 3 // Se enjuaga PB4 - LDPP
#define CENTRIFUGADO 4 // Se seca la ropa PB3 - LDPD
#define sel

//ESTADOS DE INTENSIDAD    // Estos estados van a definir que tipo de tiempo debe estarse contando
#define I_BAJA 4       
#define I_MEDIA 5        
#define I_ALTA 6
int display = 0;
int Contador_interrupt=0;// Se encarga de contar los ciclos
int contador = 0;
int estado_lavadora_t = 0;
int intensidad_seleccionada = 0;
int I_BAJA = 0;
int I_MEDIA = 0;
int I_ALTA = 0;
int t_total = 0
int decenas = 0;
int unidades = 0;
int estado_actual = LAVADO;


//INTERRUPCION BOTON
//////////////// ISR //////////////////////////////////
// ISR para el boton de intensidad I_BAJA (GPIO0)
ISR(PCINT0_vect) {
	if (estado_actual==INTENSIDAD)
	{
		// Actualizar el estado de la lavadora en funcion de la intensidad I_BAJA
    	if(GPIO0==1){/*. i.e. boton bajo, seleccione estado bajo*/
    		intensidad_seleccionada = I_BAJA;
    	}else if(GPIO1==1){ /*registro del boton se enciende o se apaga, haga algo. i.e. boton medio, seleccione estado medio*/
      		intensidad_seleccionada = I_MEDIA;
    	}else if(GPIO2==1){/*registro del boton se enciende o se apaga, haga algo. i.e. boton alto, seleccione estado alto*/
      		intensidad_seleccionada = I_ALTA;
    }
	}
	else{
		estado_actual=estado_actual;
	}
}
//ISR para el boton de estado_lavadora/play]
ISR(PCINT1_vect){ 
  //boton inicio/pausa -> Contador inicia en 0,
  if(contador== 0){ //Le da play
    estado_lavadora_t = 1; //estado de play
    contador = 1; //siguiente iteracion, contador es 1
  }else if(contador==1){ //si contador es 0, se pausa
    estado_lavadora_t = 0;
    contador = 0;
  }
  }

//INTERRUPCION DE TIEMPO PARA MANEJAR DELAY
ISR (TIMER0_OVF_vect)      //Interrupt vector for Timer0
{
  Delay_tiempos();
}
//###########################################################################
//############################FUNCIONES######################################
//###########################################################################

//###########################################################################
void Delay_tiempos(){ //cada 31 veces, es 1 segundo (con preescaler de Chus)
    Contador_interrupt++;
}
//###########################################################################
//Esta funcion configura los relojes e habilita las interrupciones generales y timer0, ademas de hacer el prescaling de TCCR0B
void Timer_config() 
{
 TCCR0A = 0x00; //Modo normal
 TCCR0B = 0x00; 
 TCCR0B |= (1 << CS00) | (1 << CS02); //Prescaling a 1024 
 sei();// Interrupción global
 TCNT0 = 0;
 TIMSK |= (1 << TOIE0); //Habilitando la interrupción del timer0 
}

//###########################################################################
//CONFIGURACION DE INTERRUPCION EXTERNA: Basado en link de referencia
void interruptor_externo()  
{
  DDRB |= (1 << PB3)|(1 << PB4); //Valores de salida PB
  DDRD |= (1 << PD4) |(1 << PD5); //Valores de salida PD
  // MCUCR |= (1<<ISC01)|(1<<ISC11); // SOLO SIRVE PARA LOS INT Configurando como flanco = Falling edge genera interrupt request

  // Se configuran 3 pines para interrupciones y se agregan a Pin Change Mask n
  // Estos son las intensidades baja media y alta
  PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2);
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

void cuenta_regresiva(t_total){

  display = t_total/31;
  
  decenas = display / 10;
	unidades = display % 10;

  if (decenas==0) // Si decenas=0
			{	
				GP1 = 0;
				GP2 = 0;
				GP4 = 0;
				GP5 = 0;
			}
			else if ( decenas==1) //  Si  decenas=1
			{
				GP1 = 0;
				GP2 = 0;
				GP4 = 0;
				GP5 = 1;
			}
			else if ( decenas==2) //  Si  decenas=2
			{
				GP1 = 0;
				GP2 = 0;
				GP4 = 1;
				GP5 = 0;
			}
			else if ( decenas==3) //  Si  decenas=3
			{
				GP1 = 0;
				GP2 = 0;
				GP4 = 1;
				GP5 = 1;
			}
			else if ( decenas==4) //  Si  decenas=4
			{
				GP1 = 0;
				GP2 = 1;
				GP4 = 0;
				GP5 = 0;
			}
			else if ( decenas==5) //  Si  decenas=5
			{
				GP1 = 0;
				GP2 = 1;
				GP4 = 0;
				GP5 = 1;
			}
			else if ( decenas==6) //  Si  decenas=6
			{
				GP1 = 0;
				GP2 = 0;
				GP4 = 1;
				GP5 = 1;
			}
			else if (decenas==7) //  Si  decenas=7
			{
				GP1 = 0;
				GP2 = 1;
				GP4 = 1;
				GP5 = 1;
			}
			else if ( decenas==8) //  Si  decenas=8
			{
				GP1 = 1;
				GP2 = 0;
				GP4 = 0;
				GP5 = 0;
			}
			else if ( decenas==9) //  Si  decenas=9
			{
				GP1 = 1;
				GP2 = 0;
				GP4 = 0;
				GP5 = 1;
			}

		delay(10);
		GP0=~GP0;

			if (unidades==0) // Si unidades=0
			{	
				GP1 = 0;
				GP2 = 0;
				GP4 = 0;
				GP5 = 0;
			}
			else if ( unidades==1) //  Si  unidades=1
			{
				GP1 = 0;
				GP2 = 0;
				GP4 = 0;
				GP5 = 1;
			}
			else if ( unidades==2) //  Si  unidades=2
			{
				GP1 = 0;
				GP2 = 0;
				GP4 = 1;
				GP5 = 0;
			}
			else if ( unidades==3) //  Si  unidades=3
			{
				GP1 = 0;
				GP2 = 0;
				GP4 = 1;
				GP5 = 1;
			}
			else if ( unidades==4) //  Si  unidades=4
			{
				GP1 = 0;
				GP2 = 1;
				GP4 = 0;
				GP5 = 0;
			}
			else if ( unidades==5) //  Si  unidades=5
			{
				GP1 = 0;
				GP2 = 1;
				GP4 = 0;
				GP5 = 1;
			}
			else if ( unidades==6) //  Si  unidades=6
			{
				GP1 = 0;
				GP2 = 0;
				GP4 = 1;
				GP5 = 1;
			}
			else if ( unidades==7) //  Si  unidades=7
			{
				GP1 = 0;
				GP2 = 1;
				GP4 = 1;
				GP5 = 1;
			}
			else if ( unidades==8) //  Si  unidades=8
			{
				GP1 = 1;
				GP2 = 0;
				GP4 = 0;
				GP5 = 0;
			}
			else if ( unidades==9) //  Si  unidades=9
			{
				GP1 = 1;
				GP2 = 0;
				GP4 = 0;
				GP5 = 1;
			}
			delay(10);
			GP0=~GP0;
}

//###########################################################################
void FMS_lavadora{

  // Actualizar el estado de la lavadora según la interrupcion que se active
  switch (estado_actual) {

    case INTENSIDAD:
      switch (intensidad_seleccionada) {
        case I_BAJA:
          t_total = 279; //31*9 = 279 ciclos == 9 segundos
          //Meter timer del display 7 segmentos
          PB5 = 1; //Enciende LED (B0 creo que esta malo
          estado_actual = SUMINISTRO_AGUA;
          cuenta_regresiva();
          break;
        case I_MEDIA:
          t_total = 589; //31*19 = 589 ciclos == 19 segundos
          //Meter timer del display 7 segmentos
          PB6 = 1; //Enciende LED 
          estado_actual = SUMINISTRO_AGUA;
          break;
        case I_ALTA:
          t_total = 837; //31*27 = 837 ciclos == 27 segundos
          //Meter timer del display 7 segmentos
          PB7 = 1; //Enciende LED
          estado_actual = SUMINISTRO_AGUA;
          break;
        default:
      }
    case SUMINISTRO_AGUA:

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
      }
      
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
      intensidad_seleccionada = ENJUAGUE;
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
      intensidad_seleccionada = CENTRIFUGADO;
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
      intensidad_seleccionada = INTENSIDAD;
      break;
  }
}

int main(void)
{
  PORTB &= (0<<PB3)|(0<<PB4); // Se entablecen los pines B3 y B4 como salidas
  PORTD &= (0<<PD4) | (0<<PD5); // Se entablecen los pines D4 y D5 como salidas
  estado = INTENSIDAD; // Se inicializa en estado paso de vehiculos
  estado_lavadora_t = 1; //Lavadora inicia en play
  contador = 0;
    Setup_INTERRUPCIONES();
  while (1) {
    FMS_lavadora(); //Maquina de estados de lavadora empieza a funcionar
    cuenta_regresiva(t_total);
  }
  return 0;
}