#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

typedef struct process
{
    int *alloted;
    int *max;
    int *required;
    int completed;
} process;

int *get_res(int res_count);
int *get_ps(process *ps, int ps_count, int res_count);
int print_ps(process *ps, int ps_count, int res_count);
int *bankers(process *ps, int *res, int *total, int ps_count, int res_count);
void print_state(int *safe_state, int ps_count);

int main(void)
{
    int res_count;
    printf("Enter number of resources: ");
    scanf("%d", &res_count);
    int *res = get_res(res_count);

    int ps_count;
    printf("Enter number of processes: ");
    scanf("%d", &ps_count);

    process *ps = (process *)malloc(ps_count * sizeof(process));
    int *total = get_ps(ps, ps_count, res_count);

    print_ps(ps, ps_count, res_count);

    int *safe_state = bankers(ps, res, total, ps_count, res_count);
    if (safe_state == NULL)
    {
        printf("Deadlock can't be avoided");
    }
    else
    {
        print_state(safe_state, ps_count);
    }

    return 0;
}

int *get_res(int res_count)
{
    int *res = (int *)malloc(res_count * sizeof(int));
    for (int i = 0; i < res_count; i++)
    {
        printf("Enter R%d instances: ", i + 1);
        scanf("%d", &res[i]);
    }
    return res;
}

int *get_ps(process *ps, int ps_count, int res_count)
{
    int *total = (int *)malloc(res_count * sizeof(int));

    for (int i = 0; i < ps_count; i++)
    {
        ps[i].alloted = (int *)malloc(res_count * sizeof(int));
        ps[i].max = (int *)malloc(res_count * sizeof(int));
        ps[i].required = (int *)malloc(res_count * sizeof(int));
        ps[i].completed = 0;

        printf("For P%d\n", i + 1);
        for (int j = 0; j < res_count; j++)
        {
            printf("\tR%d Max Required : ", j + 1);
            scanf("%d", &(ps[i].max[j]));
            printf("\tR%d Allotted : ", j + 1);
            scanf("%d", &(ps[i].alloted[j]));
            ps[i].required[j] = ps[i].max[j] - ps[i].alloted[j];
            total[j] += ps[i].alloted[j];
        }
    }

    return total;
}

int print_ps(process *ps, int ps_count, int res_count)
{
    printf("\nProcess%*sAllocation%*sMax%*sRequired\n", res_count * 3, "", res_count * 6, "", res_count * 7, "");
    printf("\t\t");
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < res_count; j++)
        {
            printf("R%d  ", j + 1);
        }
        printf("\t\t");
    }
    printf("\n");

    for (int i = 0; i < ps_count; i++)
    {
        printf("P%d\t\t", i + 1);
        for (int j = 0; j < res_count; j++)
        {
            printf("%d   ", ps[i].alloted[j]);
        }
        printf("\t\t");
        for (int j = 0; j < res_count; j++)
        {
            printf("%d   ", ps[i].max[j]);
        }
        printf("\t\t");
        for (int j = 0; j < res_count; j++)
        {
            printf("%d   ", ps[i].required[j]);
        }
        printf("\n");
    }

    return 0;
}

int *bankers(process *ps, int *res, int *total, int ps_count, int res_count)
{
    int curr = -1;
    int *safe_state = (int *)malloc(ps_count * sizeof(int));

    int *left_res = (int *)malloc(sizeof(int) * res_count);
    for (int i = 0; i < res_count; i++)
    {
        left_res[i] = res[i] - total[i];
    }

    for (int i = 0; i < ps_count; i++)
    {
        if (!ps[i].completed)
        {
            int runable = 1;
            for (int j = 0; j < res_count; j++)
            {
                if (ps[i].required[j] > left_res[j])
                {
                    runable = 0;
                    break;
                }
            }
            if (runable)
            {
                ps[i].completed = 1;
                for (int j = 0; j < res_count; j++)
                {
                    left_res[j] += ps[i].alloted[j];
                }
                safe_state[++curr] = i + 1;
                i = 0;
            }
        }
    }
    for (int i = 0; i < ps_count; i++)
    {
        if (!ps[i].completed)
        {
            return NULL;
        }
    }
    return safe_state;
}

void print_state(int *safe_state, int ps_count)
{
    printf("\nSafe State:\n|");
    for (int i = 0; i < ps_count; i++)
    {
        printf("\tP%d\t|", safe_state[i]);
    }
    printf("\n");
}
