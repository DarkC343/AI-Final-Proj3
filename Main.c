#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

struct chromosome
{
	int data[8];
	int fitness;
};
typedef struct chromosome chromosome_t;

struct population {
	chromosome_t** chromosomes;
	size_t used;
	size_t size;
};
typedef struct population population_t;

struct solution
{
	chromosome_t* chromosome;
	int generation;
};
typedef struct solution solution_t;

void insert_population(population_t* population, chromosome_t* chromosome) {
	if (population->used == population->size) {
		population->size *= 2;
		population->chromosomes = (chromosome_t**)realloc(population->chromosomes, population->size * sizeof(chromosome_t*));
	}
	population->chromosomes[population->used++] = chromosome;
}

void free_population(population_t* population) {
	for (int i = 0; i < 8; i++)
	{
		free(population->chromosomes[i]);
	}
	free(population->chromosomes);
	population->chromosomes = NULL;
	population->used = population->size = 0;
}

int fitness(chromosome_t* chromosome)
{
	int fitness = 28; // N * (N-1)/2 edges for complete graph (i.e solved N-Queen: non-attacking pairs)
	for (int i = 0; i < 8; i++)
		for (int j = i + 1; j < 8; j++)
			if ((chromosome->data[i] == chromosome->data[j]) ||
				(i - chromosome->data[i] == j - chromosome->data[j]) ||
				(i + chromosome->data[i] == j + chromosome->data[j]))
				fitness--;
	return fitness;
}

void generate_population(population_t* population)
{
	bool* domain = (bool*)malloc(8 * sizeof(bool*));
	int index;
	for (int i = 0; i < 10; i++)
	{
		for (int i = 0; i < 8; ++i) domain[i] = true;
		chromosome_t* chromosome = (chromosome_t*)malloc(sizeof(chromosome_t));
		for (int j = 0; j < 8; ++j)
		{
			while (true)
			{
				index = rand() % 8;
				if (domain[index])
				{
					domain[index] = false;
					chromosome->data[j] = index;
					break;
				}
			}
		}
		chromosome->fitness = fitness(chromosome);
		insert_population(population, chromosome);
	}
}

chromosome_t* reproduce(chromosome_t* chromosome_1, chromosome_t* chromosome_2)
{
	chromosome_t* child_chromosome = (chromosome_t*)malloc(sizeof(chromosome_t));
	int rand_point = rand() % 8;
	for (int i = 0; i < rand_point; i++)
	{
		child_chromosome->data[i] = chromosome_1->data[i];
	}
	for (int j = rand_point; j < 8; j++)
	{
		child_chromosome->data[j] = chromosome_2->data[j];
	}
	child_chromosome->fitness = fitness(child_chromosome);
	return child_chromosome;
}

void mutate(chromosome_t* chromosome)
{
	chromosome->data[rand() % 8] = rand() % 8;
	chromosome->fitness = fitness(chromosome);
}

solution_t* genetic_algorithm(population_t* population)
{
	chromosome_t* child_chromosome, * top1, * top2;
	int generation = 0;
	while (true)
	{
		population_t* new_population = (population_t*)malloc(sizeof(population_t));
		new_population->used = 0;
		new_population->size = 50; // initial new_population size
		new_population->chromosomes = (chromosome_t**)malloc(new_population->size * sizeof(chromosome_t*));

		for (int i = 0; i < population->used; i++)
		{
			// sort
			chromosome_t* temp_chromose;
			for (int i = 0; i < population->used; i++)
			{
				for (int j = i + 1; j < population->used; j++)
				{
					if (population->chromosomes[i]->fitness < population->chromosomes[j]->fitness)
					{
						temp_chromose = population->chromosomes[i];
						population->chromosomes[i] = population->chromosomes[j];
						population->chromosomes[j] = temp_chromose;
					}
				}
			}

			top1 = population->chromosomes[rand() % 2];
			top2 = population->chromosomes[rand() % 2];

			child_chromosome = reproduce(top1, top2);

			if (rand() % 2)
			{
				mutate(child_chromosome);
			}

			if (child_chromosome->fitness == 28)
			{
				solution_t* solution = (solution_t*)malloc(sizeof(solution));
				solution->chromosome = child_chromosome;
				solution->generation = generation;
				return solution;
			}
			insert_population(new_population, child_chromosome);
		}
		population = new_population;
		generation++;
	}
	return NULL;
}

int main()
{
	population_t* population = (population_t*)malloc(sizeof(population_t));
	population->used = 0;
	population->size = 50; // initial polulation size
	population->chromosomes = (chromosome_t**)malloc(population->size * sizeof(chromosome_t*));

	srand(time(0));

	// create zeroth (random) generation
	generate_population(population);

	FILE* file = fopen("output.json", "w");

	// check for zeroth generation
	for (int i = 0; i < 8; i++)
	{
		if (population->chromosomes[i]->fitness == 28)
		{
			printf("Generation #0 : ");
			for (int i = 0; i < 8; i++)
			{
				printf("%d", population->chromosomes[i]->data[i]);
			}
			printf("\n");
		}
	}

	clock_t begin = clock();
	// apply genetic algorithm to find a solution
	solution_t* solution = genetic_algorithm(population);
	if (solution)
	{
		printf("Generation #%d : ", solution->generation);
		for (int i = 0; i < 8; i++)
		{
			printf("%d", solution->chromosome->data[i]);
		}
		printf("\n");
	}
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Elapsed: %f miliseconds\n", time_spent*1000);

	fprintf(file, "queens = \'[");
	for (int i = 0; i < 8; ++i)
	{
		fprintf(file, "{\"pos\" : %d", solution->chromosome->data[i]);
		if (i == 7) fprintf(file, "}");
		else fprintf(file, "},");
	}
	fprintf(file, "]\';");
	fclose(file);

	free_population(population);

	return 0;
}