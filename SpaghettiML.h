#ifndef SPAGHETTIML_HEADER
#define SPAGHETTIML_HEADER

#include "LinkedList.h"

#include <vector>
#include <cmath>
#include <unordered_map>
#include <cstdlib>
#include <fstream>

namespace SpaghettiML {
	float rand_float(float lower, float higher) {
		return lower + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * std::abs(lower - higher);
	}

	unsigned int rand_int(unsigned int lower, unsigned int higher) {
		return lower + (std::rand() % (higher - lower));
	}

	struct Neuron {
		float bias;
		std::vector<std::pair<float, size_t>> to;

		Neuron(float bias = 0, std::vector<std::pair<float, size_t>> to = std::vector<std::pair<float, size_t>>()) {
			this->bias = bias;
			this->to = to;
		}

		void RandBias(float min, float max) {
			this->bias = rand_float(min, max);
		}

		void AddTo(size_t to_neuron, float min, float max) {
			this->to.push_back(std::pair<float, size_t>(rand_float(min, max), to_neuron));
		}

		size_t GetRandToID() {
			return rand_int(0, this->to.size());
		}

		void ToRandWeight(size_t toID, float min, float max) {
			this->to[toID].first = rand_float(min, max);
		}

		bool RandToRandWeight(float min, float max) {
			if (this->to.size() == 0)
				return false;
			this->ToRandWeight(this->GetRandToID(), min, max);
			return true;
		}

		void RemoveTo(size_t toID) {
			this->to.erase(this->to.begin() + toID);
		}

		bool RandRemoveTo() {
			if (this->to.size() == 0)
				return false;
			this->RemoveTo(this->GetRandToID());
			return true;
		}
	};

	struct Brain {
		float minBias;
		float maxBias;
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

		Brain(float minBias = 0, float maxBias = 0, float minWeight = 0, float maxWeight = 0, bool hasThreshold = false, float threshold = 0,
			std::vector<Neuron> neurons = std::vector<Neuron>(),
			LinkedList::List<std::pair<size_t, float>> inputs = LinkedList::List<std::pair<size_t, float>>(),
			LinkedList::List<size_t> outputOrder = LinkedList::List<size_t>(),
			std::unordered_map<size_t, float> outputs = std::unordered_map<size_t, float>(),
			LinkedList::List<size_t> opInstructions = LinkedList::List<size_t>(),
			std::unordered_map<size_t, float> opNeurons = std::unordered_map<size_t, float>()) {
			this->minBias = minBias;
			this->maxBias = maxBias;
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
			this->neurons.back().RandBias(minBias, maxBias);
		}

		void AddInput(float value) {
			this->inputs.push(std::pair<size_t, float>(this->neurons.size(), value));
			this->AddNeuron();
			this->neurons.back().RandBias(minBias, maxBias);
		}

		void AddOutput() {
			this->outputOrder.push(this->neurons.size());
			this->outputs.insert(std::pair<size_t, float>(this->neurons.size(), 0));
			this->neurons.push_back(Neuron());
			this->neurons.back().RandBias(minBias, maxBias);
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

		void RandBias() {
			this->GetRandNeuron().RandBias(this->minBias, this->maxBias);
		}

		bool RandToRandWeight() {
			return this->GetRandNeuron().RandToRandWeight(this->minWeight, this->maxWeight);
		}

		void RandAddTo() {
			Neuron& from = this->GetRandNeuron();
			size_t to = this->GetRandNeuronID();
			from.AddTo(to, minWeight, maxWeight);
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
				this->RandBias();
				break;
			case 2:
				return this->RandToRandWeight();
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
				float value = this->opNeurons[neuronID] + neuron.bias;
				this->opInstructions.erase_head();
				this->opNeurons.erase(neuronID);
				auto isOutput = this->outputs.find(neuronID);
				if (isOutput != this->outputs.end())
					isOutput->second += value;
				if (value == 0 || (this->hasThreshold && value < this->threshold))
					continue;
				for (std::pair<float, size_t>& connection : neuron.to) {
					float addValue = value * connection.first;
					auto isFound = this->opNeurons.find(connection.second);
					if (isFound != this->opNeurons.end()) {
						isFound->second += addValue;
						continue;
					}
					this->opInstructions.push(connection.second);
					this->opNeurons.insert(std::pair<size_t, float>(connection.second, addValue));
				}
			}
		}

		void ClearOperatingNeurons() {
			for (std::pair<const size_t, float>& neuron : this->opNeurons)
				neuron.second = 0;
		}

		void ClearOutputs() {
			for (std::pair<const size_t, float>& neuron : this->outputs)
				neuron.second = 0;
		}

		LinkedList::List<float> GetOutputs() {
			LinkedList::List<float> outputValues = LinkedList::List<float>();
			for (LinkedList::Node<size_t>* i = this->outputOrder.head; i != nullptr; i = i->next)
				outputValues.push(this->outputs[i->value]);
			return outputValues;
		}

		void Save() {
			//std::cout << "Not Implemented Yet!";
		}

		void Load() {
			//std::cout << "Not Implemented Yet!";
		}
	};
}

#endif