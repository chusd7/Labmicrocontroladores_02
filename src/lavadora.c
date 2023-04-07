#include<avr/io.h>
#include <avr/interrupt.h>
#include<util/delay.h>

// Estados de la lavadora
enum {
  INICIO_PAUSA,
  SUMINISTRO_AGUA,
  LAVADO,
  ENJUAGUE,
  CENTRIFUGADO,
};

// Subestados de cada estado,corresponden a la intensidad seleccionada
enum {
  BAJA,
  MEDIA,
  ALTA,
};

int estado_actual = INICIO_PAUSA;
int intensidad_seleccionada = BAJA;

int main(void){


  while (1) {
  // Leer los botones y actualizar la intensidad seleccionada
  // ...

  // Actualizar el estado de la lavadora según el botón presionado
  switch (estado_actual) {
    case INICIO_PAUSA:
      // Esperando que se presione el botón de inicio/pausa
      if (boton_inicio_pausa_presionado()) {
        estado_actual = SUMINISTRO_AGUA;
      }
      break;

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
      estado_actual = INICIO_PAUSA;
      break;
  }
}
}