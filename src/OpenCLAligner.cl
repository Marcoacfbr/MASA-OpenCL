#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

#ifndef THREADS_COUNT
#define THREADS_COUNT 64
#endif

#define MAX_BLOCKS_COUNT    512

#define ALPHA (4)

#define MAX_GRID_HEIGHT     (THREADS_COUNT*MAX_BLOCKS_COUNT)

#define DNA_MATCH       (1)

#define DNA_MISMATCH    (-3)

#define DNA_GAP_OPEN    (3)

#define DNA_GAP_EXT     (2)

#define DNA_GAP_FIRST   (DNA_GAP_EXT+DNA_GAP_OPEN)

#define INF (999999999)



// ******************************************************************************* //

int cl_findMax(int idx, int val) {
    local int s_max[THREADS_COUNT];
    local int s_idx;
    int count = THREADS_COUNT>>1;
    s_max[idx] = val;
    //barrier(CLK_GLOBAL_MEM_FENCE);
    barrier(CLK_LOCAL_MEM_FENCE);

    while (count > 0) {
        if (idx < count) {
            if (s_max[idx] < s_max[idx+count]) {
                s_max[idx] = s_max[idx+count];
            }
        }
        count = count >> 1;
        //barrier(CLK_GLOBAL_MEM_FENCE);
		barrier(CLK_LOCAL_MEM_FENCE);
    }
    if (s_max[0] == val) {
        s_idx = idx;
    }
    //barrier(CLK_GLOBAL_MEM_FENCE);
    barrier(CLK_LOCAL_MEM_FENCE);
    return (s_idx == idx);
}

// ******************************************************************************* //

int cl_findMaxSmall(int idx, int val) {
	local int s_max;
	local int s_idx;
	local int response;

	s_max = -INF;
	//barrier(CLK_GLOBAL_MEM_FENCE);
	barrier(CLK_LOCAL_MEM_FENCE);

   	response = atomic_max(&s_max, val);
	if (s_max == val) {
		s_idx = idx;
	}
    //barrier(CLK_GLOBAL_MEM_FENCE);
    barrier(CLK_LOCAL_MEM_FENCE);
    return (s_idx == idx);
}

// ******************************************************************************* //

static int cl_get_flush_id(const int i, const int i0, const int i1) {
    //return (-1);
    if (i < i0 || i >= i1) {
    	return 0;
    } else {
    	return (i+ALPHA < i1) ? -1 : ((i1-i0-1)%ALPHA)+1;
    }
}

// ******************************************************************************* //

static int my_max(int a, int b) {
    return (a>b)?a:b;
}

// ******************************************************************************* //

static int my_max4(int a, int b, int c, int d) {
    return my_max(my_max(a,b), my_max(c,d));
}

// ******************************************************************************* //

static void cl_fetchSeq0(
	const int i,  
	uchar4* ss,  
	__global char* t_seq0) {

	if (i >= 0) {
        ss->x = t_seq0[i];
        ss->y = t_seq0[i+1];
        ss->z = t_seq0[i+2];
        ss->w = t_seq0[i+3];
	}
}


// ******************************************************************************* //

void cl_kernel_sw(uchar sa, uchar sb,  int * e00,  int * f00, const int h01, const int h11, const int h10,  int * h00) {

    *e00 = my_max(h01-DNA_GAP_OPEN, *e00)-DNA_GAP_EXT; // Horizontal propagation
    *f00 = my_max(h10-DNA_GAP_OPEN, *f00)-DNA_GAP_EXT; // Vertical propagation
    int v1 = h11+((sb!=sa)?DNA_MISMATCH:DNA_MATCH);

    *h00 = my_max4(0, v1, *e00, *f00);
}

// ******************************************************************************* //

void cl_kernel_check_max(const int i, const int j,  int *max,
		 int2 *pos, const int h00, const int inc, const bool absolute_row) {
	if (*max < h00) {
		*max = h00;
		pos->x = j;
		if (absolute_row) {
			pos->y = i+inc;
		} else {
			pos->y = inc;
		}
	}
}

// ******************************************************************************* //

