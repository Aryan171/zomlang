extern "C" double fibonacci(double x) {
    if (x == 0) return 0;
    if (x == 1) return 1;

    int prev2 = 0;
    int prev1 = 1;
    int current = 0;
    
    int i = 2;
    while (i <= x) {
        current = prev1 + prev2;
        prev2 = prev1;
        prev1 = current;
        i++;
    }
    
    return current;
}