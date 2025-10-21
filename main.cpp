#include <iostream>   
#include <iomanip>    
#include <type_traits>

// Clase base abstracta que define la interfaz común para cualquier matriz
template <typename T>
class MatrizBase {
protected:
	int _filas;   // Cantidad de filas
	int _columnas;// Cantidad de columnas

public:
	
	MatrizBase(int filas, int columnas) : _filas(filas), _columnas(columnas) {}

	// Destructor virtual para garantizar destrucción polimórfica
	virtual ~MatrizBase() {}

	
	virtual void cargarValores() = 0;

	
	virtual MatrizBase<T>* sumar(const MatrizBase<T>& otra) const = 0;

	
	virtual void imprimir() const = 0;

	// Acceso genérico a elementos para permitir suma cruzada
	virtual T getValor(int i, int j) const = 0;
	virtual void setValor(int i, int j, const T& valor) = 0;

	
	MatrizBase<T>* operator+(const MatrizBase<T>& otra) const { return this->sumar(otra); }

	
	int filas() const { return _filas; }
	int columnas() const { return _columnas; }
};


template <typename T>
class MatrizDinamica : public MatrizBase<T> {
private:
	T** _datos; // Arreglo de punteros a filas


	void reservar() {
		_datos = new T*[this->_filas];
		for (int i = 0; i < this->_filas; ++i) {
			_datos[i] = new T[this->_columnas];
			for (int j = 0; j < this->_columnas; ++j) _datos[i][j] = T();
		}
	}

	// Libera toda la memoria reservada de forma segura
	void liberar() {
		if (_datos) {
			for (int i = 0; i < this->_filas; ++i) {
				delete[] _datos[i];
			}
			delete[] _datos;
			_datos = nullptr;
		}
	}

public:
	// Constructor principal que asigna memoria según las dimensiones
	MatrizDinamica(int filas, int columnas)
		: MatrizBase<T>(filas, columnas), _datos(nullptr) {
		reservar();
	}

	// Destructor que garantiza la liberación de memoria 
	~MatrizDinamica() override {
		
		liberar();
	}

	// Constructor de copia
	MatrizDinamica(const MatrizDinamica& otra)
		: MatrizBase<T>(otra._filas, otra._columnas), _datos(nullptr) {
		reservar();
		for (int i = 0; i < this->_filas; ++i)
			for (int j = 0; j < this->_columnas; ++j)
				_datos[i][j] = otra._datos[i][j];
	}

	// Constructor de movimiento 
	MatrizDinamica(MatrizDinamica&& otra) noexcept
		: MatrizBase<T>(otra._filas, otra._columnas), _datos(otra._datos) {
		otra._datos = nullptr;
		otra._filas = 0;
		otra._columnas = 0;
	}

	// Operador de asignación por copia
	MatrizDinamica& operator=(const MatrizDinamica& otra) {
		if (this == &otra) return *this;
		if (this->_filas != otra._filas || this->_columnas != otra._columnas) {
			liberar();
			this->_filas = otra._filas;
			this->_columnas = otra._columnas;
			reservar();
		}
		for (int i = 0; i < this->_filas; ++i)
			for (int j = 0; j < this->_columnas; ++j)
				_datos[i][j] = otra._datos[i][j];
		return *this;
	}

	// Operador de asignación por movimiento (libera lo actual y roba recursos)
	MatrizDinamica& operator=(MatrizDinamica&& otra) noexcept {
		if (this == &otra) return *this;
		liberar();
		this->_filas = otra._filas;
		this->_columnas = otra._columnas;
		_datos = otra._datos;
		otra._datos = nullptr;
		otra._filas = 0;
		otra._columnas = 0;
		return *this;
	}

	
	void setValor(int i, int j, const T& valor) override { _datos[i][j] = valor; }

	// Obtiene un valor en (i,j)
	T getValor(int i, int j) const override { return _datos[i][j]; }

	// Carga interactiva desde cin (mantiene el contrato, aunque no se usa en la demo)
	void cargarValores() override {
		for (int i = 0; i < this->_filas; ++i) {
			for (int j = 0; j < this->_columnas; ++j) {
				T v{};
				std::cin >> v;
				_datos[i][j] = v;
			}
		}
	}

	// Impresión con formato tipo tabla (ajusta precisión si el tipo es flotante)
	void imprimir() const override {
		bool esFloat = std::is_floating_point<T>::value;
		if (esFloat) std::cout << std::fixed << std::setprecision(1);
		for (int i = 0; i < this->_filas; ++i) {
			std::cout << "| ";
			for (int j = 0; j < this->_columnas; ++j) {
				std::cout << _datos[i][j] << " | ";
			}
			std::cout << '\n';
		}
		if (esFloat) std::cout.unsetf(std::ios::floatfield);
	}