void cl_kernel_sw4(const int i, const int j, uchar si, uchar4 ss,  int4 *left_e,  int *up_f, const int4 left_h, const int diag_h, const int up_h,  int4 *curr_h, const int flush_id) {
        
       int * leftx, * lefty, * leftz, * leftw;
       int * currx, * curry, * currz, * currw;

       leftx = (int *) left_e;
       lefty = (int *) left_e + 1;
       leftz = (int *) left_e + 2;
       leftw = (int *) left_e + 3;
       
       
	   currx = (int *) curr_h;
       curry = (int *) curr_h + 1;
       currz = (int *) curr_h + 2;
       currw = (int *) curr_h + 3;

       
    cl_kernel_sw(si, ss.x, leftx, up_f, left_h.x, diag_h, up_h, currx);
	cl_kernel_sw(si, ss.y, lefty, up_f, left_h.y, left_h.x, curr_h->x, curry);
	cl_kernel_sw(si, ss.z, leftz, up_f, left_h.z, left_h.y, curr_h->y, currz);
	cl_kernel_sw(si, ss.w, leftw, up_f, left_h.w, left_h.z, curr_h->z, currw);
}


// ******************************************************************************* //

void cl_kernel_check_max4(const int i, const int j, int *max,  int2 *max_pos, int4 *curr_h, const bool absolute_row, const int flush_id) {
		cl_kernel_check_max(i, j, max, max_pos, curr_h->x, 0, absolute_row);
		cl_kernel_check_max(i, j, max, max_pos, curr_h->y, 1, absolute_row);
		cl_kernel_check_max(i, j, max, max_pos, curr_h->z, 2, absolute_row);
		cl_kernel_check_max(i, j, max, max_pos, curr_h->w, 3, absolute_row);
}

// ******************************************************************************* //

void cl_kernel_load(
	const int idx, 
	const int bank, 
	const int j,  
	__global int2* busH,  
	int *h,  
	int *f,  
	unsigned char *s,  
	__global char* t_seq1,  
	__local int s_colx[][THREADS_COUNT],  
	__local int s_coly[][THREADS_COUNT]) {
	
    *s = t_seq1[j];
    if (idx) {
    	// Threads (except the first one) must read from the shared memory.
        *h = s_colx[bank][idx];
        *f = s_coly[bank][idx];
    } else {
   	// First thread of the block must read from the horizontal bus.
        int2 temp;
       	temp = busH[j]; // read directly from the busH global vector
        *h = temp.x; // H-component
        *f = temp.y; // F-component
    }
}


// ******************************************************************************* //

void cl_kernel_flush(
	const int i,
	const int j,
	const int idx,
	const int bank,
	__global int2* busH,
	__global int2* extraH,
	const int4* h,
	const int f,
	const int last_thread,
	const int flush_id,
	__local int s_colx[][THREADS_COUNT],
	__local int s_coly[][THREADS_COUNT]) {
	
	if (idx == last_thread) {
		int2 temp = (int2)(h->w, f);
        	busH[j] = temp; // Store into the busH global vector.
      } else {
    		// Store into the shared memory.
        	s_colx[bank][idx+1] = h->w;
        	s_coly[bank][idx+1] = f;
    }
}


// ******************************************************************************* //

void cl_kernel_check_bound(
	const int i0, 
	const int j0, 
	const int i1, 
	const int j1,  
	int *i,  
	int *j, 
	uchar4* ss,  
	int4 *ee,  
	int *h10,   
	int4 *h00,  
	int *flush_id, 
	__global int4* loadColumn_h,  
	__global int4* loadColumn_e, 
	__global int4* flushColumn_h,  
	__global int4* flushColumn_e, 
	const int idx, 
	const int HEIGHT,  
	__global int* d_split,  
	__global char* t_seq0) {

    size_t nugr = get_num_groups(0);

    if (*j>=d_split[nugr]) {
		*j=d_split[0];
        *i+=HEIGHT;

		*flush_id = cl_get_flush_id(*i, i0, i1);
	
		*ee=(int4)(-INF,-INF,-INF,-INF);
		*h00=(int4)(0,0,0,0);
		*h10=0;

		cl_fetchSeq0(*i, ss, t_seq0);
    }
}


// ******************************************************************************* //

