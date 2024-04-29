#include <M5Core2.h>
#define BLUE1 0x3366ff
#define BLUE2 0x3333ff
#define BLUE3 0x000099
#define BLUE4 0x000066

#define FRAME 100 //ループの長さ(ns)
#define BCNT 5 //バンパーの作用時間(FRAME)
#define GCNT 21 //ゴールカウント(FRAME)

#define STAGENUM 4

int ballX, ballY;
int speedX, speedY;
int map_now[7][10]; // 0=nothing, 1=block, 2=bumper, 3=goal, 4=hole, 5=ball

//ステージの配列
int map_array[4][7][10] = {
    {
      {4, 4, 4, 4, 4, 4, 4, 4, 4, 4},
      {4, 0, 0, 0, 0, 0, 0, 0, 0, 4},
      {4, 0, 0, 0, 0, 0, 0, 0, 0, 4},
      {4, 0, 3, 0, 1, 0, 5, 0, 0, 4},
      {4, 0, 0, 0, 0, 0, 0, 0, 0, 4},
      {4, 0, 0, 0, 0, 0, 0, 0, 0, 4},
      {4, 4, 4, 4, 4, 4, 4, 4, 4, 4}  //stage1
    },
    {
      {4, 4, 4, 4, 4, 4, 4, 4, 4, 4},
      {4, 0, 0, 0, 0, 0, 0, 0, 0, 4},
      {4, 0, 0, 0, 0, 0, 0, 0, 1, 4},
      {4, 0, 3, 0, 2, 0, 5, 0, 1, 4},
      {4, 0, 0, 0, 0, 0, 0, 0, 1, 4},
      {4, 0, 0, 0, 0, 0, 0, 0, 0, 4},
      {4, 4, 4, 4, 4, 4, 4, 4, 4, 4}  //stage2
    },
    {
      {4, 4, 4, 4, 4, 4, 4, 4, 4, 4},
      {4, 4, 4, 4, 4, 4, 1, 1, 0, 4},
      {4, 0, 0, 0, 4, 4, 0, 5, 1, 4},
      {4, 0, 3, 0, 0, 0, 0, 0, 1, 4},
      {4, 0, 0, 0, 4, 4, 0, 0, 1, 4},
      {4, 4, 4, 4, 4, 4, 1, 1, 0, 4},
      {4, 4, 4, 4, 4, 4, 4, 4, 4, 4}  //stage3
    },
    {
      {4, 4, 4, 4, 4, 4, 4, 4, 4, 4},
      {4, 0, 0, 0, 0, 0, 0, 2, 0, 4},
      {4, 0, 2, 4, 4, 4, 0, 0, 0, 4},
      {4, 0, 1, 0, 0, 0, 1, 0, 5, 4},
      {4, 0, 0, 0, 1, 0, 0, 1, 1, 4},
      {4, 0, 0, 2, 1, 0, 0, 0, 3, 4},
      {4, 4, 4, 4, 4, 4, 4, 4, 4, 4}  //stage4
    }
};

int mapnum = 0;

int titleflag = 1;
int dropflag = 0;
int bumperflag = 0;
int bumpercnt = 0;
int goalflag = 0;
int goalcnt = 0;
int startflag = 1;

int goalposX = 0;
int goalposY = 0;

void setmap(){
  // マップのコピー
  int i, j;
  for (i=0; i < 7; ++i) {
      for (j=0; j < 10; ++j) {
          map_now[i][j] = map_array[mapnum][i][j];
      }
  }
}

void printBall(int x, int y){
  M5.Lcd.fillCircle(x, y, 12, 0xfac4);
  M5.Lcd.fillCircle(x, y, 10, 0xfae4);
  M5.Lcd.fillCircle(x-2, y-2, 9, 0xfb44);
  M5.Lcd.fillRect(x-5, y-6, 3, 3, 0xfb85);
}

void printBallDrop(int x, int y, int n){

  if(n == 0){
    M5.Lcd.fillCircle(x, y, 12, TFT_BLACK);
    printMap();
    M5.Lcd.fillCircle(x, y, 9, 0xaaa7);
    M5.Lcd.fillCircle(x-2, y-2, 7, 0xa2e7);
    M5.Lcd.fillRect(x-5, y-6, 2, 2, 0xab08);
  }
  if(n == 1){
    M5.Lcd.fillCircle(x, y, 9, TFT_BLACK);
    printMap();
    M5.Lcd.fillCircle(x, y, 7, 0x6a47);
    M5.Lcd.fillCircle(x-1, y-1, 5, 0x6a47);
    M5.Lcd.fillRect(x-4, y-3, 2, 2, 0x6a67);
  }
  if(n == 2){
    M5.Lcd.fillCircle(x, y, 9, TFT_BLACK);
    printMap();
  }
}

