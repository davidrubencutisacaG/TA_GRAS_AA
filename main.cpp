#include <iostream>
#include <cstdlib>
#include <limits.h>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <fstream>
#include <iomanip>

#define alfa 0.25
#define iteraciones  100000        

using namespace std;
///////////////////////////////////STRUCTS/////////////////////////////////////

struct item {
    const char *nombre;
    double peso;
    double volumen;

};

struct espacio {
    int num;
    double peso;
    double volumen;
};
////////////////////////////FUNCIONES AUXILIARES////////////////////////////////

bool ordenar(item a, item b) {
    return a.peso > b.peso;
}

bool ordenarespacio(espacio a, espacio b) {
    return a.peso > b.peso;
}

void insertarespacios(vector<espacio>&vespacios, int n, double maxpeso,
        double maxvolumen) {
    for (int i = 0; i < n; i++) {
        vespacios.push_back({i + 1, maxpeso, maxvolumen});
    }
}

int obtenercantidad(vector<item>vitems, int rcl1) {
    int cant = 0;
    for (int i = 0; i < vitems.size(); i++) {
        if (vitems[i].peso >= rcl1)cant++;
    }
    return cant;
}

void obtenerpesoaux(vector<espacio>vespacio, vector<espacio>&vespacioaux,
        item vitem) {
    for (int i = 0; i < vespacio.size(); i++) {
        if (vespacio[i].peso >= vitem.peso and
                vespacio[i].volumen >= vitem.volumen) {
            vespacioaux.push_back(vespacio[i]);
        }
    }
}

void actualizar(vector<espacio>&vespacios, espacio vespaciosaux) {
    for (int i = 0; i < vespacios.size(); i++) {
        if (vespacios[i].num == vespaciosaux.num) {
            vespacios[i].peso = vespaciosaux.peso;
            vespacios[i].volumen = vespaciosaux.volumen;
            break;
        }
    }
}

int cantidadcontenedores(vector<espacio> vespacios, int maxpeso) {
    int cant = 0;
    for (int i = 0; i < vespacios.size(); i++) {
        if (vespacios[i].peso < maxpeso)cant++;
    }
    return cant;

}
//////////////////////////////FUNCIONES PRINCIPALES/////////////////////////////

void mostrarContenedores(vector<vector<item>> respuesta, int n) {

    //cout << "Solucion mejorada: " << endl;
    cout << endl;
    int numcontenedor = 1;
    for (int i = 0; i < n; i++) {
        if (respuesta[i].size() != 0) {
            cout << "Contenedor " << numcontenedor << " :";
            for (int j = 0; j < respuesta[i].size(); j++) {
                cout << respuesta[i][j].peso << " "
                        << respuesta[i][j].volumen << "  ";
            }
            numcontenedor++;
            cout << endl;
        }
    }
    cout << endl;
}

void GraspItemsMejoria(vector<vector<item>> &respuestafinal, double maxpeso,
        double maxvolumen, int n) {
    for (int i = 0; i < respuestafinal.size(); i++) {
        for (int j = i + 1; j < respuestafinal.size(); j++) {
            if (not respuestafinal[j].empty()) {
                for (int k = 0; k < respuestafinal[j].size(); k++) {
                    item auxItem = respuestafinal[j][k];
                    double totalPeso = 0, totalVolumen = 0;

                    //Calculamos el peso y volumen ocupados en el contenedor actual
                    for (auto &obj : respuestafinal[i]) {
                        totalPeso += obj.peso;
                        totalVolumen += obj.volumen;
                    }

                    //Verificamos si podemos agregar el objeto
                    if (totalPeso + auxItem.peso <= maxpeso and
                            totalVolumen + auxItem.volumen <= maxvolumen) {
                        respuestafinal[i].push_back(auxItem); //Movemos el objeto
                        respuestafinal[j].erase(respuestafinal[j].begin() + k); //Eliminamos del contenedor original
                        k--; //Ajustamos el índice para evitar saltos
                    }
                }
            }
        }
    }
}

void GraspItemsConstruccion(vector<vector<item>> &respuestafinal, item *items,
        int n, double maxpeso, double maxvolumen) {

    vector<vector < item >> respuestaparcial(n);
    vector<item>vitems;
    vector<espacio>vespacios;

    insertarespacios(vespacios, n, maxpeso, maxvolumen);
    vitems.insert(vitems.begin(), items, items + n);
    sort(vitems.begin(), vitems.begin() + n, ordenar);

    while (not vitems.empty()) {
        sort(vespacios.begin(), vespacios.begin() + n, ordenarespacio);
        vector<espacio>vespaciosaux;

        int beta = vitems[0].peso;
        int tao = vitems[vitems.size() - 1].peso;
        int rcl = round(beta - alfa * (beta - tao));
        int cant1 = obtenercantidad(vitems, rcl);
        int pos1 = rand() % cant1;

        obtenerpesoaux(vespacios, vespaciosaux, vitems[pos1]);

        int cant2 = vespaciosaux.size();
        int pos2 = rand() % cant2;

        if (vespaciosaux[pos2].peso >= vitems[pos1].peso and
                vespaciosaux[pos2].volumen >= vitems[pos2].volumen) {
            vespaciosaux[pos2].peso -= vitems[pos1].peso;
            vespaciosaux[pos2].volumen -= vitems[pos1].volumen;
            actualizar(vespacios, vespaciosaux[pos2]);
            respuestaparcial[vespacios[pos2].num - 1].push_back(vitems[pos1]);
        }
        vitems.erase(vitems.begin() + pos1);
    }
    respuestafinal = respuestaparcial;
}

