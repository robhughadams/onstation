
/*
        Loop closure subroutines
        Copyright (c) Jan 1995 J.P. Lebel / Calgary AB.
        Please forward any proposed upgrade suggestions to:
*/
#include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "fcntl.h"
#include "conio.h"
#include "jpclose.h"
#include "node.h"
#include "surveyshot.h"
#include "surveyleg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSurveyShot *little_shot_info, **LittleShots;

float deg_rad={3.141592632f/180.0f};
float pi={3.141592632f};

/* solver linear equation structure */
struct Lin_eq svv_big, svv_little;
unsigned long BigNumNodes;
unsigned long LittleNumNodes;
unsigned long *BigNodeNum_A, *LittleNodeNum_A;
unsigned long *BigNodeNum_B, *LittleNodeNum_B;
CNode  **BigNodes;
CNode  **LittleNodes;

/* misc storage */
//extern long     *PlotOrder;     /* TACO doesn't need this */

//#include <jpclose.i>

int CClose::mat_inv(float *to,
            float *from,
            unsigned int size)  /* invert square matrix from & to can be same */
{
        float *work1,   temp;
        unsigned int    ia,     ix, iy, *flagr,*flagc, xp, yp;

        work1 = (float*)        malloc(size * size * sizeof(float));
        flagr = (unsigned *)          malloc(size * sizeof(int));
        flagc = (unsigned *)          malloc(size * sizeof(int));
        
        /* transfer matrix into work space */
        for(iy=0 ; iy < size ; iy++){
                for(ix=0 ; ix < size ; ix++){
                        work1[size*iy+ix] = from[size*iy+ix];
                        to[size*iy+ix] = 0.0f;
                }       
                flagr[iy] = 1;
                flagc[iy] = 1;
                to[size*iy+iy] = 1.0f;
        }

        /* once for each row */
        for(ia=0 ; ia < size ; ia++){

                /* find largest remaining diagonal entry for pivot */
                temp = 0.0f;
                for(iy=0 ; iy < size ; iy++){
                        if(flagr[iy] && flagc[iy]){
                                if(fabs(work1[size*iy+iy]) > temp){
                                        xp = iy; yp = iy;temp = (float)fabs(work1[size*iy+iy]);
                                }       
                        }
                }

                if((xp >= size) | (yp >= size)){
                        free(work1);
                        free(flagr);
                        free(flagc);
                        return((int) 0); /* singular */
                }

                /* normalize pivot row & initialize pivot column in "to" array */
                temp = work1[size*yp+xp];
                for(ix=0 ; ix < size ; ix++){
                        work1[size*yp+ix]       /= temp;
                        to[size*yp+ix]          /= temp;
                }

                for(iy=0 ; iy < size ; iy++){
                        if(iy != yp){
                                temp = -work1[size*iy+xp];
                                for(ix=0 ; ix < size ; ix++){
                                        if(flagc[ix]){
                                                work1[size*iy+ix]       += temp * work1[size*yp+ix];
                                        }
                                        to[size*iy+ix]          += temp * to[size*yp+ix];
                                }
                        }
                }
                flagc[xp] = 0;
                flagr[yp] = 0;
        }

        free(work1);
        free(flagr);
        free(flagc);
        return((int) 1);
}
/* end mat_inv */


int CClose::mat_x_mat(float *to,
              float *from1,
              float *from2,
              unsigned int size)        /* cross multiply two (square) matrices */
                                                                                                                        /* all three can be same matrix */
{
        float *work1;
        unsigned int    ia,     ix, iy;
        work1 = (float*)        malloc(size * size * sizeof(float));

        /* zero target array */
        for(iy=0 ; iy < size ; iy++){
                for(ix=0 ; ix < size ; ix++){
                        work1[size*iy+ix] = 0.0f;
                }       
        }

        /* each column in target */
        for(ia=0 ; ia < size ; ia++){

                /* each element in target column */
                for(iy=0 ; iy < size ; iy++){

                        /* each element in source */
                        for(ix=0 ; ix < size ; ix++){
                                work1[size*iy+ia] += from1[size*iy+ix] * from2[size*ix+ia];
                        }       
                }
        }

        /* transfer results */
        for(iy=0 ; iy < size*size ; iy++){
                        to[iy] = work1[iy];
        }

        free(work1);
        return((int) 0);
}
/* end mat_x_mat */


int CClose::mat_x_vect(float *to,
               float *mat,
               float *vect,
               unsigned int size)       /* cross multiply square matrix and vector*/
                                                                                                                        /* to can be same as vect */
{
        float *work1;
        unsigned int    ix, iy;
        work1 = (float*)        malloc(size * sizeof(float));

        /* zero target array */
        for(iy=0 ; iy < size ; iy++){
                        work1[iy] = 0.0f;
        }

        /* each element in target column */
        for(iy=0 ; iy < size ; iy++){

                /* each element in source */
                for(ix=0 ; ix < size ; ix++){
                        work1[iy] += mat[size*iy+ix] * vect[ix];
                }       
        }

        /* transfer results */
        for(iy=0 ; iy < size ; iy++){
                        to[iy] = work1[iy];
        }

        free(work1);
        return((int) 1);
}
/* end mat_x_vect */



int CClose::vect_add(float *to,
             float *vect1,
             float *vect2,
             unsigned int size,
             unsigned int oper) /* add two vectors */
                                                                                                                        /* to can be same as either vect */
                                                                                                                        /* oper : 0 add both 1 sub first,
                                                                                                                                        2 sub 2nd  3 sub both  */
{
        float *work1;
        unsigned int    iy;
        work1 = (float*)        malloc(size * sizeof(float));

        /*  target array */
        if(oper == 0 | oper == 2){
                for(iy=0 ; iy < size ; iy++){
                        work1[iy] = vect1[iy];
                }
        }
        else{
                for(iy=0 ; iy < size ; iy++){
                        work1[iy] = -vect1[iy];
                }
        }
        if(oper < 2){
                for(iy=0 ; iy < size ; iy++){
                        work1[iy] += vect2[iy];
                }
        }
        else{
                for(iy=0 ; iy < size ; iy++){
                        work1[iy] -= vect2[iy];
                }
        }

        /* transfer results */
        for(iy=0 ; iy < size ; iy++){
                        to[iy] = work1[iy];
        }

        free(work1);
        return((int) 1);
}
/* end vect_add */



int CClose::mat_disp(float *from,
             char *name,
             unsigned int size) /* display (square) matrix */
{
//        unsigned int    ix, iy;

        /* display array */
//        printf("\n       ARRAY = %15s",name);
  //      for(iy=0 ; iy < size ; iy++){
    //            printf("\nrow %5i = ",iy);
      //          for(ix=0 ; ix < size ; ix++) printf("  %10.4f",from[size*iy+ix]);
        //}
//        printf("\n\n hit any key");
  //      getch();

        return((int) 1);
}
/* end mat_disp */



