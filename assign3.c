/**
* Author: Sidney Shane Dizon
* UCID: 10149277
* Date: March 14, 2018
*
* This is a program that can calculate goood routing paths for the Hobbit reunion meeting
* Input:
*   The program will read a network topology file (representing the map), and starting
*   location information about each of the dwarves.
* Output:
*   The program will display information about the travel route recommended for each dwarf,
*   and some simple summary statistics to quantify how good these routes are.
* Routing Algorithms Supported:
*   1. Shortest Hop Path (SHP)
*   2. Shortest Distance PAth (SDP)
*   3. Shortest Time Path (STP)
*   4. Fewest Trolls Path (FTP)
*
*/

/* Standard Libraries */
 #include <stdio.h>
 #include <unistd.h>
 #include <stdlib.h>

 /* Library for parsing Strings */
 #include <string.h>
 #include<curses.h>
 #include<sys/types.h>
 #include<math.h>

 //smaller file
 #define TOPOLOGY_MAP "topologyMap1.txt"
 #define DWARF_STARTING_LOCATION "dwarfLocation1.txt"
 //bigger file
 //#define TOPOLOGY_MAP "fullMap.txt"
 //#define DWARF_STARTING_LOCATION "completeAddress.txt"
 #define IN 10000 //temporary value for infinty
 #define N 26 //maximum number of nodes possible

 //methods
 int dijkstra(int cost[][N], int source, int target);
 char *strrev(char *str);
 char * dijkstra2(int cost[N][N],int source,int target);
 int calculateTotalCost(int cost[N][N], char * str);



