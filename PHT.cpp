/*
Clase PSHARE implementa el predictor de historia privada. Para ello se tiene un vector cuyas entradas
son valores numeros los cuales se utilizan como registros de historia. Ademas el predictor tiene su propia
tabla BHT la cual accesa de forma directa. Cuando se crea un predictor de historia privada se deben indicar
la cantidad de bits de historia que se desean tener y la cantidad de entradas para tanto el predictor de historia
privada como su tabla BHT interna. 
*/
#include <stdio.h>

#include <cmath>
#include <vector>
//#include "BHT.cpp"
class PSHARE {
    int SIZE;
    long mask;
    long maskBHT;
    long accessDIR = 0;
    long xor_PSHARE = 0;
    long accessDIR_BHT = 0;

   public:
    BHT predBimodal;
    std::vector<long> table;
    PSHARE();
    PSHARE(int, int);
    long get_current_History(int);
    void update_PSHARE(char);
    void PRINT_CONTENT();
    char get_Prediction(long);
};
//Constructur por defecto.
PSHARE::PSHARE() {
}
/* Constructor del predictor. Recibe como parametros de entrada la cantidad de bits y la cantidad de
bits para la tabla BHT. */
PSHARE::PSHARE(int bit_length, int size_in) {
    mask = pow(2, bit_length) + 0.5;
    SIZE = pow(2, size_in) + 0.5;
    table.resize(SIZE, 0);
    mask = mask - 1;
    maskBHT = SIZE - 1;
    BHT BIMODAL(size_in);
    predBimodal = BIMODAL;
}
//Funcion no tiene parametros de entrada y imprime en contenido de su tabla BHT.
void PSHARE::PRINT_CONTENT() {
    predBimodal.SHOW_ALL();
}
//Funcion recibe de parametro de entrada una direccion del PC y retorna la prediccion para dicha direccion.
char PSHARE::get_Prediction(long PC_address) {
    accessDIR = PC_address & maskBHT;
    xor_PSHARE = PC_address ^ get_current_History(accessDIR);
    accessDIR_BHT = xor_PSHARE & maskBHT;
    return predBimodal.get_Prediction(accessDIR_BHT);
}
//Funcion recibe una direccion y retorna la historia privada de esa localizacion.
long PSHARE::get_current_History(int Dir) {
    return table.at(Dir);
}
/*Funcion actualiza la historia una entrada de la tabla de historias privadas en base a su parametro de
entrada T_or_N el cual indica si el ultimo salto es un taken o not taken. Su otro parametro de entrada es
la direccion actual de acceso la cual se actualiza en la funcion get_Prediction. Por ultimo actualiza 
lo que se encuentra en su tabla BHT. */
void PSHARE::update_PSHARE(char T_or_N) {
    if (T_or_N == 'T') {
        table[accessDIR] = table[accessDIR] << 1;
        table[accessDIR] = table[accessDIR] & mask;
        table[accessDIR] = table[accessDIR] | 0b1;
    } else if (T_or_N = 'N') {
        table[accessDIR] = table[accessDIR] << 1;
        table[accessDIR] = table[accessDIR] & mask;
        table[accessDIR] = table[accessDIR] | 0b0;
    } else {
    }
    predBimodal.comparePrediction(T_or_N, accessDIR_BHT);
}