int CClose::pmat_insert(struct Lin_eq *matrix,
                                                                                                        float *sub,
                          unsigned long row,
                          unsigned long col,
                          float fact)   /* scale and add submatrix */
{
        float *values;
        unsigned long   r,      r1,     r2,     r3,     col3;
        unsigned        long    *c;

        /* Add new values */
        r1 = matrix->m_RI[row*3];
        r2 = matrix->m_RI[row*3+1];
        r3 = matrix->m_RI[row*3+2];
        c  = matrix->m_C + r1;
        col3 = col * 3;

        /* find correct first column */
  for(r=r2 ; (r1<r) && *c != col3 ; c++) {r1++; r2++; r3++;}

        values = matrix->m_A + r1;
        *values++ += fact * sub[0];
        *values++ += fact * sub[1];
        *values++ += fact * sub[2];

        values = matrix->m_A + r2;
        *values++ += fact * sub[1];
        *values++ += fact * sub[4];
        *values++ += fact * sub[5];

        values = matrix->m_A + r3;
        *values++ += fact * sub[2];
        *values++ += fact * sub[5];
        *values++ += fact * sub[8];

        if(row == col){
                r1 = row*3;
                r2 = r1+1;
                r3 = r2+1;
                matrix->m_D[r1] += (fact * sub[0]);
                matrix->m_D[r2] += (fact * sub[4]);
                matrix->m_D[r3] += (fact * sub[8]);
        }

        if(  (fabs(sub[1]-sub[3]) > .1f)
                || (fabs(sub[6]-sub[2]) > .1f)
                || (fabs(sub[7]-sub[5]) > .1f)
                || (fabs(sub[0]) < 1e-10f)
                || (fabs(sub[4]) < 1e-10f)
                || (fabs(sub[8]) < 1e-10f) )
		{
			ASSERT(FALSE);
			TRACE("Submatrix problem\n");
			return FALSE;
//                printf("\nWarning, matrix problem row=%li  col=%li",row,col);
  //              mat_disp(sub,"sub-array values",3);
    //            getch();
        }

        return(TRUE);
}
/* end pmat_insert */


int CClose::pmat_dump(struct Lin_eq *matrix, unsigned long num) /* dump packed matrix to file */
{
/*
        float value;
        long    ic;
        unsigned        long    ix, iy, ia;
        FILE    *fil;

        fil = fopen("OUTMATR.LDP","wa");

  for (iy=0 ; iy<num * 3 ; iy++)
  {
                
                for(ix=0 ; ix < num * 3 ; ix++){
                        value = 0.0f;
                        ic = -1;
                        for(ia=matrix->m_RI[iy] ; ia<matrix->m_RI[iy+1] && ic < 0 ; ia++){
                                if(matrix->m_C[ia] == ix) ic = ia;
                        }
                        if(ic >= 0) value = matrix->m_A[ic];
                        fprintf(fil,"%12.6f",value);
                }
                fprintf(fil,"\n");
        }
        fclose(fil);
	*/
        return((int) 1);
}
/* end pmat_dump */


int CClose::vect_dump(float *from,
              unsigned long size)       /* dump packed matrix to file */
{
/*
        unsigned        long    iy;
        FILE    *fil;

        fil = fopen("OUTVECT.LDP","wa");

  for(iy=0 ; iy<size ; iy++) fprintf(fil,"%10.5f\n",*from++);

        fclose(fil);
		*/
        return((int) 1);
}
/* end vect_dump */

