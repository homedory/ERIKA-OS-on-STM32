/*
 * path.c
 *
 *  Created on: 2024. 7. 4.
 *      Author: homedory
 */

#define INF 1000000000

void findShortestPath(int source, int destination, char buffer[], int *len)
{
	int dist[1000];
	int vertex_count;
	int edge[1000][1000];
	int prev_vertex[1000];

	// fill edge from map_data.txt


	/* initialize dist array */
	for (int i = 0; i <= vertex_count; i++)
		dist[i] = INF;

	dist[source] = 0;

	for (int i = 1; i <= vertex_count; i++) {
		int min_dist = INF;
		int start_vertex = 0;
		for (int vertex_idx = 1; vertex_idx <= vertex_count; vertex_idx++) {
			if (min_dist > dist[vertex_idx]) {
				min_dist = dist[vertex_idx];
				start_vertex = vertex_idx;
			}
		}

		if (start_vertex == INF)
			break;

		/* dijkstra algorithm */
		for (int vertex_idx = 1; vertex_idx <= vertex_count; vertex_idx++) {
			if (edge[start_vertex][vertex_idx] == INF)
				continue;

			if (dist[vertex_idx] > dist[start_vertex] + edge[start_vertex][vertex_idx]) {
				dist[vertex_idx] = dist[start_vertex] + edge[start_vertex][vertex_idx];
				prev_vertex[vertex_idx] = start_vertex;
			}
		}

		/* put shortest path string into buffer */
		int vertex = source;
		int buffer_idx = 0;

		buffer[buffer_idx++] = vertex;
		while(vertex != destination) {
			vertex = prev_vertex[vertex];

			buffer[buffer_idx++] = ' ';
			buffer[buffer_idx++] = '-';
			buffer[buffer_idx++] = '>';
			buffer[buffer_idx++] = ' ';
			buffer[buffer_idx++] = vertex;
		}

		*len = dist[destination];
	}
}



