#pragma once
#include<iostream>
using std::cout;
#include<cstdlib>
#include <vector>
using std::vector;
#include <random>
using std::default_random_engine;
using std::normal_distribution;
#include <chrono>
#include <utility>
using std::pair;
#include <math.h>
#include <numeric>
using std::inner_product;


class Network {
	public:
		/*   Constructor
		sizes is a list of layer sizes (eg if sizes = {4, 5, 8}, the network will have 3 layers, with the first
		layer having a size of 4, the second layer 5, and the third layer 8)
		*/
		Network(vector<int> sizes);
		
		//biases and weights control the output of each individual "neuron"
		vector<vector<double>> biases;
		vector<vector<vector<double>>> weights;
		int num_layers = 0;
		
		/*
		takes a vector<double> which is the input layer (in_layer), and the size of in_layer should be equal to the 
		first size in the sizes member. This function feeds the in_layer to the second layer, then feeds the output of
		the second layer to the third layer, and so on. The output of the final layer is returned.
		*/
		vector<double> feedforward(vector<double> in_layer);
		
		/*
		The backpropagation algorithm calculates the gradient of the cost function wrt the weights and biases of the 
		network, for a single training example. Takes an input vector and a vector that is the ideal output given that
		input. Returns a std::pair, with the first element being the gradient of the biases, and the second being the 
		gradient of the weights.
		*/
		pair< vector<vector<double>>, vector<vector<vector<double>>> > backprop(vector<double> input, vector<double> desired_output);
		
		/*
		xs is a list of training inputs, ys is a list of the ideal outputs corresponding to the inputs. lr is the 
		learning rate, which controls how much the weights and biases change with each epoch.
		*/
		void update(vector<vector<double>> xs, vector<vector<double>> ys, double lr);
		
		/*
		breaks the data up into batches and calls the update function until all data is used 
		*/
		void train(vector<vector<double>> xs, vector<vector<double>> ys, int batch_size,
				   double lr);
		
		/*
		calls feedforward with each vector in xs, then checks if the output is correct. Outputs the
		amount correct to cout.
		*/		
		void evaluate(vector<vector<double>> xs, vector<wchar_t> correct_chars, vector<wchar_t> char_set);
};

