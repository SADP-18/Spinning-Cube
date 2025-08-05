#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

// Constants
#define CUBE_WIDTH 20
#define WIDTH 160
#define HEIGHT 44
#define BACKGROUND_ASCII ' '
#define DISTANCE_FROM_CAM 100
#define K1 40
#define INCREMENT_SPEED 0.6f

// Global variables
float A = 0, B = 0, C = 0;
float zBuffer[WIDTH * HEIGHT];
char buffer[WIDTH * HEIGHT];

// 3D coordinate calculations
float calculateX(int i, int j, int k) {
    return j * sinf(A) * sinf(B) * cosf(C) - k * cosf(A) * sinf(B) * cosf(C) + 
           j * cosf(A) * sinf(C) + k * sinf(A) * sinf(C) + i * cosf(B) * cosf(C);
}

float calculateY(int i, int j, int k) {
    return j * cosf(A) * cosf(C) + k * sinf(A) * cosf(C) -
           j * sinf(A) * sinf(B) * sinf(C) + k * cosf(A) * sinf(B) * sinf(C) - 
           i * cosf(B) * sinf(C);
}

float calculateZ(int i, int j, int k) {
    return k * cosf(A) * cosf(B) - j * sinf(A) * cosf(B) + i * sinf(B);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, char ch) {
    float x = calculateX(cubeX, cubeY, cubeZ);
    float y = calculateY(cubeX, cubeY, cubeZ);
    float z = calculateZ(cubeX, cubeY, cubeZ) + DISTANCE_FROM_CAM;

    if (z <= 0) return;  // Skip if behind camera

    float ooz = 1 / z;
    int xp = (int)(WIDTH/2 - 2 * CUBE_WIDTH + K1 * ooz * x * 2);
    int yp = (int)(HEIGHT/2 + K1 * ooz * y);
    int idx = xp + yp * WIDTH;

    if (idx >= 0 && idx < WIDTH * HEIGHT) {
        if (ooz > zBuffer[idx]) {
            zBuffer[idx] = ooz;
            buffer[idx] = ch;
        }
    }
}

int main() {
    printf("\x1b[2J");  // Clear screen
    
    while(1) {
        // Reset buffers
        memset(buffer, BACKGROUND_ASCII, WIDTH * HEIGHT);
        memset(zBuffer, 0, WIDTH * HEIGHT * sizeof(float));

        // Draw cube surfaces
        for(float cubeX = -CUBE_WIDTH; cubeX < CUBE_WIDTH; cubeX += INCREMENT_SPEED) {
            for(float cubeY = -CUBE_WIDTH; cubeY < CUBE_WIDTH; cubeY += INCREMENT_SPEED) {
                calculateForSurface(cubeX, cubeY, -CUBE_WIDTH, '.');
                calculateForSurface(CUBE_WIDTH, cubeY, cubeX, '$');
                calculateForSurface(-CUBE_WIDTH, cubeY, -cubeX, '~');
                calculateForSurface(-cubeX, cubeY, CUBE_WIDTH, '#');
                calculateForSurface(cubeX, -CUBE_WIDTH, -cubeY, ';');
                calculateForSurface(cubeX, CUBE_WIDTH, cubeY, '+');
            }
        }

        // Render frame
        printf("\x1b[H");  // Move cursor to home position
        for(int k = 0; k < WIDTH * HEIGHT; k++) {
            putchar(k % WIDTH ? buffer[k] : '\n');
        }

        // Rotate cube
        A += 0.005f;
        B += 0.005f;
        usleep(30000);  // 30ms delay (~30 FPS)
    }

    return 0;
}