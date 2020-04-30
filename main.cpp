/*  Tarea 1. Estructuras de Computadoras 2. Primer Ciclo 2020. 
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
*/
//Librerias necesarias.
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
//Clases necesarias para cada predictor.
#include "BHT.cpp"
#include "HG.cpp"
#include "PHT.cpp"
#include "TORNEO.cpp"

using namespace std;

std::regex get_PC_address("[0-9]+");  //Regex para encontrar Pc_address
std::regex get_JUMP_result("[A-Z]");  //Regex para encontrar el resultado del jump sea T o N.
std::smatch match;                    //Indica que debe hacer match con strings.

long PC_adress;    //Variable global la cual contiene el valor del PC_address actual, require ser un long
std::string jump;  //Variable global que contiene el resultado del jump actual
long accessDIR_BHT;
long accessDIR_PSHARE;

//Contadores utilizados en el programa.

long branchNumber = 0;
long correctPred_Taken = 0;
long correctPred_NTaken = 0;
long incorrectPred_Taken = 0;
long incorrectPred_NTaken = 0;
double porcentaje_Correcto = 0;
long correct_Prediction = 0;
long incorrect_Prediction = 0;
/*
Variables de entrada por medio de linea de comandos, indican los sizes de cada predictor y 
que tipo de prediccion se piensa utilizar sobre el archivo pasado por linea de comandos.
*/
int BHT_SIZE = 0;
int GSHARE_SIZE = 0;
int PSHARE_SIZE = 0;
int PREDICTION_TYPE = 0;
int META_PRED_SIZE = 0;
int OUTPUT_FILE_FLAG = 0;
int LINES_TO_WRITE = 0;

/*Funcion recibe la linea de entrada pasada por la terminal y se encarga de por medio de regex
encontar el valor del PC_COUNTER al hacer el salto y encontrar si el salto se tomo o no. */
void match_line(std::string line_in) {
    if (std::regex_search(line_in, match, get_PC_address)) {
        PC_adress = stol(match.str(0));  // Guarda el valor del PC para un salto, por la longitud del PC es un long.
    }
    if (std::regex_search(line_in, match, get_JUMP_result)) {
        jump = match.str(0);  //Guarda el string del jump actual.
        branchNumber++;
    }
}
/*Funcion se encarga de imprimir los resultado en terminal. Sus parametros de entrada son el tipo de
prediccion seleccionada y los contadores globales del programa. No tiene parametros de salida. */
void print_Console(int selectedPrediction) {
    cout << "----------------------------------------------" << endl;
    cout << "Prediction Parameters" << endl;
    cout << "----------------------------------------------" << endl;
    if (selectedPrediction == 0) {
        cout << "Branch Precition Type:                    Bimodal" << endl;
    }
    if (selectedPrediction == 1) {
        cout << "Branch Precition Type:                    Private History" << endl;
    }
    if (selectedPrediction == 2) {
        cout << "Branch Precition Type:                    Global History" << endl;
    }
    if (selectedPrediction == 3) {
        cout << "Branch Precition Type:                    Predictor de Torneo" << endl;
    }
    std::cout << "BHT size (entries)                            " << (int)pow(2, BHT_SIZE) << endl;  //bht size debe estar al cuadrado
    std::cout << "Global History register size:                 " << GSHARE_SIZE << endl;
    std::cout << "Private History register size:                " << PSHARE_SIZE << endl;
    std::cout << "Simulation Results ---------------------------" << endl;
    std::cout << "Number of Branch                              " << branchNumber << endl;
    std::cout << "Number of correct prediction of taken Branches       " << correctPred_Taken << endl;
    std::cout << "Number of incorrect prediction of taken Branches     " << incorrectPred_Taken << endl;
    std::cout << "Number of correct prediction of not taken Branches   " << correctPred_NTaken << endl;
    std::cout << "Number of incorrect prediction of not taken Branches " << incorrectPred_NTaken << endl;
    std::cout << "Porcentage of Correct Prediction                     " << porcentaje_Correcto << endl;
}

