void commaAdd(int x) {
    int numCounter = 0;
    int totalCounter = 0;
    char y[20] = "";
    for (int i = x; i > 0; i /= 10) {
        y[totalCounter] = (char)(i % 10 + 48);
        numCounter++;
        totalCounter++;
        if ((numCounter % 3) == 0 && (i / 10) != 0) {
            y[totalCounter] = ',';
            totalCounter++;
        }
    }
    
    char* c = new char[totalCounter];
    for (int i = 0; i < totalCounter; i++) {
        if ((totalCounter - 1 - i) < 0) {
            break;
        }
        c[i] = (char)y[totalCounter - 1 - i];
    }
    for (int i = 0; i < totalCounter; i++) {
        printf("%c", c[i]);
    }
    //return *c;
}