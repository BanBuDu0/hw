# HW

## 算法

## Tarjan

R. Tarjan, Enumeration of the elementary circuits of a directed graph, SIAM J. Comput., 2 (1973)  
Tarjan在1973年提出，最坏情况下复杂度为$o(n*e(c+1)$

## Johnson

D.B.Johnson, Finding all the elementary circuits of a directed graph, SIAM J. Comput., 4 (1975)  
Johnson在1975年提出，是对Tarjan算法的改进，改进了Tarjan算法的复杂度，最坏情况下为$o((n+e)(c+1))$，n是顶点数、e是边数，c是环数

## Hawick and James

K. A. Hawick, H. A. James. Enumerating Circuits and Loops in Graphs with Self-Arcs and Multiple-Arcs. Computational Science Technical Note CSTN-013, 2008  
Hawick and James在2008年提出，对Johnson算法的改进，消除其局限性，即能检测出指向自己的边和两个顶点之间的多条相同的边，如 $a -> a$或$a -> b, a -> b$
本实验中不需要此功能

## 大事记

### 0404

使用C++实现了Tarjan算法，在ubuntu上运行速度和java差不多。。。  
等实现了Johnson算法之后再去华为服务器上试试  
TarjanMultiThread，使用终极多线程，在小数据集上运行时间超慢，创建线程开销太大了
Johnson算法数据跑不对啊，不知道是不是我写错了

### 0406

做DFS的时候如果深度大于7就开始退栈
现在跑官方的数据可以到8ms
***在DFS的时候减少操作***
***实现一下桌面上那个johnson代码***

### 0407

把map改成unordered_map性能提升一倍。。。
**大事记**，第一次跑完了28W数据，用时3202832+2049+160 ms
                               java本地用时2529096ms

## 想法

先找出所有联通分量，再在每个连通分量上找连通图

### 优化

1. 简单函数使用内联函数
2. 不要大结构体传参
