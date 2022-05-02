//Nearest Neighbor + random更新 + 描画ファイル出力(gnuplot用) tsp
# include<iostream>
# include<fstream>
# include<sstream>
# include<string>
# include<cmath>
# include<vector>
# include<random>
#include <chrono>
# define N 1000000
using namespace std;
// 最低限の実時間(ms)計測用
struct Stopwatch {
    std::chrono::system_clock::time_point s, e;
    std::vector<double> lap; // ラップタイム

    // 再生
    void start() {
        s = std::chrono::system_clock::now();
    }

    // 計測終了, ミリ秒を返す
    double stop() {
        e = std::chrono::system_clock::now();
        return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count());
    }

    // 計測時間を返す(stop後でないと無意味)
    double time() const {
        return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count());
    }

    // 経過時間をラップタイムとして保存 & 再スタート
    void section() {
        e = std::chrono::system_clock::now();
        lap.emplace_back(static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count()));
        s = std::move(e);
    }

    // 計測時間・ラップタイムのクリア
    void clear() {
        s = std::chrono::system_clock::now();
        e = std::chrono::system_clock::now();
        lap.clear();
    }
};
struct point{
  int x;
  int y;
  bool t;         //点が選ばれたらfalse
  int next;       //移動先の点の添字
  int bef;        //移動前の点の添字
  //double edge;    //次の点までの距離を保存
};
void split(string tmp[], string &str){ //文字列をスペース区切りにする関数
  istringstream is(str);
  string s;
  int i=0;
  while(getline(is, s, ' ')){
   // cout << s <<" a";
    if(s=="") { s=""; continue;}
    tmp[i] = s;
   // cout << tmp[i] << "\n";
    i++;
   // cout << i << "\n";
  }
}
double distance(point a, point b){ //ユークリッド距離を計算
  return sqrt((b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y));
}
void nearest_point(int idx, vector<point> &P){ //点(添字idx)からユークリッド距離が一番近い点を検出
  int near_idx = 0;
  double min = -1;
  for(int i=0; i<P.size(); i++){
    if(P[i].t){
      double l = distance(P[idx], P[i]);
      if(min == -1 || min > l){
        min = l;
        near_idx = i;
      }
      //cout << min <<"\n";
    }
  }
  P[idx].next = near_idx;
  P[near_idx].t = false;
  //P[idx].edge = min;
  return;
}
double sum_distance(vector<point> p, int a, int b){  //点（添字）aからbまでの距離の総和を算出
  double sum = 0;
  int idx = a;
  while(true){
    sum += distance(p[idx], p[p[idx].next]);
    idx = p[idx].next;
    if(idx == b) break;
  }
  return sum;
}
void reverse(vector<point>& p, int a, int b){  //任意の2点間の順序を逆転
  int a_bef = p[a].bef;
  int b_nex = p[b].next;
  int idx = a;
  while(true){
    int tmp = p[idx].next;
    p[idx].next = p[idx].bef;
    p[idx].bef = tmp;
    idx = tmp;
    if(idx == b_nex){
      p[b].bef = a_bef;
      p[a].next = b_nex;
      p[a_bef].next = b;
      p[b_nex].bef = a;
      break;
    }
  }
  return;
}
void print(vector<point> p, int a, int b){  //任意の2点間の点を表示
  int idx = a;
  int end = b;
  int cnt = 0;
  while(true){
    cout << setw(3) << idx <<" ("<< setw(4) << p[idx].x <<","<< setw(5) << p[idx].y <<")\n";
    idx = p[idx].next;
    cnt++;
    if(idx == end){
      cout <<"Total vertex: "<< cnt <<"\n";
      break;
    }
    if(cnt > p.size()){
      cout <<"error\n";
      break;
    }
  }
  return;
}

