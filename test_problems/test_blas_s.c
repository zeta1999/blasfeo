/**************************************************************************************************
*                                                                                                 *
* This file is part of BLASFEO.                                                                   *
*                                                                                                 *
* BLASFEO -- BLAS For Embedded Optimization.                                                      *
* Copyright (C) 2016-2017 by Gianluca Frison.                                                     *
* Developed at IMTEK (University of Freiburg) under the supervision of Moritz Diehl.              *
* All rights reserved.                                                                            *
*                                                                                                 *
* HPMPC is free software; you can redistribute it and/or                                          *
* modify it under the terms of the GNU Lesser General Public                                      *
* License as published by the Free Software Foundation; either                                    *
* version 2.1 of the License, or (at your option) any later version.                              *
*                                                                                                 *
* HPMPC is distributed in the hope that it will be useful,                                        *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                                  *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                                            *
* See the GNU Lesser General Public License for more details.                                     *
*                                                                                                 *
* You should have received a copy of the GNU Lesser General Public                                *
* License along with HPMPC; if not, write to the Free Software                                    *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA                  *
*                                                                                                 *
* Author: Gianluca Frison, giaf (at) dtu.dk                                                       *
*                          gianluca.frison (at) imtek.uni-freiburg.de                             *
*                                                                                                 *
**************************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>


#include "../include/blasfeo_common.h"
#include "../include/blasfeo_block_size.h"
#include "../include/blasfeo_s_aux_ext_dep.h"
#include "../include/blasfeo_i_aux_ext_dep.h"
#include "../include/blasfeo_s_aux.h"
#include "../include/blasfeo_s_blas.h"


#if defined(REF_BLAS_OPENBLAS)
#include <f77blas.h>
void openblas_set_num_threads(int n_thread);
#endif
#if defined(REF_BLAS_BLIS)
#include <blis/blis.h>
#endif
#if defined(REF_BLAS_NETLIB)
//#include "../reference_code/blas.h"
#endif



#define GHZ_MAX 3.3



int main()
	{
		
#if defined(REF_BLAS_OPENBLAS)
	openblas_set_num_threads(1);
#endif
#if defined(REF_BLAS_BLIS)
	omp_set_num_threads(1);
#endif

	printf("\n");
	printf("\n");
	printf("\n");

	printf("BLAS performance test - float precision\n");
	printf("\n");

	// maximum frequency of the processor
	const float GHz_max = GHZ_MAX;
	printf("Frequency used to compute theoretical peak: %5.1f GHz (edit test_param.h to modify this value).\n", GHz_max);
	printf("\n");

	// maximum flops per cycle, float precision
#if defined(TARGET_X64_INTEL_HASWELL)
	const float flops_max = 32;
	printf("Testing BLAS version for AVX2 and FMA instruction sets, 64 bit (optimized for Intel Haswell): theoretical peak %5.1f Gflops\n", flops_max*GHz_max);
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	const float flops_max = 16;
	printf("Testing BLAS version for AVX instruction set, 64 bit (optimized for Intel Sandy Bridge): theoretical peak %5.1f Gflops\n", flops_max*GHz_max);
#elif defined(TARGET_X64_INTEL_CORE)
	const float flops_max = 8;
	printf("Testing BLAS version for SSE3 instruction set, 64 bit (optimized for Intel Core): theoretical peak %5.1f Gflops\n", flops_max*GHz_max);
#elif defined(TARGET_X64_AMD_BULLDOZER)
	const float flops_max = 16;
	printf("Testing BLAS version for SSE3 and FMA instruction set, 64 bit (optimized for AMD Bulldozer): theoretical peak %5.1f Gflops\n", flops_max*GHz_max);
#elif defined(TARGET_ARMV7A_ARM_CORTEX_A15)
	const float flops_max = 8;
	printf("Testing BLAS version for VFPv4 instruction set, 32 bit (optimized for ARM Cortex A15): theoretical peak %5.1f Gflops\n", flops_max*GHz_max);
#elif defined(TARGET_GENERIC)
	const float flops_max = 2;
	printf("Testing BLAS version for generic scalar instruction set: theoretical peak %5.1f Gflops ???\n", flops_max*GHz_max);
#endif
	
	FILE *f;
	f = fopen("./test_problems/results/test_blas.m", "w"); // a

#if defined(TARGET_X64_INTEL_HASWELL)
	fprintf(f, "C = 's_x64_intel_haswell';\n");
	fprintf(f, "\n");
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	fprintf(f, "C = 's_x64_intel_sandybridge';\n");
	fprintf(f, "\n");
#elif defined(TARGET_X64_INTEL_CORE)
	fprintf(f, "C = 's_x64_intel_core';\n");
	fprintf(f, "\n");
#elif defined(TARGET_X64_AMD_BULLDOZER)
	fprintf(f, "C = 's_x64_amd_bulldozer';\n");
	fprintf(f, "\n");
#elif defined(TARGET_ARMV7A_ARM_CORTEX_A15)
	fprintf(f, "C = 's_armv7a_arm_cortex_a15';\n");
	fprintf(f, "\n");
#elif defined(TARGET_GENERIC)
	fprintf(f, "C = 's_generic';\n");
	fprintf(f, "\n");
#endif

	fprintf(f, "A = [%f %f];\n", GHz_max, flops_max);
	fprintf(f, "\n");

	fprintf(f, "B = [\n");
	


	int i, j, rep, ll;
	
	const int bss = S_BS;
	const int ncs = S_NC;

/*	int info = 0;*/
	
	printf("\nn\t  sgemm_blasfeo\t  sgemm_blas\n");
	printf("\nn\t Gflops\t    %%\t Gflops\t    %%\n\n");
	