	// Suma polimórfica: acepta cualquier derivada con mismas dimensiones y devuelve MatrizDinamica
	MatrizBase<T>* sumar(const MatrizBase<T>& otra) const override {
		if (this->_filas != otra.filas() || this->_columnas != otra.columnas()) {
			std::cerr << "[Error] Dimensiones incompatibles para suma.\n";
			return nullptr;
		}
		MatrizDinamica<T>* res = new MatrizDinamica<T>(this->_filas, this->_columnas);
		for (int i = 0; i < this->_filas; ++i)
			for (int j = 0; j < this->_columnas; ++j)
				res->setValor(i, j, this->_datos[i][j] + otra.getValor(i, j));
		return res; // Devuelve objeto creado dinámicamente 
	}
};


// Matriz estática con dimensiones fijas en tiempo de compilación y arreglo interno
template <typename T, int M, int N>
class MatrizEstatica : public MatrizBase<T> {
private:
	T _datos[M][N]; // Almacenamiento fijo

public:
	// Constructor que fija dimensiones y limpia a valor por defecto
	MatrizEstatica() : MatrizBase<T>(M, N), _datos{} {}

	// Carga interactiva desde cin
	void cargarValores() override {
		for (int i = 0; i < M; ++i) {
			for (int j = 0; j < N; ++j) {
				T v{}; std::cin >> v; _datos[i][j] = v;
			}
		}
	}

	// Impresión con formato tabla
	void imprimir() const override {
		bool esFloat = std::is_floating_point<T>::value;
		if (esFloat) std::cout << std::fixed << std::setprecision(1);
		for (int i = 0; i < M; ++i) {
			std::cout << "| ";
			for (int j = 0; j < N; ++j) {
				std::cout << _datos[i][j] << " | ";
			}
			std::cout << '\n';
		}
		if (esFloat) std::cout.unsetf(std::ios::floatfield);
	}

	// Acceso a elementos
	T getValor(int i, int j) const override { return _datos[i][j]; }
	void setValor(int i, int j, const T& valor) override { _datos[i][j] = valor; }

	// Suma polimórfica
	MatrizBase<T>* sumar(const MatrizBase<T>& otra) const override {
		if (this->_filas != otra.filas() || this->_columnas != otra.columnas()) {
			std::cerr << "[Error] Dimensiones incompatibles para suma.\n";
			return nullptr;
		}
		MatrizEstatica<T, M, N>* res = new MatrizEstatica<T, M, N>();
		for (int i = 0; i < M; ++i)
			for (int j = 0; j < N; ++j)
				res->setValor(i, j, _datos[i][j] + otra.getValor(i, j));
		return res; // Devuelve objeto creado de la misma clase (estática)
	}
};


static void titulo(const char* t) { std::cout << "\n>> " << t << " <<\n\n"; }