void printBlock(int x, int y){
  M5.Lcd.fillRect(x, y, 32, 32, 0xb5b6);
  M5.Lcd.drawRect(x, y, 32, 32, TFT_BLACK);
  M5.Lcd.fillRect(x, y, 30, 30, 0x3186);
}

void printBumper(int x, int y){
  M5.Lcd.fillRect(x, y, 32, 32, 0xb5b6); //床
  M5.Lcd.fillCircle(x+5, y+5, 5, 0x81fd);
  M5.Lcd.fillCircle(x+27, y+5, 5, 0x81fd);
  M5.Lcd.fillCircle(x+27, y+27, 5, 0x81fd);
  M5.Lcd.fillCircle(x+5, y+27, 5, 0x81fd);
  M5.Lcd.fillRect(x+5, y, 22, 32, 0x81fd); 
  M5.Lcd.fillRect(x, y+5, 32, 22, 0x81fd); 

  M5.Lcd.fillCircle(x+6, y+6, 4, 0x81b9);
  M5.Lcd.fillCircle(x+26, y+6, 4, 0x81b9);
  M5.Lcd.fillCircle(x+26, y+26, 4, 0x81b9);
  M5.Lcd.fillCircle(x+6, y+26, 4, 0x81b9);
  M5.Lcd.fillRect(x+6, y, 20, 30, 0x81b9); 
  M5.Lcd.fillRect(x, y+6, 30, 20, 0x81b9); 

  M5.Lcd.fillCircle(x+10, y+10, 4, 0x4248);
  M5.Lcd.fillCircle(x+22, y+10, 4, 0x4248);
  M5.Lcd.fillCircle(x+22, y+22, 4, 0x4248);
  M5.Lcd.fillCircle(x+10, y+22, 4, 0x4248);
  M5.Lcd.fillRect(x+10, y+6, 12, 20, 0x4248); 
  M5.Lcd.fillRect(x+6, y+10, 20, 12, 0x4248); 

  M5.Lcd.fillCircle(x+10, y+10, 2, 0x2965);
  M5.Lcd.fillCircle(x+22, y+10, 2, 0x2965);
  M5.Lcd.fillCircle(x+22, y+22, 2, 0x2965);
  M5.Lcd.fillCircle(x+10, y+22, 2, 0x2965);
  M5.Lcd.fillRect(x+10, y+8, 12, 16, 0x2965); 
  M5.Lcd.fillRect(x+8, y+10, 16, 12, 0x2965); 
}

void printGoal(int x, int y){

  if(goalcnt==0){
    M5.Lcd.fillRect(x, y, 32, 32, 0xb5b6);
    M5.Lcd.fillCircle(x+16, y+16, 16, 0xfac4);
    M5.Lcd.fillCircle(x+16, y+16, 14, 0xfae4);
    M5.Lcd.fillRect(x+14, y, 4, 32, 0xb5b6);
    M5.Lcd.fillRect(x, y+14, 32, 4, 0xb5b6);
    M5.Lcd.fillCircle(x+16, y+16, 13, 0xb5b6);
  }
  else{
    M5.Lcd.fillRect(x, y, 32, 32, 0xb5b6); //床
    M5.Lcd.fillCircle(x+16, y+16, 16, 0x26ff);
    M5.Lcd.fillCircle(x+16, y+16, 14, 0x269f);
    M5.Lcd.fillRect(x+14, y, 4, 32, 0xb5b6);
    M5.Lcd.fillRect(x, y+14, 32, 4, 0xb5b6);
    M5.Lcd.fillCircle(x+16, y+16, 13, 0xb5b6);
  }

}

void printFloor(int x, int y){
  M5.Lcd.fillRect(x, y, 32, 32, 0xb5b6);
}

void printHole(int x, int y){
  M5.Lcd.fillRect(x, y, 32, 32, TFT_BLACK);
}