int CClose::build_matrix( CSurveyShot **BigShots
                , unsigned long NumBigShots
                , CNode  **node_tbl)      /* build matrix to solve for station coordinates */
{
    ASSERT(AfxCheckMemory());    
	unsigned long
                                BigLegNum,      LittleLegNum,                   NodeA,  NodeB, NodeT,   count,
                                BigNodeNumTmp_A,                BigNodeNumTmp_B,
                                LittleNodeNumTmp_A, LittleNodeNumTmp_B,
                                ix,    KnownA,   KnownB, OpenA, OpenB,
                                newvar,                    mode,
                                *useds,         *usedn, reverse,
                                *useds_tmp,     *usedn_tmp, num_junct, num_junct_conn, little_count,
              last_from,  ConFlag;
        long  *ORDtmp, *order;

        CSurveyShot      *BigLegTmp, *LittleLegTmp;
        CNode     *BigNodeTmp_A, *BigNodeTmp_B, *BigNodeTmp_T;
        CNode     *LittleNodeTmp_A, *LittleNodeTmp_B, *LittleNodeTmp_T;

        float   kmat[9],        dp[3], LittleCmat[9], LittleRHS_Tmp[3], lmat[9];

        float
                len,                            thd,                            inc,
                vect[3],
                sdlen,                  sdazi,
                sdinc,                  a,b,c,d,w1,w2,w3;

        /* solver storage pointers */
        float                                           **svv_xp
                                                                , *svv_rhs;
        float                                           *svv_D
                                                                ,       *svv_a;
        unsigned        long    *svv_c;
        unsigned long           *svv_ri;

        /* solver linear equation pointers
                 - don't want to reference through structure all the time */
        svv_xp  = svv_big.m_ANS;
        svv_rhs = svv_big.m_B;
        svv_D           =       svv_big.m_D;
        svv_a           =       svv_big.m_A;
        svv_c           =       svv_big.m_C;
        svv_ri  = svv_big.m_RI;

        /* initialize stuff */
        sdlen = 0.02f;
        sdazi = 2.0f * deg_rad;
        sdinc = 2.0f * deg_rad;

        /* get good order to process BigShots */
		TRACE("BM1\n");
		ASSERT(AfxCheckMemory());
        useds     = (unsigned long*) malloc(NumBigShots * sizeof(long));
        useds_tmp = useds;
		TRACE("BM1a\n");
        for(count=0 ; count<NumBigShots ; count++ )
			*(useds_tmp++) = 0;
        useds_tmp = useds;

        usedn     = (unsigned long*) malloc(BigNumNodes * sizeof(long));
        usedn_tmp = usedn;
        for(count=0 ; count<BigNumNodes ; count++ )
			*(usedn_tmp++) = 0;
        usedn_tmp = usedn;
		TRACE("BM2\n");
        newvar = 1;
        order   = (long*) malloc(NumBigShots * sizeof(long));
        ORDtmp = order;
        mode = 0;  /* means find a new string start point */
                   /* 1=match NodeA 2=match NodeB 3=match either */
        NodeA = 0;
        NodeB = 0;
        num_junct = 0;
        num_junct_conn = 0; 
		TRACE("BM3\n");
        for(count = 0 ; (count<NumBigShots) && newvar>0 ; )
		{
                newvar = 0;

                /* scan through BigShots & look at those that aren't yet processed */
                for(BigLegNum = 0 ; BigLegNum < NumBigShots ; BigLegNum++)
				{

                        /* is not yet processed? */
                        if(useds[BigLegNum] == 0){

                                BigLegTmp = BigShots[BigLegNum];
                                BigNodeNumTmp_A = BigNodeNum_A[BigLegNum];
                                BigNodeNumTmp_B = BigNodeNum_B[BigLegNum];
                                BigNodeTmp_A = BigLegTmp->m_FromNode;
                                 BigNodeTmp_B = BigLegTmp->m_ToNode;

                                /* when mode == 0, we can find any node that's known
                                         and it's the start of a new string otherwise we find
                                         a shot containing the last node used */
                                NodeT = 999999999;
                                KnownA = (usedn[BigNodeNumTmp_A] > 0)
                                             || (BigNodeTmp_A->m_Position.m_iNodeType == 1);
                                KnownB = (usedn[BigNodeNumTmp_B] > 0)
                                             || (BigNodeTmp_B->m_Position.m_iNodeType == 1);
                                OpenA  =( BigNodeTmp_A->m_Position.m_iTotalOfAllShots
                                                    - usedn[BigNodeNumTmp_A]) > 0;
                                OpenB  =( BigNodeTmp_B->m_Position.m_iTotalOfAllShots
                                                    - usedn[BigNodeNumTmp_B]) > 0;

        /* mode=0 says can connect to either node in this shot
                                          so long as it qualifies */
                                if(mode == 0){
                                        if(KnownA &&  OpenA){
                                                NodeT = BigNodeNumTmp_A;
                                                mode = 2;
                                                reverse = 0;
                                                if( ( BigNodeTmp_B->m_Position.m_iTotalOfAllShots
                                                    - usedn[BigNodeNumTmp_B]) == 1 ) mode = 0;
                                        }
                                        else{
                                                if(KnownB &&  OpenB){
                                                        NodeT = BigNodeNumTmp_B;
                                                        mode = 1;
                                                        reverse = 1;
                                                        if( ( BigNodeTmp_A->m_Position.m_iTotalOfAllShots
                                                            - usedn[BigNodeNumTmp_A]) == 1 ) mode = 0;
                                                }
                                        }
                                }
                                else{

          /* mode=1|3 says can connect to prev node A in this shot */
                                        if(mode == 1 || mode == 3){
                                          if((NodeA == BigNodeNumTmp_A) && OpenA){
                                                        NodeT = BigNodeNumTmp_A;
                                                        mode = 2;
                                                        reverse = 0;
                                                        if( ( BigNodeTmp_B->m_Position.m_iTotalOfAllShots
                                                            - usedn[BigNodeNumTmp_B]) == 1 ) mode = 0;
                                                }
                                                else{
                                                  if((NodeA == BigNodeNumTmp_B) && OpenB){
                                                                NodeT = BigNodeNumTmp_B;
                                                                mode = 1;
                                                                reverse = 1;
                                                                if( ( BigNodeTmp_A->m_Position.m_iTotalOfAllShots
                                                                    - usedn[BigNodeNumTmp_A]) == 1 ) mode = 0;
                                                        }
                                                }
                                        }

          /* mode=2|4 says can connect to node B in this shot */
                                        if( (mode == 2 || mode == 4) && NodeT == 999999999){
                                          if((NodeB == BigNodeNumTmp_A) && OpenA){
                                                        NodeT = BigNodeNumTmp_A;
                                                        mode = 2;
                                                        reverse = 0;
                                                        if( ( BigNodeTmp_B->m_Position.m_iTotalOfAllShots
                                                            - usedn[BigNodeNumTmp_B]) == 1 ) mode = 0;
                                                }
                                                else{
                                                  if((NodeB == BigNodeNumTmp_B) && OpenB){
                                                                NodeT = BigNodeNumTmp_B;
                                                                mode = 1;
                                                                reverse = 1;
                                                                if( ( BigNodeTmp_A->m_Position.m_iTotalOfAllShots
                                                                    - usedn[BigNodeNumTmp_A]) == 1 ) mode = 0;
                                                        }
                                                }
                                        }
                                }

                                if(NodeT < 999999999){

                                        *(ORDtmp++) = (reverse ? -1 : 1) * (BigLegNum+1);
                                        newvar++;count++;
                                        useds[BigLegNum] = 1;

                                        /* Count the number of junction nodes - all nodes with other
                                           than two connections are considered junctions. Ie. dead ends,
                                           constrained nodes - this'll count the connections twice,
                                           (one from each end, so we'll divide by two later */
                                        if(  (  (BigNodeTmp_A->m_Position.m_iTotalOfAllShots != 2)
                                             || (BigNodeTmp_A->m_Position.m_iNodeType == 1) )
                                          && (usedn[BigNodeNumTmp_A] == 0) ){
                                                num_junct += 1;
                                                num_junct_conn += BigNodeTmp_A->m_Position.m_iTotalOfAllShots;
                                        }

                                        /* mark from nodes as used */
                                        usedn[BigNodeNumTmp_A] += 1;

                                        if(  (  (BigNodeTmp_B->m_Position.m_iTotalOfAllShots != 2)
                                             || (BigNodeTmp_B->m_Position.m_iNodeType == 1) )
                                          && (usedn[BigNodeNumTmp_B] == 0) ){
                                                num_junct += 1;
                                                num_junct_conn += BigNodeTmp_B->m_Position.m_iTotalOfAllShots;
                                        }

                                        /* mark nodes as used */
                                        usedn[BigNodeNumTmp_B] += 1;
                                        NodeB = BigNodeNumTmp_B;
                                        NodeA = BigNodeNumTmp_A;
                                }
                        }
                }
        }
		TRACE("BM4\n");
        free(useds);

        if(count < NumBigShots){
			TRACE("count<NumBigShots\n");
			ASSERT(FALSE);
                free(order);
                return(FALSE);
        }
        num_junct_conn = num_junct_conn / 2;
		TRACE("BM5\n");
        /* set up little shot table (just defines the connections) */
        LittleShots     = (CSurveyShot **) 
                           malloc( num_junct_conn       * sizeof(void*));
        little_shot_info = (CSurveyShot *) 
                           malloc( num_junct_conn       * sizeof(CSurveyShot));

        ORDtmp = order;
        mode = 0;  /* start with looking for a from node */
        little_count = 0;
		TRACE("BM6\n");
        for(count=0 ; count<NumBigShots ; count++){

                last_from = 0;
                BigLegNum = (*ORDtmp) < 0 ? -(*ORDtmp) - 1 : (*ORDtmp) - 1;
                ORDtmp++;
                BigLegTmp = BigShots[BigLegNum];
                BigNodeTmp_A = BigLegTmp->m_FromNode;
                BigNodeTmp_B = BigLegTmp->m_ToNode;

                if(mode == 0){
                        if(  (BigNodeTmp_A->m_Position.m_iTotalOfAllShots != 2)
                          || (BigNodeTmp_A->m_Position.m_iNodeType == 1) ){
                                (little_shot_info+little_count)->m_FromNode = BigNodeTmp_A;
                                LittleShots[little_count] = little_shot_info + little_count;
                                mode = 1;
                                last_from = 1;
                        }
                        else{
                                if(  (BigNodeTmp_B->m_Position.m_iTotalOfAllShots != 2)
                                  || (BigNodeTmp_B->m_Position.m_iNodeType == 1) ){
                                        (little_shot_info+little_count)->m_FromNode = BigNodeTmp_B;
                                        LittleShots[little_count] = little_shot_info + little_count;
                                        mode = 1;
                                        last_from = 2;
                                }
                        }
                }

                /* a single shot from one junction to another or to itself
                   we must make sure we don't
                   use the same pointer twice (last_from test)  */
                if(mode == 1){
                        if(  (  (BigNodeTmp_A->m_Position.m_iTotalOfAllShots != 2)
                             || (BigNodeTmp_A->m_Position.m_iNodeType == 1) )
                          && last_from != 1){
                                (little_shot_info+little_count)->m_ToNode = BigNodeTmp_A;
                                mode = 0;
                                little_count++;
                        }
                        else{
                                if(  (  (BigNodeTmp_B->m_Position.m_iTotalOfAllShots != 2)
                                     || (BigNodeTmp_B->m_Position.m_iNodeType == 1) )
                                  && last_from != 2){
                                        (little_shot_info+little_count)->m_ToNode = BigNodeTmp_B;
                                        mode = 0;
                                        little_count++;
                                }
                        }
                }
        }
		TRACE("BM7\n");

        /* set up little problem */
        /* allocates array, sets LittleNumNodes, fills shot_fr(to)_index,
           fills array with pointers */
        LittleNodes = build_node_table( LittleShots, little_count
                                       , &LittleNodeNum_A, &LittleNodeNum_B
                                       , &LittleNumNodes);

        /* allocates little solver arrays, initializes row & column indices */
  init_Lineq( &svv_little, LittleNodes, LittleNodeNum_A, LittleNodeNum_B
                  , little_count, LittleNumNodes);

        /* TACO doesn't need this it just stores an efficient plot order */
//        for(count=0 ; count<NumBigShots ; count++)
  //              PlotOrder[count] = order[count];

        /* process all shots in the preferred order */
        usedn_tmp = usedn;
        for(count=0 ; count<BigNumNodes ; count++ ) *(usedn_tmp++) = 0;
        usedn_tmp = usedn;

		TRACE("BM7\n");

        mode = 0; /* means we are starting a new little_leg */
        LittleLegNum = 0;
        ORDtmp = order;
        for(count=0 ; count<NumBigShots ; count++){

                BigLegNum       = (*ORDtmp) < 0 ? -(*ORDtmp) - 1 : (*ORDtmp) - 1;
                ORDtmp++;
                BigLegTmp       = BigShots[BigLegNum];
                BigNodeNumTmp_A = BigNodeNum_A[BigLegNum];
                BigNodeNumTmp_B = BigNodeNum_B[BigLegNum];
                BigNodeTmp_A    = BigLegTmp->m_FromNode;
                BigNodeTmp_B    = BigLegTmp->m_ToNode;

                /* initialize and check which bignode corresponds
                   to one of the little nodes */
                if(mode == 0){

                        LittleLegTmp       = LittleShots[LittleLegNum];
                        LittleNodeNumTmp_A = LittleNodeNum_A [LittleLegNum];
                        LittleNodeNumTmp_B = LittleNodeNum_B [LittleLegNum];
                        LittleNodeTmp_A    = LittleLegTmp->m_FromNode;
                        LittleNodeTmp_B    = LittleLegTmp->m_ToNode;
                        ConFlag = 0;
                        LittleNodeTmp_T =  (LittleNodeTmp_A == BigNodeTmp_A)
                                        || (LittleNodeTmp_A == BigNodeTmp_B)
                                        ?  LittleNodeTmp_B
                                        :  LittleNodeTmp_A;
                        BigNodeTmp_T =  (LittleNodeTmp_A == BigNodeTmp_A)
                                     || (LittleNodeTmp_B == BigNodeTmp_A)
                                     ?  BigNodeTmp_B
                                     :  BigNodeTmp_A;

                        ConFlag =  (LittleNodeTmp_A == BigNodeTmp_A)
                                || (LittleNodeTmp_B == BigNodeTmp_A)
                                ?  1
                                :  (  (LittleNodeTmp_A == BigNodeTmp_B)
                                   || (LittleNodeTmp_B == BigNodeTmp_B)
                                   ?  2
                                   :  0  );  /* says that we must connect to second big shot */

                        if(ConFlag == 0){
							ASSERT(FALSE);
//                                printf("\nWhoops! bad junction configuration Hari Kari!");
  //                              getch(); /* TACO doesn't need this */
    //                            exit(1);
                        }

                        /* Initialize string submatrix information */
                        LittleCmat [0]   = 0.0f;
                        LittleCmat [1]   = 0.0f;
                        LittleCmat [2]   = 0.0f;
                        LittleCmat [3]   = 0.0f;
                        LittleCmat [4]   = 0.0f;
                        LittleCmat [5]   = 0.0f;
                        LittleCmat [6]   = 0.0f;
                        LittleCmat [7]   = 0.0f;
                        LittleCmat [8]   = 0.0f;
                        LittleRHS_Tmp[0] = 0.0f;
                        LittleRHS_Tmp[1] = 0.0f;
                        LittleRHS_Tmp[2] = 0.0f;
                        mode = 1; /* means we are now looking for the end of little leg */
                }
                else{

                        BigNodeTmp_T =  (BigNodeTmp_T == BigNodeTmp_A)
                                     || (BigNodeTmp_T == BigNodeTmp_A)
                                     ?  BigNodeTmp_B
                                     :  BigNodeTmp_A;
                }

                /* setup stuff */
                len             = BigLegTmp->GetCalculationLength();
                thd             = (90.0f - BigLegTmp->GetCalculationCompass()) * deg_rad;
                thd             = thd >  pi
                                        ? thd - 2.0f * pi
                                        : (thd < -pi ? (thd + 2.0f * pi) : thd);
                inc             = BigLegTmp->GetCalculationClino() * deg_rad;
                a       = (float) cos(thd); c = (float) sin(thd);
                b       = (float) cos(inc); d = (float) sin(inc);

                /* direct calculation of stiffness matrix */
//TVI
				w1=1.0f/BigLegTmp->GetLeg()->GetStandardTapeError();
				w2=1.0f/(BigLegTmp->GetLeg()->GetStandardCompassError()*deg_rad);
				w3=1.0f/(BigLegTmp->GetLeg()->GetStandardClinoError()*deg_rad);
//                w1 = 1.0f / (len * sdlen + .05f);
  //              w2 = 1.0f / (sdazi * (len < 0.1f ? 0.1f : len));
    //            w3 = 1.0f / (sdinc * (len < 0.1f ? 0.1f : len));

                kmat[0] =  w1 * a * a * b * b
                                                +  w2 * c * c
                                                +  w3 * a * a * d * d;
                kmat[1] =  a * c *  ( w1 * b * b
                                                        - w2
                                                                                                + w3 * d * d );
                kmat[2] =  (w1 - w3) * a * b * d;
                dp[0]           =  len   * a * b;                                       /* dx to-from */

                kmat[4] = w1 * b * b * c * c
                                                + w2 * a * a
                                                + w3 * c * c * d * d;
                kmat[5] = (w1 - w3) * b * c * d;
                dp[1]           =  len   * c * b;                                       /* dy to-from */

                kmat[8] = w1 * d * d
                                                + w3 * b * b;
                dp[2]           =  len * d;                                                             /* dz to-from */

                kmat[3] =  kmat[1];
                kmat[6] =  kmat[2];
                kmat[7] =  kmat[5];

/*      mat_disp(&(kmat[0][0]),"kmat",3);*/

                /* add submatrix into packed solver matrix */
                pmat_insert( &svv_big, &(kmat[0])
                           , BigNodeNumTmp_A, BigNodeNumTmp_A, 1.0f);
                pmat_insert( &svv_big, &(kmat[0])
                           , BigNodeNumTmp_B, BigNodeNumTmp_B, 1.0f);
                pmat_insert( &svv_big, &(kmat[0])
                           , BigNodeNumTmp_A, BigNodeNumTmp_B,-1.0f);
                pmat_insert( &svv_big, &(kmat[0])
                           , BigNodeNumTmp_B, BigNodeNumTmp_A,-1.0f);

                /* set up direct estimate of unknown node position (one is always known
                   with the ordering used) if not constrained
                   (this does little problem too!!) */
                if(  (usedn[BigNodeNumTmp_B] == 0)
                  && (BigNodeTmp_B->m_Position.m_iNodeType != 1) ){

                        BigNodeTmp_B->m_Position.m_ClosedPosition.m_Items[0]
                                = BigNodeTmp_A->m_Position.m_ClosedPosition.m_Items[0] + dp[0];
                        BigNodeTmp_B->m_Position.m_ClosedPosition.m_Items[1]
                                = BigNodeTmp_A->m_Position.m_ClosedPosition.m_Items[1] + dp[1];
                        BigNodeTmp_B->m_Position.m_ClosedPosition.m_Items[2]
                                = BigNodeTmp_A->m_Position.m_ClosedPosition.m_Items[2] + dp[2];
						//Added by taco
						BigNodeTmp_B->m_Position.m_ClosedPosition.m_Items[3]=1.0f;
                }
                else
				{
                        if((usedn[BigNodeNumTmp_A] == 0) && (BigNodeTmp_A->m_Position.m_iNodeType != 1)){

                                BigNodeTmp_A->m_Position.m_ClosedPosition.m_Items[0]
                                        = BigNodeTmp_B->m_Position.m_ClosedPosition.m_Items[0] - dp[0];
                                BigNodeTmp_A->m_Position.m_ClosedPosition.m_Items[1]
                                        = BigNodeTmp_B->m_Position.m_ClosedPosition.m_Items[1] - dp[1];
                                BigNodeTmp_A->m_Position.m_ClosedPosition.m_Items[2]
                                        = BigNodeTmp_B->m_Position.m_ClosedPosition.m_Items[2] - dp[2];
								//By taco
								BigNodeTmp_A->m_Position.m_ClosedPosition.m_Items[3]=1.0f;
                        }
                }
                usedn[BigNodeNumTmp_A] += 1;
                usedn[BigNodeNumTmp_B] += 1;

                /* invert stiffness matrix and add to little string submatrix */
                mat_inv(lmat,kmat,3);
                for(ix=0 ; ix<9 ; ix++) LittleCmat[ix] += lmat[ix];
/*mat_disp(LittleCmat,"LittleCmat build",3);*/

                /* add deltas to little string rhs */
                if(BigNodeTmp_T == BigNodeTmp_B){
                        LittleRHS_Tmp[0] += dp[0];
                        LittleRHS_Tmp[1] += dp[1];
                        LittleRHS_Tmp[2] += dp[2];
                }
                else
				{
                        LittleRHS_Tmp[0] -= dp[0];
                        LittleRHS_Tmp[1] -= dp[1];
                        LittleRHS_Tmp[2] -= dp[2];      
                }

                /* set up right hand sides */
                mat_x_vect(vect,&(kmat[0]),dp,3);
                vect_add( &(svv_rhs[BigNodeNumTmp_A*3])
                        , &(svv_rhs[BigNodeNumTmp_A*3]),vect,3,2);
                vect_add( &(svv_rhs[BigNodeNumTmp_B*3])
                        , &(svv_rhs[BigNodeNumTmp_B*3]),vect,3,0);

                /* decide if we've arrived at the next junction */
/*printf("\nconflg=%lu ",ConFlag);
printf("\nlita=%lu litb=%lu",LittleNodeTmp_A,LittleNodeTmp_B);
printf("\nbiga=%lu bigb=%lu",BigNodeTmp_A,BigNodeTmp_B);
getch();*/

                if(  ((ConFlag == 1) && (LittleNodeTmp_T == BigNodeTmp_B))
                  || ((ConFlag == 2) && (LittleNodeTmp_T == BigNodeTmp_A))
                  || (  (ConFlag == 0)
                     && (  (LittleNodeTmp_T == BigNodeTmp_B)
                        || (LittleNodeTmp_T == BigNodeTmp_A) ) ) )
				{

                        /* invert little submatrix and insert it */
                        mat_inv(LittleCmat,LittleCmat,3);
/*mat_disp(LittleCmat,"LittleCmat",3);*/

                        /* add little submatrix into packed solver matrix */
                        pmat_insert( &svv_little, LittleCmat
                                   , LittleNodeNumTmp_A, LittleNodeNumTmp_A, 1.0f);
                        pmat_insert( &svv_little, LittleCmat
                                   , LittleNodeNumTmp_B, LittleNodeNumTmp_B, 1.0f);
                        pmat_insert( &svv_little, LittleCmat
                                   , LittleNodeNumTmp_A, LittleNodeNumTmp_B,-1.0f);
                        pmat_insert( &svv_little, LittleCmat
                                   , LittleNodeNumTmp_B, LittleNodeNumTmp_A,-1.0f);

                        /* set up right hand sides */
                        mat_x_vect(LittleRHS_Tmp, LittleCmat, LittleRHS_Tmp, 3);
                        vect_add( &(svv_little.m_B[LittleNodeNumTmp_A*3])
                                , &(svv_little.m_B[LittleNodeNumTmp_A*3])
                                , LittleRHS_Tmp, 3, 2);
                        vect_add( &(svv_little.m_B[LittleNodeNumTmp_B*3])
                                , &(svv_little.m_B[LittleNodeNumTmp_B*3])
                                , LittleRHS_Tmp, 3, 0);

                        /* indicate that we have finished with this string */
                        LittleLegNum++;
                        mode = 0;
                } 
                ConFlag = 0; /* says that we can connect to either big node now */

        }

        free(order);
        free(usedn);

        return(TRUE); 
}

