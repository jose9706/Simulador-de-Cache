/*Clase GSHARE implementa el predictor de historia global para ello se utiliza una variable tipo long
llama history_Table donde se utiliza similara un registro donde se guardan los bits de historia que han pasado.
El predictor de historia global ademas tiene su propia tabla BHT dentro, la cual accese directamente en distintas 
funciones. Por medio de cierta logica utilizando mascaras se recibe el valor del PC_ADDRESS y se accese a la tabla BHT.
*/
#include <stdio.h>
#include <cmath>

class GSHARE {
    int SIZE;                //Cantidad de bits de historia.
    int sizeBHT;             // Size de la tabla BHT.
    long history_Table;      // Registro que contiene la historia.
    long mask;               // Mascara para modificar la historia del predictor.
    long maskBHT;            // Mascara para obtener la direccion de acceso a la tabla BHT.
    long xor_GSHARE = 0;     //Contiene el valor del XOR del registro de historia con el PC
    long accessDIR_BHT = 0;  //Contiene la direccion a accesar en la tabla BHT para un PC dado.
   public:
    BHT predBimodal;
    GSHARE();
    GSHARE(int, int);
    long current_History();
    void update_GSHARE(char);
    char get_Prediction(long);
};

//Constructor por defecto.
GSHARE::GSHARE() {
    SIZE = 2;
    history_Table = 0;
}
/*Funcion constructura recibe de entrada el numero de bits del predictor global y la 
cantidad de bits para la tabla BHT.*/
GSHARE::GSHARE(int A, int sizeBHT) {
    SIZE = A;
    BHT BIMODAL(sizeBHT);
    sizeBHT = pow(2, sizeBHT) + 0.5;
    maskBHT = sizeBHT - 1;  // La mascara siempre es 2^size -1, de forma tal que si son 2 bits la mascara es 3 para solo accesar esos 2 bits.
    predBimodal = BIMODAL;  // Se copia la tabla BHT creada a la que esta en la clase.
    history_Table = 0;      // Se inicializa la historia con NOT TAKEN.
    mask = (long)(pow(2, SIZE) + 0.5);
    mask = mask - 1;  // Mascara para mantener solo cierto numero de bits de historia.
}
/* Funcion recibe de entrada una direccion de branch y retorna la prediccion para esa direccion */
char GSHARE::get_Prediction(long PC_address) {
    xor_GSHARE = PC_address ^ current_History();
    accessDIR_BHT = xor_GSHARE & maskBHT;
    return predBimodal.get_Prediction(accessDIR_BHT);
}
//Funcion tiene de salida el estado actual de la tabla de historia. No tiene entradas.
long GSHARE::current_History() {
    return history_Table;
}
/*Funcion recibe de entrada el tipo de branch (T o N) y actualiza su historia respecto a eso.
Tambien actuliza su registro BHT  en base al branch.*/
void GSHARE::update_GSHARE(char T_or_N) {
    if (T_or_N == 'T') {
        history_Table = history_Table << 1;
        history_Table = history_Table & mask;
        history_Table = history_Table | 0b1;
    } else if (T_or_N == 'N') {
        history_Table = history_Table << 1;
        history_Table = history_Table & mask;
        history_Table = history_Table | 0b0;
    } else {
    }
    predBimodal.comparePrediction(T_or_N, accessDIR_BHT);
}