void cl_process_internal_diagonals_short_phase(
	const int idx, 
	const int tidx,
	int i,
	int j,
	const int i0,
	const int j0,
	const int i1,
	const int j1,
	__global int2* busH,
	__global int2* extraH,
	__global int4* busV_h,
	__global int4* busV_e, 
	__global int3* busV_o,
	int* max,
	int2* max_pos,
	int flush_id,
	__global int4* loadColumn_h,
	__global int4* loadColumn_e,
	__global int4* flushColumn_h,
	__global int4* flushColumn_e,
	__global int* d_split,
	__global char* t_seq0,
	__global char* t_seq1)
	
{
    size_t nugr = get_num_groups(0);
    size_t glsize = get_global_size(0);
    
    __local int s_colx[2][THREADS_COUNT];
    __local int s_coly[2][THREADS_COUNT];

    s_colx[0][idx] = s_colx[1][idx] = busV_o[tidx].x; // TODO poderia ser pego de busV_h[tidx].w. Certo?
    s_coly[0][idx] = s_coly[1][idx] = busV_o[tidx].y;

    int4 left_h = busV_h[tidx];
    int4 left_e = busV_e[tidx];
    int  diag_h = busV_o[tidx].z;

    uchar4 ss;
    cl_fetchSeq0(i, &ss, t_seq0);

    //barrier(CLK_GLOBAL_MEM_FENCE); 
    barrier(CLK_LOCAL_MEM_FENCE); 

	/*
	 *  We need THREADS_COUNT-1 Steps to complete the pending cells.
	 */

	int _k = (THREADS_COUNT>>1)-1; // we divide per 2 because we are loop-unrolling. THREADS_COUNT must be even.
	for (; _k; _k--) {
		int4 curr_h;
		int up_h = -INF;
		int up_f = -INF;

		/* Loop-unrolling #1 */
		cl_kernel_check_bound(i0, d_split[0], i1, d_split[nugr], &i, &j, &ss, &left_e, &diag_h, &left_h, &flush_id, loadColumn_h, loadColumn_e, flushColumn_h, flushColumn_e, idx, glsize*ALPHA, d_split, t_seq0);
		if (flush_id) {
			unsigned char s1;
			cl_kernel_load(idx, 1, j, busH, &up_h, &up_f, &s1, t_seq1, s_colx, s_coly);
			cl_kernel_sw4(i, j, s1, ss, &left_e, &up_f, left_h, diag_h , up_h , &curr_h, flush_id);
			cl_kernel_check_max4(i, j, max, max_pos, &curr_h, true, flush_id);
			cl_kernel_flush(i, j, idx, 0, busH, extraH, &curr_h, up_f, THREADS_COUNT-1, flush_id, s_colx, s_coly);
		}
		j++;
		//barrier(CLK_GLOBAL_MEM_FENCE); // barrier
		barrier(CLK_LOCAL_MEM_FENCE);
		/* Loop-unrolling #2 */
		cl_kernel_check_bound(i0, d_split[0], i1, d_split[nugr], &i, &j, &ss, &left_e, &up_h, &curr_h, &flush_id, loadColumn_h, loadColumn_e, flushColumn_h, flushColumn_e, idx, glsize*ALPHA, d_split, t_seq0);
		if (flush_id) {
			unsigned char s1;
			cl_kernel_load(idx, 0, j, busH, &diag_h, &up_f, &s1, t_seq1, s_colx, s_coly);
			cl_kernel_sw4(i, j, s1, ss, &left_e, &up_f, curr_h, up_h , diag_h, &left_h, flush_id);
			cl_kernel_check_max4(i, j, max, max_pos, &left_h, true, flush_id);
			cl_kernel_flush(i, j, idx, 1, busH, extraH, &left_h, up_f, THREADS_COUNT-1, flush_id, s_colx, s_coly);
		}
		j++;
		//barrier(CLK_GLOBAL_MEM_FENCE); // barrier
		barrier(CLK_LOCAL_MEM_FENCE);
	}

	{
		/* Last iteration is odd, so we put it outside the unrolled loop */
		int4 curr_h;
		int up_h  = -INF;
		int up_f  = -INF;
		cl_kernel_check_bound(i0, j0, i1, j1, &i, &j, &ss, &left_e, &diag_h, &left_h, &flush_id, loadColumn_h, loadColumn_e, flushColumn_h, flushColumn_e, idx, glsize*ALPHA, d_split, t_seq0);
		if (flush_id) {
		unsigned char s1;
			cl_kernel_load(idx, 1, j, busH, &up_h, &up_f, &s1, t_seq1, s_colx, s_coly);
			cl_kernel_sw4(i, j, s1, ss, &left_e, &up_f, left_h, diag_h, up_h, &curr_h, flush_id);
			cl_kernel_check_max4(i, j, max, max_pos, &curr_h, true, flush_id);
			cl_kernel_flush(i, j, idx, 0, busH, extraH, &curr_h, up_f, THREADS_COUNT-1, flush_id, s_colx, s_coly);
		}
		j++;
		//barrier(CLK_GLOBAL_MEM_FENCE); // barrier
		barrier(CLK_LOCAL_MEM_FENCE);

		/* We store the result in the vertical bus to be read by the next block */

		cl_kernel_check_bound(i0, j0, i1, j1, &i, &j, &ss, &left_e, &up_h, &curr_h, &flush_id, loadColumn_h, loadColumn_e, flushColumn_h, flushColumn_e, idx, glsize*ALPHA, d_split, t_seq0);
		
		busV_h[tidx] = curr_h;
		busV_e[tidx] = left_e;
		busV_o[tidx].x = s_colx[0][idx];
		busV_o[tidx].y = s_coly[0][idx];
		busV_o[tidx].z = up_h;
	}
}


