

#include "mm1.h"

struct event
{
   int id; // id of 
   int type; // to differentiate type of event
   double timeval; // holds cumulative arrival time
   struct event * link; // to hold address of next node.
   double service; // time taken to service the packet - same for all event nodes per packet.
   double syst_time; // time spent in system.
   double equeue; // time at which node enters queue
   double exitq; // time at which node exits the queue.

};
struct event * front = NULL;

//======================================================== variable declaration =====================================================================

double pkt_arrival_time = 0.0;
double pkt_service_time = 0.0;
int f1=0, f2=0, fl, fd=1, fe=0, fs=0, ft=0; //flags
double lam_flag=0.0, mu_flag=0.0;
double lam_time=0.0, mu_time=0.0;
long det_flag=20; 
int s_flag=10;
int arrival=0, departure=0;
FILE * fp;
FILE * fp_ctr;
int server_flag = 0;
double servicing_time;
double total_iat = 0.0;
double total_service_time = 0.0;
double total_time_in_system = 0.0;
double total_time_in_queue = 0.0;
double prev_time = 0.0;
double total_syst_sq = 0.0;
int lines;
char filename[50] = {};
double prev_finish;
double prev_pkt;
double cur = 0.0;




//====================================================== Initialising random number generator =============================================================

void InitRandom(long l_seed)
        /*
         * initialize the random number generator
         * if l_seed is 0, will seed the random number generator using the current clock
         * if l_seed is NOT 0, your simulation should be repeatable if you use the same l_seed
         * call this function at the beginning of main() and only once
         */
{
        if (l_seed == 0L) {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);

            srand48(ts.tv_nsec);
        } else {
            srand48(l_seed);
        }
}

//=================================================== Random number generator for exponential mode ==========================================================
    
int ExponentialInterval(double rate)
        /*
         * returns an exponentially distributed random interval (in milliseconds) with mean 1/rate
         * rate is either lambda or mu (in packets/second)
         * according to the spec: w = ln(1-r) / (-rate)
         */
{
        double dval;
        do {
            dval = drand48();
            /* if dval is too small or too large, try again */
        } while (dval < 0.000001 || dval > 0.999999);

        dval = ((double)1.0) - dval;
        dval = -log(dval);
        dval = ((dval / rate) * ((double)1000));

        return round(dval);
}


//==================================================== boundary condition check for event queue ============================================================

int isEmpty()
{
	if( front == NULL )
	return 1;
	else
	return 0;

}

//=================================================== function to insert event nodes into event queue =========================================================

void insert(struct event * tmp)
{
	struct event *p;

	if(tmp==NULL)
	{
	printf("Memory not available\n");
	return;
	}
	double item_priority = tmp->timeval;

	if( isEmpty() || item_priority < front->timeval )
	{
		tmp->link=front;
		front=tmp;
	}
	else
	{
		p = front;
		while( p->link!=NULL && p->link->timeval<=item_priority )
			p=p->link;
		tmp->link=p->link;
		p->link=tmp;
	}
}

//=============================================== display function to display simulation times ========================================================================

