// Clase BHT implementa el predictor bimodal por medio de un vector
// cada entrada del vector contiene el estado actual el cual se puede modificar
// segun las predicciones sean correctas o incorrectas. Pasando de N->n->t->T.
// Las letras en mayuscula significan STRONGLY TAKEN / NOT TAKEN, mientras que las
// minusculas significan WEAKLY TAKEN / NOT TAKEN.
// La clase ademas tiene varios contadores los cuales mantienen un registro de resultados.
// Dependiendo del numero de bits que se deseen usar la tabla crece de forma 2^n.
#include <array>
#include <cmath>
#include <vector>

class BHT {
    int SIZE;

   public:
    //Contadores utilizados:
    long correct_Prediction = 0;    //Contiene cantidad de predicciones correctas
    long incorrect_Prediction = 0;  //Contiene predicciones incorrectas
    long correctPred_taken = 0;     //Predicciones correctas de branches taken.
    long correctPred_Ntaken = 0;    //Prediccion correctas de branches not taken.
    long incorrectPred_taken = 0;   //Predicciones incorrectas de branches taken.
    long incorrectPred_Ntaken = 0;  //Predicciones incorrectas de branches not taken
    std::vector<char> table;        // Tabla BHT.
    BHT();
    BHT(int a);
    BHT(const BHT &);
    BHT &operator=(const BHT &in);
    char get_Prediction(int);
    void modify_weights(char, long);
    char show_Content(int);
    void SHOW_ALL();
    bool comparePrediction(char, long);
};
//Constructor para copias, recibe de parametro de entrada un objeto BHT.
BHT::BHT(const BHT &copy) {
    SIZE = copy.SIZE;
    table = copy.table;
}
/*Funcion sobreescribe el comportamiento del operador '=' para cuando se quiere crear un objeto en base a otro.
su parametro de entrada es el objeto BHT a copiar.*/
BHT &BHT::operator=(const BHT &in) {
    SIZE = in.SIZE;
    table = in.table;
};
/*Funcion recibe el salto tomado (sea T o N) y la direccion a accesar en el BHT como parametros de entrada.
Luego revisa si la prediccion fuera correcta o no y modifica los pesos internos de la tabla. Ademas mantiene
un registro de prediccion correctas e incorrectas y su tipo. Como parametro de salida da un valor bool
que es true cuando la prediccion fue correcta, caso contrario false. */
bool BHT::comparePrediction(char jumpValue, long DIR) {
    if (get_Prediction(DIR) == jumpValue) {
        correct_Prediction++;
        if (jumpValue == 'N') {
            correctPred_Ntaken++;
        } else {
            correctPred_taken++;
        }
        modify_weights('C', DIR);  //Peso se modifica para una prediccion correcta.
        return true;
    } else {
        incorrect_Prediction++;
        if (jumpValue == 'N') {
            incorrectPred_Ntaken++;
        } else {
            incorrectPred_taken++;
        }
        //Si la prediccion es incorrecta se modifica los pesos dentro del predictor dependiendo del PC_adress.
        modify_weights('I', DIR);
        return false;
    }
}
//Constructor default, crea una tabla BHT de 4 entradas.
BHT::BHT() {
    table.resize(4);
    SIZE = 4;
    table.resize(SIZE, 'N');
}
//Constructor para la tabla BHT, recibe el numero de bits a usar. Luego crea un vector de 2^n entradas inicializas con 'N'.
BHT::BHT(int a) {
    SIZE = pow(2, a) + 0.5;
    table.resize(SIZE, 'N');
}
//Funcion no tiene parametros de entrada, lo unico que hace es imprimir el contenido de la tabla BHT en la terminal
void BHT::SHOW_ALL() {
    for (int i = 0; i < SIZE; i++) {
        std::cout << table[i];
    }
    std::cout << std::endl;
}
/*Funcion recibe de entrada la fila a revisar, luego retorna el valor actual dentro de ese lugar. 
Sirve para revisar el contenido de un lugar en especifico.*/
char BHT::show_Content(int ADDRESS) {
    return table[ADDRESS];
}
/*Funcion recibe de entrada el lugar a revisar y retorna la prediccion en base a su contenido. */
char BHT::get_Prediction(int ADDRESS) {
    if (table[ADDRESS] == 'N' || table[ADDRESS] == 'n')
        return 'N';
    else
        return 'T';
}
/*Esta funcion recibe dos parametros de entrada, uno el cual indica si la prediccion fue correcta o 
incorrecta y la entrada de la tabla a modificar. No retorna nada. Modifica los pesos pasando de:
Strongly taken <-> weakly taken <-> weakly not taken <-> strongly not taken. 
*/
void BHT::modify_weights(char Correct_or_Wrong, long ADDRESS) {
    if (Correct_or_Wrong == 'C') {
        if (table[ADDRESS] == 'n') {
            table[ADDRESS] = 'N';
        }
        if (table[ADDRESS] == 't') {
            table[ADDRESS] = 'T';
        }
    } else {
        if (table[ADDRESS] == 't') {
            table[ADDRESS] = 'n';
        } else if (table[ADDRESS] == 'T') {
            table[ADDRESS] = 't';
        } else if (table[ADDRESS] == 'n') {
            table[ADDRESS] = 't';
        } else if (table[ADDRESS] == 'N') {
            table[ADDRESS] = 'n';
        }
    }
}
