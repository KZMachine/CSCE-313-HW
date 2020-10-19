/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>

using namespace std;

char* commaNumber(int x) {
    int numCounter = 0;
    int totalCounter = 0;
    char y[20] = "";
    for (int i = x; i > 0; i /= 10) {
        y[totalCounter] = (char)(i % 10 + '0');
        numCounter++;
        totalCounter++;
        if ((numCounter % 3) == 0 && i / 10 != 0) {
            y[totalCounter] = ',';
            totalCounter++;
        }
    }

    char* c = new char[totalCounter];
    for (int i = 0; i < totalCounter; i++) {
        c[i] = y[totalCounter - 1 - i];
    }
    return c;
}

int main()
{
    cout << commaNumber(5000000);

    return 0;
}
