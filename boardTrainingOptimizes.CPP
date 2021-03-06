//Written by: Rijo Simon
//Email: rssimon@alaska.edu




#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <ctime>
#include <math.h> //For tanh

using namespace std;

// returns a float in the range -1.0f -> - 1.0f
#define RANDOM_CLAMP    (((float)rand()-(float)rand())/RAND_MAX)

//returns a float between 0 & 1
#define RANDOM_NUM		((float)rand()/(RAND_MAX+1))

//using namespace std;

class Dendrite {
       public:
               double d_weight; //Weight of the neuron
               unsigned long d_points_to; //The index of the neuron of the next layer to which it points
               Dendrite(double weight=0.0, unsigned long points_to=0){ //Constructor
                           d_weight=weight;
                           d_points_to=points_to; //Give it a initial value
               }

      };

class Neuron  {
      public:
         unsigned long n_ID; //ID of a particular neuron in a layer
                               //Used to find a particular neuron in an array
	     double n_value; //Value which Neuron currently is holding
	     double n_bias;  //Bias of the neuron
         double n_delta; //Used in back  prop. Note it is backprop specific
         Dendrite *Dendrites; //Dendrites
      //Constructor assigning initial values
	     Neuron(unsigned long ID=0,double value=0.0,double bias=0.0){
                      n_ID=ID;
                      n_value=value;
                      n_bias=bias;
                      n_delta=0.0;

                 }
         void SetDendrites(unsigned long dendrite){ //Set the dendrites from the neuron to given dendrite
              Dendrites=new Dendrite[dendrite];
			  //#pragma omp parallel for
              for(int i=0;i<dendrite;i++){
                      Dendrites[i].d_points_to=i; // Initialize the dendrite to attach to next layer
                      }
         }
};

class Layer{
      public:
             Neuron *Neurons; //Pointer to array of neurons

             /*Layer(unsigned long size=1){    //size is no. of neurons in it
             Neurons = new Neuron [size];
                       }  */
              void Initialize(unsigned long size) {   //Initialize the layer
                    Neurons = new Neuron [size];

                    }
             ~Layer(){ //destructor deletes Neurons from the memory
                   delete Neurons;
               }
             Neuron GetNeuron(unsigned long index){  //Give the neuron at index
                    return Neurons[index];
                                            }
             void SetNeuron(Neuron neuron,unsigned long index){ //sets the neuron
                    Neurons[index]=neuron;
                    }
};

class Network { //The real neural network
      public:
             double net_learning_rate; //Learning rate of network
             Layer *Layers; //The total layers in network
             unsigned long net_tot_layers; //Number of layers
             double *net_inputs; //Input array
             double *net_outputs;//Output layers
             unsigned long *net_layers; //Array which tells no. of neurons in each layer
             //double GetRand(void);
             Network() {
             //Blank Constructor
             }

             int SetData(double learning_rate,unsigned long layers[],unsigned long tot_layers) { //Function to set various parameters of the net
                 if (tot_layers<2) return(-1); //Return error if total no. of layers < 2
                                               //Because input and output layers are necessary
                 net_learning_rate=learning_rate;
                 net_layers= new unsigned long [tot_layers]; //Initialize the layers array
                 Layers=new Layer[tot_layers];
				 #pragma omp parallel for
                 for(int i=0;i<tot_layers;i++){
                         net_layers[i]=layers[i];
                         Layers[i].Initialize(layers[i]); //Initialize each layer with the specified size
                         }

                 net_inputs=new double[layers[0]];
                 net_outputs=new double[layers[tot_layers-1]];
                 net_tot_layers=tot_layers;
                 return 0;
                 }
				 
             int SetInputs(double inputs[]){ //Function to set the inputs
				 #pragma omp parallel for
                 for(int i=0;i<net_layers[0];i++){
                         Layers[0].Neurons[i].n_value=inputs[i];
                         }
                         return 0;}
						 
             void RandomizeWB(void){ //Randomize weights and biases
                  int i,j,k;
                  for(i=0;i<net_tot_layers;i++){
                          for(j=0;j<net_layers[i];j++){
                                  if(i!=(net_tot_layers-1)){ //Last layer does not require weights
                                       Layers[i].Neurons[j].SetDendrites(net_layers[i+1]); //Initialize the dendites
									   //#pragma omp parallel for
                                       for(k=0;k<net_layers[i+1];k++){
                                            Layers[i].Neurons[j].Dendrites[k].d_weight=GetRand(); //Let weight be the random value
                                       }
                                   }
                                   if(i!=0){ //First layer does not need biases
                                   Layers[i].Neurons[j].n_bias=GetRand();
                                   }
                                 }
                               }
                    }
			
           double * GetOutput(void){ //Gives the output of the net
                  double *outputs;
                  int i,j,k;

                  outputs=new double[net_layers[net_tot_layers-1]]; //Temp ouput array

                  for(i=1;i<net_tot_layers;i++){
                      for(j=0;j<net_layers[i];j++){
                      Layers[i].Neurons[j].n_value=0;
						  #pragma omp parallel for
                          for(k=0;k<net_layers[i-1];k++){
                              Layers[i].Neurons[j].n_value=Layers[i].Neurons[j].n_value+Layers[i-1].Neurons[k].n_value*Layers[i-1].Neurons[k].Dendrites[j].d_weight; //Multiply and add all the inputs

                              }
                              Layers[i].Neurons[j].n_value=Layers[i].Neurons[j].n_value+Layers[i].Neurons[j].n_bias; //Add bias
                              Layers[i].Neurons[j].n_value=Limiter(Layers[i].Neurons[j].n_value);  //Squash that value
                             }
                            }
						   #pragma omp parallel for
                           for(i=0;i<net_layers[net_tot_layers-1];i++){
                           outputs[i]=Layers[net_tot_layers-1].Neurons[i].n_value;
                           }
                           return outputs; 
                           }
			
