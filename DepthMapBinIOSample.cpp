// DepthMapBinIOSample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DepthMapBinFileIO.h"
#include <Windows.h>
//This is a sample program to load a depth video file (*.bin). Author: Zicheng Liu
//A depth video file consists of a sequence of depth maps. Each frame corresponds to one depth map.
//A depth map is a matrix of depth values (see CDepthMap in depthmap.h).
//I purposely did not create a data structure to hold all the frames of the depth video because it would result
//in large memory footprint.
int _tmain(int argc, _TCHAR* argv[])
{
	char depthFileName[] = "E:\\MSR-Action3D\\a01_s01_e01_sdepth.bin";
	FILE * fp;
	fopen_s(&fp,depthFileName, "rb");

	if(fp == NULL)
		return 1;




	int nofs = 0; //number of frames conatined in the file (each file is a video sequence of depth maps)
	int ncols = 0;
	int nrows = 0;
	ReadDepthMapBinFileHeader(fp, nofs, ncols, nrows);

	//三个视图
	float front[240][320];
	float side[1000][320];
	float top[240][1000];

	int f_x_left,f_x_right,f_y_top,f_y_bottom;
	int s_x_left,s_x_right,s_y_top,s_y_bottom;
	int t_x_left,t_x_right,t_y_top,t_y_bottom;

	printf("number of frames=%i\n", nofs);

	//read each frame
	int f; //frame index
	CDepthMap depthMap;
	depthMap.SetSize(ncols, nrows); //it allocates space


	ReadDepthMapBinFileNextFrame(fp, ncols, nrows, depthMap);
	//投影到三个视图上
	for(int i=0;i<nrows;i++)
	{
		for(int j=0;j<ncols;j++)
		{
			int temp=(int)depthMap.GetItem(i,j);
			if(temp>0)
			{
				//投影
				front[i][j]=1;
				top[temp][j]=1;
				side[i][temp]=1;

				for(int m=0;m<temp;m++)
					top[m][j]=1;
				for (int n=0;n<temp;n++)
					side[i][n]=1;

				//计算非零区域
				if(i<f_x_left)f_x_left=i;
				if (i>f_x_right)f_x_right=i;
				if(j<f_y_top)f_y_top=j;
				if (j>f_y_bottom)f_y_bottom=j;

				if(i<s_x_left)s_x_left=i;
				if (i>s_x_right)s_x_right=i;
				if(j<s_y_top)s_y_top=j;
				if (j>s_y_bottom)s_y_bottom=j;

				if(i<s_x_left)s_x_left=i;
				if (i>s_x_right)s_x_right=i;
				if(j<s_y_top)s_y_top=j;
				if (j>s_y_bottom)s_y_bottom=j;
			}
		}
	}


	for(f=0; f<nofs; f++)
	{
		//the data will be stored in <depthMap>
		ReadDepthMapBinFileNextFrame(fp, ncols, nrows, depthMap);
		//投影到三个视图上
		for(int i=0;i<nrows;i++)
		{
			for(int j=0;j<ncols;j++)
			{
				int temp=(int)depthMap.GetItem(i,j);
				if(temp>0)
				{
					//投影
					front[i][j]=1-front[i][j];
					top[temp][j]=1-top[temp][j];
					side[i][temp]=1-side[i][temp];

					for(int m=0;m<temp;m++)
						top[m][j]=1-top[m][j];
					for (int n=0;n<temp;n++)
						side[i][n]=1;
				}
			}
		}

		//check to see what has been loaded for DEBUG purpose:
		int nNonZeroPoints = depthMap.NumberOfNonZeroPoints();
		float avg = depthMap.AvgNonZeroDepth();
		printf("frame[%02i], ncols=%i, nrows=%i, count=%i, avg=%f\n", f, depthMap.GetNCols(), depthMap.GetNRows(), nNonZeroPoints, avg );
	}

	fclose(fp);
	fp=NULL;
	system("pause");
	return 0;
}