int main() {
	std::cout << "--- Sistema genérico de Álgebra Lineal ---\n\n";

	// Demostración con tipo float (usa valores del ejemplo, pero ambas matrices son dinámicas)
	titulo("Demostración de Genericidad (Tipo FLOAT)");
	std::cout << "Creando Matriz Dinámica A (3x2)...\nA =\n";
	MatrizBase<float>* A = new MatrizDinamica<float>(3, 2);
	// Carga manual de valores de A según el ejemplo
	static_cast<MatrizDinamica<float>*>(A)->setValor(0, 0, 1.5f);
	static_cast<MatrizDinamica<float>*>(A)->setValor(0, 1, 2.0f);
	static_cast<MatrizDinamica<float>*>(A)->setValor(1, 0, 0.0f);
	static_cast<MatrizDinamica<float>*>(A)->setValor(1, 1, 1.0f);
	static_cast<MatrizDinamica<float>*>(A)->setValor(2, 0, 4.5f);
	static_cast<MatrizDinamica<float>*>(A)->setValor(2, 1, 3.0f);
	A->imprimir();

	std::cout << "\nCreando Matriz Estática B (3x2)...\nB =\n";
	MatrizBase<float>* B = new MatrizEstatica<float, 3, 2>();
	static_cast<MatrizEstatica<float, 3, 2>*>(B)->setValor(0, 0, 0.5f);
	static_cast<MatrizEstatica<float, 3, 2>*>(B)->setValor(0, 1, 1.0f);
	static_cast<MatrizEstatica<float, 3, 2>*>(B)->setValor(1, 0, 2.0f);
	static_cast<MatrizEstatica<float, 3, 2>*>(B)->setValor(1, 1, 3.0f);
	static_cast<MatrizEstatica<float, 3, 2>*>(B)->setValor(2, 0, 1.0f);
	static_cast<MatrizEstatica<float, 3, 2>*>(B)->setValor(2, 1, 1.0f);
	B->imprimir();

	std::cout << "\nSUMANDO: Matriz C = A + B ...\n";
	MatrizBase<float>* C = (*A + *B);
	if (C) {
		std::cout << "\nMatriz Resultado C (3x2, Tipo FLOAT):\n";
		C->imprimir();
	}

	
	std::cout << "\n>> Demostración de Limpieza de Memoria <<\n";
	std::cout << "Liberando C...\n";
	delete C;
	std::cout << "Liberando B...\n";
	delete B;
	std::cout << "Liberando A...\n";
	delete A;

	
	titulo("Demostración de Genericidad (Tipo INT)");
	MatrizBase<int>* X = new MatrizDinamica<int>(2, 3);
	MatrizBase<int>* Y = new MatrizDinamica<int>(2, 3);
	// Carga manual de X
	static_cast<MatrizDinamica<int>*>(X)->setValor(0, 0, 1);
	static_cast<MatrizDinamica<int>*>(X)->setValor(0, 1, 2);
	static_cast<MatrizDinamica<int>*>(X)->setValor(0, 2, 3);
	static_cast<MatrizDinamica<int>*>(X)->setValor(1, 0, 4);
	static_cast<MatrizDinamica<int>*>(X)->setValor(1, 1, 5);
	static_cast<MatrizDinamica<int>*>(X)->setValor(1, 2, 6);
	std::cout << "X =\n"; X->imprimir();
	// Carga manual de Y
	static_cast<MatrizDinamica<int>*>(Y)->setValor(0, 0, 6);
	static_cast<MatrizDinamica<int>*>(Y)->setValor(0, 1, 5);
	static_cast<MatrizDinamica<int>*>(Y)->setValor(0, 2, 4);
	static_cast<MatrizDinamica<int>*>(Y)->setValor(1, 0, 3);
	static_cast<MatrizDinamica<int>*>(Y)->setValor(1, 1, 2);
	static_cast<MatrizDinamica<int>*>(Y)->setValor(1, 2, 1);
	std::cout << "Y =\n"; Y->imprimir();
	// Suma y resultado
	MatrizBase<int>* Z = (*X + *Y);
	std::cout << "\nZ = X + Y\n";
	if (Z) Z->imprimir();
	// Limpieza
	delete Z; delete Y; delete X;

	// Suma Estática + Estática (3x2) en float
	titulo("Suma Estática + Estática (FLOAT)");
	MatrizBase<float>* E1 = new MatrizEstatica<float, 3, 2>();
	MatrizBase<float>* E2 = new MatrizEstatica<float, 3, 2>();
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 2; ++j) {
			static_cast<MatrizEstatica<float, 3, 2>*>(E1)->setValor(i, j, 1.0f);
			static_cast<MatrizEstatica<float, 3, 2>*>(E2)->setValor(i, j, 2.0f);
		}
	}
	std::cout << "E1 =\n"; E1->imprimir();
	std::cout << "E2 =\n"; E2->imprimir();
	MatrizBase<float>* E3 = (*E1 + *E2);
	std::cout << "\nE3 = E1 + E2\n"; if (E3) E3->imprimir();
	delete E3; delete E2; delete E1;

	// Suma Estática + Dinámica (2x2) en int
	titulo("Suma Estática + Dinámica (INT)");
	MatrizBase<int>* SD_S = new MatrizEstatica<int, 2, 2>();
	MatrizBase<int>* SD_D = new MatrizDinamica<int>(2, 2);
	static_cast<MatrizEstatica<int, 2, 2>*>(SD_S)->setValor(0, 0, 1);
	static_cast<MatrizEstatica<int, 2, 2>*>(SD_S)->setValor(0, 1, 2);
	static_cast<MatrizEstatica<int, 2, 2>*>(SD_S)->setValor(1, 0, 3);
	static_cast<MatrizEstatica<int, 2, 2>*>(SD_S)->setValor(1, 1, 4);
	static_cast<MatrizDinamica<int>*>(SD_D)->setValor(0, 0, 4);
	static_cast<MatrizDinamica<int>*>(SD_D)->setValor(0, 1, 3);
	static_cast<MatrizDinamica<int>*>(SD_D)->setValor(1, 0, 2);
	static_cast<MatrizDinamica<int>*>(SD_D)->setValor(1, 1, 1);
	std::cout << "S (estática) =\n"; SD_S->imprimir();
	std::cout << "D (dinámica) =\n"; SD_D->imprimir();
	MatrizBase<int>* SD_R1 = (*SD_S + *SD_D);
	MatrizBase<int>* SD_R2 = (*SD_D + *SD_S);
	std::cout << "\nR1 = S + D (Resultado Estático)\n"; if (SD_R1) SD_R1->imprimir();
	std::cout << "\nR2 = D + S (Resultado Dinámico)\n"; if (SD_R2) SD_R2->imprimir();
	delete SD_R2; delete SD_R1; delete SD_D; delete SD_S;

	std::cout << "\nSistema cerrado.\n";
	return 0;
}

