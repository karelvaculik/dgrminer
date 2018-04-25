/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*   Description:  an I/O library for reading a graph    			 	   */                                                   
/*                                                                           		   */
/*                                                                           		   */
/*   Authors: Md. Mostofa Ali Patwary and Bharath Pattabiraman             		   */
/*            EECS Department, Northwestern University                       		   */
/*            email: {mpatwary,bpa342}@eecs.northwestern.edu                 		   */
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

#include "graphIO.h"

CGraphIO::~CGraphIO()
{
	m_vi_Vertices.clear();
	m_vi_OrderedVertices.clear();
	m_vi_Edges.clear();
	m_vd_Values.clear();
}


void CGraphIO::CalculateVertexDegrees()
{
		int i_VertexCount = m_vi_Vertices.size() - 1;

		m_i_MaximumVertexDegree =  -1;
		m_i_MinimumVertexDegree = -1;

		for(int i = 0; i < i_VertexCount; i++)
		{
			int i_VertexDegree = m_vi_Vertices[i + 1] - m_vi_Vertices[i];

			if(m_i_MaximumVertexDegree < i_VertexDegree)
			{
				m_i_MaximumVertexDegree = i_VertexDegree;
			}

			if(m_i_MinimumVertexDegree == -1)
			{
				m_i_MinimumVertexDegree = i_VertexDegree;
			}
			else if(m_i_MinimumVertexDegree > i_VertexDegree)
			{
				m_i_MinimumVertexDegree = i_VertexDegree;
			}
		}

		m_d_AverageVertexDegree = (double)m_vi_Edges.size()/i_VertexCount;

		return;
}
