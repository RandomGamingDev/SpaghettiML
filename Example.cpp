#include <iostream>

#include "SpaghettiML.h"

int main() {
	SpaghettiML::Brain brain = SpaghettiML::Brain(-2, 2, -2, 2); 
	brain.AddInput(0);
	brain.AddInput(1);
	brain.AddOutput();
	float output;
	while (true) {
		brain.InitInputs();
		for (unsigned int i = 0; i < 5; i++)
			brain.Tick();
		output = brain.GetOutputs().head->value;
		brain.ClearOutputs();
		brain.ClearOperatingNeurons();
		std::cout << output << '\n';
		if (output > 0 && output < 1) {
			std::cout << output;
			return 0;
		}
		brain.RandMutate();
	}

	return 0;
}