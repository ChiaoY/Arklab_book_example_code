//for table

/*
名稱：加權遞推平均濾波法(Weighted Moving Average,WMA)
方法：
    即不同時刻的數據加以不同的權,通常是越接近現時刻的數據，權取得越大。
    給予新採樣值的權係數越大，則靈敏度越高，但信號平滑度越低。
優點：
    適用於有較大純滯後時間常數的對象，和採樣週期較短的系統。
缺點：
    對於純滯後時間常數較小，採樣週期較長，變化緩慢的信號;
    不能迅速反應系統當前所受干擾的嚴重程度，濾波效果差。
*/
 
int Filter_Value;
char serInString[25];
int before_filter;

void setup() {
  Serial.begin(9600);       // 初始化序列通信
  randomSeed(analogRead(0)); // 產生隨機種籽
  //Serial.println("#S|CPTEST|[]#");
  //Serial.read(serInString,1000);
}
 
void loop() {
  Filter_Value = Filter();       // 獲得滤波器輸出值
  //Serial.print("#S|LOGTEST|[");
  Serial.print(before_filter);
  Serial.print("\t");
  Serial.println(Filter_Value,DEC); // 序列輸出
  //readSerialString(serInString,1000);

  //Serial.println("]#");
  delay(50);
}
 
// 隨機產生一個大約300的值
int Get_AD() {
  return random(290, 310);//　產生的值介於290到310之間
}
 
// 加權遞推平均濾波法
#define FILTER_N 12  //取值的個數
int coe[FILTER_N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};    // 加權係數表(越高位(越接近現時刻)加權越高)
int sum_coe = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 + 11 + 12; // 加權細數和
int filter_buf[FILTER_N + 1];
int Filter() {
  int i;
  int filter_sum = 0;
  before_filter = Get_AD(); //取得最新值
  filter_buf[FILTER_N] = before_filter;
  for(i = 0; i < FILTER_N; i++) {
    filter_buf[i] = filter_buf[i + 1]; // 所有數據左移，低位扔掉
    filter_sum += filter_buf[i] * coe[i]; //加權後的總值
  }
  filter_sum /= sum_coe;//取加權平均值,加權平均值=Σ(值*加權值)/加權後的總值
  return filter_sum;//回傳加權平均值
}
