# CVRPSEP-for-RCC
Code for finding the lower bound of CVRP instances using Gurobi and the CVRPSEP package.
OBS: Currently only working for instances of EDGE_WEIGHT_TYPE: EUC_2D not including the Uchoa et al. instances.

To make the code work follow these steps:
1. Follow the steps provided from Gurobi for configuration of a Microsoft Visual Studio Project: https://support.gurobi.com/hc/en-us/articles/360013194392-How-do-I-configure-a-new-Gurobi-C-project-with-Microsoft-Visual-Studio-
- Notice, that when using Gurobi 10.0, the file "gurobi95.lib" should be replaced by "gurobi100.lib".

2. For "CVRPSEP-for-RCC/RCC_CVRP/load.h" line 34, the "PATH" should be replaced by the location of your .vrp files.
3. For "CVRPSEP-for-RCC/RCC_CVRP/base_model.h" line 2, the "PathToGurobi" should be replaced by the location of your Gurobi installion.
