#include <vector>
using namespace std;

///////////////////////////////////////////// 参数 /////////////////////////////////////////////

// object definitions
class Station
{
    public:
        int id;
        double cargo;
        double decay;
        vector<double> pos;
        double time = 0;
        // 一些用于评价算法性能的参数
        double wait = 0;
};
vector<Station> stations;

class Car
{
    public:
        double cargo;           // 当前小车载货量
        vector<double> pos;     // 当前小车的位置
        vector<int> route;      // 当前小车剩余的路径安排
        int status;          // status:1表示正在路上,2表示正在卸货,3表示正在返程, 4表示正在装货
        double unloadLeft = 0;  // 剩余需要卸货的量
        int stopStation = -1;    // 当前车辆停靠的站点id
        vector<int> scope;      // 该车负责的站点在stations中的下标
        // 一些用于评价算法性能的参数
        int backCount = 0;      // 回仓次数
        double onTheWay = 0;    // 车辆在路上的时间
        double unloadTime = 0;  // 车辆总共卸货时间
        double reloadTime = 0;  // 车辆总共装货时间
};
vector<Car> cars;
vector<double> depot = {0, 0};

// parameter settings
int carNum = 5;
int stationNum = 500;
int iterNum = 1000;
double carSpeed = 5;
double unloadSpeed = 5, reloadSpeed = 5;
double carLoad = 200;
double cargoDecayRange = 0.01;
double emergencyRate = 0.001;
int threshold = 1200;
int area = 50;
double cargoLowerBound = 40;
double alpha = 0.1;

// 算法中要用的参数
vector<vector<double>> DM;  // 距离矩阵

///////////////////////////////////////////// 函数 /////////////////////////////////////////////
// Build Distance Matrix
vector<vector<double>> Build_Distance_Matrix(vector<Station> stations)
{
    vector<vector<double>> res;
    // 开辟空间
    for (int i = 0; i < stations.size(); i++)
    {
        vector<double> tmp(stations.size());
        res.push_back(tmp);
    }
    // 赋值
    for (int i = 0; i < stations.size(); i++)
    {
        for (int j = 0; j < stations.size(); j++)
            if (i == j) res[stations[i].id][stations[j].id] = INFINITY;
            else res[stations[i].id][stations[j].id] = distance(stations[i].pos, stations[j].pos);
    }
    return res;
}

// initialization of parameters
void initObjects(int stationNum, int carNum)
{
    stations.clear(); cars.clear(); DM.clear(); 
    // stations
    for (int i = 0; i < stationNum; i++)
    {
        Station s;
        s.id = i;
        s.cargo = randarea(cargoLowerBound, 100);
        s.pos = {randarea(-area, area), randarea(-area, area)};
        stations.push_back(s);
    }
    // vehicles
    for (int i = 0; i < carNum; i++)
    {
        Car c;
        c.pos = depot;
        c.cargo = carLoad;
        c.status = 1;
        cars.push_back(c);
    }
    // distance matrix
    DM = Build_Distance_Matrix(stations);
}
