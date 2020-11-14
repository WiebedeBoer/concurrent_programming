// concurrent2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//Extend the Mandelbrot program you created in the previous exercise with multithreading. 
//Let each thread calculate a part of the Mandelbrot set.
//compile with: g++ -std=c++11 -pthread concurrent2.cpp -o

#include "pch.h"
#include <complex>
#include <thread> //for threading
#include <iostream>

// mandelbrot.cpp
// compile with: g++ -std=c++11 mandelbrot.cpp -o mandelbrot
// view output with: eog mandelbrot.ppm

#include <fstream>
#include <complex> // if you make use of complex number facilities in C++


template <class T> struct RGB { T r, g, b; };

template <class T>
class Matrix {
public:
	Matrix(const size_t rows, const size_t cols) : _rows(rows), _cols(cols) {
		_matrix = new T*[rows];
		for (size_t i = 0; i < rows; ++i) {
			_matrix[i] = new T[cols];
		}
	}
	Matrix(const Matrix &m) : _rows(m._rows), _cols(m._cols) {
		_matrix = new T*[m._rows];
		for (size_t i = 0; i < m._rows; ++i) {
			_matrix[i] = new T[m._cols];
			for (size_t j = 0; j < m._cols; ++j) {
				_matrix[i][j] = m._matrix[i][j];
			}
		}
	}
	~Matrix() {
		for (size_t i = 0; i < _rows; ++i) {
			delete[] _matrix[i];
		}
		delete[] _matrix;
	}
	T *operator[] (const size_t nIndex)
	{
		return _matrix[nIndex];
	}
	size_t width() const { return _cols; }
	size_t height() const { return _rows; }
protected:
	size_t _rows, _cols;
	T **_matrix;
};

// Portable PixMap image
class PPMImage : public Matrix<RGB<unsigned char> >
{
public:
	PPMImage(const size_t height, const size_t width) : Matrix(height, width) { }
	void save(const std::string &filename)
	{
		std::ofstream out(filename, std::ios_base::binary);
		out << "P6" << std::endl << _cols << " " << _rows << std::endl << 255 << std::endl;
		for (size_t y = 0; y < _rows; y++)
			for (size_t x = 0; x < _cols; x++)
				out << _matrix[y][x].r << _matrix[y][x].g << _matrix[y][x].b;
	}
};

const unsigned width = 1600;
const unsigned height = 1600;
const unsigned int max_ite = 10000;
PPMImage image(height, width);
const unsigned halfheight = 800;


//first thr upper half
void thr1() {

	for (unsigned int piX = 0; piX < width; piX++) {
		for (unsigned int piY = 0; piY < halfheight; piY++) {
			double x0 = (piX - width / 2.0)*4.0 / width;
			double y0 = (piY - height / 2.0)*4.0 / width;
			double x = 0, y = 0;
			int i = 0;
			while (x*x + y * y <= 4 && i < max_ite) {
				double xt = x * x - y * y + x0;
				y = 2 * x*y + y0;
				x = xt;
				i++;
			}
			if (i < max_ite) {
				image[piY][piX].r = image[piY][piX].g = image[piY][piX].b = 255; //white
			}
			else {
				image[piY][piX].r = image[piY][piX].g = image[piY][piX].b = 0; //black
			}
		}
	}


}

//second thr lower half
void thr2() {

	for (unsigned int piX = 0; piX < width; piX++) {
		for (unsigned int piY = halfheight; piY < height; piY++) {
			double x0 = (piX - width / 2.0)*4.0 / width;
			double y0 = (piY - height / 2.0)*4.0 / width;
			double x = 0, y = 0;
			int i = 0;
			while (x*x + y * y <= 4 && i < max_ite) {
				double xt = x * x - y * y + x0;
				y = 2 * x*y + y0;
				x = xt;
				i++;
			}
			if (i < max_ite) {
				image[piY][piX].r = image[piY][piX].g = image[piY][piX].b = 255; //white
			}
			else {
				image[piY][piX].r = image[piY][piX].g = image[piY][piX].b = 0; //black
			}
		}
	}

}


