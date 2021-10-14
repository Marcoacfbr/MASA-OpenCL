/*******************************************************************************
 *
// * Copyright (c) 2010-2014   Edans Sandes
// *
// * This file is part of CUDAlign.
// *
// * CUDAlign is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * (at your option) any later version.
// *
// * CUDAlign is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with CUDAlign.  If not, see <http://www.gnu.org/licenses/>.
// *
// ******************************************************************************/
//


void cl_bind_textures(cl_structures_t * op, const char* seq0, const int seq0_len, const char* seq1, const int seq1_len);
void cl_unbind_textures() ;
void cl_copy_split(cl_structures_t * op, const int* split, const int blocks);
void cl_initializeBusHInfinity(cl_structures_t * op, const int p0, const int p1);
void cl_lauch_external_diagonals(cl_context context, cl_device_id device, int COLUMN_SOURCE, int COLUMN_DESTINATION, int RECURRENCE_TYPE, int CHECK_LOCATION, const int blocks, const int threads,
const int i0, const int i1, int step, const int2 cutBlock, cuda_structures_t* cuda, cl_structures_t* op);