int main(void){
  /* Open and Read the Topology File and dwarf locations */
  FILE *file_ptr_map, *file_ptr_dwarfs, *file_ptr_map_copy, *file_ptr_dwarfs_copy;
  // Opening the file for the dwarves to count number of dwarves
  file_ptr_dwarfs_copy = fopen(DWARF_STARTING_LOCATION, "r");
  if (!file_ptr_dwarfs_copy){
    printf("Error in reading the dwarf location file.\n");
    return 1;
  } else {
    //printf("%s is open for reading.\n", DWARF_STARTING_LOCATION);
  }
  //count the number of dwarves
  int no_of_dwarves = 0;
  char file_content[1200];
  while(fgets(file_content, 1200, file_ptr_dwarfs_copy) != NULL){
    no_of_dwarves++;
  }
  fclose(file_ptr_dwarfs_copy);
  //printf("The no of dwarves are: %d\n", no_of_dwarves);
  //Opening the file for the map to store the values in 2D array
  file_ptr_map = fopen(TOPOLOGY_MAP, "r");
  if (!file_ptr_map){
    printf("Error in reading the topology file.\n");
    return 1;
  } else {
    //printf("%s is open for reading.\n", TOPOLOGY_MAP);
  }
  // Read and store the contents of the map to 2D arrays
  int propdelay[N][N], distances[N][N], golds[N][N], trolls[N][N], hop_path[N][N];
  //Initialize the 2D array as infinity values
  for(int i=0; i<N; i++){
    for(int j=0; j<N; j++){
      if(i==j){
        propdelay[i][j] = 0;
      } else {
        propdelay[i][j] = IN;
      }
    }
  }
  for(int i=0; i<N; i++){
    for(int j=0; j<N; j++){
      if(i==j){
        distances[i][j] = 0;
      } else {
        distances[i][j] = IN;
      }
    }
  }
  for(int i=0; i<N; i++){
    for(int j=0; j<N; j++){
      if(i==j){
        golds[i][j] = 0;
      } else {
        golds[i][j] = IN;
      }
    }
  }
  for(int i=0; i<N; i++){
    for(int j=0; j<N; j++){
      if(i==j){
        trolls[i][j] = 0;
      } else {
        trolls[i][j] = IN;
      }
    }
  }
  for(int i=0; i<N; i++){
    for(int j=0; j<N; j++){
      if(i==j){
        hop_path[i][j] = 0;
      } else {
        hop_path[i][j] = IN;
      }
    }
  }

  /* Store the values of the Topology Map to the 2D arrays */
  char start, destination;
  int delay, distance, gold, troll;
  int row, col;
  while(fscanf(file_ptr_map, "%c %c %d %d %d %d\n", &start, &destination, &distance, &delay, &gold, &troll) == 6) {
    row = start - 'A';
    col = destination - 'A';

    propdelay[row][col] = propdelay[col][row] = delay;
    distances[row][col] = distances[col][row] = distance;
    golds[row][col] = golds[col][row] = gold;
    trolls[row][col] = trolls[col][row] = troll;
    hop_path[row][col] = hop_path [col][row] = 1;

  }

  fclose(file_ptr_map);

  //Open and read the address book
  file_ptr_dwarfs = fopen(DWARF_STARTING_LOCATION, "r");
  if (!file_ptr_dwarfs){
    printf("Error in reading the dwarf location file.\n");
    return 1;
  } else {
    //printf("%s is open for reading.\n", DWARF_STARTING_LOCATION);
  }

  //store the name and home of each dwarf
  char name[12];
  char home;
  char dwarf_names[no_of_dwarves][12];
  char dwarf_homes[no_of_dwarves];
  int row_name = 0;
  while(fscanf(file_ptr_dwarfs, "%s %c", &name, &home) == 2){
    strcpy(dwarf_names[row_name], name);
    dwarf_homes[row_name] = home;
    row_name++;
  }
  fclose(file_ptr_dwarfs);

  //Find the final destination of the path
  int end_destination;
  for(int i=0; i<no_of_dwarves; i++){
      if (strcmp(dwarf_names[i], "Bilbo") == 0){
        end_destination = dwarf_homes[i] - 'A';
      }
  }

  //Now Use start_locations to find out the path for each dwarf
  int start_locations[no_of_dwarves-1];
  char start_locations_names[no_of_dwarves-1][12];
  int convert_to_int;
  int index = 0;
  for(int i=0; i<no_of_dwarves; i++){
    convert_to_int = dwarf_homes[i] - 'A';
    if(convert_to_int != end_destination){
      start_locations[index] = convert_to_int;
      strcpy(start_locations_names[index], dwarf_names[i]);
      index++;
    } else {
      //printf("start = end = %d\n", convert_to_int);
    }
  }

  //ask for the user which Algorithm to use
  printf("Which algorithm do you want to show results: SHP, SDP, STP, FTP\n");
  char user_input[5];
  scanf("%s", user_input);
  //printf("input: %s\n", user_input);

  //For storing the cost values for each categories
  char result_path_traversal[no_of_dwarves-1][no_of_dwarves];
  char *p;
  int distance_cost[no_of_dwarves-1];
  int time_cost[no_of_dwarves-1];
  int troll_cost[no_of_dwarves-1];
  int gold_cost[no_of_dwarves-1];
  int hop_cost[no_of_dwarves-1];
  char algorithm_name[50];

  //For calculating average
  float average_hops, average_distance, average_time, average_gold, average_trolls;
  int hop_total, distance_total, time_total, gold_total, trolls_total;

  /* Iterate through the start locations to get the cost for each category */
  //SHP (Shortest Hop Path)
  if (strcmp(user_input, "SHP") == 0){
    strcpy(algorithm_name, "SHP (SHORTEST HOP PATH)");
    for(int i=0; i<no_of_dwarves-1; i++){
      //printf("%d\n",result_table[i][0]);
      hop_cost[i] = dijkstra(hop_path, start_locations[i], end_destination);
      hop_total = hop_total + hop_cost[i];
      //printf("\n%c = %d\n", result_table[i][0] + 'A', result_table[i][1]);
      //result_table[i][dist] = dijkstra(distances, result_table[i][node_col], end_destination);
    }
    average_hops = hop_total / (float)(no_of_dwarves-1);

    //Get the path traversed
    for(int i=0; i<no_of_dwarves-1; i++){
      p = dijkstra2(hop_path, start_locations[i], end_destination);
      //printf("p = %s\n", p);
      strcpy(result_path_traversal[i], p);
    }

    //find out the costs depending on the path traversed
    //Distance Costs
    for(int i=0; i<no_of_dwarves-1; i++){
      distance_cost[i] = calculateTotalCost(distances, result_path_traversal[i]);
      distance_total = distance_total + distance_cost[i];
    }
    average_distance = distance_total / (float)(no_of_dwarves-1);

    //time_cost
    for(int i=0; i<no_of_dwarves-1; i++){
      time_cost[i] = calculateTotalCost(propdelay, result_path_traversal[i]);
      time_total = time_total + time_cost[i];
    }
    average_time = time_total / (float)(no_of_dwarves-1);

    //Gold Costs
    for(int i=0; i<no_of_dwarves-1; i++){
      gold_cost[i] = calculateTotalCost(golds, result_path_traversal[i]);
      gold_total = gold_total + gold_cost[i];
    }
    average_gold = gold_total / (float)(no_of_dwarves-1);

    //Troll Costs
    for(int i=0; i<no_of_dwarves-1; i++){
      troll_cost[i] = calculateTotalCost(trolls, result_path_traversal[i]);
      trolls_total = trolls_total + troll_cost[i];
    }
    average_trolls = trolls_total / (float)(no_of_dwarves-1);

  }
  //SDP (Shortest Distance Path)
  else if (strcmp(user_input, "SDP") == 0){
    strcpy(algorithm_name, "SDP (SHORTEST DISTANCE PATH)");
    for(int i=0; i<no_of_dwarves-1; i++){
      //printf("%d\n",result_table[i][0]);
      distance_cost[i] = dijkstra(distances, start_locations[i], end_destination);
      distance_total = distance_total + distance_cost[i];
      //printf("\n%c = %d\n", result_table[i][0] + 'A', result_table[i][1]);
      //result_table[i][dist] = dijkstra(distances, result_table[i][node_col], end_destination);
    }
    average_distance = distance_total / (float)(no_of_dwarves-1);

    //Get the path traversed
    for(int i=0; i<no_of_dwarves-1; i++){
      p = dijkstra2(distances, start_locations[i], end_destination);
      //printf("p = %s\n", p);
      strcpy(result_path_traversal[i], p);
    }

    //find out the costs depending on the path traversed
    //Hop Costs
    for(int i=0; i<no_of_dwarves-1; i++){
      hop_cost[i] = calculateTotalCost(hop_path, result_path_traversal[i]);
      hop_total = hop_total + hop_cost[i];
    }
    average_hops = hop_total / (float)(no_of_dwarves-1);

    //time_cost
    for(int i=0; i<no_of_dwarves-1; i++){
      time_cost[i] = calculateTotalCost(propdelay, result_path_traversal[i]);
      time_total = time_total + time_cost[i];
    }
    average_time = time_total / (float)(no_of_dwarves-1);

    //Gold Costs
    for(int i=0; i<no_of_dwarves-1; i++){
      gold_cost[i] = calculateTotalCost(golds, result_path_traversal[i]);
      gold_total = gold_total + gold_cost[i];
    }
    average_gold = gold_total / (float)(no_of_dwarves-1);

    //Troll Costs
    for(int i=0; i<no_of_dwarves-1; i++){
      troll_cost[i] = calculateTotalCost(trolls, result_path_traversal[i]);
      trolls_total = trolls_total + troll_cost[i];
    }
    average_trolls = trolls_total / (float)(no_of_dwarves-1);

  }
  //STP (Shortest Time Path)
  else if (strcmp(user_input, "STP") == 0){
    strcpy(algorithm_name, "STP (SHORTEST TIME PATH)");
    for(int i=0; i<no_of_dwarves-1; i++){
      //printf("%d\n",result_table[i][0]);
      time_cost[i] = dijkstra(propdelay, start_locations[i], end_destination);
      time_total = time_total + time_cost[i];
      //printf("\n%c = %d\n", result_table[i][0] + 'A', result_table[i][1]);
      //result_table[i][dist] = dijkstra(distances, result_table[i][node_col], end_destination);
    }
    average_time = time_total / (float)(no_of_dwarves-1);

    //Get the path traversed
    for(int i=0; i<no_of_dwarves-1; i++){
      p = dijkstra2(propdelay, start_locations[i], end_destination);
      //printf("p = %s\n", p);
      strcpy(result_path_traversal[i], p);
    }

    //find out the costs depending on the path traversed
    //Hop Costs
    for(int i=0; i<no_of_dwarves-1; i++){
      hop_cost[i] = calculateTotalCost(hop_path, result_path_traversal[i]);
      hop_total = hop_total + hop_cost[i];
    }
    average_hops = hop_total / (float)(no_of_dwarves-1);

    //distance cost
    for(int i=0; i<no_of_dwarves-1; i++){
      distance_cost[i] = calculateTotalCost(distances, result_path_traversal[i]);
      distance_total = distance_total + distance_cost[i];
    }
    average_distance = distance_total / (float)(no_of_dwarves-1);

    //Gold Costs
    for(int i=0; i<no_of_dwarves-1; i++){
      gold_cost[i] = calculateTotalCost(golds, result_path_traversal[i]);
      gold_total = gold_total + gold_cost[i];
    }
    average_gold = gold_total / (float)(no_of_dwarves-1);

    //Troll Costs
    for(int i=0; i<no_of_dwarves-1; i++){
      troll_cost[i] = calculateTotalCost(trolls, result_path_traversal[i]);
      trolls_total = trolls_total + troll_cost[i];
    }
    average_trolls = trolls_total / (float)(no_of_dwarves-1);

  }
  //FTP (fewest Troll Path)
  else if (strcmp(user_input, "FTP") == 0){
    strcpy(algorithm_name, "FTP (FEWEST TROLL PATH)");
    for(int i=0; i<no_of_dwarves-1; i++){
      //printf("%d\n",result_table[i][0]);
      troll_cost[i] = dijkstra(trolls, start_locations[i], end_destination);
      trolls_total = trolls_total + troll_cost[i];
      //printf("\n%c = %d\n", result_table[i][0] + 'A', result_table[i][1]);
      //result_table[i][dist] = dijkstra(distances, result_table[i][node_col], end_destination);
    }
    average_trolls = trolls_total / (float)(no_of_dwarves-1);

    //Get the path traversed
    for(int i=0; i<no_of_dwarves-1; i++){
      p = dijkstra2(trolls, start_locations[i], end_destination);
      //printf("p = %s\n", p);
      strcpy(result_path_traversal[i], p);
    }

    //find out the costs depending on the path traversed
    //Distance Costs
    for(int i=0; i<no_of_dwarves-1; i++){
      distance_cost[i] = calculateTotalCost(distances, result_path_traversal[i]);
      distance_total = distance_total + distance_cost[i];
    }
    average_distance = distance_total / (float)(no_of_dwarves-1);

    //time_cost
    for(int i=0; i<no_of_dwarves-1; i++){
      time_cost[i] = calculateTotalCost(propdelay, result_path_traversal[i]);
      time_total = time_total + time_cost[i];
    }
    average_time = time_total / (float)(no_of_dwarves-1);

    //Gold Costs
    for(int i=0; i<no_of_dwarves-1; i++){
      gold_cost[i] = calculateTotalCost(golds, result_path_traversal[i]);
      gold_total = gold_total + gold_cost[i];
    }
    average_gold = gold_total / (float)(no_of_dwarves-1);

    //Hop Costs
    for(int i=0; i<no_of_dwarves-1; i++){
      hop_cost[i] = calculateTotalCost(hop_path, result_path_traversal[i]);
      hop_total = hop_total + hop_cost[i];
    }
    average_hops = hop_total / (float)(no_of_dwarves-1);

  }
  //invalid input
  else {
    printf("User input invalid. Restart Again.\n");
    return 1;
  }

  //Print the result to the console
  printf("%s\n", algorithm_name);
  printf("Destination is always Bilbo's home at node C\n\n");
  printf("DWARF\t\tHOME\tHOPS\tDISTANCE\tTIME\tGOLD\tTROLLS\t\tPATH\n");
  printf("---------------------------------------------------------------------------------------------------------\n");
  for(int i=0; i<no_of_dwarves-1; i++){
    printf("%s\t\t%c\t%d\t%d\t\t%d\t  %d\t  %d\t\t", start_locations_names[i], start_locations[i]+'A', hop_cost[i], distance_cost[i], time_cost[i], gold_cost[i], troll_cost[i]);
    for(int j=0; j<strlen(result_path_traversal[i]); j++){
      printf("%c ", result_path_traversal[i][j]);
    }
    printf("\n");
  }
  printf("----------------------------------------------------------------------------------------------------------\n");


  printf("AVERAGE\t\t\t%.2f\t%.1f\t\t%.1f\t%.1f\t%.1f\n", average_hops, average_distance, average_time, average_gold, average_trolls);

}

