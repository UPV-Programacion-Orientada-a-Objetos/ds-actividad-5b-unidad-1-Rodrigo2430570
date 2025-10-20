[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/s4DYBMK-)
[![Open in Codespaces](https://classroom.github.com/assets/launch-codespace-2972f46106e565e64193e422d61a12cf1da4916b45550586e14ef0a7c637dd04.svg)](https://classroom.github.com/open-in-codespaces?assignment_repo_id=21181022)
# Actividad Práctica: Sistema Genérico de Álgebra Lineal con Jerarquía de Matrices

## Definición del Problema a Resolver

Una empresa de software matemático (MathSoft) necesita construir el core de su módulo de álgebra lineal. El sistema debe ser capaz de manejar diferentes tipos de estructuras de datos matriciales (estáticas y dinámicas) bajo una jerarquía de clases unificada, garantizando que todas las operaciones fundamentales (como la suma) se realicen de manera genérica, independientemente del tipo numérico (`int`, `float`, `double`).

El objetivo es crear un sistema jerárquico que obligue al uso de clases abstractas para definir el comportamiento de cualquier matriz y templates para asegurar la reutilización del código.

## Temas Relacionados y Necesarios

Para resolver este problema, los alumnos deberán integrar los siguientes conocimientos:

| Tema Principal                         | Concepto a Aplicar                                                                                                                                                                                                                                                             |
| :------------------------------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| POO Avanzada                           | Diseño de una Clase Base Abstracta (`MatrizBase<T>`) y el uso de métodos virtuales puros para polimorfismo.                                                                                                                                                                   |
| Genericidad (Templates)                | Uso de `typename T` en todas las clases y métodos para aceptar cualquier tipo numérico.                                                                                                                                                                                        |
| Matrices Dinámicas con Punteros        | Implementación de `MatrizDinamica<T>` con punteros dobles (`T**`) y gestión manual de la memoria (`new`/`delete[]`).                                                                                                                                                         |
| Matrices Estáticas                     | Implementación de `MatrizEstatica<T, M, N>` usando arreglos nativos fijos (`T datos[M][N]`).                                                                                                                                                                                |
| Vectores Dinámicos/Estáticos           | Uso de vectores estáticos internos para almacenar dimensiones fijas, y punteros para estructuras dinámicas auxiliares.                                                                                                                                                       |
| Memoria Manual                         | Implementación de la Regla de los Cinco (Destructor, Constructor de Copia, Operador de Asignación, etc.) en las clases que usan punteros.                                                                                                                                   |

## Definición y Detalles del Proceso a Desarrollar

### Diseño de la Jerarquía de Clases (POO)

El sistema debe basarse en la siguiente jerarquía:

* **Clase Base Abstracta:**

    ```c++
    template <typename T>
    class MatrizBase {
    protected:
        int _filas;
        int _columnas;

    public:
        virtual void cargarValores() = 0;
        virtual MatrizBase<T>* sumar(const MatrizBase<T>& otra) const = 0;
        virtual void imprimir() const = 0;
    };
    ```

* **Clases Concretas (Implementaciones):**

    ```c++
    template <typename T>
    class MatrizDinamica : public MatrizBase<T> {
        T **_datos;
    };

    template <typename T, int M, int N>
    class MatrizEstatica : public MatrizBase<T> {
        T _datos[M][N];
    };
    ```

### Proceso de Solución Detallado

1. **Implementación de la Matriz Dinámica:**
    * **Constructor:** Asignar memoria para `T**` y sus filas.
    * **Destructor y Regla de los Cinco:** Implementar el destructor para liberar la memoria correctamente. Implementar el Constructor de Copia y el Operador de Asignación para realizar una copia profunda (deep copy) de la matriz de punteros.
2. **Implementación de la Matriz Estática:**
    * El constructor solo debe inicializar los miembros. El manejo de memoria es automático, pero debe cumplir con la interfaz de `MatrizBase`.
3. **Operación Central (Suma Polimórfica):**
    * El método `sumar` debe estar definido en la clase base como virtual puro.
    * La implementación en las clases derivadas (`MatrizDinamica` y `MatrizEstatica`) debe:
        * Verificar que las dimensiones sean compatibles.
        * Crear y devolver un nuevo objeto dinámico de la misma clase que la que realiza la operación (ej., si se suma una `MatrizDinamica` con otra, el resultado debe ser una nueva `MatrizDinamica`), utilizando `new`.
4. **Uso en `main`:**
    * El programa principal debe demostrar el polimorfismo instanciando punteros a la clase base (`MatrizBase<float>*`) y asignándoles objetos de las clases derivadas.

## Requerimientos Funcionales y No Funcionales

### Requisitos Funcionales

* **Jerarquía POO Completa:** Implementar la clase abstracta `MatrizBase` y las dos clases concretas (`MatrizDinamica` y `MatrizEstatica`).
* **Operación Polimórfica:** El programa debe ser capaz de sumar dos matrices, donde ambas matrices están siendo referenciadas por punteros a la clase base (`MatrizBase<T>*`).
* **Genericidad:** Demostrar la funcionalidad completa (creación y suma) para los tipos `int` y `float`.
* **Gestión Dinámica:** La `MatrizDinamica` debe manejar la asignación y liberación completa de sus datos mediante punteros.
* **Sibrecarga de operadores**: Se deberá realizar la carga del operador `+` para realizar la suma de matrices de manera natural.

### Requisitos No Funcionales

* **Exclusividad de Punteros:** Prohibido el uso de la STL (`std::vector`, `std::array`). Toda estructura variable debe usar `new` y `delete`.
* **Implementación Manual:** La suma, carga y manipulación de datos debe hacerse mediante lógica de punteros y ciclos manuales.
* **Dificultad Alta:** La implementación de la Regla de los Cinco con templates y punteros en la clase dinámica es obligatoria.
* **Modularidad:** Las clases deben ser auto-suficientes y cumplir con la interfaz.

---

## Ejemplo de Entradas y Salidas del Problema en Consola

### Simulación de Interacción

```Bash
--- Sistema gérico de Álgebra Lineal ---

>> Demostración de Genericidad (Tipo FLOAT) <<

// 1. Creación de Matriz Dinámica (a través del puntero base)
Creando Matriz Dinámica A (3x2)...
A =
| 1.5 | 2.0 |
| 0.0 | 1.0 |
| 4.5 | 3.0 |

// 2. Creación de Matriz Estática (a través del puntero base)
Creando Matriz Estática B (3x2)...
B =
| 0.5 | 1.0 |
| 2.0 | 3.0 |
| 1.0 | 1.0 |

// 3. Operación Polimórfica (Suma)
SUMANDO: Matriz C = A + B ...
(La suma es manejada por el método virtual de MatrizDinamica)

Matriz Resultado C (3x2, Tipo FLOAT):
| 2.0 | 3.0 |
| 2.0 | 4.0 |
| 5.5 | 4.0 |

>> Demostración de Limpieza de Memoria <<
Llamando al destructor de C...
Liberando memoria de C (Matriz Dinámica)...
Llamando al destructor de A...
Liberando memoria de A (Matriz Dinámica)...
Sistema cerrado.
```
---

## Temas Adicionales de Investigación Necesarios

Para llevar a cabo esta actividad con éxito, el alumno deberá investigar a fondo:

*   **Regla de los Cinco y Templates:** El mecanismo exacto para implementar el Constructor de Copia y el Operador de Asignación dentro de una clase template que usa punteros, asegurando que se realice una copia profunda (deep copy).
*   **Manejo de Punteros en el Polimorfismo:** Entender por qué el método `sumar` debe devolver un puntero a la clase base (`MatrizBase<T>*`) y por qué es necesario usar el operador `new` dentro de la implementación de `sumar` para construir el objeto resultante.
*   **Sintaxis de Clases Anidadas y Amigas:** Si se elige hacer la clase `MatrizBase` amiga de las clases derivadas para un acceso más limpio a los atributos protegidos.
*   **Conversión de Tipos Genéricos:** Asegurar que las operaciones matemáticas (`+`, `-`, `*`) funcionen correctamente con el tipo genérico `T`.