void display(struct event *pac)
{
	struct event *ptr;
	ptr=front;
	//double prev_time = 0.0;
	if( isEmpty() )
	printf("Queue is empty\n");
	else
	{   
		
		
		
		while(ptr!=NULL)
		{
			//ptr=front;
			if((pac->timeval > ptr->timeval)||(pac->id==det_flag))
			{
					if(ptr->type==0)
					{
						// a = 'a';
						 total_iat = total_iat + (ptr->timeval -cur);

						 printf("%012.3lfs: p%d arrives and enters Q, inter-arrival time = %.3lfs\n", ptr->timeval, ptr->id, ptr->timeval - cur);
						 //arr[ptr->id] = ptr->timeval;
						// printf("Cur: %.6g%s\n", cur);
						// printf("%s\n", );
						 cur = ptr->timeval;
						 arrival++;
						 if(server_flag==0 && ptr->id ==1)
						 	printf("	       p%d leaves Q and begins service at S, time in Q  = 0.000s, requesting %.3lfs of service\n", 1, ptr->service);
						 	
						 server_flag = 1;

					}
					else if(ptr->type==1)
					{
						//a = 'd';
						printf("%012.3lfs: p%d departs from S, service time = %.3lfs, time in system = %.3lfs\n", ptr->timeval, ptr->id, ptr->service, ptr->timeval - ptr->equeue);
						total_time_in_system = total_time_in_system + ptr->timeval - ptr->equeue; 
						total_syst_sq = total_syst_sq + ((ptr->timeval - ptr->equeue)*(ptr->timeval - ptr->equeue)); 
						total_service_time = total_service_time + ptr->service;
						prev_time = ptr->timeval;
						server_flag=0;
						departure++;
					}
					else
					{
						if(arrival==departure)
						{

							printf("               p%d leaves Q and begins service at S, time in Q = %.3lfs, requesting %.3lfs of service\n", ptr->id, (ptr->exitq - ptr->equeue) ,ptr->service);
							total_time_in_queue = total_time_in_queue + ptr->exitq - ptr->equeue;
						}
						else
						{
							printf("               p%d leaves Q and begins service at S, time in Q = %.3lfs, requesting %.3lfs of service\n", ptr->id, (ptr->exitq - ptr->equeue) ,ptr->service);
							total_time_in_queue = total_time_in_queue + ptr->exitq - ptr->equeue;
						}
						//total_time_in_queue = total_time_in_queue + prev_time - cur;
						
						//prev_time = ptr->timeval;
					//	printf("               p1 leaves Q and begins service at S, time in Q = 0.000s, requesting 2.850s of service\n");
						server_flag = 1;
					}
					//printf("%c 		%012.3lfs       %5d\n",a, ptr->timeval,ptr->id);
					ptr=ptr->link;
					front = ptr;
					//free(ptr);
					
			}
			else
				break;

		}
	}
	//printf("               simulation ends\n\n");
}


//===================================================== Reading input from command line ================================================================