//Function that calculated the lowest cost
int dijkstra(int cost[N][N],int source,int target) {
  int dist[N],prev[N],selected[N]={0},i,m,min,start,d,j;
  char path[N]; //path traversed
  for(i=1;i< N;i++) { //i = 0
      dist[i] = IN; // Unknown distance function from source to other vertices
      prev[i] = -1; // Previous node in optimal path from source
  }

  start = source;
  selected[start]=1; //nodes that has been visited
  dist[start] = 0;
  //while target is not yet visited
  while(selected[target] == 0) {
      min = IN;
      m = 0;
      //printf("\nstart = %c\n", start+'A');
      for(i=1;i< N;i++) { // i = 0
          d = dist[start] + cost[start][i]; //total cost
          //printf("d = %d < dist[%d] = %d, start[%d = %c] = %d, end[%c] = %d\n", d, i, dist[i], start, start+'A', dist[start], i+'A', cost[start][i]);
          if(d < dist[i]&&selected[i]==0){ //if cost < previous cost distance and node has not yet visited
              dist[i] = d; // cost of the travel
              prev[i] = start;
              //printf("d = %d\n", d);
          }
          if(min>dist[i] && selected[i]==0){ //if min cost distance is greater than current total cost
              min = dist[i];
              m = i;
          }
      }
      start = m;
      selected[start] = 1;
  }
  return dist[target];
}

