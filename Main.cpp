#include <iostream>
#include <string>
#include <vector>

using namespace std;

class tipo {
public:
    virtual int tamaño(int x) const = 0;
    virtual string nombre() const = 0;
};

struct integer : public tipo {
    string nombre() const override { return "Entero"; }
    int tamaño(int x) const override { return x * 4; }
};

struct floating : public tipo {
    string nombre() const override { return "Flotante"; }
    int tamaño(int x) const override { return x * 4 - 1; }
};

struct varchar : public tipo {
    string nombre() const override { return "Cadena de caracteres"; }
    int tamaño(int x) const override { return x + 1; }
};

struct columna {
    string nombre;
    tipo* tipos;
    vector<pair<vector<char>, vector<int>>> datos;

    columna(string nombre_columna, tipo* tipo_dato)
        : nombre(nombre_columna), tipos(tipo_dato) {}

    void insertar(const vector<char>& valor, const vector<int>& ubicacion) {
        datos.push_back({valor, ubicacion});
    }
};

struct BD {
    int platos, superficies, pistas, sectores, bytes;
    int ocupado = 0;
    int plato = 1, superficie = 1, pista = 1, sector = 1;
    int bytes_ocupados_sector = 0;

    BD(int p, int s, int pi, int sec, int b)
        : platos(p), superficies(s), pistas(pi), sectores(sec), bytes(b) {}

    int espacio_disponible() const {
        return platos * superficies * pistas * sectores * bytes - ocupado;
    }

    void asignar_espacio(int tamaño, vector<int>& ubicacion) {
        if (tamaño > espacio_disponible()) {
            cout << "Error: No hay suficiente espacio en la base de datos." << endl;
            return;
        }

        if (bytes_ocupados_sector + tamaño > bytes) {
            sector++;
            bytes_ocupados_sector = 0;

            if (sector > sectores) {
                sector = 1;
                pista++;
                if (pista > pistas) {
                    pista = 1;
                    superficie++;
                    if (superficie > superficies) {
                        superficie = 1;
                        plato++;
                    }
                }
            }
        }

        bytes_ocupados_sector += tamaño;
        ocupado += tamaño;
        ubicacion = {plato, superficie, pista, sector};
    }
};

struct tabla {
    string nombre;
    vector<columna> columnas;

    tabla(string nombre_tabla) : nombre(nombre_tabla) {}

    void agregar_columna(string nombre_columna, tipo* tipos) {
        columnas.emplace_back(nombre_columna, tipos);
    }

    void insertar(const vector<vector<char>>& valores, BD& base) {
        if (valores.size() != columnas.size()) {
            cout << "Error: Número de valores no coincide con las columnas." << endl;
            return;
        }

        for (size_t i = 0; i < columnas.size(); ++i) {
            int espacio = columnas[i].tipos->tamaño(valores[i].size());
            vector<int> ubicacion;
            base.asignar_espacio(espacio, ubicacion);

            if (ubicacion.empty()) {
                cout << "Error: No se pudo asignar espacio para el valor." << endl;
                return;
            }

            columnas[i].insertar(valores[i], ubicacion);
        }
    }

    void mostrar() const {
        for (const auto& col : columnas) {
            cout << col.nombre << " (" << col.tipos->nombre() << "): ";
            for (const auto& [dato, ubicacion] : col.datos) {
                for (char c : dato) {
                    cout << c;
                }
                cout << " [P:" << ubicacion[0]
                     << " S:" << ubicacion[1]
                     << " Pi:" << ubicacion[2]
                     << " Sec:" << ubicacion[3] << "] | ";
            }
            cout << endl;
        }
    }
};

int main() {
    
    BD base(1, 2, 3, 4, 512);

    integer int_type;
    varchar varchar_type;

    tabla t("MiTabla");
    t.agregar_columna("ID", &int_type);
    t.agregar_columna("Nombre", &varchar_type);

    t.insertar({{'1'}, {'A', 'n', 'a'}}, base);
    t.insertar({{'2'}, {'B', 'o', 'b'}}, base);

    t.mostrar();

    return 0;
}