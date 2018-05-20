/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*   Description:  a library for finding the maximum clique of a graph     		   */                                                   
/*                                                                           		   */
/*                                                                           		   */
/*   Authors: Bharath Pattabiraman and Md. Mostofa Ali Patwary               		   */
/*            EECS Department, Northwestern University                       		   */
/*            email: {bpa342,mpatwary}@eecs.northwestern.edu                 		   */
/*                                                                           		   */
/*   Copyright, 2014, Northwestern University			             		   */
/*   See COPYRIGHT notice in top-level directory.                            		   */
/*                                                                           		   */
/*   Please site the following publication if you use this package:           		   */
/*   Bharath Pattabiraman, Md. Mostofa Ali Patwary, Assefaw H. Gebremedhin2, 	   	   */
/*   Wei-keng Liao, and Alok Choudhary.	 					   	   */
/*   "Fast Algorithms for the Maximum Clique Problem on Massive Graphs with           	   */
/*   Applications to Overlapping Community Detection"				  	   */
/*   http://arxiv.org/abs/1411.7460 		 					   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "findClique.h"


int getDegree(vector<int>* ptrVtx, int idx)
{
	return ( (*ptrVtx)[idx+1] - (*ptrVtx)[idx] );
}