void calcularEspacioSobrante(vector<vector <item>> respuestafinal,
        int &pesoSobrante, int &volumenSobrante, int maxpeso, int maxvolumen) {

    for (int i = 0; i < respuestafinal.size(); i++) {
        int sumaPesoPorContenedor = 0;
        int sumaVolumenPorContenedor = 0;
        for (int j = 0; j < respuestafinal[i].size(); j++) {
            sumaPesoPorContenedor += respuestafinal[i][j].peso;
            sumaVolumenPorContenedor += respuestafinal[i][j].volumen;
        }
        if (not respuestafinal[i].empty()) {
            pesoSobrante += (maxpeso - sumaPesoPorContenedor);
            volumenSobrante += (maxvolumen - sumaVolumenPorContenedor);
        }
    }
}

void llenarVectoresAuxiliaresReporte(vector<vector<item>> respuestafinal,
        int n, int maxpeso, int maxvolumen,
        vector<int> &cantidadContenedores,
        vector<int>&desperdicioPesoContenedores,
        vector<int>&desperdicioVolumenContenedores) {

    int peso = 0, volumen = 0, cant = 0;
    for (int i = 0; i < respuestafinal.size(); i++) {
        if (!respuestafinal[i].empty()) {
            cant++;
            //            for (int j = 0; j < respuestafinal[i].size(); j++) {
            //                item dato = respuestafinal[i][j];
            //                peso += dato.peso;
            //                volumen += dato.volumen;
            //            }
        }
    }
    calcularEspacioSobrante(respuestafinal, peso, volumen, maxpeso, maxvolumen);
    cantidadContenedores.push_back(cant);
    desperdicioPesoContenedores.push_back(peso);
    desperdicioVolumenContenedores.push_back(volumen);
}

void mostrarReporte(vector<int> contSM, vector<int> despPesoSM, vector<int> despVolSM,
        vector<int> contCM, vector<int> despPesoCM, vector<int> despVolCM) {
    ofstream arch("Reporte.txt", ios::out);
    if (not arch.is_open()) {
        cout << "Error al abrir el archivo de Reporte.txt" << endl;
        exit(true);
    }
    int cantDatos = contSM.size(); //todos los vectores que paso por referencia tienen
    //el mismo tamaño
    arch << setprecision(2) << fixed;
    arch << "Términos clave:" << endl;
    arch << "SM: Sin mejoría" << endl;
    arch << "CM: Con mejoría" << endl << endl;

    // Cabecera
    arch << left;
    arch << setw(12) << "Iteración"
            << setw(25) << "Contenedores (SM)"
            << setw(25) << "Desperdicio Peso (SM)"
            << setw(25) << "Desperdicio Volumen (SM)"
            << setw(25) << "Contenedores (CM)"
            << setw(25) << "Desperdicio Peso (CM)"
            << setw(25) << "Desperdicio Volumen (CM)"
            << setw(25) << "Mejoría Cont. (%)"
            << setw(25) << "Mejoría Peso (%)"
            << setw(25) << "Mejoría Vol. (%)"
            << setw(25) << "Mejoría General (%)"
            << endl;

    double totalMejoria = 0.0;

    // Datos
    for (int i = 0; i < cantDatos; i++) {
        arch << setw(12) << i + 1
                << setw(25) << contSM[i]
                << setw(25) << despPesoSM[i]
                << setw(25) << despVolSM[i]
                << setw(25) << contCM[i]
                << setw(25) << despPesoCM[i]
                << setw(25) << despVolCM[i];

        double porMejorCont = ((double) (contSM[i] - contCM[i]) / contSM[i]) * 100;
        double porMejorDespPeso = ((double) (despPesoSM[i] - despPesoCM[i]) / despPesoSM[i]) * 100;
        double porMejorDespVol = ((double) (despVolSM[i] - despVolCM[i]) / despVolSM[i]) * 100;

        arch << setw(25) << porMejorCont
                << setw(25) << porMejorDespPeso
                << setw(25) << porMejorDespVol;

        double porMejoriaGeneral = (porMejorCont + porMejorDespPeso + porMejorDespVol) / 3;
        totalMejoria += porMejoriaGeneral;

        arch << setw(25) << porMejoriaGeneral << endl;
    }

    // Resumen
    double porMejoriaTOTAL = totalMejoria / cantDatos;
    arch << endl;
    arch << right;
    arch << setw(10) << "Mejoría total (%): " << porMejoriaTOTAL << '%' << endl;
    arch.close();
}

