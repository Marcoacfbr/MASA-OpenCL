void findMax(__constant uint * const input, __global uint * const output) {
    __local cl_int s_max[THREADS_COUNT];
    __local cl_int s_idx;
    cl_int count = THREADS_COUNT>>1;
    s_max[input[0]] = input[1];
    barrier();

    while (count > 0) {
        if (idx < count) {
            if (s_max[input[0]] < s_max[input[0]+count]) {
                s_max[input[0]] = s_max[input[0]+count];
            }
        }
        count = count >> 1;
        barrier();
    }
    if (s_max[0] == input[1]) {
        s_idx = input[0];
    }
    barrier();
    output[0] = (s_idx == input[0]);
}