//constexpr auto max_row = 22, max_column = 78, max_iteration = 20;
int x;
int y;
int main()
{
	//const unsigned width = 1600;
	//const unsigned height = 1600;
	//const unsigned halfheight = 800;
	//PPMImage image(height, width);
	//const unsigned int max_ite = 10000;
	
	std::thread t1(thr1);
	std::thread t2(thr2);

	/*
	std::thread t1([image]() {
		for (unsigned int piX = 0; piX < width; piX++) {
			for (unsigned int piY = 0; piY < halfheight; piY++) {
				double x0 = (piX - width / 2.0)*4.0 / width;
				double y0 = (piY - height / 2.0)*4.0 / width;
				double x = 0, y = 0;
				int i = 0;
				while (x*x + y * y <= 4 && i < max_ite) {
					double xt = x * x - y * y + x0;
					y = 2 * x*y + y0;
					x = xt;
					i++;
				}
				if (i < max_ite) {
					image[piY][piX].r = image[piY][piX].g = image[piY][piX].b = 255; //white
				}
				else {
					image[piY][piX].r = image[piY][piX].g = image[piY][piX].b = 0; //black
				}
			}
		}
	});
	std::thread t2([image]() {
		for (unsigned int piX = 0; piX < width; piX++) {
			for (unsigned int piY = halfheight; piY < height; piY++) {
				double x0 = (piX - width / 2.0)*4.0 / width;
				double y0 = (piY - height / 2.0)*4.0 / width;
				double x = 0, y = 0;
				int i = 0;
				while (x*x + y * y <= 4 && i < max_ite) {
					double xt = x * x - y * y + x0;
					y = 2 * x*y + y0;
					x = xt;
					i++;
				}
				if (i < max_ite) {
					image[piY][piX].r = image[piY][piX].g = image[piY][piX].b = 255; //white
				}
				else {
					image[piY][piX].r = image[piY][piX].g = image[piY][piX].b = 0; //black
				}
			}
		}
	});
	*/
	t1.join();
	t2.join();

	image.save("mandelbrot.ppm");

	/*
	const unsigned width = 1600;
	const unsigned height = 1600;
	PPMImage image(height, width);
	int i;
	int j;
	//int *count;
	int *count;
	count = new int[width*height];
	int c;
	int c_max = 0;
	//first height
	for (i = 0; i < height; i++)
	{
		//then width
		for (j = 0; j < width; j++)
		{
			if (count[i + j * width] % 2 == 1)
			{
				image[y][x].r = image[y][x].g = image[y][x].b = 255; // white pixel
			}
			else {
				//image[y][x].r = image[y][x].g = image[y][x][b] = 0; // black pixel
				//image[y][x].r = image[y][x].g = image[y][x].b = 0; // black pixel

				// red pixel
				//image[y][x].r = 255;
				//image[y][x].g = 0;
				//image[y][x].b = 0;

				c = (int)(255.0 * sqrt(sqrt(sqrt(((double)(count[i + j * height]) / (double)(c_max))))));
				//image[y][x].r[i + j * height] = 3 * c / 5;
				//image[y][x].g[i + j * height] = 3 * c / 5;
				//image[y][x].b[i + j * height] = c;
				image[y][x].r = 3 * c / 5;
				image[y][x].g = 3 * c / 5;
				image[y][x].b = c;
			}
		}
	}
	*/

	//image.save("mandelbrot.ppm");


	/*
	for (auto row = 0; row < max_row; ++row) {
		for (auto column = 0; column < max_column; ++column) {
			std::complex<float> z, c = {
				(float)column * 2 / max_column - 1.5f,
				(float)row * 2 / max_row - 1
			};
			int iteration = 0;
			while (abs(z) < 2 && ++iteration < max_iteration)
				z = pow(z, 2) + c;
			std::cout << (iteration == max_iteration ? '#' : '.');
		}
		std::cout << '\n';
	}
	*/


	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
