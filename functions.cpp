#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <numeric>
#include <cmath>
#include <iostream>
using namespace std;

#define NDAY 250
int find_max_index(vector<double> vec){
	int n = vec.size();	

	int index = 0;
	double max = fabs(vec[0]);
	for (int i=0;i<n;i++){
		if(fabs(vec[i])>max){
			index = i;
			max = fabs(vec[i]);
		}
	}
    return index;
}

vector<double> normalize(vector<double> vec){
	int n = vec.size();	
	vector<double> returnvec(n);
	double max = fabs(vec[0]);
	for (int i=0;i<n;i++){
		if(fabs(vec[i])>max){
			max = fabs(vec[i]);
		}
	}
	for (int i =0;i<n;i++){
		returnvec[i] = vec[i]/max;
	}
	return returnvec;
}

double distance(vector<double> vec1, vector<double> vec2){
	int n = vec1.size();
	double sum = 0;
	for(int i=0;i<n;i++){
		sum += fabs(vec1[i] - vec2[i]);
	}
	return sum;
}

vector<double> Qxw(vector<vector<double> > mat, vector<double> vec){
	int n = vec.size();
	vector<double> returnvec(n);

	double temp_sum = 0; 
	for(int i=0;i<n;i++){
		temp_sum = 0;
		for(int j=0;j<n;j++){
			temp_sum += mat[i][j]*vec[j];
 		}
	    returnvec[i] = temp_sum;
	}
	return returnvec;
}

vector<vector<double> > refresh(vector<vector<double> > Q, vector<double> w, double lambda){
	int n = Q.size();
	double squaresum = 0;
	for(int i=0;i<n;i++){
		squaresum += w[i] * w[i];
	}	
	vector<vector<double> > newQ = Q;
	for(int i=0;i<n;i++){
	    for(int j=0;j<n;j++){
	    	newQ[i][j] -= lambda * w[i] * w[j] / squaresum;
	    }

	}
	return newQ;
}


vector<double> powermethod(vector<vector<double> > Q){
	int n = Q.size();
	vector<double> returnvec(10);
	vector<double> w(n);
	

	for(int k = 0; k < 10; k++){
		for(int i=0;i<n;i++){
			w[i] = fabs(100*rand()/(double)RAND_MAX);
		}
		vector<double> w0 = w;
		while(distance(normalize(Qxw(Q, w)), w) > 0.00001){
			w = normalize(Qxw(Q, w));

		}
		int ind = find_max_index(w);
		returnvec[k] = Qxw(Q, w)[ind]/w[ind];
		Q = refresh(Q, w, returnvec[k]);
		w = w0;
    }
	return returnvec;
}

vector<vector<double> > powermethod_geteigenvector(vector<vector<double> > Q){
	int n = Q.size();
	vector<vector<double> > returnvec(10, vector<double>(n));
	vector<double> w(n);
	double eigenvalue;
	

	for(int k = 0; k < 10; k++){
		for(int i=0;i<n;i++){
			w[i] = fabs(100*rand()/(double)RAND_MAX);
		}
		vector<double> w0 = w;
		while(distance(normalize(Qxw(Q, w)), w) > 0.00001){
			w = normalize(Qxw(Q, w));

		}
		int ind = find_max_index(w);
		eigenvalue = Qxw(Q, w)[ind]/w[ind];
		Q = refresh(Q, w, eigenvalue);
		printf("%f\n", eigenvalue);
		returnvec[k] = w;
		w = w0;
    }
	return returnvec;
}





double mean(vector<double> array){
	return 	accumulate(array.begin(), array.end(),double(0))/array.size();
}

double var(vector<double> array){
	double mu = mean(array);
	double sum = 0;
	for(int i=0;i<array.size();i++){
		sum += (array[i] - mu)*(array[i] - mu);
	}
	return sum/array.size();
}

double cov(vector<double> array1, vector<double> array2){
	double mu1 = mean(array1);
	double mu2 = mean(array2);
	double sum = 0;
	for(int i=0;i<array1.size();i++){
		sum += array1[i]*array2[i];
	}
	return sum/array1.size() - mu1 * mu2; //cov(X,Y) = E(XY) - E(X)E(Y)
}


int readit(char *filename, vector<double> * pave_ret,  vector<double> *pvar_ret, vector<vector<double> > *pcovs, int * pstocknum){
	int readcode = 0, fscancode;
	FILE *datafile = NULL;
	char buffer[100000];
	vector<vector<double> > prices(1000, vector<double>(NDAY));


	const char * split = ",";

	datafile = fopen(filename, "r");
	if (!datafile){
		printf("cannot open file %s\n", filename);
		readcode = 2; 
	}

	printf("reading data file %s\n", filename);

	fscanf(datafile, "%s", buffer);//we do not need the first line

    *pstocknum = 0;
    fscanf(datafile, "%s", buffer);//1st stock read in buffer

	while(fscancode != EOF){
   		char *p;
    	p = strtok(buffer, split);
    	p = strtok(NULL, split);

    	int day = 0;
    	while(day < NDAY){
      		prices[*pstocknum][day] = atof(p);
      		p = strtok(NULL, split);
      		day++;
    	}	
    	*pstocknum = *pstocknum + 1;
    	fscancode = fscanf(datafile, "%s", buffer);
	}
	printf("total %d stocks\n", *pstocknum);
	
	vector<vector<double> > returns(*pstocknum, vector<double>(NDAY-1));
    for(int i=0;i < *pstocknum ;i++){
    	for(int j=0;j < NDAY-1;j++){
    		returns[i][j] = (prices[i][j+1] - prices[i][j])/prices[i][j];
    	}
    }
    for(int i=0;i < *pstocknum ;i++){
    	(*pave_ret)[i] = mean(returns[i]);
    	(*pvar_ret)[i] = var(returns[i]);
    }
    for(int i=0;i < *pstocknum ;i++){
    	for(int j=0;j < *pstocknum;j++){
    		(*pcovs)[i][j] = cov(returns[i],returns[j]);
    	}
    }
    return readcode;
}

