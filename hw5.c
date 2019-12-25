/*
  Author:Anthony Garcia
  Email: anthony2018@my.fit.edu
  Course: CSE 2010
  Section: 02
  Description: Create a digital calendar using skiplist. The app lets the user visualize his calendar, add events, 
  remove events, and request information by time intervals or specific time.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


int getRandHeight();

//structure of the skip_list nodes
typedef struct node{
  struct node *parent;
  struct node *child;
  struct node *previous;
  struct node *next;
  int height;
  unsigned int time;
  char event[30];
}SKIP_NODE;

/*createnew_skipline receives the address of the maximum line of the skip_list and the difference in height between
the maximum and the wanted height. The function creates height_diff amount of empty skip_line and returns the position
of the highest line.*/
SKIP_NODE* createnew_skipline(SKIP_NODE **max_skiplist, int height_diff);
/*removeold_skipline receives the max height of the node removed and returns true(1) or false(0). The function removes
all the extra empty lines in the skip_list.*/
int removeold_skipline(SKIP_NODE **max_skiplist);
/*The add_skiplist function receives the address of the maximum line of the skip list, the height wanted, and the 
values for the event. Adds the value into the skip_list in the height that is wanted and with all its values.*/
void add_skiplist(SKIP_NODE **skipnode, SKIP_NODE **parent_node, int height, int time, char event[]);
/*the remove_skiplist receives the location of the node and removes this node from the skip_list*/
void remove_skiplist(SKIP_NODE **skipnode);
/*the function return true(1) or false(0). The function receives address of a node in one of the lines, the time
that will check, and the name of the event. If the time is found then it will return the address of the node and 
if the time is not found then it will return NULL*/
SKIP_NODE* checkall_skiplist(SKIP_NODE **skipnode, int time);
/*create node function has no parameters but returns a pointer that all the necessary parts initialized*/
SKIP_NODE* create_node();
/*submap_extremum_finder receives the beginning of the skiplist and the time that is wanted. The function returns
the node wanted(or close to the node wanted). Used for finding the extremums for the get event interval.*/
SKIP_NODE* submap_extremum_finder(SKIP_NODE **skipnode, int time);
/*GetEvent receives the maximum line of the skip list and the time. The function finds the node with the wanted time.*/
void GetEvent(SKIP_NODE **max_skiplist, int time);
/*The GetEventBetween receives the maximum line of the skip list and two times. The function finds all the events between 
an interval*/
void GetEventBetween(SKIP_NODE **max_skiplist, int time1, int time2);
/*Print skiplist receives the maximum line of the skip list and prints the whole skip list.*/
void PrintSkipList(SKIP_NODE **skipnode);
/*AddEvent receives the maximum line of the skip list and the values needed to add a node. The function adds a node to the skip
list if possible*/
void AddEvent(SKIP_NODE **max_skiplist, int height, int time, char event[]);
/*CancelEvent receives the maximum line of the skip list and the time. The function eliminates the wanted node from the skip list*/
void CancelEvent(SKIP_NODE **skipnode, int time);
/*Initialize skiplist creates an empty skip_line and returns the address of the left pointer*/
SKIP_NODE* initialize_skiplist();

int main(int argc, char* argv[]){
  //max_skiplist points to the top left corner
  SKIP_NODE *max_skiplist = initialize_skiplist();
  //this variables are for saving the values read from the file
  char word[50];
  char time1[7];
  char time2[7];
  //used for tokenizing the string
  char delim[] = " \n\r";
  //used for reading the whole line
  char complete_line[100];
  FILE *file_pointer = fopen(argv[argc-1], "r");
  while(!feof(file_pointer)){
    //reading the whole line and saving the first word
    fgets(complete_line, 99, file_pointer);
    strcpy(word, strtok(complete_line, delim));
    if (!feof(file_pointer))
      printf("%s ", word);
    //compare with all the string and do the correct "query"
    if (strcmp(word,"AddEvent") == 0 && !feof(file_pointer)){
      strcpy(time1, strtok(NULL, delim));
      strcpy(word, strtok(NULL, delim));
      printf("%.6d %s ", atoi(time1), word);
      //adds the even to the skiplist
      if(checkall_skiplist(&max_skiplist, atoi(time1)) == NULL)
        AddEvent(&max_skiplist, getRandHeight(), atoi(time1), word);
      else
        AddEvent(&max_skiplist, 0, atoi(time1), word);
      printf("\n");
    }
    else if (strcmp(word,"CancelEvent") == 0 && !feof(file_pointer)){
      strcpy(time1, strtok(NULL, delim));
      printf("%.6d ", atoi(time1));
      //eliminates the even from the skiplist
      CancelEvent(&max_skiplist, atoi(time1));
    }
    else if (strcmp(word,"GetEvent") == 0 && !feof(file_pointer)){
      //gets the even from the skiplist
      strcpy(time1, strtok(NULL, delim));
      GetEvent(&max_skiplist, atoi(time1));
    }
    else if (strcmp(word,"GetEventsBetweenTimes") == 0 && !feof(file_pointer)){
      strcpy(time1, strtok(NULL, delim));
      strcpy(time2, strtok(NULL, delim));
      printf("%.6d %.6d ", atoi(time1), atoi(time2));
      //gets the event between a given interval
      GetEventBetween(&max_skiplist, atoi(time1), atoi(time2));
    }
    else if (strcmp(word,"GetEventsForOneDay") == 0 && !feof(file_pointer)){
      strcpy(time1, strtok(NULL, delim));
      printf("%.4d ", atoi(time1));
      //gets the event between a given interval
      GetEventBetween(&max_skiplist, atoi(time1)*100, atoi(time1)*100+23);
    }
    else if (strcmp(word,"GetEventsForTheRestOfTheDay") == 0 && !feof(file_pointer)){
      strcpy(time1, strtok(NULL, delim));
      printf("%.6d ", atoi(time1));
      //gets the event between a given interval
      GetEventBetween(&max_skiplist, atoi(time1), ((int)(atoi(time1)/100))*100+23);
    }
    else if (strcmp(word,"GetEventsFromEarlierInTheDay") == 0 && !feof(file_pointer)){
      strcpy(time1, strtok(NULL, delim));
      printf("%.6d ", atoi(time1));
      //gets the event between a given interval
      GetEventBetween(&max_skiplist, ((int)(atoi(time1)/100))*100, atoi(time1));
    }
    else if (strcmp(word,"PrintSkipList") == 0 && !feof(file_pointer)){
      printf("\n");
      //prints the skiplist
      PrintSkipList(&max_skiplist);
    }
  }

  fclose(file_pointer);
  return 0;
}

