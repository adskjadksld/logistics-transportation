#include <vector>
#include <algorithm>
#include <bits/stdc++.h>
using namespace std;

vector<int> modifyRoute(vector<int> route);

vector<int> genInitRoute(Car car, vector<Station> stations)
{
    vector<int> route;
    vector<int> tmp = car.scope;

    // 在该车负责的站点中找到距离该车最近的站点
    int minInd = 0;
    double minDist = distance(stations[tmp[minInd]].pos, car.pos);
    for (int i = 1; i < car.scope.size(); i++)
    {
        double newDist = distance(stations[tmp[i]].pos, car.pos);
        if (newDist < minDist) { minInd = i; minDist = newDist; }
    }
    route.push_back(tmp[minInd]);
    
    // 除了距离depot最近的点之外，其他的点随机排列放入route中
    tmp.erase(tmp.begin() + minInd);
    random_shuffle(tmp.begin(), tmp.end());
    route.insert(route.end(), tmp.begin(), tmp.end());
    route.push_back(route[0]);

    // 使用HH对当前的路径进行优化
    route = modifyRoute(route);
    
    return route;
}


// 使用pareto前沿来优化路径
bool sortByLen(const tuple<vector<int>, double, double>& a, const tuple<vector<int>, double, double>& b) {return (get<1>(a) < get<1>(b));}
double lenOfRoute(vector<int> route) // 计算路径的长度
{
    double res = 0;
    for (int i = 0; i < route.size() - 1; i++) res += DM[stations[route[i]].id][stations[route[i+1]].id]; 
    return res;
}
double visitOfRoute(vector<int> route) // 计算路径中缺货站点优先访问程度
{
    double res = 0;
    for (int i = 1; i < route.size() - 1; i++) res += (100 - stations[route[i]].cargo) * i; 
    return res;
}
vector<int> modifyRoute(vector<int> route)
{
    vector<int> res = route; double len = lenOfRoute(res); double vis = visitOfRoute(route);
    vector<tuple<vector<int>, double, double>> routes; // (路径，长度，优先访问程度)元组
    routes.push_back(make_tuple(res, len, vis));

    for (int i = 0; i < 50; i++) // 10次迭代
    {
        int p = randareaInt(1, res.size() - 2);
        for (int j = 1; j < res.size() - 1; j++) // 遍历性子段逆置
            if (j != p)
            {
                // 计算子段逆置后的路径及其长度与优先访问程度
                vector<int> tmp = res;
                if (j < p) reverse(tmp.begin() + j, tmp.begin() + p + 1);
                else reverse(tmp.begin() + p, tmp.begin() + j + 1);
                double l = lenOfRoute(tmp), v = visitOfRoute(tmp);
                // 在routes中寻找，如果有元素支配该路径则丢弃，如果支配某元素则替换某元素，
                // 否则按照长度从小到大的顺序插入到合适的位置
                int flag = 1;
                for (int k = 0; k < routes.size(); k++)
                    if (l >= get<1>(routes[k]) && v >= get<2>(routes[k])) { flag = 0; break;}
                    else if (l <= get<1>(routes[k]) && v <= get<2>(routes[k])) { routes.erase(routes.begin() + k); k--;}
                if (flag) routes.insert(routes.end(), make_tuple(tmp, l, v));
            }
        sort(routes.begin(), routes.end(), sortByLen);
        
        vector<double> tmp;
        for (int i = 0; i < routes.size(); i++)
            tmp.push_back(get<1>(routes[i])/get<1>(routes[routes.size() - 1]) * get<2>(routes[i])/get<2>(routes[0]));
        res = get<0>(routes[index(tmp, min(tmp))]);
    }
    return res;
}