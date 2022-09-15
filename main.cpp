#include <cstdio>
#include <vector>
#include <algorithm>
#include "tools.h"
#include "parameters.h"
#include "stationAssignment.h"
#include "pathPlanning.h"
#include "vehicleWork.h"
using namespace std;


int main()
{

    /////////////// initialization ///////////////
    initObjects(stationNum, carNum);
    vector<vector<int>> clusters = divideInSection(stations, carNum);
    for (int i = 0; i < cars.size(); i++) { cars[i].scope = clusters[i]; cars[i].route = genInitRoute(cars[i], stations); }
    vector<int> sortedStationInds; // 按照顺时针排序好的站点下标
    for (int i = 0; i < clusters.size(); i++) sortedStationInds.insert(sortedStationInds.end(), clusters[i].begin(), clusters[i].end());
        

    /////////////// main loop ///////////////
    double allDecay = 0;
    int iter = 1;
    while (iter <= iterNum)
    {
        // cargo consumption in stations
        for (int i = 0; i < stations.size(); i++)
        {
            double r = randarea(0, 1) * cargoDecayRange;
            stations[i].cargo -= r;
            allDecay += r;
            stations[i].time ++;
        }
        int periodNum = 5;
        for (int i = 0; i < stationNum / 5; i++)
            if (randarea(0, 1) < emergencyRate)
            {
                allDecay += stations[sortedStationInds[stationNum / periodNum * ((iter - 1) / (iterNum / periodNum)) + i]].cargo * 0.5;
                stations[sortedStationInds[stationNum / periodNum * ((iter - 1) / (iterNum / periodNum)) + i]].cargo *= 0.5;
            }
            
        // work of vehicles
        for (int i = 0; i < cars.size(); i++)
        {
            double timeLeft = 1;
            while (timeLeft > 0)
            {
                if (cars[i].status == 1 && timeLeft > 0)
                {
                    CarAndTimeLeft ct = toNextStation(cars[i], timeLeft);
                    cars[i] = ct.car; timeLeft = ct.timeLeft;
                }
                
                if (cars[i].status == 2 && timeLeft > 0)
                {
                    CarAndTimeLeft ct = unload(cars[i], timeLeft);
                    cars[i] = ct.car; timeLeft = ct.timeLeft;
                }

                if (cars[i].status == 3 && timeLeft > 0)
                {
                    CarAndTimeLeft ct = backToDepot(cars[i], timeLeft);
                    cars[i] = ct.car; timeLeft = ct.timeLeft;
                }
                
                if (cars[i].status == 4 && timeLeft > 0)
                {
                    CarAndTimeLeft ct = reload(cars[i], timeLeft);
                    cars[i] = ct.car; timeLeft = ct.timeLeft;
                }
            }
        }

        // station reassignment
        vector<double> shortageEachCar;
        for (int i = 0; i < cars.size(); i++)
        {
            double tmp = 0;
            for (int j = 0; j < cars[i].scope.size(); j++) tmp += 100 - stations[cars[i].scope[j]].cargo;
            shortageEachCar.push_back(tmp);
        }
        if (max(shortageEachCar) - min(shortageEachCar) > 400)
        {
            vector<vector<int>> clusters = divideInSectionByCargo(stations, carNum);
            for (int i = 0; i < cars.size(); i++) { cars[i].scope = clusters[i]; cars[i].route = genInitRoute(cars[i], stations); }
        }

        // update of some information
        double allCargo = 0;
        for (int i = 0; i < stations.size(); i++)
        {
            stations[i].wait += (100 - stations[i].cargo) / 100;
            allCargo += stations[i].cargo;
        }
        iter++;

    }
    /////////////// results calculation ///////////////
    double wait = 0;
    for (int i = 0; i < stations.size(); i++) wait += stations[i].wait;
    double OTW = 0, ULT = 0, RLT = 0;
    for (int i = 0; i < cars.size(); i++)
    {
        ULT += cars[i].unloadTime;
        RLT += cars[i].reloadTime;
        OTW += cars[i].onTheWay;
    }
    
    printf("wait:%.2lf, OTW:%.2lf, alldecay:%.2lf, ULT:%.2lf, RLT:%.2lf\n", wait, OTW, allDecay/unloadSpeed, ULT, RLT);

    
}

