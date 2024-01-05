#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

typedef struct process
{
    char *pid;
    int burst_time;
    int arrival_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
    int remaining_time;
} process;

typedef struct schedule
{
    int pid;
    int start_time;
    int end_time;
    struct schedule *next_ps;
} schedule;

double get_process(process *p, int n);
void print_process(process *p, int n, double total_time, double total_turnaround, double total_waiting, double total_response);
void fcfs(process *p, int n, double *total_turnaround, double *total_waiting, double *total_response, schedule **head);
void rr(process *p, int n, int time_quan, double total_time, double *total_turnaround, double *total_waiting, double *total_response, schedule **head);
void gantt_chart(schedule *head, process *p);
schedule *createNode(int pid, int start_time, int end_time);

int main(void)
{
    while (1)
    {
        int ch = 0;
        printf("1.First Come First Serve(FCFS)\n");
        printf("2.Round Round(RR)\n");
        printf("3.Exit\n");
        scanf("%d", &ch);
        if (ch == 3)
        {
            goto out_of_program;
        }

        // input processes
        int n;
        printf("Enter the number of processes: ");
        scanf("%d", &n);
        if (n <= 0)
        {
            printf("NO PROCESS\n");
            continue;
        }
        process *p = (process *)malloc(n * sizeof(process));
        double total_time, total_turnaround, total_waiting, total_response;
        total_response = total_turnaround = total_waiting = 0;

        total_time = get_process(p, n);
        schedule *head = NULL;

        switch (ch)
        {
        case 1:
            fcfs(p, n, &total_turnaround, &total_waiting, &total_response, &head);
            print_process(p, n, total_time, total_turnaround, total_waiting, total_response);
            gantt_chart(head, p);
            break;
        case 2:
            int time_quan;
            printf("Enter time quantum: ");
            scanf("%d", &time_quan);
            rr(p, n, time_quan, total_time, &total_turnaround, &total_waiting, &total_response, &head);
            print_process(p, n, total_time, total_turnaround, total_waiting, total_response);
            gantt_chart(head, p);
            break;
        default:
            printf("Invalid choice. Select again\n");
        }
    }
out_of_program:
    printf("Thank you for using the cpu scheduling program\n");
    return 0;
}

double get_process(process *p, int n)
{
    double total_time = 0;

    for (int i = 0; i < n; i++)
    {
        printf("Enter process id: ");
        char temp[100];
        scanf("%s", temp);
        p[i].pid = (char *)malloc(strlen(temp));
        strcpy(p[i].pid, temp);
        printf("Arrival time for '%s': ", p[i].pid);
        scanf("%d", &p[i].arrival_time);
        if (p[i].arrival_time > total_time)
            total_time = p[i].arrival_time;
        printf("Burst time for '%s': ", p[i].pid);
        scanf("%d", &p[i].burst_time);
        total_time += p[i].burst_time;
        p[i].remaining_time = p[i].burst_time;
        p[i].response_time = -1;
    }
    return total_time;
}

void print_process(process *p, int n, double total_time, double total_turnaround, double total_waiting, double total_response)
{
    printf("\nProcess ID\tArrival Time\tBurst Time\tCompletion Time\tTurnaround Time\tWaiting Time\tResponse Time\n");

    for (int i = 0; i < n; i++)
    {
        printf("%s\t\t%d\t\t%d\t\t%d\t\t", p[i].pid, p[i].arrival_time, p[i].burst_time, p[i].completion_time);
        printf("%d\t\t%d\t\t%d\n", p[i].turnaround_time, p[i].waiting_time, p[i].response_time);
    }
    printf("Total Time: %.2lf \n", total_time);
    printf("Average Turnaround Time: %.2lf\n", total_turnaround / n);
    printf("Average Waiting Time: %.2lf\n", total_waiting / n);
    printf("Average Response Time: %.2lf\n", total_response / n);
}