// ******************************************************************************* //

__kernel void cl_kernel_short_phase(
	const int i0, 
	const int i1,
	const int step, 
	const int2 cutBlock,  
	__global int4* blockResult,
	__global int2* busH,  
	__global int2* extraH,
	__global int4* busV_h,  
	__global int4* busV_e,  
	__global int3* busV_o,
	__global int4* loadColumn_h,  
	__global int4* loadColumn_e,
	__global int4* flushColumn_h,  
	__global int4* flushColumn_e,
    __global int* d_split,  
	__global char* t_seq0,  
	__global char* t_seq1)  
	
{
    int bx = get_group_id(0);
    size_t grid = get_group_id(0);
    size_t loid = get_local_id(0);
    size_t glsize = get_global_size(0);
    size_t nugr = get_num_groups(0);
    
    int by = step-bx;

    if (by < 0) return;

    int idx = loid;

    const volatile int x0 = d_split[bx];
    const int xLen = d_split[bx+1] - x0;

    int i=(by*THREADS_COUNT)+idx;
    int tidx = (i % glsize);


    // Block Pruning
    bool pruneBlock;
    if (bx != 0) {
    	pruneBlock = (bx < cutBlock.x || bx > cutBlock.y);
    } else {
    	pruneBlock = (cutBlock.x > 0 && cutBlock.y < grid);
    }
	if (pruneBlock) {
		return;
	}

    const int j0 = d_split[0];
    const int j1 = d_split[nugr];

    int j=x0-idx;
    i *= ALPHA;
    i += i0;
    if (j <= j0) {
        j += (j1 - j0);
        i -= glsize*ALPHA;
    }

    int2 max_pos;
    max_pos.x = blockResult[grid].x;
    max_pos.y = blockResult[grid].y;
    int max = blockResult[grid].w;

    int flush_id = cl_get_flush_id(i, i0, i1);
    
	//if (i < i1) {
		cl_process_internal_diagonals_short_phase(idx, tidx, i, j, i0, j0, i1, j1, busH, extraH, busV_h, busV_e, busV_o, &max, &max_pos, flush_id, loadColumn_h, loadColumn_e, flushColumn_h, flushColumn_e, d_split, t_seq0, t_seq1);
    //}

  
	/* Updates the block result with the block best score */

	if (cl_findMax(loid, max)) {
	    	    
        blockResult[grid].w = max;
		blockResult[grid].x = max_pos.x;
		blockResult[grid].y = max_pos.y;
		
		if (blockResult[grid].z < max) {
			blockResult[grid].z = max;
		}
	}

}

// ******************************************************************************* //

