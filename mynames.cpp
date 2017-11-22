
#include <stdio.h>
#include <stdlib.h>


#include "gurobi_c++.h"



int main(void)
{
  int retcode = 0;
  GRBenv   *env = NULL;
  GRBmodel *model = NULL;
  int n, j;
  double *obj      = NULL;
  double *lb       = NULL;
  double *ub       = NULL;
  double *x;
  int *qrow, *qcol, Nq;
  double *qval;
  int *cind;
  double rhs;
  char sense;
  double *cval;
  int numnonz;

  char **names, *vartype;

  /** from the file, we need the following data */
  int numasset;
  int numnames = 100;
  double *ave_ret;
  ave_ret = (double *) calloc(numasset, sizeof(double));
  double *var_ret;
  var_ret = (double *) calloc(numasset, sizeof(double));
  double *cov_matrix;
  cov_matrix = (double *) calloc(numasset * numasset, sizeof(double));


  /* here we need a readit fucntion to read and calculate it from the csv file */




  /* for PCA part, we need the following*/

  int numfactor = 10;
  double *e_value;
  e_value = (double *) calloc(numfactor, sizeof(double));
  double *e_vector;
  e_vector = (double *) calloc(numfactor * numasset, sizeof(double));

  /* here we need to call powermethod to generate: eigenvalues, eigenvectors */





  

  n = numasset + numfactor + numasset; /** numasset 'x' variables, numfactor factor variables, numasset binary variables **/


  retcode = GRBloadenv(&env, "factormodel.log");
  if (retcode) goto BACK;

 /* Create initial model */
  retcode = GRBnewmodel(env, &model, "second", n, 
                      NULL, NULL, NULL, NULL, NULL);
  if (retcode) goto BACK;

  names = (char **) calloc(n, sizeof(char *));

  /** next we create the remaining attributes for the n columns **/
  obj     = (double *) calloc (n, sizeof(double));
  ub     = (double *) calloc (n, sizeof(double));
  lb     = (double *) calloc (n, sizeof(double));
  x     = (double *) calloc (n, sizeof(double));
  vartype = (char *)calloc(n, sizeof(char));

  for(j = 0; j < numasset; j++){
    names[j] = (char *)calloc(3, sizeof(char));
    if(names[j] == NULL){
      retcode = 1; goto BACK;
    }
    sprintf(names[j],"x%d", j);
    obj[j] = - ave_ret[j];
    ub[j] = 0.02;
  }
  for(j = numasset; j < numasset + numfactor; j++){
    names[j] = (char *)calloc(3, sizeof(char));
    if(names[j] == NULL){
		  retcode = 1; goto BACK;
    }
    sprintf(names[j],"y%d", j - numasset);
    ub[j] = 100;
    lb[j] = -100;    
  }

//  obj[0] = -.233; obj[1] = -3.422; obj[2] = -.1904; obj[3] = -.5411;
//  obj[4] = -.045; obj[5] = -1.271; obj[6] = -0.955;

  /** calloc initializes memory to zero, so all other obj[j] are zero **/

  /**next, the upper bounds on the x variables **/
//  ub[0] = 0.4; ub[1] = 0.3; ub[2] = 0.25; ub[3] = 0.5;
//  ub[4] = 0.5; ub[5] = 0.26; ub[6] = 0.99;
  
  /** the upper bounds on the two factor variables -- we make them large **/
//  ub[7] = 100; ub[8] = 100;
  /** the lower bounds on the factor variables **/
// lb[7] = -100; lb[8] = -100;

  /** finally, do the binary variables **/
  for (j = numasset + numfactor; j < n; j++){
	  names[j] = (char *)calloc(3, sizeof(char));
	  if (names[j] == NULL){
		  retcode = 1; goto BACK;
	  }
	  sprintf(names[j], "z%d", j - numasset - numfactor);
	  lb[j] = 0; /* redundant because of calloc */
	  ub[j] = 1.0;
	  obj[j] = 0.0; /* redundant, again */
  }

  /* initialize variables */
  for(j = 0; j < n; j++){
    retcode = GRBsetstrattrelement(model, "VarName", j, names[j]);
    if (retcode) goto BACK;

    retcode = GRBsetdblattrelement(model, "Obj", j, obj[j]);
    if (retcode) goto BACK;

    retcode = GRBsetdblattrelement(model, "LB", j, lb[j]);
    if (retcode) goto BACK;

    retcode = GRBsetdblattrelement(model, "UB", j, ub[j]);
    if (retcode) goto BACK;

	if (j < numasset + numfactor) vartype[j] = GRB_CONTINUOUS;
	else vartype[j] = GRB_BINARY;
	
	retcode = GRBsetcharattrelement (model, "VTYPE", j, vartype[j]);
	if (retcode) goto BACK;
  }

  /** next, the quadratic -- there are (numasset + numfactor) nonzeroes: numasset residual variances plus the numfactor
									factor covariance matrix**/

  Nq = numasset + numfactor; 
  qrow = (int *) calloc(Nq, sizeof(int));  /** row indices **/
  qcol = (int *) calloc(Nq, sizeof(int));  /** column indices **/
  qval = (double *) calloc(Nq, sizeof(double));  /** values **/

  if( ( qrow == NULL) || ( qcol == NULL) || (qval == NULL) ){
    printf("could not create quadratic\n");
    retcode = 1; goto BACK;
  }

  for (j = 0; j < numasset; j++){
    qval[j] = var_ret[j];
    qrow[j] = j;
    qcol[j] = j;
  }
  for (j = numasset; j < numasset + numfactor; j++){
    qval[j] = e_value[j - numasset];
    qrow[j] = j;
    qcol[j] = j;
  }

  retcode = GRBaddqpterms(model, Nq, qrow, qcol, qval);
  if (retcode) goto BACK;

  /** now we will add one constraint at a time **/
  /** we need to have a couple of auxiliary arrays **/

  cind = (int *)calloc(n, sizeof(int));  /** n is over the top since no constraint is totally dense;
					     but it's not too bad here **/
  cval= (double *)calloc(n, sizeof(double));

  /** numfactor factor constraints, first one is next**/

  for(j=0; j<numfactor; j++){
    for(k=0; k<numasset; k++){
      cval[j][k] = e_vector[j * numasset + k];
      cind[j][k] = k;
    }
    cval[j][numasset] = -1;
    cind[j][numasset] = numasset + j;
    numnonz = numasset + 1;
    rhs = 0;
    sense = GRB_EQUAL;
    retcode = GRBaddconstr(model, numnonz, cind, cval, sense, rhs, "No.%d_constraint",j+1);
    if (retcode) goto BACK;
  }


  /** sum of x variables = 1 **/
  for(j=0; j<numasset; j++){
    cval[j] = 1.0;
    cind[j] = j;
  }
  numnonz = numasset;
  rhs = 1.0;
  sense = GRB_EQUAL;

  retcode = GRBaddconstr(model, numnonz, cind, cval, sense, rhs, "convexity");
  if (retcode) goto BACK;


  /** sum of z variables <= names_constraint **/
  for(j=0; j<numasset; j++){
    cval[j] = 1.0;
    cind[j] = j + numasset + numfactor;
  }

  numnonz = numasset;
  rhs = float(numnames);
  sense = GRB_LESS_EQUAL;

  retcode = GRBaddconstr(model, numnonz, cind, cval, sense, rhs, "convexity");
  if (retcode) goto BACK;


  /** now say xj = 0 unless zj = 1 **/

  for (j = 0; j < numasset; j++){
	  cval[0] = 1.0;  cind[0] = j;
	  cval[1] = -1.0;  cind[1] = numasset + numfactor + j;

	  numnonz = 2;
	  rhs = 0.0;
	  sense = GRB_LESS_EQUAL;

	  /* let's reuse some space */
	  sprintf(names[0], "control%d", j);

	  retcode = GRBaddconstr(model, numnonz, cind, cval, sense, rhs, names[0]);
	  if (retcode) goto BACK;
  }


  retcode = GRBupdatemodel(model);
  if (retcode) goto BACK;

  /** optional: write the problem **/

  retcode = GRBwrite(model, "factorwithnames.lp");
  if (retcode) goto BACK;


  retcode = GRBoptimize(model);
  if (retcode) goto BACK;


  /** get solution **/


  retcode = GRBgetdblattrarray(model,
                               GRB_DBL_ATTR_X, 0, n,
                               x);
  if(retcode) goto BACK;

  /** now let's see the values **/

  for(j = 0; j < n; j++){
    printf("%s = %g\n", names[j], x[j]);
  }

  GRBfreeenv(env);


 BACK:
  printf("\nexiting with retcode %d\n", retcode);
  return retcode;
}
