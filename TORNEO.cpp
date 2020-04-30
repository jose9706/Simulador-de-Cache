/* Clase torneo funciona de forma similar a la clase BHT. Tiene un vector con varias entradas las cuales
contienen los pesos actuales. El predictor se inicializa con strongly prefered PSHARE y se va modificando
conforme uno u otro predictor (GSHARE o PSHARE) predicen correctamente un salto. 
Sus pesos son :
P = Strongly prefered PSHARE.
p = Weakly Prefered PSHARE.
g = weakly prefered GSHARE.
G = strongly prefered GSHARE.*/

#include <array>
#include <cmath>
#include <vector>

class TORNEO {
    int SIZE;
    int mask;  // Mascara para solo accesar los bits necesarios.

   public:
    std::vector<char> table;
    TORNEO();
    TORNEO(int);
    char get_PreferedPredictor(int);
    void modify_weights(char, long);
    char show_Content(int);
    void SHOW_ALL();
    void comparePredictor(char, long);
};
//Constructor por defecto construye una tabla de 4 entradas. Iniciadas con Strongly Prefered Pshare.
TORNEO::TORNEO() {
    table.resize(4);
    SIZE = 4;
    table.resize(SIZE, 'P');
}
//Constructor recibe como entrada la cantidad de bits a usar en el predictor. En base a eso se expande el vector y se inicializa.
TORNEO::TORNEO(int size_in) {
    SIZE = pow(2, size_in) + 0.5;
    mask = SIZE - 1;
    table.resize(SIZE, 'P');
}
//Funcion no recibe nada de entrada y lo unico que hace es imprimir en la terminal el contenido del predictor de torneo.
void TORNEO::SHOW_ALL() {
    for (int i = 0; i < SIZE; i++) {
        std::cout << table[i];
    }
}
//Funcion recibe de entrada una direccion a la cual accesa y retorna su contenido.
char TORNEO::show_Content(int ACCESS_DIR) {
    return table[ACCESS_DIR];
}
//Funcion recibe una direccion del PC y retorna el tipo de predictor preferido para esa direccion.
char TORNEO::get_PreferedPredictor(int PC_address) {
    long ACCESS_DIR = PC_address & mask;
    if (table.at(ACCESS_DIR) == 'P' || table.at(ACCESS_DIR) == 'p')
        return 'P';
    else
        return 'G';
}
/*Esta funcion recibe dos entradas la nueva preferencia la cual indica cual predictor estuvo correcto en el ultimo salto 
y la direccion del PC para saber cual entrada modificar con el nuevo peso. Utiliza la mascara para solo ver los bis validos 
de la direccion del PC.
*/
void TORNEO::modify_weights(char preferenceUpdate, long PC_address) {
    long ACCESS_DIR = PC_address & mask;
    if (preferenceUpdate == 'P') {
        if (table[ACCESS_DIR] == 'g') {
            table[ACCESS_DIR] = 'p';
        } else if (table[ACCESS_DIR] == 'p') {
            table[ACCESS_DIR] = 'P';
        } else if (table[ACCESS_DIR] == 'G') {
            table[ACCESS_DIR] = 'g';
        }
    } else {
        if (table[ACCESS_DIR] == 'P') {
            table[ACCESS_DIR] = 'p';
        } else if (table[ACCESS_DIR] == 'p') {
            table[ACCESS_DIR] = 'g';
        } else if (table[ACCESS_DIR] == 'g') {
            table[ACCESS_DIR] = 'G';
        }
    }
}