void cl_process_internal_diagonals_long_phase(
	const int xLen, 
	const int idx, 
	const int tidx,
	const int i, 
	int j,
	__global int2* busH, 
	__global int2* extraH,
	__global int4* busV_h, 
	__global int4* busV_e, 
	__global int3* busV_o,
	int* max, 
	int2* max_pos,
	const int flush_id, 
	__global char* t_seq0, 
	__global char* t_seq1) 
	
{
    __local int s_colx[2][THREADS_COUNT]; 
    __local int s_coly[2][THREADS_COUNT]; 


    s_colx[0][idx] = s_colx[1][idx] = busV_o[tidx].x;
    s_coly[0][idx] = s_coly[1][idx] = busV_o[tidx].y;

    int4 left_h = busV_h[tidx];
    int4 left_e = busV_e[tidx];
    int  diag_h = busV_o[tidx].z;

    uchar4 ss;
    cl_fetchSeq0(i, &ss, t_seq0);

    //barrier(CLK_GLOBAL_MEM_FENCE);
    barrier(CLK_LOCAL_MEM_FENCE);

	int _k = xLen;
	if (_k&1) { // if odd
		int4 cur_h;
		int up_h;
		int up_f;

		unsigned char s1;
		cl_kernel_load(idx, 1, j, busH, &up_h, &up_f, &s1, t_seq1, s_colx, s_coly);
		cl_kernel_sw4(i, j, s1, ss, &left_e, &up_f, left_h, diag_h, up_h, &cur_h, flush_id);
		cl_kernel_check_max4(i, j, max, max_pos, &cur_h, false, flush_id);
		cl_kernel_flush(i, j, idx, 0, busH, extraH, &cur_h, up_f, THREADS_COUNT-1, flush_id, s_colx, s_coly);
		j++;
		//barrier(CLK_GLOBAL_MEM_FENCE);
		barrier(CLK_LOCAL_MEM_FENCE);
		s_colx[1][idx] = s_colx[0][idx];
		s_coly[1][idx] = s_coly[0][idx];
		diag_h = up_h;
		left_h = cur_h;
		//barrier(CLK_GLOBAL_MEM_FENCE);
		barrier(CLK_LOCAL_MEM_FENCE);
		_k--;
	}
	_k >>= 1; // we divide per 2 because we are loop-unrolling.
	for (; _k; _k--) {
		int4 cur_h;
		int up_h;
		int up_f;

		unsigned char s1;

		/* Loop-unrolling #1 */
		cl_kernel_load(idx, 1, j, busH, &up_h, &up_f, &s1, t_seq1, s_colx, s_coly);
		cl_kernel_sw4(i, j, s1, ss, &left_e, &up_f, left_h, diag_h, up_h, &cur_h, flush_id);
		cl_kernel_check_max4(i, j, max, max_pos, &cur_h, false, flush_id);
		cl_kernel_flush(i, j, idx, 0, busH, extraH, &cur_h, up_f, THREADS_COUNT-1, flush_id, s_colx, s_coly);
		j++;
		//barrier(CLK_GLOBAL_MEM_FENCE);
		barrier(CLK_LOCAL_MEM_FENCE);

		/* Loop-unrolling #2 */
		cl_kernel_load(idx, 0, j, busH, &diag_h, &up_f, &s1, t_seq1, s_colx, s_coly);
		cl_kernel_sw4(i, j, s1, ss, &left_e, &up_f, cur_h, up_h, diag_h, &left_h, flush_id);
		cl_kernel_check_max4(i, j, max, max_pos, &left_h, false, flush_id);
		cl_kernel_flush(i, j, idx, 1, busH, extraH, &left_h, up_f, THREADS_COUNT-1, flush_id, s_colx, s_coly);
		j++;
		//barrier(CLK_GLOBAL_MEM_FENCE);
		barrier(CLK_LOCAL_MEM_FENCE);

	}

	/* We store the result in the vertical bus to be read by the next block */

    busV_h[tidx]=left_h;
    busV_e[tidx]=left_e;
    busV_o[tidx].x=s_colx[1][idx];
    busV_o[tidx].y=s_coly[1][idx];
    busV_o[tidx].z=diag_h;

}

// ******************************************************************************* //