void printMap(){
  int i, j;
  int x, y; //座標
  // i=行, j=列
  for(i=0; i<7; i++){
    for(j=0; j<10; j++){
        x = 32*j;
        y = 8+32*i;

        switch(map_now[i][j]) {
          case 0:
            printFloor(x, y);
            break;
          case 1:
            printBlock(x, y);
            break;
          case 2:
            printBumper(x, y);
            break;
          case 3:
            printGoal(x, y);
            goalposX = x;
            goalposY = y;
            break;
          case 4:
            printHole(x, y);
            break;
          case 5:
            if(startflag){
              printFloor(x, y);
              ballX = x+16;
              ballY = y+16;
              startflag = 0;
            }
            else{
              printFloor(x, y);
            }
        }

    }
  }
}

int getGrid(int x, int y){
  int gridx, gridy;
  gridx = x/32;
  gridy = (y-8)/32;

  return gridy*10 + gridx;
}

int in_grid(int i, int j){
  if(((ballX > 32*j-16) && (32*j+48 > ballX)) && ((ballY > 32*i-8) && (32*i+56 > ballY))) return 1;
  else return 0;
}


void setState(int x, int y){
  int tempballX, tempballY, i;
  int w, l; //バンパーの方向w:横 l:縦

  // 簡単なやつ
  if(x > 15) x=15;
  else if(x < -15) x=-15;
  if(y > 15) y=15;
  else if(y < -15) y=-15;

  speedX += x/3;
  speedY += y/3;

  if(bumperflag == 0){
    tempballX = ballX + (x/3)*4;
    tempballY = ballY + (y/3)*4;
  }
  else{

    if(bumperflag == 1 || bumperflag == 2 || bumperflag == 5) w = -1;
    if(bumperflag == 3 || bumperflag == 4 || bumperflag == 6) w = 1;
    else w = 0;

    if(bumperflag == 1 || bumperflag == 3 || bumperflag == 7) l = -1;
    if(bumperflag == 2 || bumperflag == 4 || bumperflag == 8) l = 1;
    else l = 0;  

    if(w!=0 && l!= 0){
      tempballX = ballX + w*9;
      tempballY = ballY + l*9;
    }
    else{
      tempballX = ballX + w*13;
      tempballY = ballY + l*13;
    }

    bumpercnt -= 1;
    if(bumpercnt == 0) bumperflag = 0;
  }

  int grid, gridX, gridY; // 現在のグリッド
  int centerGridPosX, centerGridPosY;

  grid = getGrid(ballX, ballY);
  gridX = grid%10;
  gridY = grid/10;
  centerGridPosX = gridX*32 + 16;
  centerGridPosY = gridY*32 + 16 + 8;


  if(map_now[gridY][gridX] == 4) dropflag = 1; //穴に落ちたか判定

  if(map_now[gridY][gridX] == 3) goalcnt += 1; //ゴール上か判定
  else goalcnt = 0;

  if(goalcnt >= GCNT) goalflag = 1;

  ballX = tempballX;
  ballY = tempballY;

  // 壁の処理
  // 右下
  if(map_now[gridY+1][gridX+1] == 1 && in_grid(gridY+1, gridX+1)){
    ballX = centerGridPosX + 4;
    ballY = centerGridPosY + 4;
  }
  // 右上
  else if(map_now[gridY-1][gridX+1] == 1 && in_grid(gridY-1, gridX+1)){
    ballX = centerGridPosX + 4;
    ballY = centerGridPosY - 4;
  }
  // 左下
  else if(map_now[gridY+1][gridX-1] == 1 && in_grid(gridY+1, gridX-1)){
    ballX = centerGridPosX - 4;
    ballY = centerGridPosY + 4;
  }
  // 左上
  else if(map_now[gridY-1][gridX-1] == 1 && in_grid(gridY-1, gridX-1)){
    ballX = centerGridPosX - 4;
    ballY = centerGridPosY - 4;
  }

  // 右
  if(map_now[gridY][gridX+1] == 1 && in_grid(gridY, gridX+1)){
    ballX = centerGridPosX + 4;
  }
  // 左
  else if(map_now[gridY][gridX-1] == 1 && in_grid(gridY, gridX-1)){
    ballX = centerGridPosX - 4;
  }
  // 下
  if(map_now[gridY+1][gridX] == 1 && in_grid(gridY+1, gridX)){
    ballY = centerGridPosY + 4;
  }
  // 上
  else if(map_now[gridY-1][gridX] == 1 && in_grid(gridY-1, gridX)){
    ballY = centerGridPosY - 4;
  }

  // バンパーの処理
  // 右下
  if(map_now[gridY+1][gridX+1] == 2 && in_grid(gridY+1, gridX+1)){
    bumperflag = 1;
    bumpercnt = BCNT;
  }
  // 右上
  else if(map_now[gridY-1][gridX+1] == 2 && in_grid(gridY-1, gridX+1)){
    bumperflag = 2;
    bumpercnt = BCNT;
  }
  // 左下
  else if(map_now[gridY+1][gridX-1] == 2 && in_grid(gridY+1, gridX-1)){
    bumperflag = 3;
    bumpercnt = BCNT;
  }
  // 左上
  else if(map_now[gridY-1][gridX-1] == 2 && in_grid(gridY-1, gridX-1)){
    bumperflag = 4;
    bumpercnt = BCNT;
  }

  // 右
  if(map_now[gridY][gridX+1] == 2 && in_grid(gridY, gridX+1)){
    bumperflag = 5;
    bumpercnt = BCNT;
  }
  // 左
  else if(map_now[gridY][gridX-1] == 2 && in_grid(gridY, gridX-1)){
    bumperflag = 6;
    bumpercnt = BCNT;
  }
  // 下
  if(map_now[gridY+1][gridX] == 2 && in_grid(gridY+1, gridX)){
    bumperflag = 7;
    bumpercnt = BCNT;
  }
  // 上
  else if(map_now[gridY-1][gridX] == 2 && in_grid(gridY-1, gridX)){
    bumperflag = 8;
    bumpercnt = BCNT;
  }

  if(goalcnt != 0){
    M5.Lcd.setCursor(5, 8); // 表示位置を設定
    M5.Lcd.print("count: ");
    M5.Lcd.print(3 - goalcnt/7);
  }

  // 範囲を超えない(画面サイズ320*240, ボール半径12)
  if(ballX > 320) ballX = 320;
  if(ballX < 0) ballX = 0;

  if(ballY > 240) ballY = 240;
  if(ballY < 0) ballY = 0;

}