SKIP_NODE* initialize_skiplist(){
  //creates empty nodes
  SKIP_NODE *max = create_node();
  SKIP_NODE *min = create_node();
  //make the max and min for an empty skiplist
  max->time = -1;
  min->time = 0;
  //make them point to each other
  min->next = max; 
  max->previous = min;

  //returns the left pointer
  return min;
}

void AddEvent(SKIP_NODE **max_skiplist, int height, int time, char event[]){
  //checks if a node with the same time exists
  SKIP_NODE *curr_node = checkall_skiplist(&(*max_skiplist), time);
  if (curr_node == NULL){
    SKIP_NODE *up_node = NULL;
    //if not then add new lines if necessary
    *max_skiplist = createnew_skipline(&(*max_skiplist), height-((*max_skiplist)->height));
    curr_node = *max_skiplist;
    //move the node to the correct height
    for (int i = 0; i < ((*max_skiplist)->height)-height; ++i)
      curr_node = curr_node->child;
    //add the node
    add_skiplist(&curr_node, &up_node, height, time, event);
  }
  //print an error
  else
    printf("ExistingEventError:%s", curr_node->event);
}

void CancelEvent(SKIP_NODE **skipnode, int time){
  //checks if the node exists
  SKIP_NODE *curr_node = checkall_skiplist(&(*skipnode), time);
  //if it doesnt then print an error
  if (curr_node == NULL)
    printf("NoEventError\n");
  //else remove the node and all the unnecessary empty lines
  else{
    printf("%s\n", curr_node->event);
    remove_skiplist(&curr_node);
    removeold_skipline(&(*skipnode));
  }
}

void GetEvent(SKIP_NODE **max_skiplist, int time){
  //creates a curr_node that receaives the position of the node wanted
  SKIP_NODE *curr_node = checkall_skiplist(&(*max_skiplist), time);
  //if its null then it doesn't exist and prints none
  if (curr_node == NULL)
    printf("%.6d none\n", time);
  else
    printf("%.6d %s\n", time, curr_node->event);
}

void GetEventBetween(SKIP_NODE **max_skiplist, int time1, int time2){
  //creates two curr_nodes so we can have the min and max points for printing
  SKIP_NODE *curr_node1 = submap_extremum_finder(&(*max_skiplist), time1);
  SKIP_NODE *curr_node2 = submap_extremum_finder(&(*max_skiplist), time2);
  //counting the amount of printings
  int print_counter = 0;
  curr_node1 = curr_node1->next;
  //if the next position is the one wanted then change to that one
  if (curr_node2->next->time == time2)
    curr_node2 = curr_node2->next;
  //prints and moves until all the nodes between the end points (including the endpoints)
  while(curr_node1->previous->time != curr_node2->time){
    printf("%.6d:%s ", curr_node1->time, curr_node1->event);
    curr_node1 = curr_node1->next;
    print_counter++;
  }
  //if it didn't enter the loop print none
  if (print_counter == 0)
    printf("none");
  printf("\n");
}