void fcfs(process *p, int n, double *total_turnaround, double *total_waiting, double *total_response, schedule **head)
{
    if (n == 0)
        return;

    int ps_num = 0; // process in execution
    double time = 0;

    do
    {
        if (time < p[ps_num].arrival_time)
        {
            *head = createNode(-1, time, p[ps_num].arrival_time);
            head = &(*head)->next_ps;
            time = p[ps_num].arrival_time; // arrived
        }

        p[ps_num].waiting_time = time - p[ps_num].arrival_time;
        time += p[ps_num].burst_time; // executed

        *head = createNode(ps_num, p[ps_num].arrival_time, time);
        head = &(*head)->next_ps;

        p[ps_num].completion_time = time;
        p[ps_num].turnaround_time = p[ps_num].completion_time - p[ps_num].arrival_time;
        p[ps_num].waiting_time = p[ps_num].turnaround_time - p[ps_num].burst_time;
        p[ps_num].response_time = p[ps_num].waiting_time;
        *total_response += p[ps_num].response_time;
        *total_waiting += p[ps_num].waiting_time;
        *total_turnaround += p[ps_num].turnaround_time;
        ps_num++;
    } while (ps_num < n);
}

void rr(process *p, int n, int time_quan, double total_time, double *total_turnaround, double *total_waiting, double *total_response, schedule **head)
{
    if (n == 0)
        return;

    int pid = 0;
    double time = 0;

    // ready queue
    int *queue = (int *)malloc(sizeof(n * sizeof(int)));
    int front, back;
    front = back = 0;
    queue[0] = pid;
    pid++;

    while (time < total_time)
    {
        int curr_pid = queue[front];
        front = (front + 1) % n;
        if (p[curr_pid].remaining_time)
        {
            if (p[curr_pid].arrival_time > time)
            {
                *head = createNode(-1, time, p[curr_pid].arrival_time);
                head = &(*head)->next_ps;
                time = p[curr_pid].arrival_time; // arrived
            }

            int temp_time = time; // start of this process
            if (p[curr_pid].response_time == -1)
            {
                // first time CPU
                p[curr_pid].response_time = time - p[curr_pid].arrival_time;
                *total_response += p[curr_pid].response_time;
            }
            if (p[curr_pid].remaining_time <= time_quan)
            {
                time += p[curr_pid].remaining_time;
                p[curr_pid].remaining_time = 0;
                p[curr_pid].completion_time = time;
                p[curr_pid].turnaround_time = p[curr_pid].completion_time - p[curr_pid].arrival_time;
                p[curr_pid].waiting_time = p[curr_pid].turnaround_time - p[curr_pid].burst_time;
                *total_turnaround += p[curr_pid].turnaround_time;
                *total_waiting += p[curr_pid].waiting_time;
            }
            else
            {
                time += time_quan;
                p[curr_pid].remaining_time -= time_quan;
            }
            *head = createNode(curr_pid, temp_time, time);
            head = &(*head)->next_ps;
        }

        while (pid < n && p[pid].arrival_time <= time)
        {
            // all process arrived while the execution of current process
            queue[(back + 1) % n] = pid;
            back = (back + 1) % n;
            pid++;
        }

        if (p[curr_pid].remaining_time)
        {
            queue[(back + 1) % n] = curr_pid;
            back = (back + 1) % n;
        }
    }
}

void gantt_chart(schedule *head, process *p)
{
    printf("\nGantt charts:\n0");
    while (head != NULL)
    {
        int index = head->pid;
        // printf("%d---", head->start_time);
        if (index == -1)
        {
            printf("---NA");
        }
        else
        {
            printf("----%s", p[index].pid);
        }
        printf("----%d", head->end_time);
        head = head->next_ps;
    }
    printf("\n\n");
}

schedule *createNode(int pid, int start_time, int end_time)
{
    schedule *newNode = (schedule *)malloc(sizeof(schedule));
    newNode->pid = pid;
    newNode->start_time = start_time;
    newNode->end_time = end_time;
    return newNode;
}