/* end build_matrix */



int CClose::constr_equ( struct Lin_eq *matrix
             , unsigned long NumNodes
             , CNode  **node_tbl
             , int level)       /* build matrix to solve for station coordinates */
{
        unsigned long
                                node, *col0, *col1, node3, ix;

        CNode     *NodeP;

        float           *val0,          *val1,          *val2;

        /* solver storage pointers */
        float                                           **svv_xp
                                                                , *svv_rhs;
        float                                           *svv_D
                                                                ,       *svv_a;
        unsigned        long    *svv_c;
        unsigned long           *svv_ri;

        /* solver linear equation pointers
                 - don't want to reference through structure all the time */
        svv_xp  = matrix->m_ANS;
        svv_rhs = matrix->m_B;
        svv_D           =       matrix->m_D;
        svv_a           =       matrix->m_A;
        svv_c           =       matrix->m_C;
        svv_ri  = matrix->m_RI;

        /* process all constrained nodes */
        for(node = 0 ; node < NumNodes ; node++)
		{

                node3 = node*3;
                NodeP = node_tbl[node];

                /* set up pointers to answers */
                *(svv_xp+node3)   = &(NodeP->m_Position.m_ClosedPosition.m_Items[0]);
                *(svv_xp+node3+1) = &(NodeP->m_Position.m_ClosedPosition.m_Items[1]);
                *(svv_xp+node3+2) = &(NodeP->m_Position.m_ClosedPosition.m_Items[2]);

                if(  (NodeP->m_Position.m_iNodeType > 0)
                  && (NodeP->m_Position.m_iNodeType <= level) )
				{

                        /* set right hand sides to position values */
                        svv_rhs[node3]          = NodeP->m_Position.m_ClosedPosition.m_Items[0];
                        svv_rhs[node3+1]        = NodeP->m_Position.m_ClosedPosition.m_Items[1];
                        svv_rhs[node3+2]        = NodeP->m_Position.m_ClosedPosition.m_Items[2];

                        /* modify all right hand sides to restore symmetry */
                        /* scan through the three columns corresponding with "node" */

                        /* since matrix is symmetric, can use row values 
                                 rather than column values, in future we won't
                                 even store the lower triangular matrix
                                 "column" is the column index corresponding with "value" */

                        /* all 3 (rows)cols will have same */
                        col0 = svv_c + svv_ri[node3]; 
                        col1 = svv_c + svv_ri[node3+1];
                        val0 = svv_a + svv_ri[node3];
                        val1 = svv_a + svv_ri[node3+1];
                        val2 = svv_a + svv_ri[node3+2];

                        /* test for col allows us to get only elements for this row */
                        for( ; col0 < col1 ; col0++)
						{

                                /* don't adjust rows belonging to "node" as it must stay put */
                                if((*col0 < node3) || (*col0 >= (node+1)*3))
								{
                                        svv_rhs[*col0]  -= *val0 * svv_rhs[node3];
                                        svv_rhs[*col0]  -= *val1 * svv_rhs[node3+1];
                                        svv_rhs[*col0]  -= *val2 * svv_rhs[node3+2];
                                        *val0++ = 0.0f;
                                        *val1++ = 0.0f;
                                        *val2++ = 0.0f;
                                }
                                else
								{
                                        *val0++ = *col0 == node3   ? 1.0f : 0.0f;
                                        *val1++ = *col0 == node3+1 ? 1.0f : 0.0f;
                                        *val2++ = *col0 == node3+2 ? 1.0f : 0.0f;
                                        svv_D[node3]   = 1.0f;
                                        svv_D[node3+1] = 1.0f;
                                        svv_D[node3+2] = 1.0f;
                                }
                        }

                        /* eliminate other values in matrix corresponding to node */
                        for(ix=0 ; ix<NumNodes*3 ; ix++)
						{

                                /* don't change values in rows belonging to node */
                                if((ix < node3) || (ix > node3+2))
								{
                                        col0 = svv_c + svv_ri[ix];
                                        col1 = svv_c + svv_ri[ix+1];
                                        val0 = svv_a + svv_ri[ix];
                                        for( ; col0 < col1 ; )
										{
                                                if((*col0 >= node3) && (*col0 < node3+3)) *val0 = 0.0f;
                                                col0++; val0++;
                                        }
                                }
                        }
                }
        }
	return TRUE;
}