__kernel void cl_kernel_long_phase(
	const int i0, 
	const int i1,
	const int step,
	const int2 cutBlock,  
	__global int4* blockResult,
	__global int2* busH,  
	__global int2* extraH,
	__global int4* busV_h,  
	__global int4* busV_e,  
	__global int3* busV_o,
    __global int* d_split,  
	__global char* t_seq0,  
	__global char* t_seq1)  
	
{
	size_t grid = get_group_id(0);
	size_t nugr = get_num_groups(0);
	size_t loid = get_local_id(0);
	size_t glid = get_global_id(0);
	size_t losize = get_local_size(0);
	
    const int bx = grid; 

	if (bx < cutBlock.x || bx > cutBlock.y) {
		// Block Pruning
		if (step - bx >= 0) {
			int tidx = ((step-bx) % nugr)*THREADS_COUNT + loid;
			busV_h[tidx]=(int4)(-INF,-INF,-INF,-INF);
			busV_e[tidx]=(int4)(-INF,-INF,-INF,-INF);
			busV_o[tidx]=(int3)(-INF,-INF,-INF);
			blockResult[bx].w = -INF;
		}
		return;
	}
    
    const int by = step-bx;
    
       
    if (by < 0) return;

    const int idx = loid;

    const int x0 = d_split[bx]+(THREADS_COUNT-1);
    const int xLen = d_split[bx+1] - x0;

    const int tidx = (by % nugr)*THREADS_COUNT+idx;
    const int i=((by*THREADS_COUNT)+idx)*ALPHA + i0;

    const int j=x0-idx;
    
    int2 max_pos;
    max_pos.x = -1;  //blockResult[blockIdx.x].x;
    max_pos.y = -1;  //blockResult[blockIdx.x].y;
    int max = -INF;  //blockResult[bx].w;

    //if (i < i1) {
        int flush_id = cl_get_flush_id(i, i0, i1);
        cl_process_internal_diagonals_long_phase(xLen, idx, tidx, i, j, busH, extraH, busV_h, busV_e, busV_o, &max, &max_pos, flush_id, t_seq0, t_seq1);
    //}
	
	if (!flush_id) {
        max = -INF; 
    }
    
	if (cl_findMax(loid, max)) {
		blockResult[grid].w = max;
		blockResult[grid].x = max_pos.x;
		blockResult[grid].y = max_pos.y+i;
		if (blockResult[grid].z < max) {
			blockResult[grid].z = max;
		}
	}

}

// ******************************************************************************* //

void cl_process_internal_diagonals_single_phase(
	const int xLen, 
	const int idx,
	int i, 
	int j, 
	const int i0, 
	const int j0, 
	const int i1, 
	const int j1,
	__global int2* busH,  
	__global int2* extraH,
	__global int4* busV_h,  
	__global int4* busV_e,  
	__global int3* busV_o,
	int* max,  
	int2* max_pos,
	int flush_id,
	__global int4* loadColumn_h,  
	__global int4* loadColumn_e,
	__global int4* flushColumn_h,  
	__global int4* flushColumn_e,  
	__global int* d_split,  
	__global char* t_seq0,  
	__global char* t_seq1)  
	
