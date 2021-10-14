void get_flush_id(__constant uint * const i, __global uint * const output) {
    if (i[2] < i[0] || i[2] >= i[1]) {
    	output[0] = 0;
    } else {
    	output[0] = (i[2]+ALPHA < i[1]) ? -1 : ((i[1]-i[0]-1)%ALPHA)+1;
    }
}