/*----------*/
int CClose::solve       ( struct Lin_eq *matrix
                                        ,       unsigned long   numeq ) /* conjugate-gradient matrix solver */
{
		ASSERT(AfxCheckMemory());
        float   s1, s2, alpha, tol;
        unsigned long   iy, k, i1, i2, limit, *RItmp, *Ctmp, ia;

        float   *Dtmp, *X, *Xtmp, curr_D, *m_Btmp, *Atmp, *P, *Q, *Ptmp,
                                *Qtmp, dmax,    tmp;

        float   **m_ANS,        *m_B,   *m_D,   *m_A;
        unsigned long   *m_C,   *m_RI;

        /* set up stuff - don't want to use structure reference after here */
        m_ANS   = matrix->m_ANS;
        m_B             = matrix->m_B;
        m_D             =       matrix->m_D;
        m_A             =       matrix->m_A;
        m_C             =       matrix->m_C;
        m_RI    = matrix->m_RI;

        X                       = (float*)      malloc(numeq * sizeof(float));
        Q                       = (float*)      malloc(numeq * sizeof(float));
        P                       = (float*)      malloc(numeq * sizeof(float));
        Ptmp  = P;
        Xtmp  = X;
        Dtmp  = m_D;
        m_Btmp  = m_B;

        /* normalize diagonal and answer vectors */
        for (iy=0 ; iy<numeq ; iy++) {
                *Dtmp = (float)sqrt(*Dtmp);
                *(Xtmp++) = *(m_ANS[iy]) * (*Dtmp);

                *m_Btmp = *m_Btmp / (*Dtmp++);
                m_Btmp++;
        }

        /* each row */
        Atmp  = m_A;
        Ctmp  = m_C;
        RItmp = m_RI;
        m_Btmp  = m_B;
        Qtmp  = Q;
        Ptmp    = P;
        s1 = 0.0f;
        for (iy=0 ; iy<numeq ; iy++){

                curr_D = m_D[iy];

                i1 = *RItmp;
                i2 = *((RItmp++)+1);
                if (i1 != i2) {

                        /* each A element */
                        for (ia=i1 ; ia < i2 ; ia++){

                                *Atmp /= (curr_D * m_D[*Ctmp]);
                                *m_Btmp -= *(Atmp++) * X[*(Ctmp++)];
                        }
                        *(Ptmp++) = *m_Btmp;
                        s1 = s1 + ((*m_Btmp) * (*(m_Btmp)));
                        m_Btmp++;
                }
        }

        /* solve the matrix */
        tol = 0.001f;
        s2=1.0f;
        limit = numeq+1;

        for (k=0 ; (k<limit) && (s2 > tol); k++){
                alpha = 0.0f;

                RItmp = m_RI;
                Qtmp  = Q;
                Ptmp  = P;
                Atmp  = m_A;
                Ctmp  = m_C;
                for (iy=0 ; iy<numeq ; iy++) {

                        i1 = *RItmp;
                        i2 = *((RItmp++)+1);
                        if (i1 != i2) {

                                /* each A element */
                                *Qtmp = 0.0f;      /* *Ptmp;  zero ??? */
                                for (ia=i1 ; ia < i2 ; ia++) *Qtmp += *(Atmp++) * P[*(Ctmp++)];
                                alpha += *(Qtmp++) * (*(Ptmp++));
                        }
                }
                alpha = s1 / (alpha+1e-20f);

                s2 = 0.0f;
                Xtmp = X;
                Ptmp = P;
                m_Btmp = m_B;
                Qtmp = Q;
                Dtmp = m_D;
                dmax = 0.0f;
                for (iy=0 ; iy<numeq ; iy++){
                        tmp = alpha * (*(Ptmp++));
                        dmax = fabs(tmp / (*Dtmp)) > fabs(dmax)
                                                        ? tmp / (*Dtmp) : dmax ;
                        Dtmp++;
                        *(Xtmp++) += tmp;
                        *m_Btmp     -= alpha * (*(Qtmp++));
                        s2        += *m_Btmp * (*m_Btmp);
                        m_Btmp++;
                }

                alpha = s2 / (s1+1e-20f);
                s1 = s2;
                Ptmp = P;
                m_Btmp = m_B;
                for (iy=0 ; iy<numeq ; iy++){
                        *Ptmp = *(m_Btmp++) + alpha * (*Ptmp);
                        *Ptmp = fabs(*Ptmp)<1e-15f ? 1e-15f : *Ptmp;
                        Ptmp++;
                }

//printf("\nEnd Solver iter %li s2=%16.6f  delta=%16.6f",
  //                      k,s2,dmax);
        }

        /* un-normalize solution */
        Xtmp = X;
        Dtmp = m_D;
        for (iy=0 ; iy<numeq ; iy++){
                *(m_ANS[iy]) = *(Xtmp++) / *(Dtmp++);
        }

        free(X);
		free(P);
		free(Q);
        if(k == limit)
		{
			TRACE("\nBad News!!");
            ASSERT(FALSE);
			return FALSE;
        }
		ASSERT(AfxCheckMemory());

        return(TRUE);
}
/* end solve */


