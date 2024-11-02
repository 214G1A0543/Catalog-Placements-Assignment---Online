#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cJSON/cJSON.h"
#include "cJSON/cJSON.c"

typedef struct {
    int x;
    long long y;
} Point;


int parse_json(const char *filename, Point **points, int *k) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open file");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *file_content = (char*)malloc(filesize + 1);
    fread(file_content, 1, filesize, file);
    fclose(file);

    cJSON *json = cJSON_Parse(file_content);
    if (!json) {
        printf("Error parsing JSON\n");
        free(file_content);
        return -1;
    }

    cJSON *keys = cJSON_GetObjectItem(json, "keys");
    *k = cJSON_GetObjectItem(keys, "k")->valueint;

    
    *points = (Point*)malloc((*k) * sizeof(Point));
    int count = 0;

    cJSON *item;
    cJSON_ArrayForEach(item, json) {
        if (strcmp(item->string, "keys") != 0) {
            int x = atoi(item->string);
            int base = cJSON_GetObjectItem(item, "base")->valueint;
            char *value_str = cJSON_GetObjectItem(item, "value")->valuestring;
            long long y = strtoll(value_str, NULL, base); // Convert y to base 10
            
            (*points)[count].x = x;
            (*points)[count].y = y;
            count++;
            
            if (count >= *k) break; 
        }
    }

    cJSON_Delete(json);
    free(file_content);
    return count;
}


long long lagrange_interpolation(Point *points, int k) {
    long long constant_term = 0;
	int j;
    for (j = 0; j < k; j++) {
        long long term = points[j].y;
		int m;
        for (m = 0; m < k; m++) {
            if (m != j) {
                term *= -points[m].x;
                term /= (points[j].x - points[m].x);
            }
        }

        constant_term += term;
    }

    return constant_term;
}


long long process_test_case(const char *filename) {
    Point *points;
    int k;

    int count = parse_json(filename, &points, &k);
    if (count < k) {
        printf("Not enough points to determine the polynomial.\n");
        return -1;
    }

    long long secret = lagrange_interpolation(points, k);

    
    free(points);
    return secret;
}

int main() {
    
    long long result1 = process_test_case("C:\\Users\\keert\\OneDrive\\Documents\\DISK_SCHEDULING_ALGORITHMS\\testcase.json");
    long long result2 = process_test_case("C:\\Users\\keert\\OneDrive\\Documents\\DISK_SCHEDULING_ALGORITHMS\\testcase2.json");

    if (result1 == -1 || result2 == -1) {
        printf("Error processing one or both test cases.\n");
        return -1;
    }


    printf("Secrets for Test Case 1 and Test Case 2: %lld, %lld\n", result1, result2);

    return 0;
}

