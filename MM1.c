/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Ricardo Manzano
 *
 * Created on March 3, 2018, 3:09 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct{
    double time_ES;
    int name_ES;            //1=ARRIVAL, 2=DEPARTURE, 3=OBSERVER
}event;
event ES[100000000];  //EVENT SCHEDULER
event ES_O[10000000]; //ARRAY TO RECORD OBSERVER TIMES IN ORDER
event ES_AD[10000000]; //ARRAY TO RECORD ARRIVAL AND DEPARTURES IN ORDER

long long int pointer_ES;   //POINTER TO MOVE IN THE ARRAY EVENT SCHEDULER
long long int pointer_ES_O; //POINTER TO MOVE IN THE ARRAY OBSERVER
long long int pointer_ES_AD; //POINTER TO MOVE IN THE ARRAY ARRIVALS AND DEPARTURES

//time of simulation
double time_simulation=10000;  //Time of simulation

//variables of interest
double alfa=35; //average number of observer events per second
double length=20000; //average length of a packet
double c=2000000;  // transmission rate (bit/s)
double lambda=35; // average number of packets generated per second

//calculate arrival and departures
long long int n;  //variable used to count number of packets in system
double time;      //this variable record the current time in simulation. 
double tdeparture; //variable to record a departure
double tarrival;  //variable to record an arrival

//parameters to dequeue
 double time_current;  //variable to store the time used to dequeue each event of the event scheduler
 int event_dequeue;  //variable which describes the type of event 1=arrival, 2=departure, 3=observer
 long long int pointer_dequeue; //pointer to move in the ES structure array
             
        
   //parameters to calculate performance metrics
        long long int nq;//declare variable number of packets in queue
        int server_status;//variable of server_status 1=busy, 0=idle
        double na;  //number of arrivals
        double number_customers_system_see_arrrival[1000000];//array to save number of packets in system Na-Nd seems by arrival
        double  number_customers_queue_system_see_arrrival[1000000];//array to save number of packets in queue Nqa-Nqd seems by arrival
        long long int pointer_number_customers_arrival; //pointer to save values
        double no;  //number of observers
        double number_customers_system_see_observer[1000000];//array to save number of packets in system Na-Nd seems by observer
        double  number_customers_queue_system_see_observer[1000000];//array to save number of packets in queue Nqa-Nqd seems by observer
        long long int pointer_number_customers_observer; //pointer to save values
        double nd;  //number of departures
        double nidle; // variable to store the times when a observer comes to the system and see if the server is idle 
  
               
        

