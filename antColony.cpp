#include <iostream>
#include <vector>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <math.h>
#include <cstdlib>
#include <time.h>

using namespace std;

struct City{
	int coordX;
	int coordY;
};

double d_euclidea(const City& a, const City& b)
{
	return sqrt(pow(b.coordX-a.coordX,2) + pow(b.coordY-a.coordY,2));
}


vector<vector<double>> getDistances(const vector<City>& cities)
{
	vector<vector<double>> Map(cities.size(), vector<double>(cities.size(), 0));
	for (int i = 0; i < cities.size(); ++i)
	{
		for (int j = 0; j < cities.size(); ++j)
		{
			Map[i][j] = d_euclidea(cities[i], cities[j]);
		}
	}
	return Map;
}

double pathCost(const vector<int>& Path, const vector<vector<double>>& Map){
	double cost = 0;
	for(int i = 1; i < Path.size(); ++i)
		cost += Map[Path[i-1]][Path[i]];
	cost += Map[*(Path.end()-1)][*(Path.begin())];
	return cost;
}

void ACO(const vector<City>& cities, const vector<vector<double>>& Map, int max_it, int num_ants, double decay, double heur_coef, double pher_coef, double pher_coef_elit)
{
	srand(1);

	//Initialise colony of ants (each ant is a vector of city indices)
	vector<vector<int>> Colony(num_ants, vector<int>(cities.size(), 0));

	//Initialise pheromone matrix
	vector<vector<double>> pheromones(cities.size(), vector<double>(cities.size(), 0));
	//Initialise costs vector(for etilist expansion)
	vector<double> costs(cities.size(), 0);

	//Auxiliar vector of indices
	vector<int> cityIndices(cities.size());
	for (int i = 0; i < cities.size(); ++i)
		cityIndices[i] = i;

	//Longest distance from Map, used for heuristic values.
	vector<double> longests(cities.size(), 0);
	for(int i = 0; i < cities.size(); ++i)
		longests[i] = *(max_element(Map[i].begin(), Map[i].end()));

	const double MAX_DIST = *(max_element(longests.begin(), longests.end()));
	longests.clear();


	int i=0;
	while(i<max_it)
	{
		for(int ant_i = 0; ant_i < num_ants; ++ant_i)
		{
			cout << "Ant: " << ant_i << endl;
			//City for ant_i to start at; each ant is assigned a determined starting city
			int starting_city = (int) ((float)ant_i/num_ants*cities.size());
			//cout << starting_city << endl;
			Colony[ant_i][0] = starting_city;

			//Get a vector with the cities left to visit
			vector<int> RemainingCities = cityIndices;

			//Remove starting city from remaining cities
			RemainingCities.erase(RemainingCities.begin() + starting_city);

			//Create path for ant_i
			for(int city_i = 1; city_i < Colony[ant_i].size(); ++city_i)
			{
				cout << "Calculating city number: " << city_i << endl;
				//Create roulette for next city selection
				vector<double> Roulette(RemainingCities.size(), 0);
				double total = 0;

				//DEBUGGING SECTION
				cout << "Size Roulette: " << Roulette.size() << endl;
				cout << "Size Remain: " << RemainingCities.size() << endl;
				cout << "Size Map: " << Map.size() << " x " << Map[0].size() << endl;

				int k = 0;
				cout << "Test: Map access: " << endl;
				for(int i = 0; i < Map.size(); ++i) //	HERE IT CRASHES AT ANT NUMBER 55
					cout << Map[0][i] << " ";
				cout << endl;

				cout << "Test: Operation: " << Map[Colony[ant_i][city_i-1]][RemainingCities[k]] << endl;

				Roulette[k] = pow((MAX_DIST - Map[Colony[ant_i][city_i-1]][RemainingCities[k]]), heur_coef) + pow((pheromones[Colony[ant_i][city_i-1]][RemainingCities[k]]), pher_coef);

				//END OF DEBUGGING SECTION

				for(int j = 0; j < RemainingCities.size(); ++j)
				{
					//Heuristic value is MAX_DIST - current edge.
					Roulette[j] = pow((MAX_DIST - Map[Colony[ant_i][city_i-1]][RemainingCities[j]]), heur_coef) + pow((pheromones[Colony[ant_i][city_i-1]][RemainingCities[j]]), pher_coef);
					total += Roulette[j];
				}
				cout << endl;
				//Transform roulette into stacked probabilities
				Roulette[0] = Roulette[0]/total;

				for(int j = 1; j < Roulette.size(); ++j)
					Roulette[j] = Roulette[j-1] + Roulette[j] / total;

				//Select a city from Roulette
				int chosen = 0;
				double r = (double) rand()/RAND_MAX;
				while(Roulette[chosen] < r)
					chosen++;

				//Add chosen city to
				Colony[ant_i][city_i] = RemainingCities[chosen];
				RemainingCities.erase(RemainingCities.begin() + chosen);
			}
			cout << endl;
			//Save cost of ant_i, for elitist expansion
			costs[ant_i] = pathCost(Colony[ant_i], Map);
		}
		i++;
	}

}


vector<City> readFile(string name){

	vector<City> ciudades;
	ifstream file;
	string linea_texto;

	file.open("berlin52.tsp", ios::in); //lectura ios::in , escritura ios::out

	if(file.fail()){
		cout << "Error al abrir el archivo" << endl;
		exit(1);
	}

	while(!file.eof()){
		getline(file,linea_texto);

		//Comienza a leer coordenadas
		if(linea_texto[0] == '1' || linea_texto[0] == '2' || linea_texto[0] == '3' || linea_texto[0] == '4' || linea_texto[0] == '5' || linea_texto[0] == '6' || linea_texto[0] == '7' || linea_texto[0] == '8'|| linea_texto[0] == '9'){
			int pos = linea_texto.find(" ");
			string nuevo_texto = linea_texto.substr(pos+1);
			string::size_type sz;

			double first_coord = stod(nuevo_texto, &sz);
			double second_coord = stod(nuevo_texto.substr(sz));

			City ciudad;
			ciudad.coordX = first_coord;
			ciudad.coordY = second_coord;
			ciudades.push_back(ciudad);
		}
	}
	return ciudades;
}


int main(){

	string nameFile = "berlin52.tsp";

	int max_it, num_ants;
	double decay_factor, c_heur, c_phero, c_elite;
	vector<City> cities = readFile(nameFile);
	cout << "Number of cities: "<< cities.size()  << endl;

	//Buidling map matrix

	vector<vector<double>> Map = getDistances(cities);

	//Configuracion
	max_it = 1;
	num_ants = cities.size()*5;
	decay_factor = 0.6;
	c_heur = 1;
	c_phero = 1;
	c_elite = 0.9;
	cout << num_ants << endl;
	//Algorithm starts
	ACO(cities, Map, max_it, num_ants, decay_factor, c_heur, c_phero, c_elite);


	return 0;
}
