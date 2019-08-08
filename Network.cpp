#include "Network.h"
#include "CNN.cpp"


/*   Constructor
sizes is a list of layer sizes (eg if sizes = {4, 5, 8}, the network will have 3 layers, with the first
layer having a size of 4, the second layer 5, and the third layer 8)
*/
Network::Network(vector<int> sizes){
	this -> num_layers = sizes.size(); //number of layers in the network
	
	//for initializing biases/weights (will make this a function of its own)
	vector<double> current_layer_biases;
	vector<vector<double>> current_layer_weights;
	vector<double> current_neur_weights;
	
	//get random numbers on a normal distribution with a mean of 0 and standard deviation 1
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator (seed);
	normal_distribution<double> dist(0.0, 1.0);
	
	//initialize weights and biases with random numbers from above distribution
	for (int i = 1; i < num_layers; i++){
		current_layer_biases.clear();
		current_layer_weights.clear();
		for (int j = 0; j < sizes[i]; j++){
			current_neur_weights.clear();
			for (int k = 0; k < sizes[i-1]; k++){
				current_neur_weights.push_back(dist(generator));
			}
			current_layer_biases.push_back(dist(generator));
			current_layer_weights.push_back(current_neur_weights);
		}

		this -> biases.push_back(current_layer_biases);
		this -> weights.push_back(current_layer_weights);
	}
	
}
/*
takes a vector<double> which is the input layer (in_layer), and the size of in_layer should be equal to the 
first size in the sizes member. This function feeds the in_layer to the second layer, then feeds the output of
the second layer to the third layer, and so on. The output of the final layer is returned.
*/

vector<double> Network::feedforward(vector<double> a){
	//get reference to biases and weights to reduce clutter
	vector<vector<double>>& b = biases;
	vector<vector<vector<double>>>& w = weights;
	
	vector<double> next_layer; //stores the next layer on the following loop
	
	/*
	The output of a neuron is the sigmoid function of the dot product of the corresponding column of 
	the weight matrix and the input the neuron recieves. Since each neuron has one bias, looping through
	biases gets each neuron. 
	*/
	for (int i = 0; i < b.size(); i++){ //for each layer
		for (int j  = 0; j < b[i].size(); j++){ //for each neuron in that layer
			next_layer.push_back(sigm(inner_product(a.begin(), a.end(), 
													w[i][j].begin(), 0) + b[i][j]));
		}
		a = next_layer; //this will be passed to the next layer, unless it is the activation
		                //of the last layer.
		next_layer.clear();
	}
	return a;
}