/* Reverses the string input */
char *strrev(char *str) {
  char *p1, *p2;

  if (! str || ! *str)
        return str;
  for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
  {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
  }
  return str;
}

//Function that returns the traversal path of the lowest cost
char * dijkstra2(int cost[N][N],int source,int target) {
  int dist[N],prev[N],selected[N]={0},i,m,min,start,d,j;
  static char path[N]; //path traversed
  static int path_traversed[N];
  for(i=1;i< N;i++) {
      dist[i] = IN; // Unknown distance function from source to other vertices
      prev[i] = -1; // Previous node in optimal path from source
  }

  start = source;
  selected[start]=1; //nodes that has been visited
  dist[start] = 0;
  //while target is not yet visited
  while(selected[target] == 0) {
      min = IN;
      m = 0;
      //printf("2nd point\n");
      for(i=1;i< N;i++) {
          d = dist[start] + cost[start][i]; //total cost
          if(d < dist[i]&&selected[i]==0){ //if cost < previous cost distance and node has not yet visited
              dist[i] = d;
              prev[i] = start;
          }
          if(min>dist[i] && selected[i]==0){
              min = dist[i];
              m = i;
          }
      }
      start = m;
      selected[start] = 1;
  }
  start = target;
  //Store the path traversed
  j = 0;
  while(start != -1){
      //path_traversed[j] = start;
      path[j] = start+65;
      start = prev[start];
      j++;
  }
  path[j]='\0';
  strrev(path);
  //printf("%s\n", path);


  for(int q=0; q<j; q++){
    path_traversed[q] = path[q] - 'A';
  }

  return path;
}

//Calculates the cost for the traversal path
int calculateTotalCost(int cost[N][N], char * traversal){
  //printf("traversal = %s\n", traversal);
  int total = 0;
  int start, end;
  int j = 1;
  //calculate total cost
  for(int i=0, j=i+1; i<strlen(traversal); i++){
    if (j < strlen(traversal)){
      //printf("%c ", traversal[i]);
      start = traversal[i] - 'A';
      end = traversal[j] - 'A';
      //printf("start = %d\nend = %d\n", start, end);
      total = total + cost[start][end];

      j++;
    }
  }
  //printf("total = %d\n", total);
  return total;
}
