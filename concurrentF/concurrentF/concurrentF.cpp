// concurrentFinal.cpp : This file contains the 'main' function. Program execution begins and ends there.
//To successfully complete this course you must think up a programming project that could benefit from concurrency and implement it in C++11. 
//You may work individually or in pairs.

//You will be asked to demo and explain in a 10 minute session with your lecturer

//what your program does; how it works; which concurrency facilities of C++11 you used.
//Discuss your project idea before starting to ensure that the project is of sufficient size and complexity.

#include <iostream>
#include "pch.h"
#include "bmp.h"

#include <fstream>
#include <thread> //for threading
#include <complex> // if you make use of complex number facilities in C++
#include <string>
#include <array>
#include <vector>
#include <iterator>

/*
unsigned char* ReadBMP(char* filename)
{
	int i;
	FILE* f = fopen(filename, "rb");

	if (f == NULL)
		throw "Argument Exception";

	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

	// extract image height and width from header
	int width = *(int*)&info[18];
	int height = *(int*)&info[22];

	cout << endl;
	cout << "  Name: " << filename << endl;
	cout << " Width: " << width << endl;
	cout << "Height: " << height << endl;

	int row_padded = (width * 3 + 3) & (~3);
	unsigned char* data = new unsigned char[row_padded];
	unsigned char tmp;

	for (int i = 0; i < height; i++)
	{
		fread(data, sizeof(unsigned char), row_padded, f);
		for (int j = 0; j < width * 3; j += 3)
		{
			// Convert (B, G, R) to (R, G, B)
			tmp = data[j];
			data[j] = data[j + 2];
			data[j + 2] = tmp;

			cout << "R: " << (int)data[j] << " G: " << (int)data[j + 1] << " B: " << (int)data[j + 2] << endl;
		}
	}

	fclose(f);
	return data;
}
*/

int main()
{
	const unsigned imgwidth = 3885;
	const unsigned imgheight = 4000;
	const unsigned halfheight = imgheight / 2;

	// load the file. The constructor now does most of the work
	BitMap example_bmp("shugomap.bmp");

	// get the vector <R,G,B> for the pixel at (1,1)
	std::vector<unsigned int> example_vector = example_bmp.getPixel(100, 100);

	//thread 1
	std::thread t1([example_bmp]() {

		for (unsigned int piX = 0; piX < imgwidth; piX++) {
			for (unsigned int piY = 0; piY < halfheight; piY++) {

				// Get the value of a pixel from myBitmap.
				//Color pixelColor;
				//myBitmap.GetPixel(25, 25, &pixelColor);

				std::vector<unsigned int> example_vector = example_bmp.getPixel(piX, piY);

				//check if pixel is red
				//if (image[piY][piX].r == 255 && image[piY][piX].g == 0 && image[piY][piX].b == 0) {
					//std::cout << "<Xcoord>" << piX << "</Xcoord><Ycoord>" << piY << "</Ycoord><vector>" << example_bmp.getPixel(piX, piY) << "</vector>" << std::endl;
				std::cout << "<Xcoord>" << piX << "</Xcoord><Ycoord>" << piY << "</Ycoord><vector></vector>" << std::endl;
				//}

			}
		}

	});

				//thread 2
		std::thread t2([example_bmp]() {

			for (unsigned int piX = 0; piX < imgwidth; piX++) {
				for (unsigned int piY = halfheight; piY < imgheight; piY++) {

					// Get the value of a pixel from myBitmap.
					//Color pixelColor;
					//myBitmap.GetPixel(25, 25, &pixelColor);

					std::vector<unsigned int> example_vector = example_bmp.getPixel(piX, piY);

					//check if pixel is red
					//if (image[piY][piX].r == 255 && image[piY][piX].g == 0 && image[piY][piX].b == 0) {
					std::cout << "<Xcoord>" << piX << "</Xcoord><Ycoord>" << piY << "</Ycoord><vector></vector>" << std::endl;
					//}

				}
			}
			});
			t1.join();
			t2.join();




}
