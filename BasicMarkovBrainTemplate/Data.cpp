//
//  Data.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/31/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "Data.h"
#include <cstring>

//LOD variables
int Data::ID=0;
map<int,map<string,string>> Data::dataMap;
//Command Line Parameters
map<string,int*> Data::parameterInt;
map<string,double*> Data::parameterDouble;
map<string,string*> Data::parameterString;
map<string,bool*> Data::parameterBool;
map<string,double*> Data::defaultDoubles;
map<string,int*> Data::defaultInts;
map<string,string*> Data::defaultStrings;
map<string,bool*> Data::defaultBools;


// implementation for LOD
void Data::Add(int value,string key,Genome* to){
	dataMap[to->ID][key]=to_string(value);
}

void Data::Add(double value,string key,Genome* to){
	dataMap[to->ID][key]=to_string(value);
}

string Data::Get(string key,Genome *from){
		return dataMap[from->ID][key];
}

vector<string> Data::GetLODItem(string key,Genome *from){
	vector<string> list;
	Genome *G=from;
	while(G!=NULL){
		list.insert(list.begin(), dataMap[G->ID][key]);
		G=G->ancestor;
	}
	return list;
}
vector<Genome*> Data::getLOD(Genome *from){
	vector<Genome*> list;
	Genome *G=from;
	while(G!=NULL){
		list.insert(list.begin(), G);
		G=G->ancestor;
	}
	return list;    
}

void Data::removeGenome(Genome* who){
	if(dataMap.find(who->ID)!=dataMap.end())
		dataMap.erase(dataMap.find(who->ID));
}

int Data::registerGenome(Genome* who){
	int I=ID;
	dataMap[ID]["ID"]=to_string(ID);
	ID++;
	return I;
}


void Data::saveLOD(Genome *who,string filename){
	FILE *F=fopen(filename.c_str(),"w+t");
	Genome *G=who;
	vector<Genome*> list;
	//save the header
	fprintf(F,"N");
	for(map<string,string>::iterator I=dataMap[who->ID].begin();I!=dataMap[who->ID].end();I++)
		fprintf(F,",%s",I->first.c_str());
	fprintf(F,"\n");
	
	
	//get al the LOD pointers
	while(G!=NULL){
		list.insert(list.begin(),G);
		G=(Genome*)G->ancestor;
	}
//	for(int i=0;i<list.size();i++)
//		printf("find %p %i\n",list[i],list[i]->ID);
	//now save the data
	for(int i=0;i<list.size();i++){
		fprintf(F,"%i",i);
		for(map<string,string>::iterator I=dataMap[list[i]->ID].begin();I!=dataMap[list[i]->ID].end();I++){
			fprintf(F,",%s",I->second.c_str());
		}
		fprintf(F,"\n");
	}
	fclose(F);
}

void Data::saveGEN(Genome *who,string filename,int intervall){
	Genome *G=who;
	vector<Genome*> list;
	FILE *F=fopen(filename.c_str(),"w+t");
	//get al the LOD pointers
	while(G!=NULL){
		list.insert(list.begin(),G);
		G=G->ancestor;
	}
	for(int i=0;i<list.size();i+=intervall){
		list[i]->saveToFile(F);
	}
	fclose(F);
}

Genome* Data::getMostRecentCommonAncestor(Genome *from){
	vector<Genome*> LOD=getLOD(from);
	for(Genome *G:LOD){
		//printf("%i %i\n",G->ID,G->referenceCounter);
		if(G->referenceCounter>1)
			return G;
	}
	return from;
}

//implementation for parameters
void Data::setDefaultParameter(string key,int *variable,int value){
	parameterInt[key]=variable;
	*variable=value;
}

void Data::setDefaultParameter(string key,double *variable,double value){
	parameterDouble[key]=variable;
	*variable=value;
}

void Data::setDefaultParameter(string key,string *variable,string value){
	parameterString[key]=variable;
	*variable=value;
}

void Data::setDefaultParameter(string key,bool *variable,bool value){
	parameterBool[key]=variable;
	*variable=value;
}

void Data::UseCommandLineParameters(int argc, const char * argv[]){
	for(int i=1;i<argc;i+=2){
		string S=string(argv[i]);
		if(parameterInt.find(S)!=parameterInt.end()){
			*parameterInt[S]=atoi(argv[i+1]);
			printf("set int %s to %i\n",S.c_str(),(*parameterInt[S]));
		}
		if(parameterDouble.find(S)!=parameterDouble.end()){
			*parameterDouble[S]=(double)atof(argv[i+1]);
			printf("set double %s to %f\n",S.c_str(),(*parameterDouble[S]));
		}
		if(parameterString.find(S)!=parameterString.end()){
			*parameterString[S]=string(argv[i+1]);
			printf("set string %s to %s\n",S.c_str(),(*parameterString[S]).c_str());
		}
		if(parameterBool.find(S)!=parameterBool.end()){
			*parameterBool[S]=strcmp(argv[i+1],"true");
			printf("set bool %s to %i\n",S.c_str(),(*parameterBool[S]));
		}
	}
}


double Data::getDefaultDouble(string S){
	return (*defaultDoubles[S]);
}

int Data::getDefaultInt(string S){
	return (*defaultInts[S]);
}

string Data::getDefaultString(string S){
	return (*defaultStrings[S]);
}

bool Data::getDefaultBool(string S){
	return (*defaultBools[S]);
}

double* Data::makeDefaultDouble(string S){
	double *D=new double;
	defaultDoubles[S]=D;
	return D;
}

int* Data::makeDefaultInt(string S){
	int *I=new int;
	defaultInts[S]=I;
	return I;
}
string* Data::makeDefaultString(string S){
	string *D=new string();
	defaultStrings[S]=D;
	return D;
}

bool* Data::makeDefaultBool(string S){
	bool *B=new bool;
	defaultBools[S]=B;
	return B;
}

void Data::setDefaultDouble(string S,double to){
    (*defaultDoubles[S])=to;
}

void Data::setDefaultInt(string S,int to){
    (*defaultInts[S])=to;
    
}

void Data::setDefaultString(string S,string to){
    (*defaultStrings[S])=to;

}

void Data::setDefaultBool(string S,bool to){
    (*defaultBools[S])=to;
}

void Data::showAll(){
	for (auto& A : dataMap)
		for(auto& B : A.second)
			printf("%p %s %s\n",A.first,B.first.c_str(),B.second.c_str());
}