void read_input(int argc, char* argv[])
{
   int j;
   for(j=1; j<argc; j=j+2)
   {
     //printf("Argv: %s 	%s\n", argv[j], argv[j+1] );
      if (argv[j][0]!='-')
      {
         printf("1. Malformed Command: commandline argument must begins with a '-' character\nCommand line syntax should be : \nmm1 [-lambda lambda] [-mu mu] [-det num] [-exp num] [-s seed] [-t tsfile]\n");
         exit(0);
      }

      if (argv[j][0]=='-' && argv[j+1]==NULL)
      {
         printf("2. Malformed Command: Value cannot be empty\nCommand line syntax should be : \nmm1 [-lambda lambda] [-mu mu] [-det num] [-exp num] [-s seed] [-t tsfile]\n");
         exit(0);
      }

      else if (strcmp (argv[j],"-lambda")==0)
      {
         if (argv[j+1][0]=='-')
         {
         	printf("3. Malformed Command: Value cannot be empty\nCommand line syntax should be : \nmm1 [-lambda lambda] [-mu mu] [-det num] [-exp num] [-s seed] [-t tsfile]\n");
         	exit(0);
         }
            

         f1=1;
         lam_flag=strtod(argv[j+1], NULL);
         
         if (lam_flag<=0)
         {
         	printf ("Incorrect Input for lambda\n");
         	exit(0);
         }
            
      }
      else if (strcmp (argv[j],"-mu")==0)
      {
         if (argv[j+1][0]=='-')
         {
         	printf("4. Malformed Command: Value cannot be empty\nCommand line syntax should be : \nmm1 [-lambda lambda] [-mu mu] [-det num] [-exp num] [-s seed] [-t tsfile]\n"); 
         	exit(0);
         }
            

         f2=1;
         mu_flag=atof(argv[j+1]);

         if (mu_flag<=0)
         {
         	printf ("Incorrect Input for mu\n");
         	exit(0);
         }
            
      }

      else if (strcmp (argv[j],"-det")==0)
      {
         if (argv[j+1][0]=='-')
         {
         	printf("Malformed Command: Value cannot be empty\nCommand line syntax should be : \nmm1 [-lambda lambda] [-mu mu] [-det num] [-exp num] [-s seed] [-t tsfile]\n");
         	exit(0);
         }
            
        	fd = 1;
         det_flag=strtoul(argv[j+1],NULL,0);

         if (det_flag<=0 || det_flag> 2147483647)
         {
         	printf ("Incorrect Input for -det\n");
         	exit(0);
         }
            
      }
      else if (strcmp (argv[j],"-exp")==0)
      {
         if (argv[j+1][0]=='-')
         {
         	printf("Malformed Command: Value cannot be empty\nCommand line syntax should be : \nmm1 [-lambda lambda] [-mu mu] [-det num] [-exp num] [-s seed] [-t tsfile]\n");
         	exit(0);
         }
            

         fe = 1; fd=0;
         det_flag=strtoul(argv[j+1],NULL,0);
         if (det_flag<=0 || det_flag> 2147483647)
         {
         	printf ("Incorrect Input for -exp\n");
         	exit(0);
         }

      }
      else if (strcmp (argv[j],"-s")==0) 
      {
         if (argv[j+1][0]=='-')
         {
         	printf("Malformed Command: Value cannot be empty\nCommand line syntax should be : \nmm1 [-lambda lambda] [-mu mu] [-det num] [-exp num] [-s seed] [-t tsfile]\n");
         	exit(0);
         }
            

         fs = 1; fd=0;
         s_flag = atoi(argv[j+1]);
         if (s_flag<=0)
         {
         	printf ("Incorrect Input for -s\n");
         	exit(0);
         }
         	
         
      }
      else if (strcmp (argv[j],"-t")==0)
      {
         ft=1; fd=0;
         fp = fopen(argv[j+1], "rb");
         fp_ctr = fopen(argv[j+1], "rb");
         strcpy(filename, argv[j+1]);
         
         //strcpy(filename)
         if(!fp)
         {
         	printf("Error opening file.\n");
         	exit(0);
         }
         else
         {
         	while(!feof(fp_ctr))
			{
			  char ch = fgetc(fp_ctr);
			  if(ch == '\n')
			  {
			    lines++;
			  }
			}
			det_flag = lines-1;
			fclose(fp_ctr);
         	
         }
      }
      else
      {
         printf ("Malformed Command: Command line syntax should be : \nmm1 [-lambda lambda] [-mu mu] [-det num] [-exp num] [-s seed] [-t tsfile]\n");
      }
   }
}

//============================================================== Main method =============================================================================