/*
The backpropagation algorithm calculates the gradient of the cost function wrt the weights and biases of the 
network, for a single training example. Takes an input vector and a vector that is the ideal output given that
input. Returns a std::pair, with the first element being the gradient of the biases, and the second being the 
gradient of the weights.
*/
pair< vector<vector<double>>, vector<vector<vector<double>>> > Network::backprop (vector<double> x, vector<double> y){
	pair< vector<vector<double>>, vector<vector<vector<double>>> > result;
	
	//partial_b and partial_w are the gradients of biases and weights
	//b1, w1 and w2 are for initializing partial_b and partial_w with 0s in the shape of b and w
	vector<vector<double>> partial_b;
	vector<double> b1;
	
	vector<vector<vector<double>>> partial_w;
	vector<vector<double>> w1;
	vector<double> w2;
	
	vector<double> z;
	vector<vector<double>> zs;
	vector<double> activation = x; // first layer is x
	vector<double> next_activation; //for storing the output of the current layer
	vector<vector<double>> activations {x}; //list of activations is used for backprop process
 	
	//references to biases and weights so there is less clutter
	vector<vector<double>>& b = biases;
	vector<vector<vector<double>>>& w = weights;

	cout << x.size() << endl << y.size() << endl << b.size() << endl;
	
	/*
	this next loop fills partial_b and partial_w with 0.0 and fills the zs list with each layer's
	z vector, as well as the activation. The z vector is the activation before the sigmoid function
	is applied.
	*/
	for (int i = 0; i < b.size(); i++){
		for (int j  = 0; j < b[i].size(); j++){
			b1.push_back(0.0); //initialize partial_b to 0

			z.push_back(inner_product(activation.begin(), activation.end(), 
							  w[i][j].begin(), 0) + b[i][j]);
							  
			next_activation.push_back(sigm(z[j]));
		}
		
		//initialize partial_w to 0s
		for (int j = 0; j < w[i].size(); j++){
			for (int k = 0; k < w[i][j].size(); k++){
				w2.push_back(0.0);
			}
			w1.push_back(w2);
			w2.clear();
		}
		
		partial_w.push_back(w1);
		w1.clear();
		
		partial_b.push_back(b1);
		b1.clear();
		
		//add the activation to the list and clear next_activation for the next iteration
		activation = next_activation;
		next_activation.clear();
		activations.push_back(activation);
		
		//same as above with zs
		zs.push_back(z);
		z.clear();
	}
	
	
	vector<double> delta, prev_delta; //delta is the change in final output due to the current layer
	
	//this is the derivative of the last layer only
	vector<double> cost_deriv = cost_derivative(activations.back(), y);
	
	vector<double> sp; // holds the derivative of the current layer's activations
	vector<vector<double>> w_t; // transpose of the weight matrix
	vector<double> cw_t; // a single column of w_t
	double d_w = 0.0;

	/*
	because backpropagation utilizes the chain rule, the delta from one layer can be found if the
	delta from all layers after it is known. So, next we find the delta for the last layer, then 
	can loop through the rest of the layers.
	*/
	
	sp.clear();
    for (int i = 0; i < zs.back().size(); i++){
		sp.push_back(sig_prime(zs.back()[i]));
	}		
	for (int i = 0; i < activations.back().size(); i++){
		delta.push_back(cost_deriv[i] * sp[i]);
	}
	sp.clear();
	partial_b[partial_b.size()-1] = delta; //partial_b for this layer is equal to the delta vector
	
	//partial_w for this layer is a matrix, where each row is the elementwise product of the delta 
	//vector and the previous layer's activation vector.
	for (int i = 0; i < delta.size(); i++){
		for (int j = 0; j < activations[activations.size()-2].size(); j++){
			partial_w[partial_w.size()-1][i][j] = delta[i] * activations[activations.size()-2][j];
		}
	}
	
	cout << "point 1" << endl;

	/*
	this loop starts at the second to last layer, and ends at the first layer.
	*/
	for (int i = (b.size()) - 2; i >= 0; i--){
		prev_delta = delta; //get previous layer's delta vector
		delta.clear();
		z = zs[i];
		for (int j = 0; j < z.size(); j++){
			sp.push_back(sig_prime(z[j]));
		}
		
		cout << "point 2" << endl;
		//get transpose of weights (will make a function)
		for (int j = 0; j < w[i+1][0].size(); j++){
			for (int k = 0; k < w[i+1].size(); k++){
				cw_t.push_back(w[i+1][k][j]);
			}
			w_t.push_back(cw_t);
			cw_t.clear();
		}
		
		cout << "point 3, should loop " << prev_delta.size() << " times" << endl;
		//find the next delta vector
		for (int j = 0; j < prev_delta.size(); j++){//for each row in w_t
			d_w = 0;
			//add to d_w, dot product of prev_delta and current row of w_t
			for (int k = 0; k < w_t[i][j].size(); k++){
				d_w += prev_delta[j] * w_t[i][j][k];
			}
			cout << j << endl;
			delta.push_back(d_w * sp[j]);
		}
		sp.clear();
		
		cout << "point 4" << endl;
		partial_b[i] = delta;
		cout << delta.size() << endl;
		//get the partial_w matrix for this layer
		for (int j = 0; j < delta.size(); j++){
			cout << j << endl;
			for (int k = 0; k < activations[i].size(); k++){
				partial_w[i][j][k] = delta[j] * activations[i][k];
			}
		}
		cout << "point 5" << endl;
	}
	//return partial_b and partial_w
	result.first = partial_b;
	result.second = partial_w;
	return result;
	
}
/*
xs is a list of training inputs, ys is a list of the ideal outputs corresponding to the inputs. lr is the 
learning rate, which controls how much the weights and biases change with each epoch.
*/
void Network::update(vector<vector<double>> xs, vector<vector<double>> ys, double lr){
	vector<vector<double>>& b = biases;
	vector<vector<vector<double>>>& w = weights;

	vector<vector<double>> nb; //total of all partial derivative of biases
	vector<vector<vector<double>>> nw; //total of all partial derivative of weights
	
	//used to initialize nb and nw to zeros (will make a function)
	vector<double> b1;
	vector<vector<double>> w1;
	vector<double> w2;
	
	vector<vector<double>> dnb;
	vector<vector<vector<double>>> dnw;	
	
	//from backprop
	pair< vector<vector<double>>, vector<vector<vector<double>>> > delta_bw;
	
	
	//initialize nb and nw to zeros
	for (int i = 0; i < b.size(); i++){
		for (int j  = 0; j < b[i].size(); j++){
			b1.push_back(0.0);
		}
		nb.push_back(b1);
		b1.clear();
		
		for (int j = 0; j < w[i].size(); j++){
			for (int k = 0; k < w[i][j].size(); k++){
				w2.push_back(0.0);
			}
			w1.push_back(w2);
			w2.clear();
		}
		nw.push_back(w1);
		w1.clear();
	}
	
	
	cout << "backprop loop" << endl;
	
	//for each training example, get nb and nw (partial gradients of biases and weights)
	for (int i = 0; i < xs.size(); i++){
		cout << "start backprop" << endl;
		delta_bw = backprop(xs[i], ys[i]);
		cout << "backprop done" << endl;
		dnb = delta_bw.first;
		dnw = delta_bw.second;
		
		//for each training example, add the individual results to the total
		for(int j = 0; j < nw.size(); j++){
			for(int k = 0; k < nw[j].size(); k++){
				for(int l = 0; l < nw[j][k].size(); l++){
					nw[j][k][l] += dnw[j][k][l];
				}
			}
			for(int k = 0; k < nb[j].size(); k++){
				nb[j][k] += dnb[j][k];
			}
		}
	}
	

	//update the weights and biases. The amount subtracted is the average partial_b or partial_w
	//from the given data times the learning rate (lr).
	for(int i = 0; i < nw.size(); i++){
		for(int j = 0; j < nw[i].size(); j++){
			for(int k = 0; k < nw[i][j].size(); k++){
				w[i][j][k] = w[i][j][k] - (lr/xs.size())*nw[i][j][k];
			}
		}

		for(int j = 0; j < nb[i].size(); j++){
			b[i][j] = b[i][j] - (lr/xs.size()) * nb[i][j];
		}
	}

	
}
/*
breaks the data up into batches and calls the update function until all data is used 
*/
void Network::train(vector<vector<double>> xs, vector<vector<double>> ys, int batch_size, double lr){
	int n = xs.size();
	int batches = (n/batch_size) + 1;
	cout << "number of batches: " << batches << endl;
	
	//currently nonrandom batches are used, will be random later
	vector<vector<double>> batch_xs; //for this batch's inputs
	vector<vector<double>> batch_ys; //for this batch's outputs
	for (int i = 0; i < batches; i++){
		if (i == batches-1){ //if last batch (size may be less than batch_size)
			for (int j = 0; j < n%batch_size; j++){
				batch_xs.push_back(xs[i*batch_size + j]);
				batch_ys.push_back(ys[i*batch_size + j]);
			}
		}
			else{
			for (int j = 0; j < batch_size; j++){
				batch_xs.push_back(xs[i*batch_size + j]);
				batch_ys.push_back(ys[i*batch_size + j]);
			}
		}
		cout << "current batch: " << i+1 << endl;
		//update with the data from this batch
		update(batch_xs, batch_ys, lr);
	}
}
/*
calls feedforward with each vector in xs, then checks if the output is correct. Outputs the
amount correct to cout.
*/	
void Network::evaluate(vector<vector<double>> xs, vector<wchar_t> correct_chars, vector<wchar_t> char_set){
	vector<double> a;
	wchar_t network_output;
	int highest_a_index;
	int highest_a = 0;
	int total_ex = 0;
	int total_correct = 0;
	for (int i = 0; i < xs.size(); i++){
		a = feedforward(xs[i]);
		for (int j = 0; j < a.size(); j++){
			if(a[j] > highest_a){
				highest_a = a[j];
				highest_a_index = j;
			}
		}
		network_output = char_set[highest_a_index];
		if (network_output == correct_chars[i]){
			total_correct++;
		}
		total_ex++;
		highest_a = 0;
	}
	cout << total_correct << "/" << total_ex;
}