int main(int arg, char *argv[]){
  vector<point> P;       //入力点(x,y)
  int cnt = 0;           //点数
//ファイル読み込み///////////////////////////////////////////////
  ifstream data_file(argv[1]);
  string str;
  int k=0;
  while(getline(data_file, str)){
    if(k > 5){
      string tmp[3];
      split(tmp, str);
      if(tmp[0] == "EOF") break;
      P.push_back({stoi(tmp[1]), stoi(tmp[2]), true, 0, 0});
      cnt++;
    }
    k++;
    //cout << cnt << "読み込み\n";
  }
/*  for(auto p: P){
    cout<< p.x <<"  "<< p.y <<"\n";
  }*/

//Nearest Neighbor法////////////////////////////////////////////
  Stopwatch time;
  time.start();
  P[0].t = false;
  int idx = 0;
  cnt--;
  while(true){
    if(cnt == 0) break;
    nearest_point(idx, P);
    P[P[idx].next].bef = idx;
//    cout << P[idx].next <<"\n";
    idx = P[idx].next;
    cnt--;
  }
  P[idx].next = 0;
  P[0].bef = idx;

  double sum = sum_distance(P, 0, 0);
  cout <<"Total distance:  "<< sum <<"\n";
  cout << "time " <<time.stop()<<"\n";
//  print(P, 0, 0);
//解の改善/////////////////////////////////////////////////////
  random_device random_a;
  auto num = random_a();
  mt19937 random_b;
//  num = 2940408078;
  random_b.seed(num);
  cout <<"ランダムシード値：　"<< num << "\n";

  int score = 0;
  while(true){
    vector<point> C = P;
    int a = (int)random_b() % P.size();
    int b = (int)random_b() % P.size();
    while(a == b) b = (int)random_b() % P.size();
    if(P[b].next == a){
     int tmp = a;
     a = b;
     b = tmp;
   }
//    cout << a <<"     "<< b <<"\n";
    reverse(C, a, b);
    double old_ = distance(P[P[a].bef], P[a]) + distance(P[b], P[P[b].next]);
    double new_ = distance(C[C[b].bef], C[b]) + distance(C[a], C[C[a].next]);
    if(old_ > new_){
      cout <<"　記録更新：　"<< old_ - new_ <<"短くなった\n";
      P = C;
    }
    score++;
    if(score == N) break;
  }

  sum = sum_distance(P, 0, 0);
  cout <<"New total distance:  "<< sum <<"\n";
//ファイル出力////////////////////////////////////////////////
  ofstream output_file("tsp.dat");
  idx = 0;
  int x_max = 0;
  int y_max = 0;
  int x_min = -1;
  int y_min = -1;
  while(true){
    output_file << P[idx].x <<" "<< P[idx].y <<"\n";
    idx = P[idx].next;
    if(x_max < P[idx].x) x_max = P[idx].x;
    if(y_max < P[idx].y) y_max = P[idx].y;
    if(x_min > P[idx].x || x_min == -1) x_min = P[idx].x;
    if(y_min > P[idx].y || y_min == -1) y_min = P[idx].y;
    if(idx == 0) break;
  }
  output_file << P[0].x <<" "<< P[0].y <<"\n";

  ofstream output_file2("tsp.plot");
  output_file2 <<"set xrange[0:"<< x_max+x_min <<"]\n";
  output_file2 <<"set yrange[0:"<< y_max+y_min <<"]\n";
  output_file2 <<"plot \"tsp.dat\" using 1:2 with lines\n";
  return 0;
}
/*実行結果
ランダムに2点を選び2点間の順番を逆転させる．

Total distance:  152494
ランダムシード値：　2928249131
　記録更新：　209.034短くなった
　記録更新：　1.09715短くなった
　記録更新：　77.5649短くなった
　記録更新：　292.893短くなった
　記録更新：　84.5241短くなった
　記録更新：　292.893短くなった
　記録更新：　292.893短くなった
　記録更新：　292.893短くなった
　記録更新：　473.798短くなった
　記録更新：　100.311短くなった
　記録更新：　178.643短くなった
　記録更新：　361.794短くなった
　記録更新：　292.893短くなった
　記録更新：　381.966短くなった
　記録更新：　4525.25短くなった
　記録更新：　175.978短くなった
　記録更新：　5541.38短くなった
　記録更新：　348.799短くなった
New total distance:  138569

Total distance:  152494
ランダムシード値：　2940408078
　記録更新：　292.893短くなった
　記録更新：　591.822短くなった
　記録更新：　731.34短くなった
　記録更新：　299.123短くなった
　記録更新：　85.4434短くなった
　記録更新：　1186.31短くなった
　記録更新：　721.637短くなった
　記録更新：　840.202短くなった
　記録更新：　292.893短くなった
　記録更新：　661.502短くなった
　記録更新：　292.893短くなった
　記録更新：　520.851短くなった
　記録更新：　151.987短くなった
　記録更新：　100.311短くなった
　記録更新：　19.586短くなった
　記録更新：　5520.8短くなった
　記録更新：　450.49短くなった
　記録更新：　1394.45短くなった
　記録更新：　266.874短くなった
　記録更新：　292.893短くなった
　記録更新：　292.893短くなった
　記録更新：　292.893短くなった
　記録更新：　581.139短くなった
　記録更新：　292.893短くなった
　記録更新：　211.103短くなった
　記録更新：　130.616短くなった
　記録更新：　292.893短くなった
　記録更新：　1000短くなった
　記録更新：　189.384短くなった
　記録更新：　1385.16短くなった
　記録更新：　328.003短くなった
　記録更新：　963.105短くなった
　記録更新：　598.207短くなった
　記録更新：　1236.07短くなった
New total distance:  129985

/tsp3 att48.tsp
Total distance:  40526.4
ランダムシード値：　1370187327
　記録更新：　762.933短くなった
　記録更新：　8.95973短くなった
　記録更新：　437.367短くなった
　記録更新：　504.8短くなった
　記録更新：　69.6833短くなった
　記録更新：　1076.41短くなった
　記録更新：　258.028短くなった
　記録更新：　1006.22短くなった
　記録更新：　967.821短くなった
　記録更新：　243.374短くなった
　記録更新：　198.901短くなった
　記録更新：　19.3746短くなった
　記録更新：　423.386短くなった
New total distance:  34549.2

*/ 
