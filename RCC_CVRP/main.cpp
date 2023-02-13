#include "load.h"
#include "base_model.h"
#include "cvrpsep\cnstrmgr.h"
#include "cvrpsep\capsep.h"
#include "cvrpsep\basegrph.h"
#include <time.h>

int main() {
    const char* files[] = { "A-n54-k7", "A-n64-k9", "A-n80-k10",
        "B-n50-k8", "B-n66-k9", "B-n68-k9",
        "E-n51-k5", "E-n76-k7", "E-n76-k8", "E-n76-k10", "E-n76-k14", "E-n101-k8", "E-n101-k14",
        "M-n101-k10", "M-n121-k7", 
        "P-n50-k8", "P-n55-k10", "P-n70-k10", "P-n76-k5", "P-n101-k4"
    };
    int NoOfFiles = 20;
    for (int f = 0; f < NoOfFiles; f++) {
        const char* filename = files[f];
        printf("%s\n", filename);
        struct instance_information data = load_CVRP_instance(filename);

        char IntegerAndFeasible;
        int MaxNoOfCuts = 10000;
        int NoOfEdges;
        double EpsForIntegrality, EpsForViolation, MaxViolation;
        int* EdgeTail, * EdgeHead;
        double* EdgeX, * Demand;
        CnstrMgrPointer MyCutsCMP, MyOldCutsCMP;
        CMGR_CreateCMgr(&MyCutsCMP, 100);
        CMGR_CreateCMgr(&MyOldCutsCMP, 100); /* Contains no cuts initially */
        int i, j, ListSize;
        double RHS;
        int* List;
        List = (int*)malloc(sizeof(int) * data.num_cities);
        int(*ind);
        double(*val);


        /* Allocate memory for the three vectors EdgeTail, EdgeHead, and EdgeX */
        EdgeTail = (int*)malloc(sizeof(int) * data.num_cities * data.num_cities);
        EdgeHead = (int*)malloc(sizeof(int) * data.num_cities * data.num_cities);
        EdgeX = (double*)malloc(sizeof(double) * data.num_cities * data.num_cities);
        Demand = (double*)malloc(sizeof(double) * (data.num_cities));
        for (i = 1; i < data.num_cities; i++) {
            Demand[i] = data.demand[i];
        }

        /* Solve the initial LP */

        clock_t begin = clock();
        struct gurobi model_spec = base_model(data.num_cities, data.distances);
        printf("Initial Relaxation: %f\n", model_spec.objval);

        //Continue
        EpsForIntegrality = 0.0001;
        EpsForViolation = 0.001;
        while (1) {
            /* Store the information on the current LP solution */
            /* in EdgeTail, EdgeHead, EdgeX. */
            int k = 1;
            for (i = 0; i < data.num_cities; i++) {
                for (j = 0; j < data.num_cities; j++) {
                    if (model_spec.sol[i * data.num_cities + j] >= EpsForIntegrality) {
                        if (i == 0) {
                            if (j == 0) {
                                EdgeHead[k] = data.num_cities;
                                EdgeTail[k] = data.num_cities;
                                EdgeX[k] = model_spec.sol[i * data.num_cities + j];
                                k++;
                            }
                            else {
                                EdgeHead[k] = data.num_cities;
                                EdgeTail[k] = j;
                                EdgeX[k] = model_spec.sol[i * data.num_cities + j];
                                k++;
                            }
                        }
                        else if (j == 0) {
                            EdgeHead[k] = i;
                            EdgeTail[k] = data.num_cities;
                            EdgeX[k] = model_spec.sol[i * data.num_cities + j];
                            k++;
                        }
                        else {
                            EdgeHead[k] = i;
                            EdgeTail[k] = j;
                            EdgeX[k] = model_spec.sol[i * data.num_cities + j];
                            k++;
                        }
                    }
                }
            }
            NoOfEdges = k - 1;

            /* Call separation. Pass the previously found cuts in MyOldCutsCMP: */
            CAPSEP_SeparateCapCuts(data.num_cities - 1,
                Demand,
                data.capacity,
                NoOfEdges,
                EdgeTail,
                EdgeHead,
                EdgeX,
                MyOldCutsCMP,
                MaxNoOfCuts,
                EpsForIntegrality,
                EpsForViolation,
                &IntegerAndFeasible,
                &MaxViolation,
                MyCutsCMP);

            if (IntegerAndFeasible) break; /* Optimal solution found */
            if (MyCutsCMP->Size == 0) break; /* No cuts found */
            /* Read the cuts from MyCutsCMP, and add them to the LP */

            for (i = 0; i < MyCutsCMP->Size; i++)
            {
                ListSize = 0;
                for (j = 1; j <= MyCutsCMP->CPL[i]->IntListSize; j++)
                {
                    List[++ListSize] = MyCutsCMP->CPL[i]->IntList[j];
                }
                /* Now List contains the customer numbers defining the cut. */
                /* The right-hand side of the cut, */
                /* in the form x(S:S) <= |S| - k(S), is RHS. */
                RHS = MyCutsCMP->CPL[i]->RHS;
                /* Add the cut to the LP. */
                ind = (int*)malloc(sizeof(int) * data.num_cities * data.num_cities);
                val = (double*)malloc(sizeof(double) * data.num_cities * data.num_cities);
                k = 0;

                for (int m = 1; m <= ListSize; m++) {
                    for (int n = m + 1; n <= ListSize; n++) {
                        ind[k] = List[m] * data.num_cities + List[n];
                        val[k] = 1;
                        k++;
                    }

                }
                GRBaddconstr(model_spec.model, k, ind, val, GRB_LESS_EQUAL, RHS, NULL);
            }
            GRBwrite(model_spec.model, " mip1.lp");
            /* Resolve the LP */
            GRBoptimize(model_spec.model);
            GRBgetdblattr(model_spec.model, GRB_DBL_ATTR_OBJVAL, &model_spec.objval);
            GRBgetdblattrarray(model_spec.model, GRB_DBL_ATTR_X, 0, data.num_cities * data.num_cities, model_spec.sol);
            clock_t end = clock();
            double end_time = (double)(end - begin) / CLOCKS_PER_SEC;
            printf("Time Spent: %f; Objective Value: %f\n", end_time, model_spec.objval);

            /* Move the new cuts to the list of old cuts: */
            for (int i = 0; i < (MyCutsCMP->Size); i++)
            {
                CMGR_MoveCnstr(MyCutsCMP, MyOldCutsCMP, i, 0);
            }
            MyCutsCMP->Size = 0;
        };
        clock_t end = clock();
        double end_time = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Done: Time Spent: %f; Objective Value: %f\n", end_time, model_spec.objval);
    }
    return 0;
}