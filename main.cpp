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

	
	void setValor(int i, int j, const T& valor) { _datos[i][j] = valor; }

	// Obtiene un valor en (i,j)
	T getValor(int i, int j) const { return _datos[i][j]; }

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

	// Suma polimórfica, exige que la otra matriz sea dinámica y de mismas dimensiones
	MatrizBase<T>* sumar(const MatrizBase<T>& otra) const override {
		const MatrizDinamica<T>* p = dynamic_cast<const MatrizDinamica<T>*>(&otra);
		if (!p) {
			std::cerr << "[Error] Suma no compatible: solo MatrizDinamica + MatrizDinamica en esta mitad.\n";
			return nullptr;
		}
		if (this->_filas != p->_filas || this->_columnas != p->_columnas) {
			std::cerr << "[Error] Dimensiones incompatibles para suma.\n";
			return nullptr;
		}
		MatrizDinamica<T>* res = new MatrizDinamica<T>(this->_filas, this->_columnas);
		for (int i = 0; i < this->_filas; ++i)
			for (int j = 0; j < this->_columnas; ++j)
				res->setValor(i, j, this->_datos[i][j] + p->_datos[i][j]);
		return res; // Devuelve objeto creado dinámicamente 
	}
};


static void titulo(const char* t) { std::cout << "\n>> " << t << " <<\n\n"; }


int main() {
	std::cout << "--- Sistema genérico de Álgebra Lineal (Primera mitad) ---\n\n";

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

	std::cout << "\nCreando Matriz Dinámica B (3x2)...\nB =\n";
	MatrizBase<float>* B = new MatrizDinamica<float>(3, 2);
	static_cast<MatrizDinamica<float>*>(B)->setValor(0, 0, 0.5f);
	static_cast<MatrizDinamica<float>*>(B)->setValor(0, 1, 1.0f);
	static_cast<MatrizDinamica<float>*>(B)->setValor(1, 0, 2.0f);
	static_cast<MatrizDinamica<float>*>(B)->setValor(1, 1, 3.0f);
	static_cast<MatrizDinamica<float>*>(B)->setValor(2, 0, 1.0f);
	static_cast<MatrizDinamica<float>*>(B)->setValor(2, 1, 1.0f);
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

	std::cout << "\nSistema cerrado.\n";
	return 0;
}

