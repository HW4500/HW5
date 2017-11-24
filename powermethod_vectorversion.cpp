#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

void testfunc(){
	printf("***run dll successfully!***");
}
void PrintVec(vector<double> vec){
	int n = vec.size();	
	for (int i=0;i<n;i++){
		printf("%f,", vec[i]);
	}
	printf("\n");

}


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
	vector<double> returnvec(n);
	vector<double> w(n);
	

	for(int k = 0; k < n; k++){
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

int test(vector<vector<double> > m){
    m[0][0] = 14.14;
	return 0;
}
int main()
{
  vector<vector<double> > m(3, vector<double>(3));
  m[0][0] = 7;
  m[0][1] = 4;
  m[0][2] = 5;
  m[1][0] = 4;
  m[1][1] = 5;
  m[1][2] = 5;  
  m[2][0] = 5;
  m[2][1] = 5;
  m[2][2] = 7;
  vector<double> v = powermethod(m);
  printf("%f,%f,%f\n", v[0],v[1],v[2]);
  return 0;
}

