# HW5
(a) Using the first year of data, compute the average returns vector, and a principal components decomposition of the covariance matrix with at least ten eigenvalues (you should reuse old code).  

(b)  Solve the (long-only) portfolio optimization problem using the Gurobi callable library. You can adapt the code I have posted before. You can assume that every asset can take up at most 2 % of the budget, i.e. x_j <= 0.02 for each j. You should experiment with different levels of risk aversion until the optimal portfolio has at least 200 names. Basically you need to scale up the residual variances and the diagonal "factor covariance" matrix by a common factor.

(c)  For the level of risk aversion in (b), rerun the portfolio optimization problem using different limits on the number of names that are allowed to appear in the final portfolio, e.g. at most 150 names, at most 100 names, and so on. [Here you can reuse "names.cpp" which I have posted before.] What happens to the optimal portfolio? 

(d) Extra credit  Compute the average returns and PCA decomposition of the covariance matrix for the second year's worth of data. Take the portfolio computed in part (b) (or (c) -- whichever you want) and compute a new optimal portfolio subject to (1) having weight, on the short side, between 0.4 and 0.6, and (2) total turnover is at most 0.2. No limit is imposed on number of names in the portfolio. 