/*----------*/
int CClose::init_Lineq( struct Lin_eq  *matrix
              , CNode  **nodes
              , unsigned long *shot_fr
              , unsigned long *shot_to
              , unsigned long numshots
              , unsigned long numnodes)
{
	TRACE("InitLineq %li shots %li nodes\n",numshots,numnodes);
	ASSERT(AfxCheckMemory());
	unsigned long   ix, nx, na,     *ri,    nc,
                      node1, node2,     r1,     r2,
                      r3,       elem, ia, numeq;

        nx = numnodes * 3;
        na = 9 * numnodes + 9 * numshots * 2; //+100 fudge factor by taco;
        numeq = numnodes * 3;

        matrix->m_ANS   = (float**)     malloc(nx * sizeof(float*));
        matrix->m_B             = (float*)      malloc(nx * sizeof(float));
        matrix->m_RI    = (unsigned long*)               malloc((nx+1) * sizeof(long));
        matrix->m_D     = (float*)      malloc(nx * sizeof(float));

        matrix->m_A     = (float*)      malloc(na * sizeof(float));
        matrix->m_C     = (unsigned long*)               malloc(na * sizeof(long));

//    ASSERT(AfxCheckMemory());    
	  if(matrix->m_C == 0L){
		  TRACE("Out of memory in solver");
			return(FALSE);
        }

        for(ix=0 ; ix < nx ; ix++){
                *(matrix->m_ANS+ix) = 0;
                *(matrix->m_B+ix) = 0.0f;
                *(matrix->m_D+ix) = 0.0f;
        }

//	    ASSERT(AfxCheckMemory());    
        for(ix=0 ; ix < na ; ix++){
                matrix->m_A[ix] = 0.0f;
                matrix->m_C[ix] = (unsigned long)-1;  /* ??? this might be a glitch with some geoms */
        }

        /* Initialize matrix row indices and diagonal column indices */
        elem = 0;
        ri = matrix->m_RI;
//        ASSERT(AfxCheckMemory());    
	    for(ix=0 ; ix<numnodes ; ix++){

                nc = 3 * (1 + (*(nodes++))->m_Position.m_iTotalOfAllShots);

                matrix->m_C[elem]               = 3*ix;
                matrix->m_C[elem+1]     = 3*ix+1;
                matrix->m_C[elem+2]     = 3*ix+2;
                *ri++ = elem; elem += nc;
                matrix->m_C[elem]               = 3*ix;
                matrix->m_C[elem+1]     = 3*ix+1;
                matrix->m_C[elem+2]     = 3*ix+2;
                *ri++ = elem; elem += nc;
                matrix->m_C[elem]               = 3*ix;
                matrix->m_C[elem+1]     = 3*ix+1;
                matrix->m_C[elem+2]     = 3*ix+2;
                *ri++ = elem; elem += nc;
        }
//        ASSERT(AfxCheckMemory());    
	    *ri = elem; /* one extra value marks end */

        /* Initialize column indices  for off-diagonal elements */
        for(ix=0 ; ix<numshots ; ix++)
		{
			node1 = 3 * shot_fr[ix];
                node2 = 3 * shot_to[ix];

                /* get first empty location in node 1 rows */
                r1 = matrix->m_RI[node1];
                r2 = matrix->m_RI[node1+1];
                r3 = matrix->m_RI[node1+2];
                for (ia=r2 ; (r1<ia) && (matrix->m_C[r1] != -1) ; r1++)
				{
					r2++;
					r3++;
				};

                matrix->m_C[r1]   = node2;
                matrix->m_C[r1+1] = node2+1;
                matrix->m_C[r1+2] = node2+2;

                matrix->m_C[r2]   = node2;
                matrix->m_C[r2+1] = node2+1;
                matrix->m_C[r2+2] = node2+2;

                matrix->m_C[r3]   = node2;
                matrix->m_C[r3+1] = node2+1;
                matrix->m_C[r3+2] = node2+2;
                /* get first empty location in node 2 rows */
                r1 = matrix->m_RI[node2];
                r2 = matrix->m_RI[node2+1];
                r3 = matrix->m_RI[node2+2];

               
				for(ia=r2 ; (r1<ia) && (matrix->m_C[r1] != -1) ; r1++)
					{r2++;r3++;};

                matrix->m_C[r1]   = node1;
                matrix->m_C[r1+1] = node1+1;
                matrix->m_C[r1+2] = node1+2;

                matrix->m_C[r2]   = node1;
                matrix->m_C[r2+1] = node1+1;
                matrix->m_C[r2+2] = node1+2;

                matrix->m_C[r3]   = node1;
                matrix->m_C[r3+1] = node1+1;
                matrix->m_C[r3+2] = node1+2;
        }
		ASSERT(AfxCheckMemory());
        return(TRUE);
}

