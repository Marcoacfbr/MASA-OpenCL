void findMaxSmall(__constant uint * const input, __global uint * const output) {
    __local cl_int s_max;
    __local cl_int s_idx;
    __local cl_int response;
   
   s_max = -INF;
   barrier();

	response = atomic_max(&s_max, input[1]);
	if (s_max == input[1]) {
		s_idx = input[0];
	}
    barrier();
    output[0] = (s_idx == input[1]);
}
