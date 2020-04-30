# Simulador-de-Cache
  Tarea 1. Estructuras de Computadoras 2. Primer Ciclo 2020. 
    Jose Alberto Barrantes Abellan. B50881. 

    La siguiente tarea simulan distintos predictores de cache:
    Predictor Bimodal.
    Predictor con historia Global. GSHARE.
    Predictor con historia Privada. PSHARE.
    Predictor de torneo. 

    Todos los predictores implementados estan parametrizados y sus valores se modifican por medio de la
    terminal. Para poder correr alguno de los predictores se escoge por medio de la terminar con el parametro -bp.
    Cuando se corre el programa se deben ingresar siempre todos los parametros de entrada, se desee o no 
    correr algun predictor esto por la manera que se encuentran los valores de los parametros de entrada. 

    El programa de puede compilar con:
    g++ main.cpp -o out
    La forma de correr el programa en base a lo anterior es: 
    gunzip -c branch-trace-gcc.trace.gz j branch -s < # > -bp < # > -gh < # > -ph < # > -o < # >

    El parametro -bp indica el predictor a usar de la forma:
    0 -> predictor bimodal
    1 -> predictor de historia global
    2 -> predictor de historia privada 
    3 -> predictor de torneo.
    El parametro -s indica la cantidad de bits para la tabla BHT, el predictor de torneo y el predictor de historia privada.
    El parametro -gh indica los bits de historia en el registro de historia global.
    El parametro -ph indica la cantidad de bits de historia en el registro de historia privada.
    El parametro -o solo debe ser 0 o 1 y indica si se debe escribir un archivo de salida con el resultado de los primeros 5000 saltos 
    del programa. 