void dropHole(){
  int i;
  for(i=0; i<3; i++){
    printBallDrop(ballX, ballY, i);
    delay(500);
  }
  delay(3000);
  dropflag = 0;
  startflag = 1;
}

void goal(){
  M5.Lcd.setCursor(goalposX-10, goalposY-16); // 表示位置を設定
  M5.Lcd.print("goal!");
  mapnum += 1;
  if(mapnum < STAGENUM) setmap();
  else{ //クリア時処理
    M5.Lcd.setCursor(10, 120); // 表示位置を設定
    M5.Lcd.fillScreen(TFT_WHITE); // 背景色設定
    M5.Lcd.setTextColor(TFT_BLACK);
    M5.Lcd.print("Thank you for playing!");
    titleflag = 1;
    mapnum = 0;
    startflag = 1;
    goalflag = 0;
    delay(10000);
    M5.Lcd.fillScreen(TFT_BLACK); // 背景色設定
    setmap();
    return;
  }      
  startflag = 1;
  goalflag = 0;
  delay(4000);
  M5.Lcd.fillScreen(TFT_BLACK); // 背景色設定
}

void setup() {
  M5.begin();
  M5.Lcd.setRotation(1); // ディスプレイの向きを調整
  M5.Lcd.setTextSize(2); // テキストサイズを設定
  M5.IMU.Init(); // IMUの初期化
  setmap();
}

void loop() {

  while(titleflag){
      M5.Lcd.fillScreen(TFT_BLACK); // 背景色設定
      M5.Lcd.setTextColor(TFT_RED);
      M5.update();
      titleflag = !M5.BtnA.isPressed(); 
      M5.Lcd.setCursor(100, 120); // 表示位置を設定
      M5.Lcd.print("Press button A.");
      delay(100); 
  }

  printMap();// マップ描画

  // ピッチ角とロール角を取得
  float pitch, roll, z;
  M5.IMU.getAhrsData(&pitch, &roll, &z);
  setState(int(pitch), int(roll)); //位置更新
  printBall(ballX, ballY);
  
  // 穴に落ちた時の処理
  if(dropflag != 0) dropHole();

  // ゴールした時の処理
  if(goalflag != 0) goal();

  delay(FRAME);
}