void PrintSkipList(SKIP_NODE **skipnode){
  //if the node is empty then every line was printed
  if (*skipnode == NULL)
    return;
  else{
    //creates a curr_node for traversing at that height and printing
    SKIP_NODE *curr_node = *skipnode;
    printf("(S%d) ", curr_node->height);
    for (int i = 0; i == i; ++i){
      if (curr_node->next->time == -1){
        //if nothing was done then it was empty
        if (i == 0)
          printf("empty");
        printf("\n");
        break;
      }
      printf("%.6d:%s ", curr_node->next->time, curr_node->next->event);
      curr_node = curr_node->next;
    }
    PrintSkipList(&((*skipnode)->child));
  }
}

SKIP_NODE* submap_extremum_finder(SKIP_NODE **skipnode, int time){
  //moves the node around if the time or one position less than the time wasn't found
  if ((*skipnode)->next->time >= time && (*skipnode)->child != NULL)
    return submap_extremum_finder(&((*skipnode)->child), time);
  else if((*skipnode)->next->time < time)
    return submap_extremum_finder(&((*skipnode)->next), time);
  return *skipnode;
}

SKIP_NODE* createnew_skipline(SKIP_NODE **max_skiplist, int height_diff){
  //if height diff is -1 or less means that the height is already in the skip list
  if (height_diff > -1){
    //creates the new_node and makes the new node the maximum be in the new height of the skip list
     SKIP_NODE *head = create_node();
     SKIP_NODE *tail = create_node();
     //"negative infinity" is my 0
     head->time = 0;
     //"positive infinity" is my -1 becuase the value is unsigned so it will go the maximum value
     tail->time = -1;
     head->child = *max_skiplist;
     tail->child = (*max_skiplist)->next;
     head->next = tail;
     tail->previous = head;
     //makes the height be 1 unit bigger then the max skiplist
     head->height = ((*max_skiplist)->height) + 1;
     tail->height = head->height;
     (*max_skiplist)->parent = head;
     (*max_skiplist)->next->parent = tail;
     //makes the max_skip list be the new node
     *max_skiplist = head;
     //use recursion for adding more heights
     height_diff = height_diff-1;
     return createnew_skipline(&(*max_skiplist), height_diff);
   }
   return *max_skiplist;
}

int removeold_skipline(SKIP_NODE **max_skiplist){
  //if the node below is an empty line then move to that position and remove the line above
  while((*max_skiplist)->child != NULL && (*max_skiplist)->child->next->time == -1){
    *max_skiplist = (*max_skiplist)->child;
    free((*max_skiplist)->parent);
    (*max_skiplist)->parent = NULL;
    free((*max_skiplist)->next->parent);
    (*max_skiplist)->next->parent = NULL;
  }
}

SKIP_NODE* create_node(){
  //allocates a memory address to the curr_node and sets all the pointers inside curr_node to NULL
  SKIP_NODE *curr_node = (SKIP_NODE*)malloc(sizeof(SKIP_NODE));
  curr_node->next = NULL;
  curr_node->previous = NULL;
  curr_node->parent = NULL;
  curr_node->child = NULL;
  return curr_node;
}

SKIP_NODE* checkall_skiplist(SKIP_NODE **skipnode, int time){
  //If there's no value equal to the time then print that the event will be added
  if(*skipnode == NULL)
    return *skipnode;
  //moves the node around if the time wasn't found
  else if ((*skipnode)->next->time > time)
    return checkall_skiplist(&((*skipnode)->child), time);
  else if((*skipnode)->next->time < time)
    return checkall_skiplist(&((*skipnode)->next), time);
  //if there's a value equal to the time then print the error
  return (*skipnode)->next;
}

void add_skiplist(SKIP_NODE **skipnode, SKIP_NODE **parent_node, int height, int time, char event[]){
  //if the skip node is null then there's nothing to add since we are at the end
  if(*skipnode == NULL)
    return;
  //creates a node and adds it to the correct position
  else if ((*skipnode)->next->time > time){
    SKIP_NODE *curr_node = create_node();
    curr_node->next = (*skipnode)->next;
    (*skipnode)->next->previous = curr_node;

    curr_node->previous = *skipnode;
    (*skipnode)->next = curr_node;

    curr_node->parent = *parent_node;
    if (*parent_node != NULL)
      (*parent_node)->child = curr_node;

    curr_node->time = time;
    curr_node->height = height;
    
    strcpy(curr_node->event, event);
    *parent_node = curr_node;
    //moves one level down
    add_skiplist(&((*skipnode)->child), &(*parent_node), height-1, time, event);
    return;
  }
  //moves to the next position until it's in the correct one
  else
    add_skiplist(&((*skipnode)->next), &(*parent_node), height, time, event);
}

void remove_skiplist(SKIP_NODE **skipnode){
  //if all the nodes with that value were eliminated then end the function and start freeing
  if (*skipnode == NULL)
    return;
  (*skipnode)->previous->next = (*skipnode)->next;
  (*skipnode)->next->previous = (*skipnode)->previous;
  remove_skiplist(&((*skipnode)->child));
  free(*skipnode);
}