int main(int argc, const char *argv[]) {
    std::string line_In;  // Linea de entrada pasada por la terminal
    ofstream outfile;
    if (argc > 1) {
        BHT_SIZE = atoi(argv[2]);           //Se guarda el size ingresado para la BHT.
        GSHARE_SIZE = atoi(argv[6]);        //Se guarda el size del predictor de historia global.
        PSHARE_SIZE = atoi(argv[8]);        //Se guarda el size del predictor de historia privada.
        PREDICTION_TYPE = atoi(argv[4]);    //Se guarda el valor del tipo de prediccion ingresado.
        OUTPUT_FILE_FLAG = atoi(argv[10]);  //Se guarda si se debe o no generar un archivo de salida.
    } else {
        return 0;
    }
    /* Se crea una mascara cuando se quiere utilizar la el predictor BHT,
    el valor del PC_adress se pasa por esta mascara para solo ver la cantidad
    de bits ingresados y saber cual lugar accesar en el BHT.
    */
    int mask_BHT = pow(2, BHT_SIZE) + 0.5; /*Esta mascara tambien sirve para la tabla PHT dado que tienen
    la misma cantidad de entradas. */
    mask_BHT = mask_BHT - 1;
    //Se crea una instancia de cada predictor con la cantidad de bits ingresados.
    BHT prd_BHT(BHT_SIZE);
    GSHARE prd_GSHARE(GSHARE_SIZE, BHT_SIZE);
    PSHARE prd_PSHARE(PSHARE_SIZE, BHT_SIZE);
    TORNEO prd_TORNEO(BHT_SIZE);
    if (OUTPUT_FILE_FLAG == 1) {  //Si se debe generar un archivo de salida, se crea el archido y se escribe la primera linea.
        outfile.open("branches.txt", ios::out);
        outfile << "PC                   Outcome                      Prediction";
        outfile << "                       correct/incorrect" << endl;
        LINES_TO_WRITE = 5000;  //Cantidad de lineas a escribir.
    }
    while (getline(cin, line_In)) {  // Mientras hayan lineas de entrada se sigue corriendo el programa
        match_line(line_In);         // Se envia la linea a buscar los patrones de PC_address y jump
        //Se revisa el tipo de prediccion.
        if (OUTPUT_FILE_FLAG == 1) {  //Siempre se escribe el PC y el branch en el archivo de salida.
            outfile << PC_adress << "              ";
            outfile << jump[0] << "                             ";
        }
        if (PREDICTION_TYPE == 0) {                //Logica para el predictor BHT.
            accessDIR_BHT = PC_adress & mask_BHT;  //Se obtiene la direccion de acceso del PC con la mascara.
            if (OUTPUT_FILE_FLAG == 1) {           //Cuando se debe generar un archivo de salida se escribe la prediccion y si fue correcta o incorrecta.
                LINES_TO_WRITE--;                  //Se reduce este contador para solo escribir 5000 lineas.
                char PREDICTION = prd_BHT.get_Prediction(accessDIR_BHT);
                outfile << PREDICTION << "                              ";
                if (prd_BHT.comparePrediction(jump[0], accessDIR_BHT)) {  //Si la funcion comparar prediccion retorna true es que fue correcta sino fue incorrecta.
                    outfile << "correct" << endl;
                } else {
                    outfile << "incorrect" << endl;
                }
            } else {
                //Sino hay que escribir un archivo de salida solo se actuliza el predictor.
                prd_BHT.comparePrediction(jump[0], accessDIR_BHT);
            }
        }
        if (PREDICTION_TYPE == 1) {                                  //Logica con el predictor a usar es PSHARE.
            char PREDICTION = prd_PSHARE.get_Prediction(PC_adress);  //Siempre se corre esto para actuliza el valor del PC_adress en el predictor y se obtiene la prediccion para esa direccion.
            prd_PSHARE.update_PSHARE(jump[0]);                       //Luego se corre esto para actulizar le predictor con el ultimo salto.
            if (OUTPUT_FILE_FLAG == 1) {                             //Solo se escribe mientras la bandera de escribir en el archivo este en 1.
                LINES_TO_WRITE--;
                outfile << PREDICTION << "                              ";
                if (PREDICTION == jump[0]) {  //Si la prediccion es igual al salto actual significa que se predijo correctamente.
                    outfile << "correct" << endl;
                } else {
                    outfile << "incorrect" << endl;
                }
            }
        }
        if (PREDICTION_TYPE == 2) {                                  //Logica para el predictor GSHARE.
            char PREDICTION = prd_GSHARE.get_Prediction(PC_adress);  //Se corre esta funcion para actulilizar el valor del PC actual y obtener la prediccion.
            prd_GSHARE.update_GSHARE(jump[0]);                       //Se actuliza el predictor con el ultimo salto luego de actulizarlo con la direccion.
            if (OUTPUT_FILE_FLAG == 1) {
                LINES_TO_WRITE--;
                outfile << PREDICTION << "                              ";
                if (PREDICTION == jump[0]) {  //Se revisa si la prediccion fue correcta o incorrecta.
                    outfile << "correct" << endl;
                } else {
                    outfile << "incorrect" << endl;
                }
            }
        }
        if (PREDICTION_TYPE == 3) {  // Logica para el predictor de torneo.
            //Primero se revisa si el predictor preferido es PSHARE o GSHARE.
            if (prd_TORNEO.get_PreferedPredictor(PC_adress) == 'P') {
                //Si se debe escribir en el archivo se revisa la prediccion el predictor PSHARE.
                if (OUTPUT_FILE_FLAG == 1) {
                    LINES_TO_WRITE--;
                    char PREDICTION = prd_PSHARE.get_Prediction(PC_adress);
                    outfile << PREDICTION << "                              ";
                }
                if (prd_PSHARE.get_Prediction(PC_adress) == jump[0]) {  //Se revisa si la prediccion de PSHARE fue correcta o no.
                    if (OUTPUT_FILE_FLAG == 1) {
                        outfile << "correct" << endl;
                    }
                    correct_Prediction++;  //Se actulizan los contadores de predicciones.
                    if (jump[0] == 'T') {  //Caso cuando el salto es taken y la prediccion fue correcta.
                        correctPred_Taken++;
                    } else {
                        correctPred_NTaken++;
                    }
                } else {
                    if (OUTPUT_FILE_FLAG == 1) {
                        outfile << "incorrect" << endl;
                    }
                    incorrect_Prediction++;
                    if (jump[0] == 'T') {  //Caso cuando el salto es not taken y la prediccion incorreta.
                        incorrectPred_Taken++;
                    } else {
                        incorrectPred_NTaken++;
                    }
                }
                //Logica para el caso que el predictor preferido sea GSHARE.
            } else if (prd_TORNEO.get_PreferedPredictor(PC_adress) == 'G') {
                if (OUTPUT_FILE_FLAG == 1) {
                    LINES_TO_WRITE--;
                    char PREDICTION = prd_GSHARE.get_Prediction(PC_adress);
                    outfile << PREDICTION << "                              ";
                }
                if (prd_GSHARE.get_Prediction(PC_adress) == jump[0]) {  //Se revisa si la prediccion del GSHARE fue correcta o no.
                    if (OUTPUT_FILE_FLAG == 1) {
                        outfile << "correct" << endl;
                    }
                    correct_Prediction++;
                    if (jump[0] == 'T') {  //Se actulizan contadores.
                        correctPred_Taken++;
                    } else {
                        correctPred_NTaken++;
                    }
                } else {  //Esto sucede cuando la prediccion hecha no fue correcta y por tanto se actualizan los contadores.
                    if (OUTPUT_FILE_FLAG == 1) {
                        outfile << "incorrect" << endl;
                    }
                    incorrect_Prediction++;
                    if (jump[0] == 'T') {
                        incorrectPred_Taken++;
                    } else {
                        incorrectPred_NTaken++;
                    }
                }
            }
            //Luego se revisa si alguno de los predictores estuvo correcto y el otro no y se actualiza el predictor de torneo en base a cual estuvo correcto.
            if (prd_GSHARE.get_Prediction(PC_adress) == jump[0] & prd_PSHARE.get_Prediction(PC_adress) != jump[0]) {
                prd_TORNEO.modify_weights('G', PC_adress);  //Caso cuando GSHARE estuvo correcto y PSHARE no.
            }
            if (prd_GSHARE.get_Prediction(PC_adress) != jump[0] & prd_PSHARE.get_Prediction(PC_adress) == jump[0]) {
                prd_TORNEO.modify_weights('P', PC_adress);  // caso cuando PSHARE estuvo correcto y GSHARE no.
            }
            //Si ambos estuvieron incorrectos o ambos correctos no se modifican los pesos del predictor de torneo.
            prd_PSHARE.get_Prediction(PC_adress);  //Siempre se actualizan los valores de cada predictor PSHARE y GSHARE.
            prd_GSHARE.get_Prediction(PC_adress);
            prd_PSHARE.update_PSHARE(jump[0]);  //Se actualizan ambos predictores.
            prd_GSHARE.update_GSHARE(jump[0]);
        }
        if (LINES_TO_WRITE == 0) {  //Cuando este contador llega a 0 ya se escribieron 5000 linea y por tanto la bandera pasa a ser 0.
            OUTPUT_FILE_FLAG = 0;
        }
    }
    //Dependiendo del tipo de prediccion se actualizan los contadores globales del programa con los resultados obtenidos.
    if (PREDICTION_TYPE == 0) {  //Caso para el predictor BHT.
        correctPred_Taken = prd_BHT.correctPred_taken;
        correctPred_NTaken = prd_BHT.correctPred_Ntaken;
        incorrectPred_NTaken = prd_BHT.incorrectPred_Ntaken;
        incorrectPred_Taken = prd_BHT.incorrectPred_taken;
        correct_Prediction = prd_BHT.correct_Prediction;
        incorrect_Prediction = prd_BHT.incorrect_Prediction;
    } else if (PREDICTION_TYPE == 1) {  //Caso para el predictor PSHARE
        correctPred_Taken = prd_PSHARE.predBimodal.correctPred_taken;
        correctPred_NTaken = prd_PSHARE.predBimodal.correctPred_Ntaken;
        incorrectPred_Taken = prd_PSHARE.predBimodal.incorrectPred_taken;
        incorrectPred_NTaken = prd_PSHARE.predBimodal.incorrectPred_Ntaken;
        correct_Prediction = prd_PSHARE.predBimodal.correct_Prediction;
        incorrect_Prediction = prd_PSHARE.predBimodal.incorrect_Prediction;
    } else if (PREDICTION_TYPE == 2) {  //Caso para el predictor GSHARE.
        correct_Prediction = prd_GSHARE.predBimodal.correct_Prediction;
        incorrect_Prediction = prd_GSHARE.predBimodal.incorrect_Prediction;
    }
    //Por ultimo se calcula el porcentaje de predicciones correctas.
    porcentaje_Correcto = static_cast<double>(correct_Prediction) / static_cast<double>(branchNumber);
    //Se llama a la funcion encargada de imprimir los resultados en la consola.
    print_Console(PREDICTION_TYPE);

    //std::cout << "Correct Pred :" << correct_Prediction << endl;
    //std::cout << "Incorrect Pred :" << incorrect_Prediction << endl;
    return 0;
}
