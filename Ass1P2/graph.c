/*
graph.c

Set of vertices and edges implementation.

Implementations for helper functions for graph construction and manipulation.

Skeleton written by Grady Fitzpatrick for COMP20007 Assignment 1 2022
*/
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include "graph.h"
#include "utils.h"
#include "pq.h"

#define INITIALEDGES 32
#define INFINITY 20000
#define HEART 1
#define START (-1)
struct edge;

/* Definition of a graph. */
struct graph {
  int numVertices;
  int numEdges;
  int allocedEdges;
  struct edge **edgeList;
};
 
struct dijk_element {
  int vertex;
};

/* Definition of an edge. */
struct edge {
  int start;
  int end;
  int cost;
};

/******************************************************************************************/
struct graph *newGraph(int numVertices){
  struct graph *g = (struct graph *) malloc(sizeof(struct graph));
  assert(g);
  /* Initialise edges. */
  g->numVertices = numVertices;
  g->numEdges = 0;
  g->allocedEdges = 0;
  g->edgeList = NULL;
  return g;
}
/******************************************************************************************/
/* Adds an edge to the given graph. */
void addEdge(struct graph *g, int start, int end, int cost){
  assert(g);
  struct edge *newEdge = NULL;
  /* Check we have enough space for the new edge. */
  if((g->numEdges + 1) > g->allocedEdges){
    if(g->allocedEdges == 0){
      g->allocedEdges = INITIALEDGES;
    } else {
      (g->allocedEdges) *= 2;
    }
    g->edgeList = (struct edge **) realloc(g->edgeList,
      sizeof(struct edge *) * g->allocedEdges);
    assert(g->edgeList);
  }

  /* Create the edge */
  newEdge = (struct edge *) malloc(sizeof(struct edge));
  assert(newEdge);
  newEdge->start = start;
  newEdge->end = end;
  newEdge->cost = cost;

  /* Add the edge to the list of edges. */
  g->edgeList[g->numEdges] = newEdge;
  (g->numEdges)++;
}
/******************************************************************************************/
/* Returns a new graph which is a deep copy of the given graph (which must be 
  freed with freeGraph when no longer used). */
