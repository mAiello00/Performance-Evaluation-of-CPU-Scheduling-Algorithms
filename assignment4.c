/*
Marco Aiello
251 047 101
COMPSCI 3305 Assignment 4
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

int time_quantum = 0;

struct process
{
	char process_num[10];
	int a_time;
	int b_time;
	int total_b_time;
	int w_time;
	int t_time;
	int start_time;
	int end_time;
	int id;
	bool started;
	bool done;
};

// Create the array of process structs
struct process arr_process[10];

int main(int argc, char* argv[])
{
	char line[100];
	char strings[10][10];
	FILE *file = fopen("rr_input.txt","r");
	int ctr = 0;
	int burst_time = 0;

	while(fgets(line, 100, file))
	{
		printf("%s\n",line);
		int j = 0;
		for(int i = 0; i <=(strlen(line));i++)
		{
			if(line[i] == ' ' || line[i] == '\0')
			{
				strings[ctr][j] = '\0';
				ctr++;
				j = 0;
			}
			else
			{
				strings[ctr][j] = line[i];
				j++;
			}	
		}
		
		int max_processing_time = 0;
		char line_word[10];
		int array_counter = 0;
		for(int i = 0; i < ctr; i++)
		{
			if(i == ctr-1)
			{
				time_quantum = atoi(strings[i]);
				//printf("This is the time quantum time: %d\n", time_quantum);
			}
			else
			{
				// Get the current element in the process efinition array
				// Done so it is easier to check the first element and see if it is a process
				strcpy(line_word, strings[i]);

				if(line_word[0] == 'p' || line_word[0] == 'P')
				{
					int identifier = 1;
					struct process p;
					strcpy(p.process_num, strings[i]);
					i++;
					
					int arrival_time = atoi(strings[i]);
					p.a_time = arrival_time;
					i++;
					
					int burst_time = atoi(strings[i]);
					p.b_time = burst_time;
					p.total_b_time = burst_time;
					max_processing_time += burst_time;
					
					p.w_time = 0;
				
					p.id = identifier;
					identifier++;
					
					p.started = false;
					p.done = false;
					
					// printf("This is the process number: %s\n", p.process_num);
					// printf("This is the arrival time: %d\n", p.a_time);
					// printf("This is the process burst time: %d\n", p.b_time);
					
					arr_process[array_counter] = p;
					array_counter++;
				}
			}
		}

		/*
		printf("We will now print all the structs that got stored in the array\n");
		for(int i = 0; i < array_counter; i++)
		{
			struct process test = arr_process[i];
			printf("This is the process number: %s\n", test.process_num);
                        printf("This is the arrival time: %d\n", test.a_time);
                        printf("This is the process burst time: %d\n", test.b_time);
			
		}
		*/		

		int total_time_spent_processing = 0;
		int current_time_spent_processing = 0;
		
		//printf("This loop calculates the actual round robin values\n");
		for(int i = 0; i < array_counter; i++)
		{
			current_time_spent_processing = 0;
			//printf("Total time spent processing: %d\n", total_time_spent_processing);
			if(total_time_spent_processing >= max_processing_time)
				break;

			// Gets the process from the array
			struct process p = arr_process[i];

			// Check whether there is still processing to be done on the current process
			if(p.b_time != 0)
			{
				// Check to see whether the process we are looking has arrived yet.
				// If it has then we process it
				if(p.a_time <= total_time_spent_processing)
				{
					// If the remaining burst time for the process is less than or equal to
					// the time quantum, then the amount of time spent on the current 
					// process is the remianing burst time. 
					// We then set the remaining burst time fot the process to 0
					// and increment the total time spent processing
					if(p.b_time <= time_quantum)
					{
						current_time_spent_processing = p.b_time;
						//printf("Current time spent processing: %d for process: %s\n", current_time_spent_processing, p.process_num);
						p.b_time = 0;
						total_time_spent_processing += current_time_spent_processing;
						p.end_time = total_time_spent_processing;
					}
					else
					{	
						// If the process has arrived but has not been started yet
						if(p.started == false)
						{
							p.started = true;
							p.start_time = total_time_spent_processing - p.a_time;
							p.w_time = p.start_time - p.a_time;
						}
						
						// Reduce the remaining burst time by the time quantum
						p.b_time = p.b_time - time_quantum;
						// Increment the total time spent processing by the amount of time spent on this process
						total_time_spent_processing += time_quantum;
						// Get the amount of time spent on this process
						current_time_spent_processing += time_quantum;
					}						
				}

				// Loop through all the processes in the array and increment their waiting time
                        	// if they are have already arrived, have been started, and are waiting
                        	for(int j = 0; j < array_counter; j++)
                        	{
                                	struct process comp_process = arr_process[j];
                                	if((p.id != comp_process.id) && (comp_process.started == true) && (comp_process.b_time > 0))
                                	{
                                        	comp_process.w_time += current_time_spent_processing;
                                        	arr_process[j] = comp_process;
                                	}
                        	}
			}
			// Add the updated process back into the array of processes
			arr_process[i] = p;

			//printf("%d", i);
			// Loop back to the beginning of the array so the round robin algorithm can continue
			if(i == (array_counter-1))
				i = -1;
		}
				
		ctr = 0;

		
		//printf("\n");
		//printf("\n");
		//printf("These are the final values stored in the structs\n");
		int total_turnaround_time = 0;
		int total_wait_time = 0;
		float avg_wait_time;
		float avg_turnaround_time;

		for(int i = 0; i < array_counter; i++)
		{
			struct process p_done = arr_process[i];
			
			//printf("This is the process name: %s\n", p_done.process_num);
			//printf("This is the arrival time: %d\n", p_done.a_time);
			//printf("This is the burst time: %d\n", p_done.total_b_time);
			//printf("This is the wait time: %d\n", p_done.w_time);
			//printf("This is the start time: %d\n", p_done.start_time);
			//printf("This is the end time: %d\n", p_done.end_time);

			p_done.t_time = p_done.end_time - p_done.start_time;

			total_turnaround_time += p_done.t_time;
			total_wait_time += p_done.w_time;

			printf("Process: %s  Arrival Time: %d  Burst Time; %d  Waiting Time: %d  Turnaround Time: %d\n", p_done.process_num, p_done.a_time, p_done.total_b_time, p_done.w_time, p_done.t_time, p_done.t_time);
		}

		avg_wait_time = total_wait_time/array_counter;
		avg_turnaround_time = total_turnaround_time/array_counter;

		printf("Total turnaround time: %d \n", total_turnaround_time);
		printf("Average waiting time: %.2f \n", avg_wait_time);
		printf("Average turnaround time: %.2f \n", avg_turnaround_time);
	}
}
