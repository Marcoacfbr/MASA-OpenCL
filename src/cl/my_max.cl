 void my_max(__constant uint * const input, __global uint * const output) {
    
    output[0] = (input[0]>input[1])?input[0]:input[1];
}