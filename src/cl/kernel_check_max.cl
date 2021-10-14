void kernel_check_max(__constant uint * const i, __constant uint * const j, __constant uint * const h00, __constant uint * const inc, __constant bool * const absolute_row, _global int * max, _global int * posx, _global int * posy ) {
    if (max[0] < h00[0]) {
		max[0]	 = h00[0];
		posx[0] = j[0];
		if (absolute_row[0]) {
			posy[0] = i[0]+inc[0];
		} else {
			posy[0] = inc[0];
		}
	}
}
    
   