int main(int argc, char** argv) {
    /////////////////////////////////////DATA/////////////////////////////////
    item items[20] = {
        // Ítems originales ajustados
        {"Motor A", 5000, 11},
        {"Motor B", 6000, 14},
        {"Motor C", 4500, 10},
        {"Caja de herramientas", 500, 2},
        {"Bobina de cables", 2000, 6},
        {"Barril de aceite", 1000, 3},
        {"Palé de ladrillos", 3000, 8},
        {"Máquina industrial", 7000, 18},
        {"Equipo pesado", 8000, 20},
        {"Generador eléctrico", 4000, 9},

        // Nuevos ítems
        {"Caja de cerámica", 1200, 4},
        {"Compresor de aire", 3500, 7},
        {"Rollo de papel", 800, 2},
        {"Panel solar", 1500, 6},
        {"Tanque de agua", 2500, 12},
        {"Refrigerador industrial", 5000, 16},
        {"Batería de litio", 3000, 5},
        {"Pieza de maquinaria", 7000, 13},
        {"Contenedor químico", 4500, 8},
        {"Turbina", 8000, 22},
    };
    ///////////////////////////////////VARIABLES///////////////////////////////
    double maxpeso = 30000;
    double maxvolumen = 67;
    int n = sizeof (items) / sizeof (items[0]);
    int cont = 0;


    vector<vector < item>> mejor;
    int menorPesoSobrante = 9999999;
    int menorVolumenSobrante = 9999999;


    vector<int>cantidadContenedoresSinMejoria;
    vector<int>desperdicioPesoContenedoresSinMejoria;
    vector<int>desperdicioVolumenContenedoresSinMejoria;

    vector<int>cantidadContenedoresConMejoria;
    vector<int>desperdicioPesoContenedoresConMejoria;
    vector<int>desperdicioVolumenContenedoresConMejoria;
    srand(time(NULL));

    ////////////////////////////////////GRASP////////////////////////////////
    while (cont < iteraciones) {
        vector<vector < item>> respuestafinal;

        /////////////////////////SIN MEJORIA/////////////////////
        cout << "Solucion sin mejorar " << cont + 1 << ":" << endl;
        GraspItemsConstruccion(respuestafinal, items, n, maxpeso, maxvolumen);
//        mostrarContenedores(respuestafinal, n);

        llenarVectoresAuxiliaresReporte(respuestafinal, n, maxpeso, maxvolumen,
                cantidadContenedoresSinMejoria,
                desperdicioPesoContenedoresSinMejoria,
                desperdicioVolumenContenedoresSinMejoria);

        /////////////////////////CON MEJORIA/////////////////////
        cout << "Solucion con mejoría " << cont + 1 << ":" << endl;
        GraspItemsMejoria(respuestafinal, maxpeso, maxvolumen, n);
//        mostrarContenedores(respuestafinal, n);

        llenarVectoresAuxiliaresReporte(respuestafinal, n, maxpeso, maxvolumen,
                cantidadContenedoresConMejoria,
                desperdicioPesoContenedoresConMejoria,
                desperdicioVolumenContenedoresConMejoria);

        cont++;
    }
    /////////////////////////REPORTE DE ITERACIONES/////////////////////

    mostrarReporte(cantidadContenedoresSinMejoria,
            desperdicioPesoContenedoresSinMejoria,
            desperdicioVolumenContenedoresSinMejoria,
            cantidadContenedoresConMejoria,
            desperdicioPesoContenedoresConMejoria,
            desperdicioVolumenContenedoresConMejoria);

    return 0;
}





/////////////////////ANEXOS/////////////////
//if (esMejor(respuestafinal, menorPesoSobrante, menorVolumenSobrante,
//                maxpeso, maxvolumen)) {
//            mejor = respuestafinal;
//            //aux = cont;
// }

//cout<<endl;
//    cout<<aux<<endl;
//    cout<<"Mejor solucion de las mejorías: "<<endl;
//    mostrarContenedores(mejor, n);
//    cout << "El desperdicio de peso es: " << menorPesoSobrante << endl;
//    cout << "El desperdicio de volumen es: " << menorVolumenSobrante << endl;

//bool esMejor(vector<vector<item>> respuestafinal, int &menorPesoSobrante, int &menorVolumenSobrante,
//        int maxpeso, int maxvolumen) {
//    int pesoSobrante = 0, volumenSobrante = 0;
//    calcularEspacioSobrante(respuestafinal, pesoSobrante, volumenSobrante, maxpeso, maxvolumen);
//
//    if (pesoSobrante <= menorPesoSobrante and volumenSobrante <= menorVolumenSobrante
//            or(pesoSobrante == menorPesoSobrante and volumenSobrante <= menorVolumenSobrante)
//            or(pesoSobrante <= menorPesoSobrante and volumenSobrante == menorVolumenSobrante)) {
//        menorPesoSobrante = pesoSobrante;
//        menorVolumenSobrante = volumenSobrante;
//        return true;
//    }
//    return false;
//}