struct graph *duplicateGraph(struct graph *g){
  struct graph *copyGraph = (struct graph *) malloc(sizeof(struct graph));
  assert(copyGraph);
  copyGraph->numVertices = g->numVertices;
  copyGraph->numEdges = g->numEdges;
  copyGraph->allocedEdges = g->allocedEdges;
  copyGraph->edgeList = (struct edge **) malloc(sizeof(struct edge *) * g->allocedEdges);
  assert(copyGraph->edgeList || copyGraph->numEdges == 0);
  int i;
  /* Copy edge list. */
  for(i = 0; i < g->numEdges; i++){
    struct edge *newEdge = (struct edge *) malloc(sizeof(struct edge));
    assert(newEdge);
    newEdge->start = (g->edgeList)[i]->start;
    newEdge->end = (g->edgeList)[i]->end;
    newEdge->cost = (g->edgeList)[i]->cost;
    (copyGraph->edgeList)[i] = newEdge;
  }
  return copyGraph;
}
/******************************************************************************************/
/* Frees all memory used by graph. */
void freeGraph(struct graph *g){
  int i;
  for(i = 0; i < g->numEdges; i++){
    free((g->edgeList)[i]);
  }
  if(g->edgeList){
    free(g->edgeList);
  }
  free(g);
}
/******************************************************************************************/
struct solution *graphSolve(struct graph *g, enum problemPart part,
  int numRooms, int startingRoom, int bossRoom, int numShortcuts, 
  int *shortcutStarts, int *shortcutEnds, int numHeartRooms, int *heartRooms){
  struct solution *solution = (struct solution *)
    malloc(sizeof(struct solution));
  assert(solution);
  
  if(part == PART_A){
    /* IMPLEMENT 2A SOLUTION HERE */
    int i,j,position;
    int dist[numRooms];
    int in_queue[numRooms];
    struct pq *Q;
    struct dijk_element *curr_elem,*temp;
    struct edge *curr_edge;

    /*****************************************************************************************************************/
    /*The code below was adapted from pseudocode. The pseudocode can found in Lars Kulik's COMP20007 lecture 8 slides*/
    /*Initialising the distances*/
    for (i=0;i<numRooms;i++) {
      if (i==startingRoom) {
        dist[i]=0;
      }else {
        dist[i]=INFINITY;
      }
    }
    Q = newPQ();
    
    for (i=0;i<numRooms;i++) {
      curr_elem = malloc(sizeof(*curr_elem));
      assert(curr_elem);
      curr_elem->vertex=i;
      enqueue(Q,curr_elem,dist[i]);
      in_queue[i]=1;
    }

    while (!empty(Q)) {
      curr_elem = deletemin(Q);
      in_queue[curr_elem->vertex]=0;

      for (i=0;i<(g->numEdges);i++) {
        curr_edge=(g->edgeList)[i];
        /*Edge is not connected to the current vertex*/
        if ((curr_edge->start!=curr_elem->vertex)&&(curr_edge->end!=curr_elem->vertex)) {
          continue;
        }
       /*End vertex is in the priority queue*/
        if (in_queue[curr_edge->end]&&(dist[curr_elem->vertex]+(curr_edge->cost)<dist[curr_edge->end])) {
          dist[curr_edge->end]=dist[curr_elem->vertex]+curr_edge->cost;
          /*Update the priority queue*/
          for (j=0;j<numqueue(Q);j++) {
            temp = queue_item(Q,j);
            if (curr_edge->end==temp->vertex) {
              position=j;
              update_target(Q,position,dist[curr_edge->end]);
            }
          }
        
        }
      }
    }
    /*******************************************************************************************************************/
    solution->heartsLost = dist[bossRoom];


  } else if(part == PART_B) {
    /* IMPLEMENT 2B SOLUTION HERE */
    /*Add one edge to the list of edges and then compare*/
    int i,j,k,position;
    int dist[numRooms];
    int in_queue[numRooms];
    int minhearts=INFINITY;
    struct pq *Q;
    struct dijk_element *curr_elem,*temp;
    struct edge *curr_edge;
    struct graph *copy_graph;

    for (i=0;i<numShortcuts;i++) {
      copy_graph = duplicateGraph(g);
      /*Add a new edge for the shortcut*/
      addEdge(copy_graph,shortcutStarts[i],shortcutEnds[i],1);
      addEdge(copy_graph,shortcutEnds[i],shortcutStarts[i],1);
      /*****************************************************************************************************************/
      /*The code below was adapted from pseudocode. The pseudocode can found in Lars Kulik's COMP20007 lecture 8 slides*/
      /*Initialising the distances*/
      for (j=0;j<numRooms;j++) {
        if (j==startingRoom) {
          dist[j]=0;
        }else {
        dist[j]=INFINITY;
        }
      }
      Q = newPQ();
    
      for (j=0;j<numRooms;j++) {
        curr_elem = malloc(sizeof(*curr_elem));
        assert(curr_elem);
        curr_elem->vertex=j;
        enqueue(Q,curr_elem,dist[j]);
        in_queue[j]=1;
      }

      while (!empty(Q)) {
        curr_elem = deletemin(Q);
        in_queue[curr_elem->vertex]=0;
        for (j=0;j<(copy_graph->numEdges);j++) {
          curr_edge=(copy_graph->edgeList)[j];
          /*Edge is not connected to the current vertex*/
          if ((curr_edge->start!=curr_elem->vertex)&&(curr_edge->end!=curr_elem->vertex)) {
            continue;
          }
         /*End vertex is in the priority queue*/
          if (in_queue[curr_edge->end]&&(dist[curr_elem->vertex]+(curr_edge->cost)<dist[curr_edge->end])) {
            dist[curr_edge->end]=dist[curr_elem->vertex]+curr_edge->cost;
            /*Update the priority queue*/
            for (k=0;k<numqueue(Q);k++) {
              temp = queue_item(Q,k);
              if (curr_edge->end==temp->vertex) {
                position=k;
                update_target(Q,position,dist[curr_edge->end]);
              }
            }
          }
        }
      }
      /*******************************************************************************************************************/
      if (dist[bossRoom]<minhearts) {
        minhearts=dist[bossRoom];
      }
      freeGraph(copy_graph);
    }


    solution->heartsLost = minhearts;
  } else {
    /* IMPLEMENT 2C SOLUTION HERE */
    int i,j,position,has_heart=0;
    int dist[numRooms];
    int in_queue[numRooms];
    struct pq *Q;
    struct dijk_element *curr_elem,*temp;
    struct edge *curr_edge;

    /*****************************************************************************************************************/
    /*The code below was adapted from pseudocode. The pseudocode can found in Lars Kulik's COMP20007 lecture 8 slides*/
    /*Initialising the distances*/
    for (i=0;i<numRooms;i++) {
      if (i==startingRoom) {
        dist[i]=0;
      }else {
        dist[i]=INFINITY;
      }
    }
    Q = newPQ();
    
    for (i=0;i<numRooms;i++) {
      curr_elem = malloc(sizeof(*curr_elem));
      assert(curr_elem);
      curr_elem->vertex=i;
      enqueue(Q,curr_elem,dist[i]);
      in_queue[i]=1;
    }

    while (!empty(Q)) {
      curr_elem = deletemin(Q);
      in_queue[curr_elem->vertex]=0;
      
      /*check all the connected edges*/
      for (i=0;i<(g->numEdges);i++) {
        curr_edge=(g->edgeList)[i];
        has_heart=0;
        /*Edge is not connected to the current vertex*/
        if ((curr_edge->start!=curr_elem->vertex)&&(curr_edge->end!=curr_elem->vertex)) {
          continue;
        }
        
        for (j=0;j<numHeartRooms;j++) {
          if (curr_edge->end==heartRooms[j]) {
            has_heart=1;
            break;
          }
        }
       
       /*The room that Lonk is walking towards is in the priority queue and it contains a heart*/
       if ((has_heart)&&(in_queue[curr_edge->end]&&(dist[curr_elem->vertex]+(curr_edge->cost)-HEART<dist[curr_edge->end]))) {
         dist[curr_edge->end]=dist[curr_elem->vertex]+(curr_edge->cost)-HEART;
          /*Update the priority queue*/
          for (j=0;j<numqueue(Q);j++) {
            temp = queue_item(Q,j);
            if (curr_edge->end==temp->vertex) {
              position=j;
              update_target(Q,position,dist[curr_edge->end]);
            }
          }
       }
       
       /*The room that Lonk is walking towards is in the priority queue, but it does not contain a heart*/
        else if (in_queue[curr_edge->end]&&(dist[curr_elem->vertex]+(curr_edge->cost)<dist[curr_edge->end])) {
          dist[curr_edge->end]=dist[curr_elem->vertex]+curr_edge->cost;
          /*Update the priority queue*/
          for (j=0;j<numqueue(Q);j++) {
            temp = queue_item(Q,j);
            if (curr_edge->end==temp->vertex) {
              position=j;
              update_target(Q,position,dist[curr_edge->end]);
            }
          }
        
        }
      }
    }
    /*****************************************************************************************************************/
    solution->heartsLost = dist[bossRoom];
  }
  return solution;

}
/******************************************************************************************/

