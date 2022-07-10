#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<easyx.h>
#include<string>

using namespace std;

#define ROW 10
#define COL 10
#define IMGW 40

/**
 * @brief 扫雷游戏
 *
 * @return int
 */
void loadImages();  //加载图片
void initMap(int map[ROW][COL]);
void drawMap(int map[ROW][COL], IMAGE img[]);
void playGame(int map[ROW][COL]);
void mouseEvent(int map[ROW][COL]);
void expand(int map[ROW][COL], int row, int col);
void judge(int map[ROW][COL], int row, int col);
void openNewGame(int map[ROW][COL]);

bool isOver = false;  //点到雷结束标记
bool isWin = false;   //是否躲开所有的雷 赢了
int curOpenGridCnt = 0;  //当前已经点开的格子数
int graphWidth = 400;  //画布宽
int graphHight = 460;  //画布高
IMAGE img[13];  //定义图片数组
IMAGE face[4];//笑脸的各种状态
IMAGE bombcnt[10];  //地雷计数图片数组
IMAGE timer[10];  //时间图片数组
int numbomb1 = 0; //炸弹第一位数
int numbomb2 = 1; //炸弹第二位数
int numbomb3 = 0; //炸弹第三位数

int totalTimes = 0;//获取总时间
int time1 = 0;  //时间第一位
int time2 = 0;  //时间第二位
int time3 = 0;  //时间第三位

int runing = 1; //游戏状态。1：运行 0：结束
int faceIndex = 0;


clock_t start_t; //开始时间
clock_t end_t;  //结束时间
int first = 1;  //判断第一次点击

int mineCnt = 10; //雷的数量
int markMineCnt = 0;  //标记雷的数量(右键)

bool isClickFace = false;