int main() {
    
    n=0;                        //Initialize variable of number of customers
    pointer_ES=0;               //Initialize pointer to move within the event scheduler
    pointer_ES_O=0;             //Initialize pointer to move in the array absolute times in order
    observer_generate();        //Function to generate observer times in order 
    calculate_arrival_departure(); //Function to calculate departures with the arrivals in order
    order();                       //Function to order both arrays. Array observer. Array arrival-departure
    dequeue_initialization();      //Function to initialize variables to dequeue
    
     while(pointer_dequeue<pointer_ES){                 //Loop until all the events in the event scheduler were analyzed. Pointer_dequeue is a variable to move along the array
         time_current=ES[pointer_dequeue].time_ES;      //Time_current will have the current time of the event dequeue   
         event_dequeue=ES[pointer_dequeue].name_ES;     //Event_dequeue will have the type of event which is dequeuing
         pointer_dequeue++;                             //After analyze the first event. The pointer moves to the next event
            switch(event_dequeue)                       //This conditional is a switch and it analyzes the type of event which is in the variable event_dequeue
             {
                case 1:                                 //If the variable event_dequeue is 1, the event is an arrival
                    arrive_dequeue();
                    break;
                case 2:                                  //If the variable event_dequeue is 2, the event is an departure
                    departure_dequeue();
                    break;
                case 3:
                    observer_dequeue();                 //If the variable event_dequeue is 3, the event is an observer
                    break;
              }
          }
         
       //PRINT PERFORMANCE METRICS
       //IDLE TIME AS SEEN BY AN OBSERVER
          printf("IDLE TIME %lf \n",nidle/no);               //The idle time as seen as by an observer is the number of times the observer sees the system idle over the number that the observer comes to the system
       //CALCULATE THE AVERAGE NUMBER PACKETS IN THE SYSTEM AND IN QUEUE AS SEEN BY THE ARRIVAL   
          double avg_number_packets_system_see_arrival=0;       //Variable to add all the values of the number of packets in the system each time that the event was an arrival
          double avg_number_packets_queue_see_arrival=0;        //Variable to add all the values of the number of packets in queue each time that the event was an arrival
          int r;                                                //Pointer to move along the arrays for arrivals
          for(r=0;r<=pointer_number_customers_arrival;r++)      //Loop to move along the all of the values in the arrays
          {
          avg_number_packets_system_see_arrival=avg_number_packets_system_see_arrival+number_customers_system_see_arrrival[r]; //Variable to save the cumulative addition of Na-Nd
          avg_number_packets_queue_see_arrival=avg_number_packets_queue_see_arrival+number_customers_queue_system_see_arrrival[r]; //Variable to save the cumulative addition of packets in as seen by an arrival
          }
            printf("AVG NUMBER OF PACKETS IN SYSTEM AS SEEN BY AN ARRIVAL %lf\n", avg_number_packets_system_see_arrival/na); //Formula to calculate average number of packets in system as seen by arrival 
            printf("AVG NUMBER OF PACKETS IN QUEUE AS SEEN BY AN ARRIVAL %lf\n", avg_number_packets_queue_see_arrival/na);  //Formula to calculate average number of packets in queue as seen by arrival       
     
            //CALCULATE THE AVERAGE NUMBER PACKETS IN THE SYSTEM AND IN QUEUE AS SEEN BY THE OBSERVER   
          double avg_number_packets_system_see_observer=0;  //Variable to add all the values of the number of packets in the system each time that the event was an observer
          double avg_number_packets_queue_see_observer=0;   //Variable to add all the values of the number of packets in queue each time that the event was an observer
          for(r=0;r<=pointer_number_customers_observer;r++) //Loop to move along the all of the values in the arrays
          {
           avg_number_packets_system_see_observer=avg_number_packets_system_see_observer+number_customers_system_see_observer[r];//Variable to add all the values of the number of packets in the system each time that the event was an observer
          avg_number_packets_queue_see_observer=avg_number_packets_queue_see_observer+number_customers_queue_system_see_observer[r];//Variable to add all the values of the number of packets in queue each time that the event was an observer
          }
            printf("AVG NUMBER OF PACKETS IN SYSTEM AS SEEN BY AN OBSERVER %lf\n", avg_number_packets_system_see_observer/no); //Formula to calculate average number of packets in system as seen by an observer
            printf("AVG NUMBER OF PACKETS IN QUEUE AS SEEN BY AN OBSERVER %lf\n", avg_number_packets_queue_see_observer/no);  //Formula to calculate average number of packets in queue as seen by an observer
          
            return 0;
}