           void Update(void){ 
                GetOutput();
                }

           double Limiter(double value){ //Limiet to limit value between 1 and -1
                  //return tanh(value);   //Currently using tanh
                  return (1.0/(1+exp(-value)));
                  }
           double GetRand(void){  //Return a random number between range -1 to 1 using time to seed the srand function
                    time_t timer;
                    struct tm *tblock;
                    timer=time(NULL);
                    tblock=localtime(&timer);
                    int seed=int(tblock->tm_sec+100*RANDOM_CLAMP+100*RANDOM_NUM);
                    //srand(tblock->tm_sec);
                    srand(seed);
                    return (RANDOM_CLAMP+RANDOM_NUM);
                    }

           double SigmaWeightDelta(unsigned long layer_no, unsigned long neuron_no){ //Calculate sum of weights * delta. Used in back prop. layer_no is layer number. Layer number and neuron number can be zero
                  double result=0.0;
				  #pragma omp parallel for
                  for(int i=0;i<net_layers[layer_no+1];i++) { //Go through all the neurons in the next layer
                      result=result+Layers[layer_no].Neurons[neuron_no].Dendrites[i].d_weight*Layers[layer_no+1].Neurons[i].n_delta; //Comput the summation
                      }
                 return result;
                  }                                                        //neuron_no is neuron number. This function is used in back prop

           int Train(double inputs[],double outputs[]){ //The standard Backprop Learning algorithm
               int i,j,k;
               double Target, Actual, Delta;
               SetInputs(inputs); //Set the inputs
               Update(); //Update all the values
               //SetOutputs(outputs); //Set the outputs
               for(i=net_tot_layers-1;i>0;i--){ //Go from last layer to first layer
                   for(j=0;j<net_layers[i];j++) {//Go thru every neuron
                       if(i==net_tot_layers-1){ //Output layer, Needs special atential
                           Target=outputs[j]; //Target value
                           Actual=Layers[i].Neurons[j].n_value; //Actual value
                           Delta= (Target - Actual) * Actual * (1 - Actual); //Function to compute error
                           Layers[i].Neurons[j].n_delta=Delta; //Compute the delta
						   #pragma omp parallel for
                           for(k=0;k<net_layers[i-1];k++) {
                               Layers[i-1].Neurons[k].Dendrites[j].d_weight += Delta*net_learning_rate*Layers[i-1].Neurons[k].n_value; //Calculate the new weights
                               }

                           Layers[i].Neurons[j].n_bias = Layers[i].Neurons[j].n_bias + Delta*net_learning_rate*1; //n_value is always 1 for bias
                       } else { //Here
                            //Target value
                           Actual=Layers[i].Neurons[j].n_value; //Actual value
                           Delta=  Actual * (1 - Actual)* SigmaWeightDelta(i,j); //Function to compute error
						   #pragma omp parallel for
                           for(k=0;k<net_layers[i-1];k++){
                               Layers[i-1].Neurons[k].Dendrites[j].d_weight += Delta*net_learning_rate*Layers[i-1].Neurons[k].n_value; //Calculate the new weights
                           }
                           if(i!=0) //Input layer does not have a bias
                           Layers[i].Neurons[j].n_bias = Layers[i].Neurons[j].n_bias + Delta*net_learning_rate*1; //n_value is always 1 for bias
                       }
                      }
                     } return 0;
                  }
         ~Network(){ //delete Layers;
          }
};



int main()
{     Network my;

      unsigned long inp=2;
      unsigned long hid=2;
      unsigned long outp=1;
      unsigned long layers[3];
      layers[0]=inp;
      layers[1]=hid;
      layers[2]=outp;
      int i=0,j=0;
      unsigned long iter=0;
      cout<<"Enter number of training Iterations : ";
      cin>>iter;
      my.SetData(0.1,layers,3);

      double input[]={1,0};
      double *outputs;
      my.RandomizeWB();

      double tr_inp[4][2]={{0.0,0.0},{1.0,0.0},{0.0,1.0},{1.0,1.0}};
      double tr_out[4][1]={{0.0},{1.0},{1.0},{0.0}};
      cout<<"\nStarting Training...\n";
      std::chrono::time_point<std::chrono::system_clock> start, end;
      start = std::chrono::system_clock::now();
      for(i=0;i<iter;i++){
      for(j=0;j<4;j++){

      my.Train(tr_inp[j],tr_out[j]);

      }}
      end = std::chrono::system_clock::now();
      std::chrono::duration<double> elapsed_seconds = end-start;
	  std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	  std::cout << "Finished computation at " << std::ctime(&end_time)<< "elapsed time: " << elapsed_seconds.count() << "s\n";
      cout<<"\nEnd Training. ";
      return 0;
}