/* end init_Lineq */

/*----------*/
void CClose::free_Lineq(struct Lin_eq *matrix)
{
		ASSERT(AfxCheckMemory());
        free(matrix->m_ANS);
        free(matrix->m_B);
        free(matrix->m_RI);
        free(matrix->m_D);
        free(matrix->m_A);
        free(matrix->m_C);
		ASSERT(AfxCheckMemory());
}

/* end free_matrix */


void    CClose::mark_solved(CNode  **NodeList
                 , unsigned long NumNodes
                 , int mark)
{
        unsigned long ix;
        for(ix=0 ; ix<NumNodes ; ix++)
                if((*(NodeList+ix))->m_Position.m_iNodeType == 0)
                        (*(NodeList+ix))->m_Position.m_iNodeType = mark;
}

CNode  
**  CClose::build_node_table( CSurveyShot **shots
          , unsigned long numshots
          , unsigned long **shot_fr_temp
          , unsigned long **shot_to_temp
          , unsigned long *solv_numnodes)
{
    TRACE("build_node_table %li shots\n",numshots);    
	unsigned long   ix, iy={0};
        CNode   **nodes, **node_tmp;
        CSurveyShot *shot;

        nodes = (CNode   **) malloc((1 + numshots) * sizeof(void*));
        *shot_fr_temp = (unsigned long*) malloc(numshots * sizeof(long*));
        *shot_to_temp = (unsigned long*) malloc(numshots * sizeof(long*));

        (*solv_numnodes) = 0;
        for(ix=0 ; ix<numshots ; ix++){
                shot = shots[ix];
                node_tmp = nodes;
                if((*solv_numnodes) > 0){
                        for(iy=0 ; iy < (*solv_numnodes) ; iy++)
                                if(shot->m_FromNode == *(node_tmp++)) break;
                }
                if(iy >= (*solv_numnodes)){
                        *(node_tmp) = shot->m_FromNode;
                        (*solv_numnodes)++;
                }
                (*shot_fr_temp)[ix] = iy;

                node_tmp = nodes;
                for(iy=0 ; iy < (*solv_numnodes); iy++)
                        if(shot->m_ToNode == *(node_tmp++)) break;
                if(iy >= (*solv_numnodes)){
                        *(node_tmp) = shot->m_ToNode;
                        (*solv_numnodes)++;
                }
                (*shot_to_temp)[ix] = iy;
        }
		return(nodes);
}