//FUNCTION TO GENERATE ABSOLUTE TIMES FOR OBSERVER
observer_generate(main){
    double time;                                //Current time of the event
    time=0;                                     //Time initialization time=0s
        while(time<time_simulation)             //Loop until time_simulation is reached
    {
    ES_O[pointer_ES_O].time_ES=time+(-(log(1-((double)rand()/(double)RAND_MAX)))*(1/alfa)); //Time+ exp_random(alfa)
    ES_O[pointer_ES_O].name_ES=3;                                                           //Type of event 3=observer
    time=ES_O[pointer_ES_O].time_ES;                                                        //Time update to the current event
    pointer_ES_O++;                                                                         //Pointer advance to save the next value in the next position
     }
 };
 //FUNCTION TO CALCULATE DEPARTURE TIMES WITH ARRIVALS
 calculate_arrival_departure(){
      pointer_ES_AD=0;         //pointer to advanced in the array arrival-departure.
      time=0;                  //Time of simulation starts at 0s
      tarrival=time+(-(log(1-((double)rand()/(double)RAND_MAX)))*(1/lambda)); //The first arrival occurs at time=0s + the first random inter-arrival time
      tdeparture=1e+10;        //In the beginning the time of departure tends to infinity because it is impossible that a departure occurs without an arrival
 
      while (time<=time_simulation)  //Loop to calculate departure times of packets taking into consideration arrival time
    {        
             //arrival
                              
            if(tarrival<tdeparture)  // Compare if the time of arrival is less than time of departure. If this condition is accomplished, the event is an arrival
            { time=tarrival;         // The current time is updated to the event arrival
              ES_AD[pointer_ES_AD].time_ES=time; //This event is recorded into the array arrival-departure like an arrival
              ES_AD[pointer_ES_AD].name_ES=1;  //This event is recorded with the number 1=Arrival
              pointer_ES_AD++;                 //The pointer of the array arrival-departure advance to next position to save the next event
              n++;                             //The number of packets in the system increases in 1.
              tarrival=time+(-(log(1-((double)rand()/(double)RAND_MAX)))*(1/lambda)); //The next arrival will occur at time current time+ random inter-arrival time
              if(n==1)                        //When there is 1 packet in the system. This packet will leave of the system at time: current time+time of service
                { tdeparture=time+(-(log(1-((double)rand()/(double)RAND_MAX)))*(length/c));// Current time+time of service
                }
                  
            }
                          
            //departure
            if(tdeparture<tarrival)            //If the time of departure is less than the future arrival. Then the event is a departure
            { time=tdeparture;                   //The current time now is a departure
              ES_AD[pointer_ES_AD].time_ES=time; //The event departure is recorded in the array arrival-departure
              ES_AD[pointer_ES_AD].name_ES=2;    //This event is recorder with the number 2=Departure 
              pointer_ES_AD++;                 //The pointer of the event scheduler advance to next position to save the next event
              n--;                             //This is an event departure. For this reason with this sentence the number of customers in the system decrease in one.
              if(n>0){                         //If there is at least 1 packet in the system the condition is accomplished. The system is not idle
                tdeparture=time+(-(log(1-((double)rand()/(double)RAND_MAX)))*(length/c)); //The next departure will occur at tdeparture
                  }
                if (n==0)                      //If there is not packet in the system. The system is idle 
                {tdeparture=1e+30;             //Therefore tdeparture should be so large. It is impossible that there is a departure without a packet in the system
                }
              
            }
        } 
 }
 //FUNCTION TO ORDER BOTH ARRAYS. OBSERVER ARRAY AND ARRIVAL_DEPARTURE ARRAY
 order()
 {
  long long int ri,ca,length,pa; 
    ri=0;                                                //Pointer to move successively in the first array
    ca=0;                                                //Pointer to move successively in the second array
    length=pointer_ES_AD+pointer_ES_O;                   //Total length of the resulting array after join two ordered arrays
        while (ri <pointer_ES_O  && ca < pointer_ES_AD)  //Loop to move in both arrays simultaneously
    {
            if (ES_O[ri].time_ES <= ES_AD[ca].time_ES) { //In this condition, the values of the same position of both arrays are taken
                  ES[pointer_ES] =ES_O[ri];              //If the value of the first array is less than the second array, the value is recorded in the Event scheduler 
                  ri++;                                  //The value of the next position in the first array will be compared with the same value of the second array
            } else {                                     //If the value of the second array is less than the value of the first array is recorded in the event scheduler
                  ES[pointer_ES] = ES_AD[ca];            //The value is recorded in the event scheduler
                  ca++;                                  //The next value of the second array will be taken to compare
            }
            pointer_ES++;                                //The pointer of the event scheduler should be advanced in one to record the next order event
      }
      if (ri < pointer_ES_O) {                            //This condition is useful if one of the arrays has more elements than the other. In this case this condition only takes the remaining ordered values of the array and put it in the ES.
            for (int pa = ri; pa < pointer_ES_O; pa++) {
                  ES[pointer_ES] = ES_O[pa];
                  pointer_ES++;
            }
      } else {                                              //This condition is useful if one of the arrays has more elements than the other. In this case this condition only takes the remaining ordered values of the array and put it in the ES.
            for (int pa = ca; pa < pointer_ES_AD; pa++) {
                  ES[pointer_ES] = ES_AD[pa];
                  pointer_ES++;
            }
      }
    /*   //This part is helpful if it is necessary to see the values generated in the time scheduler
     long long int i;
     char s1[]="Arrival";   //Plot arrival
     char s2[]="Departure";  //Plot observer
     char s3[]="Observer"; //Plot departure
     
     for(i=0;i<pointer_ES;i++) //This loop it will plot all the event that happened in the simulation
      { 
          
          if (ES[i].name_ES==1){
             printf("Time %lf, Event %s \n",ES[i].time_ES,s1); 
          }
          if (ES[i].name_ES==2){
             printf("Time %lf, Event %s \n",ES[i].time_ES,s2 ); 
          }            
          if (ES[i].name_ES==3){
             printf("Time %lf, Event %s \n",ES[i].time_ES,s3 ); 
          }         
      }*/
     
 }
 
 
 dequeue_initialization(){
        time_current=0;     //Variable to store the current time to dequeue the event scheduler
        event_dequeue=0;    //Variable to store the value of the current event of the event scheduler
        pointer_dequeue=0;  //Variable to move within the event scheduler
        server_status=0;    //Initialize the server status as idle
        n=0;                //Initialize the number of packets in the system
        nq=0;               //Initialize the number of packets in queue
        na=0;               //Initialize the number of arrivals
        nd=0;               //Initialize the number of departures
        no=0;               //Initialize the number of observers
        pointer_number_customers_arrival=0;  //Pointer to move within the array which will contain Na-Nd as seen by the arrival
        pointer_number_customers_observer=0; //Pointer to move within the array which will contain Na-Nd as seen by the observer
        nidle=0;            //Initialize the number of times that an observer sees the server is idle
        }
 
     
  arrive_dequeue(main)
     {   number_customers_system_see_arrrival[pointer_number_customers_arrival]=na-nd;    //Array to store number of packets in the system at time a packet arrivals
         number_customers_queue_system_see_arrrival[pointer_number_customers_arrival]=nq; //Array to store number of packets in the queue at time a packet arrivals
         pointer_number_customers_arrival++;                                              //This pointer is useful to move to the next position both arrays described above
         na=na+1;                                                                         //When the event is an arrival, it adds one unit to the variable 
         if(server_status==1)                                                             //This condition verify if the server is busy
         { nq= nq+1;                                                                      //If the server is busy, the packet should be wait in a queue
          }
         if(server_status==0)                                                             //If the server status is idle, with the event that occurs the server should change the status to busy.
         {
             server_status=1;
         }
     };
     
          departure_dequeue(main)
     {       int s;
             nd=nd+1;                      //Cumulative variable of number of departures that happened in the system
             if (nq==0)                    //If there is not packets in queue with this event departure, the server status change to idle
             {
             server_status=0;
             }      
             else                          //If there is packets in queue, one unit is substract of the variable nq
             {
                 nq--;
            }
         
       };
       
       observer_dequeue(main)
     {   no=no+1;                                                                       //Number of times that an observer comes to the system
         number_customers_system_see_observer[pointer_number_customers_observer]=na-nd;  //Array to store number of packets in the system when the event is an observer
         number_customers_queue_system_see_observer[pointer_number_customers_observer]=nq; //Array to store number of packets in queue when the event is an observer
         pointer_number_customers_observer++;                                              //Pointer to advanced one position in the two arrays described above
         if (server_status==0)                           //If the system is idle the condition is accomplished
            {                 
            nidle=nidle+1;                               //Variable to calculate time idle as seen by an observer                             
            };
       };
      
