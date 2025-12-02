# Proyecto_Navidad_Bolas
Código para la placa de Arduino del proyecto "Bolas Flotantes" . 

El programa está en un único archivo el cual está adjuntado en el proyecto, consta de las siguientes partes:

Declaración de variables: Esta es la parte que más nos interesa, ya que está pensado para no tener que saber de código para poder modificar en funcionamiento del aparato. Las variables más interesantes a este respecto son:
    
    pwm_up / pwm_down : El valor de la velocidad del motor que va de 0-255.
    t_bajar : El tiempo en milisegundos que tarda bajando. Más tiempo más distancia. En milisegundos
    t_pausa: El tiempo de pausa al final del recorrido. En milisegundos.


Bloque SETUP: En esta parte se ejecuta la función que prepara todo para el funcionamiento.

Bloque LOOP: En este bloque se ejecuta en bucle el movimiento oscilante del motor. Se evalúa en cada vuelta si se ha presionado algún botón y en tal caso se cambia al modo manual.

Funciones auxiliares: Funciones auxiliares para ayudar a lo anteriormente explicado.Montaje.