#if 1
	int nn[] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192, 196, 200, 204, 208, 212, 216, 220, 224, 228, 232, 236, 240, 244, 248, 252, 256, 260, 264, 268, 272, 276, 280, 284, 288, 292, 296, 300, 304, 308, 312, 316, 320, 324, 328, 332, 336, 340, 344, 348, 352, 356, 360, 364, 368, 372, 376, 380, 384, 388, 392, 396, 400, 404, 408, 412, 416, 420, 424, 428, 432, 436, 440, 444, 448, 452, 456, 460, 500, 550, 600, 650, 700};
	int nnrep[] = {10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 400, 400, 400, 400, 400, 200, 200, 200, 200, 200, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 20, 20, 20, 20, 20, 20, 20, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 4, 4, 4, 4, 4};
	
	for(ll=0; ll<75; ll++)
//	for(ll=0; ll<115; ll++)
//	for(ll=0; ll<120; ll++)

		{

		int n = nn[ll];
		int nrep = nnrep[ll];

#else
	int nn[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
	
	for(ll=0; ll<24; ll++)

		{

		int n = nn[ll];
		int nrep = 40000; //nnrep[ll];
#endif

#if defined(REF_BLAS_BLIS)
		f77_int n77 = n;
#endif
	
		float *A; s_zeros(&A, n, n);
		float *B; s_zeros(&B, n, n);
		float *C; s_zeros(&C, n, n);
		float *M; s_zeros(&M, n, n);

		char c_n = 'n';
		char c_l = 'l';
		char c_r = 'r';
		char c_t = 't';
		char c_u = 'u';
		int i_1 = 1;
		int i_t;
#if defined(REF_BLAS_BLIS)
		f77_int i77_1 = i_1;
#endif
		float d_1 = 1;
		float d_0 = 0;
	
		for(i=0; i<n*n; i++)
			A[i] = i;
	
		for(i=0; i<n; i++)
			B[i*(n+1)] = 1;
	
		for(i=0; i<n*n; i++)
			M[i] = 1;
	
		int n2 = n*n;
		float *B2; s_zeros(&B2, n, n);
		for(i=0; i<n*n; i++)
			B2[i] = 1e-15;
		for(i=0; i<n; i++)
			B2[i*(n+1)] = 1;

		int pnd = ((n+bss-1)/bss)*bss;	
		int cnd = ((n+ncs-1)/ncs)*ncs;	
		int cnd2 = 2*((n+ncs-1)/ncs)*ncs;	

		float *x; s_zeros_align(&x, pnd, 1);
		float *y; s_zeros_align(&y, pnd, 1);
		float *x2; s_zeros_align(&x2, pnd, 1);
		float *y2; s_zeros_align(&y2, pnd, 1);
		float *diag; s_zeros_align(&diag, pnd, 1);
		int *ipiv; int_zeros(&ipiv, n, 1);

		for(i=0; i<pnd; i++) x[i] = 1;
		for(i=0; i<pnd; i++) x2[i] = 1;

		// matrix struct
#if 1
		struct s_strmat sA; s_allocate_strmat(n+4, n+4, &sA);
		struct s_strmat sB; s_allocate_strmat(n+4, n+4, &sB);
		struct s_strmat sC; s_allocate_strmat(n+4, n+4, &sC);
		struct s_strmat sD; s_allocate_strmat(n+4, n+4, &sD);
		struct s_strmat sE; s_allocate_strmat(n+4, n+4, &sE);
#else
		struct s_strmat sA; s_allocate_strmat(n, n, &sA);
		struct s_strmat sB; s_allocate_strmat(n, n, &sB);
		struct s_strmat sC; s_allocate_strmat(n, n, &sC);
		struct s_strmat sD; s_allocate_strmat(n, n, &sD);
		struct s_strmat sE; s_allocate_strmat(n, n, &sE);
#endif
		struct s_strvec sx; s_allocate_strvec(n, &sx);
		struct s_strvec sy; s_allocate_strvec(n, &sy);
		struct s_strvec sz; s_allocate_strvec(n, &sz);

		s_cvt_mat2strmat(n, n, A, n, &sA, 0, 0);
		s_cvt_mat2strmat(n, n, B, n, &sB, 0, 0);
		s_cvt_vec2strvec(n, x, &sx, 0);


		// create matrix to pivot all the time
		sgemm_nt_libstr(n, n, n, 1.0, &sA, 0, 0, &sA, 0, 0, 1.0, &sB, 0, 0, &sD, 0, 0);

		float *dummy;

		int info;

		/* timing */
		struct timeval tvm1, tv0, tv1, tv2, tv3, tv4, tv5, tv6, tv7, tv8, tv9, tv10, tv11, tv12, tv13, tv14, tv15, tv16;

		/* warm up */
		for(rep=0; rep<nrep; rep++)
			{
			sgemm_nt_libstr(n, n, n, 1.0, &sA, 0, 0, &sB, 0, 0, 0.0, &sC, 0, 0, &sD, 0, 0);
			}

		gettimeofday(&tv0, NULL); // stop

		for(rep=0; rep<nrep; rep++)
			{

//			sgemm_nt_lib(n, n, n, 1.0, pA, cnd, pB, cnd, 0.0, pC, cnd, pC, cnd);
//			sgemm_nn_lib(n, n, n, 1.0, pA, cnd, pB, cnd, 0.0, pC, cnd, pC, cnd);
//			ssyrk_nt_l_lib(n, n, n, 1.0, pA, cnd, pB, cnd, 1.0, pC, cnd, pD, cnd);
//			strmm_nt_ru_lib(n, n, pA, cnd, pB, cnd, 0, pC, cnd, pD, cnd);
//			spotrf_nt_l_lib(n, n, pB, cnd, pD, cnd, diag);
//			ssyrk_dpotrf_nt_l_lib(n, n, n, pA, cnd, pA, cnd, 1, pB, cnd, pD, cnd, diag);
//			ssyrk_nt_l_lib(n, n, n, pA, cnd, pA, cnd, 1, pB, cnd, pD, cnd);
//			spotrf_nt_l_lib(n, n, pD, cnd, pD, cnd, diag);
//			sgetrf_nn_nopivot_lib(n, n, pB, cnd, pB, cnd, diag);
//			sgetrf_nn_lib(n, n, pB, cnd, pB, cnd, diag, ipiv);
//			strsm_nn_ll_one_lib(n, n, pD, cnd, pB, cnd, pB, cnd);
//			strsm_nn_lu_inv_lib(n, n, pD, cnd, diag, pB, cnd, pB, cnd);
			}
	
		gettimeofday(&tv1, NULL); // stop

		for(rep=0; rep<nrep; rep++)
			{
			sgemm_nt_libstr(n, n, n, 1.0, &sA, 0, 0, &sB, 0, 0, 0.0, &sC, 0, 0, &sD, 0, 0);
//			sgemm_nn_libstr(n, n, n, 1.0, &sA, 0, 0, &sB, 0, 0, 0.0, &sC, 0, 0, &sD, 0, 0);
//			spotrf_l_libstr(n, n, &sD, 0, 0, &sD, 0, 0);
//			sgetrf_nopivot_libstr(n, n, &sB, 0, 0, &sB, 0, 0);
//			sgetrf_libstr(n, n, &sB, 0, 0, &sB, 0, 0, ipiv);
//			strmm_rlnn_libstr(n, n, 1.0, &sA, 0, 0, &sB, 0, 0, &sD, 0, 0);
//			strmm_rutn_libstr(n, n, 1.0, &sA, 0, 0, &sB, 0, 0, &sD, 0, 0);
//			strsm_llnu_libstr(n, n, 1.0, &sD, 0, 0, &sB, 0, 0, &sB, 0, 0);
//			strsm_lunn_libstr(n, n, 1.0, &sD, 0, 0, &sB, 0, 0, &sB, 0, 0);
//			strsm_rltu_libstr(n, n, 1.0, &sD, 0, 0, &sB, 0, 0, &sB, 0, 0);
//			strsm_rutn_libstr(n, n, 1.0, &sD, 0, 0, &sB, 0, 0, &sB, 0, 0);
//			sgemv_n_libstr(n, n, 1.0, &sA, 0, 0, &sx, 0, 0.0, &sy, 0, &sz, 0);
//			sgemv_t_libstr(n, n, 1.0, &sA, 0, 0, &sx, 0, 0.0, &sy, 0, &sz, 0);
//			ssymv_l_libstr(n, n, 1.0, &sA, 0, 0, &sx, 0, 0.0, &sy, 0, &sz, 0);
//			sgemv_nt_libstr(n, n, 1.0, 1.0, &sA, 0, 0, &sx, 0, &sx, 0, 0.0, 0.0, &sy, 0, &sy, 0, &sz, 0, &sz, 0);
			}

//		d_print_strmat(n, n, &sD, 0, 0);

		gettimeofday(&tv2, NULL); // stop

		for(rep=0; rep<nrep; rep++)
			{
#if defined(REF_BLAS_OPENBLAS) || defined(REF_BLAS_NETLIB) || defined(REF_BLAS_MKL)
			sgemm_(&c_n, &c_t, &n, &n, &n, &d_1, A, &n, M, &n, &d_0, C, &n);
//			sgemm_(&c_n, &c_n, &n, &n, &n, &d_1, A, &n, M, &n, &d_0, C, &n);
//			ssyrk_(&c_l, &c_n, &n, &n, &d_1, A, &n, &d_0, C, &n);
//			strmm_(&c_r, &c_u, &c_t, &c_n, &n, &n, &d_1, A, &n, C, &n);
//			spotrf_(&c_l, &n, B2, &n, &info);
//			sgetrf_(&n, &n, B2, &n, ipiv, &info);
//			strsm_(&c_l, &c_l, &c_n, &c_u, &n, &n, &d_1, B2, &n, B, &n);
//			strsm_(&c_l, &c_u, &c_n, &c_n, &n, &n, &d_1, B2, &n, B, &n);
//			strtri_(&c_l, &c_n, &n, B2, &n, &info);
//			slauum_(&c_l, &n, B, &n, &info);
//			sgemv_(&c_n, &n, &n, &d_1, A, &n, x, &i_1, &d_0, y, &i_1);
//			sgemv_(&c_t, &n, &n, &d_1, A, &n, x2, &i_1, &d_0, y2, &i_1);
//			strmv_(&c_l, &c_n, &c_n, &n, B, &n, x, &i_1);
//			strsv_(&c_l, &c_n, &c_n, &n, B, &n, x, &i_1);
//			ssymv_(&c_l, &n, &d_1, A, &n, x, &i_1, &d_0, y, &i_1);

//			for(i=0; i<n; i++)
//				{
//				i_t = n-i;
//				scopy_(&i_t, &B[i*(n+1)], &i_1, &C[i*(n+1)], &i_1);
//				}
//			ssyrk_(&c_l, &c_n, &n, &n, &d_1, A, &n, &d_1, C, &n);
//			spotrf_(&c_l, &n, C, &n, &info);

#endif

#if defined(REF_BLAS_BLIS)
//			sgemm_(&c_n, &c_t, &n77, &n77, &n77, &d_1, A, &n77, B, &n77, &d_0, C, &n77);
//			sgemm_(&c_n, &c_n, &n77, &n77, &n77, &d_1, A, &n77, B, &n77, &d_0, C, &n77);
//			ssyrk_(&c_l, &c_n, &n77, &n77, &d_1, A, &n77, &d_0, C, &n77);
//			strmm_(&c_r, &c_u, &c_t, &c_n, &n77, &n77, &d_1, A, &n77, C, &n77);
//			spotrf_(&c_l, &n77, B, &n77, &info);
//			strtri_(&c_l, &c_n, &n77, B, &n77, &info);
//			slauum_(&c_l, &n77, B, &n77, &info);
#endif
			}

		gettimeofday(&tv3, NULL); // stop

		float Gflops_max = flops_max * GHz_max;

		float flop_operation = 2.0*n*n*n; // dgemm
//		float flop_operation = 1.0*n*n*n; // dsyrk dtrmm dtrsm
//		float flop_operation = 1.0/3.0*n*n*n; // dpotrf dtrtri
//		float flop_operation = 2.0/3.0*n*n*n; // dgetrf
//		float flop_operation = 2.0*n*n; // dgemv dsymv
//		float flop_operation = 1.0*n*n; // dtrmv dtrsv
//		float flop_operation = 4.0*n*n; // dgemv_nt

//		float flop_operation = 4.0/3.0*n*n*n; // dsyrk+dpotrf

		float time_hpmpc    = (float) (tv1.tv_sec-tv0.tv_sec)/(nrep+0.0)+(tv1.tv_usec-tv0.tv_usec)/(nrep*1e6);
		float time_blasfeo  = (float) (tv2.tv_sec-tv1.tv_sec)/(nrep+0.0)+(tv2.tv_usec-tv1.tv_usec)/(nrep*1e6);
		float time_blas     = (float) (tv3.tv_sec-tv2.tv_sec)/(nrep+0.0)+(tv3.tv_usec-tv2.tv_usec)/(nrep*1e6);

		float Gflops_hpmpc    = 1e-9*flop_operation/time_hpmpc;
		float Gflops_blasfeo  = 1e-9*flop_operation/time_blasfeo;
		float Gflops_blas     = 1e-9*flop_operation/time_blas;


		printf("%d\t%7.2f\t%7.2f\t%7.2f\t%7.2f\n", n, Gflops_blasfeo, 100.0*Gflops_blasfeo/Gflops_max, Gflops_blas, 100.0*Gflops_blas/Gflops_max);
		fprintf(f, "%d\t%7.2f\t%7.2f\t%7.2f\t%7.2f\n", n, Gflops_blasfeo, 100.0*Gflops_blasfeo/Gflops_max, Gflops_blas, 100.0*Gflops_blas/Gflops_max);


		free(A);
		free(B);
		free(B2);
		free(M);
		free(x);
		free(y);
		free(x2);
		free(y2);
		free(ipiv);
		
		s_free_strmat(&sA);
		s_free_strmat(&sB);
		s_free_strmat(&sC);
		s_free_strmat(&sD);
		s_free_strmat(&sE);
		s_free_strvec(&sx);
		s_free_strvec(&sy);
		s_free_strvec(&sz);

		}

	printf("\n");

	fprintf(f, "];\n");
	fclose(f);

	return 0;
	
	}

