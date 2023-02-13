#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

struct instance_information {
    const int num_cities;
    const int capacity;
    const double *demand;
    int (*distances);
};

int euc_2d(double px, double py, double qx, double qy) {
    return int(sqrt(pow(px - qx, 2) + pow(py - qy, 2)) + 0.5);
}


int startswith(const char* prefix, const char* str)
{
    return strncmp(prefix, str, strlen(prefix)) == 0;
}

struct instance_information load_CVRP_instance(const char* filename) {
    const unsigned  MAX_LENGTH = 256;
    char            line[MAX_LENGTH];
    int             dim_cap_type = 0;
    int             num_cities;
    int             capacity;
    char            edge_weight_type[10];
    char            weight_type_identifier = NULL;
    char            file[200];

    strcpy_s(file, "PATH");
    strcat_s(file, filename);
    strcat_s(file, ".vrp");
    FILE* fp;
    fopen_s(&fp, file, "r");

    if (fp == NULL)
    {
        printf("Error: could not open file %s", file);
    }

    //Load dimension and capacity
    while (true) {
        fgets(line, MAX_LENGTH, fp);
        if (startswith("DIMENSION", line)) {
            char* next_token = NULL;
            char* token = strtok_s(line, " ", &next_token);
            token = strtok_s(NULL, " ", &next_token);
            token = strtok_s(NULL, " ", &next_token);
            num_cities = atoi(token);
            dim_cap_type++;
        }
        else if (startswith("CAPACITY", line)) {
            char* next_token = NULL;
            char* token = strtok_s(line, " ", &next_token);
            token = strtok_s(NULL, " ", &next_token);
            token = strtok_s(NULL, " ", &next_token);
            capacity = atoi(token);
            dim_cap_type++;
        }
        else if (startswith("EDGE_WEIGHT_TYPE", line)) {
            char* next_token = NULL;
            char* token = strtok_s(line, " ", &next_token);
            while (token != NULL) {
                weight_type_identifier = *(token + 1);
                token = strtok_s(NULL, " ", &next_token);
            }
            if (weight_type_identifier == 'X') strcpy_s(edge_weight_type, "EXPLICIT");
            else strcpy_s(edge_weight_type, "EUC_2D");
            dim_cap_type++;
        };
        if (dim_cap_type >= 3) break;
    };
 
    //Determine Distances
    int (*distances);
    distances = (int*)malloc(sizeof(int) * num_cities*num_cities);

    if (strcmp(edge_weight_type, "EXPLICIT") == 0) {
        //Load distance matrix
        while (true) {
            fgets(line, MAX_LENGTH, fp);
            if (startswith("EDGE_WEIGHT_SECTION", line)) break;
        };

        for (int i = 1; i < num_cities; i++) {
            int j = 0;
            while (j < i) {
                fgets(line, MAX_LENGTH, fp);
                char* next_token = NULL;
                char* token = strtok_s(line, " ", &next_token);
                while (token != NULL) {
                    distances[j * num_cities + i] = atoi(token);
                    token = strtok_s(NULL, " ", &next_token);
                    j++;
                }
            }
        }
    }

    //Load Coordinates
    double** coordinates = new double* [num_cities];
    for (int i = 0; i <= num_cities; i++) {
        coordinates[i] = new double[2];
    }

    while (true) {
        fgets(line, MAX_LENGTH, fp);
        if (startswith("NODE_COORD_SECTION", line)) break;
    };

    for (int i = 0; i < num_cities; i++) {
        const char* delim;
        if (startswith("X", filename)) {
            delim = "\t";
        } else delim = " ";

        fgets(line, MAX_LENGTH, fp);
        char* next_token = NULL;
        char* token = strtok_s(line, delim, &next_token);
        token = strtok_s(NULL, delim, &next_token);
        coordinates[i][0] = atof(token);
        coordinates[i][1] = atof(next_token);
    }

    //Load Demand
    double* demand = new double[num_cities];

    while (true) {
        fgets(line, MAX_LENGTH, fp);
        if (startswith("DEMAND_SECTION", line)) break;
    };

    for (int i = 0; i < num_cities; i++) {
        fgets(line, MAX_LENGTH, fp);
        char* next_token = NULL;
        char* token = strtok_s(line, " ", &next_token);
        demand[i] = atof(next_token);
    }

    if (strcmp(edge_weight_type, "EUC_2D") == 0) {
        //Calculate Euclidean distances
        for (int i = 0; i < num_cities; i++) {
            for (int j = i + 1; j < num_cities; j++) {
                distances[i*num_cities + j] = euc_2d(coordinates[i][0], coordinates[i][1], coordinates[j][0], coordinates[j][1]);
            }
        }
    }
    // close the file
    fclose(fp);

    struct instance_information data = {
        num_cities,
        capacity,
        demand,
        distances
    };

    return data;
};