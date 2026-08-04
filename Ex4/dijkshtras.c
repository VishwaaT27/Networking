#include <stdio.h>
#define INF 999999

struct HeapNode
{
    int vertex;
    int distance;
};

struct MinHeap
{
    struct HeapNode heap[20 * 20];
    int size;
};

int graph[20][20];
int activeRouters[20];
int distanceArray[20];
int parentArray[20];
int visitedArray[20];

void initializeHeap(struct MinHeap *minHeap) { minHeap->size = 0; }

void swapNodes(struct HeapNode *a, struct HeapNode *b)
{
    struct HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyUp(struct MinHeap *minHeap, int index)
{
    int parent;
    while (index > 0)
    {
        parent = (index - 1) / 2;
        if (minHeap->heap[parent].distance <= minHeap->heap[index].distance) break;
        swapNodes(&minHeap->heap[parent], &minHeap->heap[index]);
        index = parent;
    }
}

void heapifyDown(struct MinHeap *minHeap, int index)
{
    int left, right, smallest;
    while (1)
    {
        left = (2 * index) + 1;
        right = (2 * index) + 2;
        smallest = index;
        if (left < minHeap->size && minHeap->heap[left].distance < minHeap->heap[smallest].distance) smallest = left;
        if (right < minHeap->size && minHeap->heap[right].distance < minHeap->heap[smallest].distance) smallest = right;
        if (smallest == index) break;
        swapNodes(&minHeap->heap[index], &minHeap->heap[smallest]);
        index = smallest;
    }
}

void insertHeap(struct MinHeap *minHeap, int vertex, int distance)
{
    if (minHeap->size >= 20 * 20) return;
    minHeap->heap[minHeap->size].vertex = vertex;
    minHeap->heap[minHeap->size].distance = distance;
    heapifyUp(minHeap, minHeap->size);
    minHeap->size++;
}

struct HeapNode deleteMinimum(struct MinHeap *minHeap)
{
    struct HeapNode minimumNode;
    minimumNode.vertex = -1;
    minimumNode.distance = INF;
    if (minHeap->size == 0) return minimumNode;
    minimumNode = minHeap->heap[0];
    minHeap->size--;
    minHeap->heap[0] = minHeap->heap[minHeap->size];
    heapifyDown(minHeap, 0);
    return minimumNode;
}

void initializeDefaultGraph()
{
    int i, j;
    for (i = 0; i < 20; i++)
    {
        activeRouters[i] = (i < 9) ? 1 : 0;
        for (j = 0; j < 20; j++)
        {
            if (i == j) graph[i][j] = 0;
            else graph[i][j] = -1;
        }
    }
    graph[0][1] = 4; graph[1][0] = 4;
    graph[0][4] = 8; graph[4][0] = 8;
    graph[1][2] = 8; graph[2][1] = 8;
    graph[1][4] = 11; graph[4][1] = 11;
    graph[2][3] = 7; graph[3][2] = 7;
    graph[2][6] = 4; graph[6][2] = 4;
    graph[2][8] = 2; graph[8][2] = 2;
    graph[3][6] = 14; graph[6][3] = 14;
    graph[3][7] = 9; graph[7][3] = 9;
    graph[4][5] = 1; graph[5][4] = 1;
    graph[4][8] = 7; graph[8][4] = 7;
    graph[5][6] = 2; graph[6][5] = 2;
    graph[5][8] = 6; graph[8][5] = 6;
    graph[6][7] = 10; graph[7][6] = 10;
}

void displayGraph()
{
    int i, j;
    printf("\nAdjacency Matrix (-1 indicates no link)\n\n");
    printf("      ");
    for (i = 0; i < 20; i++) if (activeRouters[i]) printf("%4d", i);
    printf("\n");
    for (i = 0; i < 20; i++)
    {
        if (!activeRouters[i]) continue;
        printf("%4d  ", i);
        for (j = 0; j < 20; j++) if (activeRouters[j]) printf("%4d", graph[i][j]);
        printf("\n");
    }
}

void displayDistanceArray()
{
    int i;
    printf("\nDistance Array\n\nRouter\t\tDistance\n\n");
    for (i = 0; i < 20; i++)
    {
        if (!activeRouters[i]) continue;
        if (distanceArray[i] == INF) printf("%d\t\tINF\n", i);
        else printf("%d\t\t%d\n", i, distanceArray[i]);
    }
}

void displayParentArray()
{
    int i;
    printf("\nParent Array\n\nRouter\t\tParent\n\n");
    for (i = 0; i < 20; i++)
    {
        if (!activeRouters[i]) continue;
        if (parentArray[i] == -1) printf("%d\t\t-\n", i);
        else printf("%d\t\t%d\n", i, parentArray[i]);
    }
}

void displayVisitedArray()
{
    int i;
    printf("\nVisited Array\n\nRouter\t\tStatus\n\n");
    for (i = 0; i < 20; i++)
    {
        if (!activeRouters[i]) continue;
        if (visitedArray[i]) printf("%d\t\tVisited\n", i);
        else printf("%d\t\tNot Visited\n", i);
    }
}

int getNextHop(int source, int destination)
{
    int current;
    if (source == destination) return source;
    current = destination;
    while (parentArray[current] != -1 && parentArray[current] != source) current = parentArray[current];
    if (parentArray[current] == -1) return -1;
    return current;
}

void displayRoutingTable(int source)
{
    int i, nextHop;
    printf("\nRouting Table\n\nDestination\tMinimum Cost\tPrevious Router\tNext Hop\n");
    for (i = 0; i < 20; i++)
    {
        if (!activeRouters[i]) continue;
        nextHop = getNextHop(source, i);
        printf("%d\t\t", i);
        if (distanceArray[i] == INF) printf("INF\t\t");
        else printf("%d\t\t", distanceArray[i]);
        if (parentArray[i] == -1) printf("-\t\t");
        else printf("%d\t\t", parentArray[i]);
        if (nextHop == -1) printf("-\n");
        else printf("%d\n", nextHop);
    }
}

void printPath(int destination)
{
    int path[20], count = 0, i;
    while (destination != -1)
    {
        path[count++] = destination;
        destination = parentArray[destination];
    }
    for (i = count - 1; i >= 0; i--)
    {
        printf("%d", path[i]);
        if (i != 0) printf(" -> ");
    }
}

void displayShortestPaths(int source)
{
    int i;
    printf("\nShortest Paths\n\n");
    for (i = 0; i < 20; i++)
    {
        if (!activeRouters[i]) continue;
        printf("Router %d\n", i);
        if (distanceArray[i] == INF)
        {
            printf("No Path Available\n\n");
            continue;
        }
        printPath(i);
        printf("\n");
        printf("Cost = %d\n\n", distanceArray[i]);
    }
}

void dijkstra(int source)
{
    if (source < 0 || source >= 20 || !activeRouters[source])
    {
        printf("\nInvalid or Inactive Source Router\n");
        return;
    }
    int i, currentVertex, neighbour, newDistance, u;
    struct MinHeap minHeap;
    struct HeapNode minimumNode;
    initializeHeap(&minHeap);
    for (i = 0; i < 20; i++)
    {
        distanceArray[i] = INF;
        parentArray[i] = -1;
        visitedArray[i] = 0;
    }
    distanceArray[source] = 0;
    insertHeap(&minHeap, source, 0);
    while (minHeap.size > 0)
    {
        minimumNode = deleteMinimum(&minHeap);
        currentVertex = minimumNode.vertex;
        if (visitedArray[currentVertex]) continue;
        visitedArray[currentVertex] = 1;

        for (u = 0; u < 20; u++)
        {
            if (activeRouters[u] && graph[currentVertex][u] > 0)
            {
                newDistance = distanceArray[currentVertex] + graph[currentVertex][u];
                if (newDistance < distanceArray[u])
                {
                    distanceArray[u] = newDistance;
                    parentArray[u] = currentVertex;
                    insertHeap(&minHeap, u, distanceArray[u]);
                }
            }
        }
    }
    displayGraph();
    displayDistanceArray();
    displayParentArray();
    displayVisitedArray();
    displayRoutingTable(source);
    displayShortestPaths(source);
}

void modifyLink()
{
    int source, destination, cost, startRouter;
    printf("\nEnter Source Router : ");
    scanf("%d", &source);
    printf("Enter Destination Router : ");
    scanf("%d", &destination);
    printf("Enter New Cost (-1 to remove link): ");
    scanf("%d", &cost);
    if (source < 0 || source >= 20 || !activeRouters[source] || destination < 0 || destination >= 20 || !activeRouters[destination])
    {
        printf("\nInvalid Router Number\n");
        return;
    }
    graph[source][destination] = cost;
    printf("\nLink Updated Successfully\n");
    displayGraph();
    printf("\nEnter Source Router : ");
    scanf("%d", &startRouter);
    dijkstra(startRouter);
}

void addRouter()
{
    int newRouter, i, cost, source;
    printf("\nEnter New Router ID (0-19): ");
    scanf("%d", &newRouter);

    if (newRouter < 0 || newRouter >= 20)
    {
        printf("\nInvalid Router ID\n");
        return;
    }
    if (activeRouters[newRouter])
    {
        printf("\nRouter Already Exists!\n");
        return;
    }

    activeRouters[newRouter] = 1;

    for (i = 0; i < 20; i++)
    {
        if (activeRouters[i] && i != newRouter)
        {
            printf("Enter cost FROM Router %d TO Router %d (-1 for no link): ", newRouter, i);
            scanf("%d", &cost);
            graph[newRouter][i] = cost;

            printf("Enter cost FROM Router %d TO Router %d (-1 for no link): ", i, newRouter);
            scanf("%d", &cost);
            graph[i][newRouter] = cost;
        }
    }
    graph[newRouter][newRouter] = 0;

    displayGraph();
    printf("\nEnter Source Router : ");
    scanf("%d", &source);
    dijkstra(source);
}

void removeRouter()
{
    int router, i, source;
    printf("\nEnter Router Number To Remove : ");
    scanf("%d", &router);
    if (router < 0 || router >= 20 || !activeRouters[router])
    {
        printf("\nInvalid Router Number\n");
        return;
    }
    activeRouters[router] = 0;
    for (i = 0; i < 20; i++)
    {
        graph[router][i] = -1;
        graph[i][router] = -1;
    }

    printf("\nRouter %d Removed Successfully\n", router);
    displayGraph();
    printf("\nEnter Source Router : ");
    scanf("%d", &source);
    dijkstra(source);
}

void resetGraph()
{
    int source;
    initializeDefaultGraph();
    displayGraph();
    printf("\nEnter Source Router : ");
    scanf("%d", &source);
    dijkstra(source);
}

void displayMenu()
{
    printf("\n================================\n");
    printf("\nMENU\n");
    printf("\n================================\n");
    printf("\n1. Display Graph\n");
    printf("\n2. Run Dijkstra\n");
    printf("\n3. Modify Existing Link\n");
    printf("\n4. Add Router\n");
    printf("\n5. Remove Router\n");
    printf("\n6. Reset Default Graph\n");
    printf("\n7. Exit\n");
    printf("\nEnter Choice : ");
}

int main()
{
    int source, choice;
    printf("==============================================\n");
    printf("        DIJKSTRA ROUTER SIMULATION\n");
    printf("      Link State Routing Demonstration\n");
    printf("==============================================\n");
    printf("\nLoading Default Network...\n");
    initializeDefaultGraph(); displayGraph();
    printf("\nEnter Source Router : ");
    scanf("%d", &source);
    dijkstra(source);
    while (1)
    {
        displayMenu();
        if (scanf("%d", &choice) != 1) break;
        switch (choice)
        {
            case 1: displayGraph(); break;
            case 2:
                printf("\nEnter Source Router : ");
                scanf("%d", &source);
                dijkstra(source);
                break;
            case 3: modifyLink(); break;
            case 4: addRouter(); break;
            case 5: removeRouter(); break;
            case 6: resetGraph(); break;
            case 7:
                printf("\n================================\n");
                printf("\nSimulation Finished\n");
                printf("\nThank You\n");
                printf("\n================================\n");
                return 0;
            default:
                printf("\nInvalid Choice\n");
        }
    }
    return 0;
}