void showMap(int map[ROW][COL])
{
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            printf("%2d ", map[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}




int main()
{
    //创建图形窗口
 //   initgraph(COL*IMGW, ROW*IMGW, EW_SHOWCONSOLE);
    HWND hwnd = initgraph(graphWidth, graphHight, EW_SHOWCONSOLE);
    setbkcolor(RGB(189, 189, 189));
    cleardevice();

    //设置随机种子
    srand((unsigned)time(NULL));

    //定义游戏的地图
    int map[ROW][COL] = { 0 };
    //初始化数据
    initMap(map);
    //把数据转成对应的图形界面
    

    //加载图片
    loadImages();
    
    //开始玩游戏
    playGame(map);


    getchar();

    closegraph();

    return 0;
}

void loadImages()
{
    for (int i = 0; i < 13; i++)
    {
        char fileName[50] = { 0 };
        sprintf_s(fileName, "./images/%d.jpg", i);
        loadimage(img + i, fileName, IMGW, IMGW);

    }

    //装载笑脸
    loadimage(&face[0], "./images/face01.png", 40, 40);
    loadimage(&face[1], "./images/face02.png", 40, 40);
    loadimage(&face[2], "./images/face03.png", 40, 40);
    loadimage(&face[3], "./images/face04.png", 40, 40);

    string base = "./images/1_";
    //装载炸弹数字
    for (int i = 0; i < 10; i++)
    {
        string strUrl = base + to_string(i) + ".png";

        loadimage(&bombcnt[i], strUrl.c_str(), 30, 40);

    }
    //装载计时器数字
    for (int i = 0; i < 10; i++)
    {
        string strUrl = base + to_string(i) + ".png";

        loadimage(&timer[i], strUrl.c_str(), 30, 40);

    }
}

void initMap(int map[ROW][COL])
{
    //把map清零
    memset(map, 0, ROW * COL * sizeof(int));

    //给地图里随机埋10个雷 雷用-1表示
    for (int i = 0; i < 10; )
    {
        int r = rand() % ROW;  //左闭右开区间 [0,10)
        int c = rand() % COL;
        if (map[r][c] == 0)
        {
            map[r][c] = -1;
            i++;        //只有成功设置了雷，计数才自增
        }

    }

    //雷所在的九宫格所有的格子的数值加+ (雷除外)
    for (int i = 0; i < ROW; i++)
    {
        for (int k = 0; k < COL; k++)
        {
            if (map[i][k] == -1)
            {
                //遍历雷所在九宫格
                for (int r = i - 1; r <= i + 1; r++)
                {
                    for (int c = k - 1; c <= k + 1; c++)
                    {
                        //注意数组越界问题
                        if ((r >= 0 && r < ROW && c >= 0 && c < COL) && map[r][c] != -1)
                        {
                            map[r][c]++;
                        }
                    }
                }
            }
        }
    }

    //把所有的格子都用遮罩图盖起来，给每个格子都加密，比如都加上20
    for(int i = 0; i < ROW; i++)
    {
        for (int k = 0; k < COL; k++)
        {
            map[i][k] += 20;
        }
    }


}

void drawMap(int map[ROW][COL], IMAGE img[])
{
    //中间笑脸图片
    putimage(graphWidth / 2 - 20, 10, &face[faceIndex]);
    //炸弹数量图片
    putimage(10, 10, &bombcnt[numbomb1]);
    putimage(40, 10, &bombcnt[numbomb2]);
    putimage(70, 10, &bombcnt[numbomb3]);
    //计时器图片
    putimage(graphWidth - 100, 10, &timer[time1]);
    putimage(graphWidth - 70, 10, &timer[time2]);
    putimage(graphWidth - 40, 10, &timer[time3]);
    

    //把数组里面的数据，转换成对应的图片，贴到图形窗口上面
    for (int i = 0; i < ROW; i++)
    {

        for (int k = 0; k < COL; k++)
        {
            
            //空白格子和数字
            if (map[i][k] >= 0 && map[i][k] <= 8)
            {
                putimage(k * IMGW, i * IMGW + 60, img + map[i][k]);
            }
            else if (map[i][k] == -1)
            {
                //贴雷图，第9张是地雷
                putimage(k * IMGW, i * IMGW + 60, img + 9);
            }
            else if (map[i][k] >= 19 && map[i][k] <= 28)
            {
                //贴遮罩图
                putimage(k * IMGW, i * IMGW + 60, img + 10);
            }
            else if (map[i][k] == -100)
            {
                //贴红雷图，第12张是红雷
                putimage(k * IMGW, i * IMGW + 60, img + 12);
            }
            else if (map[i][k] >= 39 && map[i][k] <= 48)
            {
                //插镖旗
                putimage(k * IMGW, i * IMGW + 60, img + 11);
            }
        }
    }
}

void playGame(int map[ROW][COL])
{
    while (true)
    {
        //鼠标事件
        mouseEvent(map);

        //绘制地图
        drawMap(map, img);

        if (isOver)
        {
            int ret = MessageBox(GetHWnd(), "你踩到雷了，再来一把？", "hint", MB_OKCANCEL);
            if (ret == IDOK)
            {
                openNewGame(map);
            }
            else if (ret == IDCANCEL)
            {
                exit(1);   //退出
            }
        }

        //判断游戏是否赢了
        if (isWin)
        {
            int ret = MessageBox(GetHWnd(), "恭喜，你赢了。再来一局？", "hint", MB_OKCANCEL);

            if (ret == IDOK)
            {
                openNewGame(map);
            }
            else
            {
                exit(1);
            }
        }

        //计时器显示
        end_t = clock();
        int second = (end_t - start_t) / 1000;
        if (runing == 1 && first == 0)
        {
            time1 = second / 100;
            time2 = second / 10 % 10;
            time3 = second % 10;
        }
        else
        {
            time1 = 0;
            time2 = 0;
            time3 = 0;
        }



    }
}

void mouseEvent(int map[ROW][COL])
{
    //定义消息结构体(鼠标消息，按键消息，字符消息)
    ExMessage msg;
    //如果有消息就会返回真，并且给msg设置值
    if (peekmessage(&msg, EM_MOUSE))
    {
        //把鼠标的坐标转成数组的下标
        int r = (msg.y - 60) / IMGW;
        int c = msg.x / IMGW;


        switch (msg.message)
        {
        case WM_LBUTTONDOWN:  //鼠标左键按下
            if (msg.x >= 0 && msg.x <= graphWidth && msg.y >= 60 && msg.y <= graphHight && runing )
            {
                if (first == 1)//判读第一次点击开始计时
                {
                    start_t = clock();
                    first = 0;
                }

                if (map[r][c] >= 19 && map[r][c] <= 28)
                {
                    map[r][c] -= 20;
                    if (map[r][c] != -1)
                        curOpenGridCnt++;

                    expand(map, r, c);
                    judge(map, r, c);
                    showMap(map);


                }
            }
            else if (msg.x >= graphWidth / 2 - 20 && msg.x <= graphWidth / 2 + 20 && msg.y >= 20 && msg.y <= 50)
            {
                //点击了笑脸
                faceIndex = 1;     
                isClickFace = true;
               
            }
            

            break;

        case WM_LBUTTONUP:   //鼠标左键抬起
            if (runing == 1 && !isOver)
            { 
                faceIndex = 0;  //除了输赢和点击否则都是笑脸
            }

            if (isClickFace) {
                isClickFace = false;
                openNewGame(map);  //点击笑脸 重开一局
            }
            
            
            break;

        case WM_RBUTTONDOWN:  //鼠标右键按下
            if (msg.x >= 0 && msg.x <= graphWidth && msg.y >= 60 && msg.y <= graphHight && runing == 1)
            {
                if (first == 1)
                {
                    start_t = clock();
                    first = 0;
                }

                if (map[r][c] <= 28 && map[r][c] >= 19)
                {
                    map[r][c] += 20;
                    markMineCnt += 1;

                }
                else if (map[r][c] <= 48 && map[r][c] >= 39)
                {
                    map[r][c] -= 20;
                    markMineCnt -= 1;
                }

                //左侧顶部雷数显示
                
                if(runing == 1 && markMineCnt <= mineCnt && markMineCnt >= 0)
                {
                    int curMineCnt = mineCnt - markMineCnt;
                    numbomb1 = curMineCnt / 100;
                    numbomb2 = curMineCnt / 10 % 10;
                    numbomb3 = curMineCnt % 10;
                }
                else
                { 
                    numbomb1 = 0;
                    numbomb2 = 0;
                    numbomb3 = 0;

                }
            }
            break;

        }
        

    }
}


void expand(int map[ROW][COL], int row, int col)
{
    //判断一下当前点击的是不是空白
    if (map[row][col] == 0)
    {

        for (int i = row - 1; i <= row + 1; i++)
        {
            for (int k = col - 1; k <= col + 1; k++)
            {
                if ((i >= 0 && i < ROW && k >= 0 && k < COL) && map[i][k] >= 19 && map[i][k] <= 28)
                {
                    if (map[i][k] > 19)
                        curOpenGridCnt++;

                    map[i][k] -= 20;

                    //递归
                    expand(map, i, k);
                }
            }
        }

       
    }
    

}

void judge(int map[ROW][COL], int row, int col)
{
    
    //如果点到了雷，就game over。点一个雷，其他的雷都显示出来
    if (map[row][col] == -1)
    {
        map[row][col] = -100;  //第一个雷要标红，特殊标记
        runing = 0; //游戏结束
        for (int i = 0; i < ROW; i++)
        {
            for (int k = 0; k < COL; k++)
            {
                if(map[i][k] == 19)  //踩雷
                { 
                    map[i][k] = -1;
                }
            }
        }

        isOver = true;
        faceIndex = 2;  //哭脸
    }

    //赢了，应该点开的格子都点开了，就赢了
    /*
    * 什么是应该点开的格子?
    * curOpenGridCnt = ROW*COL - 雷的数量
    */
    if (curOpenGridCnt == ROW * COL - 10)
    {
        
        isWin = true;
        runing = 0; 
        faceIndex = 3;

    }

    //printf("curOpenGridCnt======> %d\n", curOpenGridCnt);

    

    

}


void openNewGame(int map[ROW][COL])
{
    initMap(map);
    showMap(map);
    isOver = false;
    isWin = false;
    curOpenGridCnt = 0;
    runing = 1;
    first = 1;
    markMineCnt = 0;

    numbomb1 = mineCnt / 100;
    numbomb2 = mineCnt / 10 % 10;
    numbomb3 = mineCnt % 10;

    faceIndex = 0;

}