{
    size_t losize = get_local_size(0);
    
    __local int s_colx[2][THREADS_COUNT]; // Alocar aqui
    __local int s_coly[2][THREADS_COUNT]; // Alocar aqui


    s_colx[0][idx] = s_colx[1][idx] = busV_o[idx].x;
    s_coly[0][idx] = s_coly[1][idx] = busV_o[idx].y;

    int4 left_h = busV_h[idx];
    int4 left_e = busV_e[idx];
    int  diag_h = busV_o[idx].z;

    uchar4 ss;
    cl_fetchSeq0(i, &ss, t_seq0);

    //barrier(CLK_GLOBAL_MEM_FENCE);
    barrier(CLK_LOCAL_MEM_FENCE);

    int _k = xLen;
    int index = 1;
    for (; _k; _k--) {
    	int4 curr_h;
        int  up_h;
        int  up_f;
	
	cl_kernel_check_bound(i0, j0, i1, j1, &i, &j, &ss, &left_e, &diag_h, &left_h, &flush_id, loadColumn_h, loadColumn_e, flushColumn_h, flushColumn_e, idx, losize*ALPHA, d_split, t_seq0);
        if (flush_id) {
            unsigned char s1;
            cl_kernel_load(idx, index, j, busH, &up_h, &up_f, &s1, t_seq1, s_colx, s_coly);
	        cl_kernel_sw4(i, j, s1, ss, &left_e, &up_f, left_h, diag_h  , up_h  , &curr_h, flush_id);
	        cl_kernel_check_max4(i, j, max, max_pos, &curr_h, true, flush_id);
            cl_kernel_flush(i, j, idx, 1-index, busH, extraH, &curr_h, up_f, losize-1, flush_id, s_colx, s_coly);
        }
        index = 1-index;
        j++;
        diag_h = up_h;
        left_h = curr_h;
        //barrier(CLK_GLOBAL_MEM_FENCE);
		barrier(CLK_LOCAL_MEM_FENCE);
    }
   
   cl_kernel_check_bound(i0, j0, i1, j1, &i, &j, &ss, &left_e, &diag_h, &left_h, &flush_id, loadColumn_h, loadColumn_e, flushColumn_h, flushColumn_e, idx, losize*ALPHA, d_split, t_seq0);

    busV_h[idx]=left_h;
    busV_e[idx]=left_e;
    busV_o[idx].x=s_colx[index][idx];
    busV_o[idx].y=s_coly[index][idx];
    busV_o[idx].z=diag_h;

}

// ******************************************************************************* //

__kernel void cl_kernel_single_phase(
	const int i0, 
	const int i1,
	const int step, 
	const int2 cutBlock,  
	__global int4* blockResult,
	__global int2* busH, 
	__global int2* extraH,
	__global int4* busV_h,  
	__global int4* busV_e,  
	__global int3* busV_o,
	__global int4* loadColumn_h,  
	__global int4* loadColumn_e,
	__global int4* flushColumn_h, 
	__global int4* flushColumn_e,
	__global int* d_split,  
	__global char* t_seq0, 
	__global char* t_seq1) 
	
{
   size_t losize = get_local_size(0);
   size_t loid = get_local_id(0);
   size_t nugr = get_num_groups(0);
    

    int idx = loid;
    int by = step;

    const int j0 = d_split[0];
    const int j1 = d_split[nugr];

    const int xLen = j1-j0;
    int j=j0-idx-(xLen-losize+1); // This ensures that the block fills the busH entirely in the same row
    int i=(by*losize)+idx;
    i *= ALPHA;
    i += i0;

    if (j<=j0) { // TODO era while em vez de if
        j+=(j1-j0);
        i-=losize*ALPHA;
    }

    int2 max_pos;
    max_pos.x = -1;  //blockResult[0].x;
    max_pos.y = -1;  //blockResult[0].y;
    int max = -INF;  //blockResult[0].w;
	
	int flush_id = cl_get_flush_id(i, i0, i1);
	
	//printf("\n\n ####### i: %d  ----  i1: %d  #########", i, i1);
	
    // TODO otimizar considerando warps
	//if (i < i1) {
		cl_process_internal_diagonals_single_phase(xLen, idx, i, j, i0, j0, i1, j1, busH, extraH, busV_h, busV_e, busV_o, &max, &max_pos, flush_id, loadColumn_h, loadColumn_e, flushColumn_h, flushColumn_e, d_split, t_seq0, t_seq1);
	//}

    //printf("\n\n ####### idx: %d  ----  max: %d  #########", idx, max);
   
    if (cl_findMaxSmall(idx, max)) {
      
	blockResult[0].w = max;
	blockResult[0].x = max_pos.x;
	blockResult[0].y = max_pos.y;
        if (blockResult[0].z < max) {
            blockResult[0].z = max;
        }
    }
}

// ******************************************************************************* //

__kernel void cl_kernel_initialize_busH_ungapped( 
	__global int2* busH, 
	const int j0, 
	const int len) {

    size_t glid = get_global_id(0);
    size_t glsize = get_global_size(0);

    int tidx = glid;
    while (tidx < len) {
		busH[j0+tidx].x = -INF;
        busH[j0+tidx].y = -INF;
		tidx += glsize;
    }
}


