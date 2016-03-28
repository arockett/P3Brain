//
//  TritGate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//
//
#ifndef __BasicMarkovBrainTemplate__NeuronGate__
#define __BasicMarkovBrainTemplate__NeuronGate__

#include <iostream>
#include <math.h>

#include "Gate.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

class NeuronGate: public Gate {
public:

	static const int defaultNumInputsMin;
	static const int defaultNumInputsMax;
	static const int defaultDischargeBehavior; //-1 or 0,1,2;
	static const double defaultThresholdMin;
	static const double defaultThresholdMax;

	static const bool defaultAllowRepression; // if false, neuronGates fire if they exceed charge, if true, gates can exist which always fire, unless they exceed charge
	static const double defaultDecayRateMin;
	static const double defaultDecayRateMax;
	static const double defaultDeliveryChargeMin; //
	static const double defaultDeliveryChargeMax; //
	static const double defaultDeliveryError;


	int dischargeBehavior;  // what to do when the gate delivers a charge
	double thresholdValue;  // threshold when this gate will fire (if negative, then fire when currentCharge < threshold)
	bool thresholdActivates;  // if true, fire when currentCharge is >= threshold, if false, fire unless currentCharge >
	double decayRate;  // every brain update reduce currentCharge by decayRate
	double deliveryCharge;  // charge delivered when this gate fires
	double deliveryError;  // delivery charge is reduced by random[0...deliveryError)

	double currentCharge;

	//double costOfDecay;
	//double costOfDelivery;

	NeuronGate() = delete;
	NeuronGate(vector<int> ins, int out, int _dischargeBehavior, double _thresholdValue, bool _thresholdActivates, double _decayRate, double _deliveryCharge, double _deliveryError, int _ID) {
		ID = _ID;
		inputs = ins;
		outputs.clear();
		outputs.push_back(out);
		dischargeBehavior = _dischargeBehavior;
		thresholdValue = _thresholdValue;
		thresholdActivates = _thresholdActivates;
		decayRate = _decayRate;
		deliveryCharge = _deliveryCharge;
		deliveryError = _deliveryError;
		currentCharge = 0;
	}

	virtual ~NeuronGate() = default;
	virtual void update(vector<double> & nodes, vector<double> & nextnodes) override {
		//cout << description() << endl;
		bool fire = false;
		// decay first
		//cout << currentCharge;
		currentCharge += -1 * Trit(currentCharge) * decayRate;
		//cout << "->" << currentCharge << endl;
		// add inputs to currCharge
		//cout << currentCharge;
		for (auto i : inputs) {
			currentCharge += nodes[i];
		}
		//cout << "->" << currentCharge << endl;
		// if currCharge is >= Th, fire
		//   reduce currCharge
		if (thresholdActivates) {  // fire if currCharge is greater than a positive threshold or less than a negative threshold
			if (((Trit(currentCharge) * currentCharge) > (Trit(thresholdValue) * thresholdValue)) && (Trit(currentCharge) == Trit(currentCharge))) {
				fire = true;
			}
		} else { // threshold represses. fire always unless currCharge is greater than a positive threshold (or less than a negative threshold)
			if (Trit(thresholdValue) == 1) { // if thresholdValue is positive
				if (currentCharge < thresholdValue) {
					fire = true;
				}
			} else { // if threshold is negative (or 0)
				if (currentCharge > thresholdValue) {
					fire = true;
				}
			}
		}

		if (fire) {
			//cout << "Fire!" <<  endl;
			nextnodes[outputs[0]] += deliveryCharge - Random::getDouble(0, deliveryError);
			if (dischargeBehavior == 0) {
				currentCharge = 0;
			}
			if (dischargeBehavior == 1) {
				int currentChargeSign = Trit(currentCharge);
				currentCharge = (currentChargeSign * currentCharge) - (Trit(thresholdValue) * thresholdValue) * currentChargeSign;
			}
			if (dischargeBehavior == 2) {
				currentCharge = currentCharge * .5;
			}

		}

	}

//void setupForBits(int* Ins, int nrOfIns, int Out, int logic);

	virtual string description() override {
		string s = "Gate " + to_string(ID) + " is a Neuron Gate with " + to_string(inputs.size()) + " inputs (";
		for (auto a : inputs) {
			s += to_string(a) + ",";
		}
		s.pop_back();
		s += ") and output " + to_string(outputs[0]) + "\n";
		s += "currentCharge: " + to_string(currentCharge) + "\n";
		s += "thresholdValue: " + to_string(thresholdValue) + "\n";
		s += "thresholdActivates: " + to_string(thresholdActivates) + "\n";
		s += "dischargeBehavior: " + to_string(dischargeBehavior) + "\n";
		s += "decayRate: " + to_string(decayRate) + "\n";
		s += "deliveryCharge: " + to_string(deliveryCharge) + "\n";
		s += "deliveryError: " + to_string(deliveryError) + "\n";
		return s;
	}

//double voidOutput;
};

#endif /* defined(__BasicMarkovBrainTemplate__NeuronGate__) */

//NeuronGate::NeuronGate(pair<vector<int>, vector<int>> addresses, int _ID) {
//	ID = _ID;
//	inputs = addresses.first;
//	outputs = addresses.second;
//}
//
//void TritDeterministicGate::update(vector<double> & nodes, vector<double> & nextNodes) {
//	int input = vectorToTritToInt(nodes,inputs,true);  // converts the input values into an index
//	for (size_t i = 0; i < outputs.size(); i++) {
//		nextNodes[outputs[i]] += table[input][i];
//	}
//}
//
//string TritDeterministicGate::description() {
//	return "Gate " + to_string(ID) + " is a Trit Deterministic " + Gate::description();
//}