int     CClose::surv_loop_close( CSurveyShot **big_shots
                   , unsigned long numshots,int iStage)
{
	ASSERT(AfxCheckMemory());
	switch(iStage)
	{
	case 0:
        /* allocates array, sets BigNumNodes, fills shot_fr(to)_index,
           fills array with pointers */
        BigNodes = build_node_table( big_shots, numshots
                                    , &BigNodeNum_A, &BigNodeNum_B, &BigNumNodes);
		ASSERT(AfxCheckMemory());

        /* allocates big solver arrays, initializes row & column indices */
		if (!init_Lineq( &svv_big, BigNodes, BigNodeNum_A, BigNodeNum_B
                   , numshots, BigNumNodes))
		{
				   return FALSE;
		}
		ASSERT(AfxCheckMemory());
        /* fills big and little solver arrays */
        if (!build_matrix(big_shots, numshots, BigNodes))
			return FALSE;

        /* solve little array problem */
        if (!constr_equ( &svv_little, LittleNumNodes, LittleNodes, 2))
			return FALSE;
		break;
	case 1:
        if (!solve(&svv_little,      LittleNumNodes * 3))
			return FALSE;
		break;
	case 2:
        mark_solved(LittleNodes, LittleNumNodes, 2);
        /* apply constraints from little problem to big problem */
        if (!constr_equ( &svv_big, BigNumNodes, BigNodes, 3))
			return FALSE;
		break;
	case 3:
        /* solve big array problem */
        if (!solve(&svv_big, BigNumNodes * 3))
			return FALSE;
        mark_solved(BigNodes, BigNumNodes, 3);
			break;
	case 4:
        /* free big and little arrays */
        ASSERT(AfxCheckMemory());
		free_Lineq(&svv_big);
        free_Lineq(&svv_little);
        free(BigNodes);
        free(LittleNodes);
		break;
	}
	ASSERT(AfxCheckMemory());
    return(TRUE);
}

CClose::CClose(int iColorScheme)
{
	m_iColorScheme=iColorScheme;
};