double temp_var = 0.0;
int main(int argc, char * argv[])
{
/*
	Depending on the mode of execution, paramaters are set and the events are created and pushed to event queue
	Prints simulation parameters based on given input and the mode of execution.
	3 events - 
	event 0: Packet arrives at the queue/ Packet created. 
	event 1: Previous packet departs from the server and current packet leaves queue and enters server for service.
	event 2: Packet finishes service at the server.  
*/
	read_input(argc, argv);
	int i;
	if(f1==0)
	{
		lam_flag = 1.0;
	}
	if(f2==0)
	{
		mu_flag = 0.7;
	}
	if((fd==1 && fe==1) || (fe==1 && ft==1) || (fd==1 && ft==1))
	{
		printf("Please specify one mode for execution.\n");
		exit(0);
	}
	lam_time = 1/lam_flag;
	mu_time = 1/mu_flag;
	printf("\nSimulation Parameters: \n");
	printf("	number to arrive = %d\n", (int)det_flag );
	if(ft==0 && fe==0)
	{
		//printf("Det: %ld\n", det_flag );
		printf("	lambda = %d\n", (int)lam_flag );
		printf("	mu = %.2lf\n", mu_flag );
	}
	if(fe==1)
	{
		printf("	lambda = %d\n", (int)lam_flag );
		printf("	mu = %.2lf\n", mu_flag );
		printf("	mode = exp\n");
		printf("	seed = %d\n", s_flag );
	}
	else if(fd==1)
	{
		printf("	mode = det\n");
	}
	if(ft==1)
	{
		char* bname = strdup(filename);
		char * bname1 = basename(bname);
    		printf("	tsfile = %s\n", bname1);
	}


	if(fe==1)
		InitRandom(s_flag);
//--------------------------------------------------------------------- event creation -------------------------------------------------------------------------------------	
	printf("\n%012.3lfs: simulation begins\n", 0.0);
	for(i=1;i<=det_flag;i++) // 3 events for each packet created. 
	{
		if(fe==1)
		{
			lam_time = ExponentialInterval(lam_flag)/1000.0;
			mu_time = ExponentialInterval(mu_flag)/1000.0;
		}

		if(fd==1)
		{
			lam_time = round(lam_time*1000)/1000.0;
			mu_time = round(mu_time*1000)/1000.0;

		}

		if(ft==1)
		{
			char *temp_buf = (char*)malloc(20);
         	fgets(temp_buf, 20, fp);
			fscanf (fp, "%lf", &lam_time);
			fscanf ( fp, "%lf", &mu_time );
			lam_time = lam_time/1000.0;
			mu_time = mu_time/1000.0;
		}

		//====================================================event 1 - arrival ===========================================================

		struct event * temp1 = malloc(sizeof(struct event));
		temp1->id = i;
		temp1->type=0; // arrival

		temp1->timeval = pkt_arrival_time + lam_time;
		pkt_arrival_time = pkt_arrival_time + lam_time;
		temp1->equeue = pkt_arrival_time;
		temp1->service = mu_time;

		insert(temp1);

		//================================= event 2 - current pkt leaves queue and previous pkt leaves server==================================

		if(i!=1)
		{
			// event 2 - one pkt leaves server, next pkt enters server
			struct event * temp3 = malloc(sizeof(struct event));
			temp3->id = i;
			temp3->type=2;
			if(prev_pkt<lam_time)
			{
				temp3->timeval = max(temp1->timeval, prev_finish);
			}
			else
			{
				temp3->timeval = prev_finish;
			}

			temp3->service = mu_time;
			temp3->equeue = pkt_arrival_time;
			temp3->exitq = temp3->timeval;
			temp_var = temp3->timeval;
			insert(temp3);
		}
		
		
		//====================================event 3 - departs from server ===========================================

		// event 3 - departure
		struct event * temp2 = malloc(sizeof(struct event));
		temp2->id=i;
		temp2->type=1; 
		
		if(i==1)
		{
			servicing_time = servicing_time + temp1->timeval + mu_time;
		}
		else
		{
			servicing_time = temp_var + mu_time;
		}
		temp2->timeval = servicing_time;
		temp2->service = mu_time;
		temp2->equeue = pkt_arrival_time;
		insert(temp2);

		prev_finish = servicing_time;
		prev_pkt = mu_time;
		if(i!=1)
			display(temp1); // function to display simalation times and pop events that are done..

	}
	printf("               simulation ends\n\n");
	printf("Statistics: \n\n"); 
	printf("	average packet inter-arrival time =  %.6gs\n", total_iat/det_flag);
	printf("	average packet service time = %.6gs\n\n", total_service_time/det_flag );

	printf("	average number of packets in Q = %.6g packets\n", total_time_in_queue/prev_time);

	printf("	average number of packets at S = %.6g packets\n\n", total_service_time/prev_time );

	printf("	average time a packet spent in system = %.6gs\n", total_time_in_system/det_flag );
	double variance = (total_syst_sq/det_flag) - ((total_time_in_system/det_flag)*(total_time_in_system/det_flag));

	printf("	standard deviation for time spent in system = %.6g\n\n", sqrt(variance));

	

return 0;
}
//====================================================================== end of main =======================================================================================
