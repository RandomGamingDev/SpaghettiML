#ifndef SPHAGETTIML_HEADER
#define SPHAGETTIML_HEADER

#include "LinkedList.h"

#include <vector>
#include <cmath>
#include <unordered_map>

namespace SphagettiML {
	float rand_float(float lower, float higher) {
		return lower + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * abs(lower - higher);
	}

	unsigned int rand_int(unsigned int lower, unsigned int higher) {
		return lower + (rand() % (higher - lower));
	}

	struct Neuron {
		float off;
		std::vector<size_t> to;

		Neuron(float off = 0, std::vector<size_t> to = std::vector<size_t>()) {
			this->off = off;
			this->to = to;
		}
		
		void RandOff(float min, float max) {
			this->off = rand_float(min, max);
		}

		void Disable() {
			this->off = 0;
		}

		void AddTo(size_t to_neuron) {
			this->to.push_back(to_neuron);
		}

		void RemoveTo(size_t toRemove) {
			this->to.erase(this->to.begin() + toRemove);
		}

		bool RandRemoveTo() {
			if (this->to.size() == 0)
				return false;
			this->RemoveTo(rand_int(0, this->to.size()));
			return true;
		}

		size_t GetRandToID() {
			return rand_int(0, this->to.size());
		}
	};

	struct Brain {
		float minWeight;
		float maxWeight;
		bool hasThreshold;
		float threshold;
		std::vector<Neuron> neurons;
		LinkedList::List<std::pair<size_t, float>> inputs;
		LinkedList::List<size_t> outputOrder;
		std::unordered_map<size_t, float> outputs;
		LinkedList::List<size_t> opInstructions;
		std::unordered_map<size_t, float> opNeurons;

		Brain(float minWeight = 0, float maxWeight = 0, bool hasThreshold = false, float threshold = 0,
			  std::vector<Neuron> neurons = std::vector<Neuron>(),
			  LinkedList::List<std::pair<size_t, float>> inputs = LinkedList::List<std::pair<size_t,float>>(),
			  LinkedList::List<size_t> outputOrder = LinkedList::List<size_t>(),
			  std::unordered_map<size_t, float> outputs = std::unordered_map<size_t, float>(),
			  LinkedList::List<size_t> opInstructions = LinkedList::List<size_t>(),
			  std::unordered_map<size_t, float> opNeurons = std::unordered_map<size_t, float>()) {
			this->minWeight = minWeight;
			this->maxWeight = maxWeight;
			this->hasThreshold = hasThreshold;
			this->threshold = threshold;
			this->neurons = neurons;
			this->inputs = inputs;
			this->outputOrder = outputOrder;
			this->outputs = outputs;
			this->opInstructions = opInstructions;
			this->opNeurons = opNeurons;
		}
		
		void AddNeuron() {
			this->neurons.push_back(Neuron());
		}

		void AddRandNeuron() {
			this->AddNeuron();
			this->neurons.back().RandOff(minWeight, maxWeight);
		} 

		void AddInput(float value) {
			this->inputs.push(std::pair<size_t, float>(this->neurons.size(), value));
			this->AddNeuron();
			this->neurons.back().RandOff(minWeight, maxWeight);
		}

		void AddOutput() {
			this->outputOrder.push(this->neurons.size());
			this->outputs.insert(std::pair<size_t, float>(this->neurons.size(), 0));
			this->neurons.push_back(Neuron());
			this->neurons.back().RandOff(minWeight, maxWeight);
		}

		void RemoveInput(LinkedList::Node<std::pair<size_t, float>> prevNode) {
			this->inputs.erase_next(&prevNode);
		}

		void RemoveOutput(LinkedList::Node<size_t> prevNode) {
			this->outputs.erase(prevNode.next->value);
			this->outputOrder.erase_next(&prevNode);
		}

		size_t GetRandNeuronID() {
			return rand_int(0, neurons.size());
		}

		Neuron& GetRandNeuron() {
			return this->neurons[this->GetRandNeuronID()];
		}

		void RandOff() {
			this->GetRandNeuron().RandOff(this->minWeight, this->maxWeight);
		}

		void RandDisable() {
			this->GetRandNeuron().Disable();
		}
		
		void RandAddTo() {
			Neuron& from = this->GetRandNeuron();
			size_t to = this->GetRandNeuronID();
			from.AddTo(to);
		}

		bool RandRemoveTo() {
			return this->GetRandNeuron().RandRemoveTo();
		}

		bool RandMutate() {
			switch (rand_int(0, 5)) {
			case 0:
				this->AddRandNeuron();
				break;
			case 1:
				this->RandOff();
				break;
			case 2:
				this->RandDisable();
				break;
			case 3:
				this->RandAddTo();
				break;
			case 4:
				return this->RandRemoveTo();
				break;
			}
			return true;
		}

		void InitInputs() {
			for (LinkedList::Node<std::pair<size_t, float>>* i = this->inputs.head; i != nullptr; i = i->next) {
				this->opNeurons.insert(i->value);
				this->opInstructions.push(i->value.first);
			}
		}

		void Tick() {
			size_t toOp = this->opInstructions.length;
			for (size_t i = 0; i < toOp; i++) {
				size_t neuronID = this->opInstructions.head->value;
				Neuron& neuron = this->neurons[neuronID];
				float value = this->opNeurons[this->opInstructions.head->value] * neuron.off;
				this->opInstructions.erase_head();
				this->opNeurons.erase(neuronID);
				auto isOutput = this->outputs.find(neuronID);
				if (isOutput != this->outputs.end())
					isOutput->second += value;
				if (value == 0 || (this->hasThreshold && value < this->threshold))
					continue;
				for (size_t toNeuron : neuron.to) {
					auto isFound = this->opNeurons.find(toNeuron);
					if (isFound != this->opNeurons.end()) {
						isFound->second += value;
						continue;
					}
					this->opInstructions.push(toNeuron);
					this->opNeurons.insert(std::pair<size_t, float>(toNeuron, value));
				}
			}
		}

		void ClearOutputs() {
			for (std::pair<const size_t, float>& neuron : this->opNeurons)
				neuron.second = 0;
		}

		LinkedList::List<float> GetOutputs() {
			LinkedList::List<float> outputValues = LinkedList::List<float>();
			for (LinkedList::Node<size_t>* i = this->outputOrder.head; i != nullptr; i = i->next)
				outputValues.push(this->outputs[i->value] * this->neurons[i->value].off);
			return outputValues;
		}

		void Save() {
			std::cout << "Not Implemented Yet!";
		}

		void Load() {
			std::cout << "Not Implemented Yet!";
		}
	};
}

#endif