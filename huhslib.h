#pragma once
#include <gl\glut.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>

using namespace std;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define PI 3.14159265

#define WINDOW_TITLE "Hu's Mountain"

#define BITMAP_ID 0x4D42
#define MAP_X 32
#define MAP_Z 32
#define MAP_SCALE 20.0f

GLfloat P1T[3] = { -2,3,0 };
GLfloat P2T[3] = { -3,0,0 };
GLfloat P3T[3] = { -1,0,3 };
GLfloat P4T[3] = { -4,0,0 };

void normalize(float vec[3]) {
	GLfloat d = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);

	if (d == 0) {
		cerr << "zero length vector" << endl;
		return;
	}

	vec[0] /= d;
	vec[1] /= d;
	vec[2] /= d;
}

void normal_cross_product(float vec1[3], float vec2[3], float out[3]) {
	out[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
	out[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
	out[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
	normalize(out);
}

BITMAPINFOHEADER bitmapInfoHeader;
BITMAPINFOHEADER landInfo;
BITMAPINFOHEADER waterInfo;

unsigned char* imageData;
unsigned char* landTexture;
unsigned int land;

float terrain[MAP_X][MAP_Z][3];

unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader) {
	FILE *fp;
	BITMAPFILEHEADER bitmapFileHeader;
	unsigned char *bitmapImage;
	int imageIdx = 0;
	unsigned char tempRGB;

	fp = fopen(filename, "rb");

	if (fp == NULL)
		return NULL;
	
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);

	if (bitmapFileHeader.bfType != BITMAP_ID) {
		fclose(fp);
		return NULL;
	}

	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	fseek(fp, bitmapFileHeader.bfOffBits, SEEK_SET);

	bitmapImage = (unsigned char *)malloc(bitmapInfoHeader->biSizeImage);

	if (!bitmapImage) {
		free(bitmapImage);
		fclose(fp);
		return NULL;
	}

	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, fp);

	if (bitmapImage == NULL) {
		fclose(fp);
		return NULL;
	}

	for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3) {
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	fclose(fp);
	return bitmapImage;
}

bool LoadTextures(char *filename) {
	landTexture = LoadBitmapFile(filename, &landInfo);
	if (landTexture)
		return false;

	glGenTextures(1, &land);
	glBindTexture(GL_TEXTURE_2D, land);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, landInfo.biHeight, landInfo.biWidth, GL_RGB, GL_UNSIGNED_BYTE, landTexture);

	return true;
}