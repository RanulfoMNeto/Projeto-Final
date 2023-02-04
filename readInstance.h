#include "libraries.h"

void readInstance(string fileName, Graph &grafo) {

	ifstream instance(fileName+".txt");

	if (instance) {

		//Informações da instância
		string NAME, LOCATION, COMMENT, TYPE, DISTRIBUTION, DEPOT;
		/*
			NAME: <identificação de instância única>
			LOCATION: <cidade onde foi gerado>
			COMMENT: : <uma referência geral>
			TYPE: <o tipo da instância>
			DISTRIBUTION: <distribuição a qual a instância foi gerada>
			DEPOT: <tipo de localização do depósito: 'central' ou 'aleatório'>
		*/
		int SIZE, ROUTE_TIME, TIME_WINDOW, CAPACITY;
		/*
			SIZE: <número de vértices incluindo o depósito>
			ROUTE_TIME: <horizonte de tempo da roteirização>
			TIME_WINDOW: <largura da janela de tempo>
			CAPACITY: <capacidade máxima do veículo>
		*/

		// Informação completa de cada ponto(vértice) na instância
		tipo id, p, d;
		/*
			<id>: o identificador único do ponto (o ponto 0 é o depósito único);
			<p>: o par de coleta se <id> for uma entrega; e 0 caso contrário;
			<d>: o par de entrega se <id> for uma coleta; e 0 caso contrário;
		*/
		string lat, lon;
		/*
			<lat>: latitude deste local;
			<lon>: longitude deste local;
		*/
		int dem, etw, ltw, dur, cluster;
		/*
			<dem>: a demanda deste nó (dem > 0 para coleta, dem < 0 para entrega);
			<etw>: tempo mais cedo possível para iniciar o serviço (janela de tempo);
			<ltw>: última hora possível para iniciar o serviço (janela de tempo);
			<dur>: a duração do serviço neste local;
		*/

		string fieldName, field;

		while(getline(instance, fieldName) && fieldName != "NODES") {
			//int i = 0;
			stringstream sst(fieldName);
			while(getline(sst, fieldName, ':')) {
				getline(sst, field);
				int i = 0;
				// Remove espaços em branco no começo
				while (field[i] == ' ')
					i++;
				field = field.substr(i, field.size());
				if (fieldName == "NAME")
					NAME = field;
				if (fieldName == "LOCATION")
					LOCATION = field;
				if (fieldName == "COMMENT")
					COMMENT = field;
				if (fieldName == "TYPE")
					TYPE = field;
				if (fieldName == "SIZE")
					SIZE = stoi(field);
				if (fieldName == "DISTRIBUTION")
					DISTRIBUTION = field;
				if (fieldName == "DEPOT")
					DEPOT = field;
				if (fieldName == "ROUTE-TIME")
					ROUTE_TIME = stoi(field);
				if (fieldName == "TIME-WINDOW")
					TIME_WINDOW = stoi(field);
				if (fieldName == "CAPACITY")
					CAPACITY = stoi(field);
			}
		}


		grafo.SIZE = SIZE;
		grafo.CAPACITY = CAPACITY;
		grafo.ROUTE_TIME = ROUTE_TIME;

		string node, info;

		while(getline(instance, node) && node != "EDGES") {
			int i = 0;
			cluster = 0;
			stringstream sst(node);
			while(getline(sst, info, ' ')) {
				switch (i) {
					case 0: id = stoi(info);break;
					case 1: lat = info;break;
					case 2: lon = info;break;
					case 3: dem = stoi(info);break;
					case 4: etw = stoi(info);break;
					case 5: ltw = stoi(info);break;
					case 6: dur = stoi(info);break;
					case 7: p = stoi(info);break;
					case 8: d = stoi(info);break;
					case 9: cluster = stoi(info);break;
				}
				i++;
			}
			grafo.adicionarVertice(id, lat, lon, dem, etw, ltw, dur, p, d, cluster);
		}

		string edge;
		int u = 0;
		while(getline(instance, edge) && edge != "EOF") {
			int v = 0;
			stringstream sst(edge);
			string custo;
			while(getline(sst, custo, ' ')) {
				grafo.adicionarAresta(u, v, stoi(custo));
				v++;
			}
			u++;
		}
	}
}