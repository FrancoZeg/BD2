#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Definición de tipo abstracto y sus derivados
class tipo {
public:
    virtual int tamaño(int x) const = 0;
    virtual string nombre() const = 0;
};

struct integer : public tipo {
    string nombre() const override { return "Entero"; }
    int tamaño(int x) const override { return x * 4; }
};

struct varchar : public tipo {
    string nombre() const override { return "Cadena de caracteres"; }
    int tamaño(int x) const override { return x + 1; }
};

// Columna de una tabla
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

// Base de datos simulada
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

// Tabla
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

    void mostrar(sf::RenderWindow& window, sf::Font& font) const {
        float y = 50;
        for (const auto& col : columnas) {
            sf::Text header(col.nombre + " (" + col.tipos->nombre() + ")", font, 20);
            header.setPosition(10, y);
            window.draw(header);
            y += 30;

            for (const auto& pair : col.datos) {
                const auto& dato = pair.first;
                const auto& ubicacion = pair.second;

                string data_str;
                for (char c : dato) {
                    data_str += c;
                }
                data_str += " [P:" + to_string(ubicacion[0]) +
                            " S:" + to_string(ubicacion[1]) +
                            " Pi:" + to_string(ubicacion[2]) +
                            " Sec:" + to_string(ubicacion[3]) + "]";

                sf::Text row(data_str, font, 16);
                row.setPosition(20, y);
                window.draw(row);
                y += 20;
            }
            y += 10;
        }
    }
};

// Main con SFML
int main() {
    BD base(1, 2, 3, 4, 512);

    integer int_type;
    varchar varchar_type;

    tabla t("MiTabla");
    t.agregar_columna("ID", &int_type);
    t.agregar_columna("Nombre", &varchar_type);

    t.insertar({{'1'}, {'A', 'n', 'a'}}, base);
    t.insertar({{'2'}, {'B', 'o', 'b'}}, base);

    // Configuración de SFML
    sf::RenderWindow window(sf::VideoMode(800, 600), "Base de Datos Simulada");
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        cerr << "Error al cargar la fuente" << endl;
        return -1;
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);
        t.mostrar(window, font);
        window.display();
    }

    return 0;
}
