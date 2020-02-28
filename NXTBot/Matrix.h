#pragma once

void MatrixMul(float* m1, int m1r, int m1c, float* m2, int m2r, int m2c,
	float* out);

void MatrixTranspose(float* m1, int r, int c, float* out);

bool gluInvertMatrix(float m[16], float invOut[16]);