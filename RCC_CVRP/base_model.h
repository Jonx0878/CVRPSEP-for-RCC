#pragma once
#include "PathToGurobi\win64\include\gurobi_c.h"
#include <stdlib.h>
#include <stdio.h>

struct gurobi {
	GRBmodel* model;
	double* sol;
	double objval;
};

gurobi base_model(const int num_cities, int* distances) {
	GRBenv* env = NULL;
	GRBmodel* model = NULL;

	double		(*sol);
	double		(*ub);
	int			(*ind);
	double		(*val);
	double		(*obj);
	char		(*vtype);
	int			optimstatus;
	double		objval;

	/* Create environment */
	GRBemptyenv(&env);
	GRBsetstrparam(env, " LogFile ", " mip1 .log");
	GRBsetintparam(env, "OutPutFlag", 0);
	GRBstartenv(env);

	//Create Model
	GRBnewmodel(env, &model, "CVRP", 0, NULL, NULL, NULL, NULL, NULL);

	/* Add variables */
	obj = (double*)malloc(sizeof(double) * num_cities * num_cities);
	vtype = (char*)malloc(sizeof(char) * num_cities * num_cities);
	ub = (double*)malloc(sizeof(double) * num_cities * num_cities);
	for (int i = 0; i < num_cities; i++) {
		for (int j = 0; j < num_cities; j++) {
			vtype[i * num_cities + j] = GRB_CONTINUOUS;
			if (j > i){
			obj[i * num_cities + j] = distances[i * num_cities + j];
			if (i == 0) {
				ub[i * num_cities + j] = 2;
			}
			else {
				ub[i * num_cities + j] = 1;
			};
			}
			else {
				obj[i * num_cities + j] = 0;
				ub[i * num_cities + j] = 0;
			}
		}
	}
	GRBaddvars(model, num_cities*num_cities, 0, NULL, NULL, NULL, obj, NULL, ub, vtype, NULL);

	/* Set objective sense */
	GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MINIMIZE);

	/* Constraints */
	ind = (int*)malloc(sizeof(int) * num_cities);
	val = (double*)malloc(sizeof(double) * num_cities);
	for (int i = 1; i < num_cities; i++) {
		for (int j = 0; j < num_cities; j++) {
			if (j < i) {
				ind[j] = j * num_cities + i;
				val[j] = 1;
			}
			else if (j > i) {
				ind[j] = i * num_cities + j;
				val[j] = 1;
			}
			else {
				ind[j] = i * num_cities + j;
				val[j] = 0;
			}
		}
		GRBaddconstr(model, num_cities, ind, val, GRB_EQUAL, 2.0, NULL);
	}

	/* Solve Model */
	GRBoptimize(model);


	/* Capture Solution Information */
	sol = (double*)malloc(sizeof(double) * num_cities * num_cities);
	GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
	GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, num_cities * num_cities, sol);

	struct gurobi model_spec {
		model,
		sol,
		objval
	};
	return model_spec;
}