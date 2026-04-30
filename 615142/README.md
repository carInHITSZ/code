# 615142 — 自主移动机器人 AI 系统

**第七届人工智能算法精英大赛 · 全国一等奖**

---

## 项目概述

基于 ROS (Robot Operating System) 的自主移动机器人系统，能够在 3.6m × 3.6m 复杂场景中自主导航，融合**云端视觉语言大模型 (VLM)**、**SLAM 定位建图**、**多传感器融合**和**语音交互**，完成多阶段智能任务挑战。

## 硬件平台

- **底盘**: 四轮差分驱动移动底盘
- **激光雷达**: 360° 2D LiDAR
- **摄像头**: USB 广角摄像头
- **IMU**: 9 轴惯性测量单元
- **主控**: Jetson / x86 工控机
- **语音**: 麦克风 + 扬声器

## 软件架构 (16 个 ROS 包)

```
src/
├── robot_slam/          # 核心竞赛逻辑
│   ├── navigation_multi_goals.py      # 多目标点自主导航
│   └── navigation_multi_goals2026.py  # BFS 路径规划增强版
├── abot_vlm/            # 视觉语言大模型集成
│   ├── vlm_node.py                    # VLM API 调用节点
│   └── API_KEY.py                     # 多平台 API 密钥
├── color_pkg/           # 颜色检测
│   ├── fire_detector.py               # HSV 火焰检测
│   └── line_follower.py               # Otsu 阈值巡线
├── tracker_pkg/         # 视觉跟踪
│   ├── kcf_kalman_tracker.py          # KCF + 卡尔曼滤波跟踪
│   ├── lk_tracker.py                  # Lucas-Kanade 光流跟踪
│   └── good_features.py               # Shi-Tomasi 角点检测
├── face_pkg/            # 人脸识别
│   └── face_rec.py                    # EigenFace 特征脸识别
├── abot_base/           # 底盘驱动层
│   ├── base_driver.cpp                # 串口电机控制 + 里程计
│   └── odom_ekf.py                    # EKF 里程计融合
├── imu_filter/          # IMU 姿态解算
│   ├── Madgwick_filter.cpp            # Madgwick AHRS
│   └── Mahony_filter.cpp              # Mahony AHRS
├── jie_ware/            # LiDAR 定位
│   └── lidar_loc.cpp                  # 激光扫描匹配定位
├── track_tag/           # AR 标签跟踪
│   └── ar_track.cpp                   # AlvarMarker 跟踪
├── robot_voice/         # 语音交互
│   ├── iat_publish.cpp                # 讯飞语音识别 (IAT)
│   ├── tts_subscribe.cpp              # 讯飞语音合成 (TTS)
│   └── voice_assistant.cpp            # 语音助手
├── shoot_cmd/           # 射击指令控制
│   └── SerialPort.cpp                 # 串口协议通信
└── cam_track/           # 摄像头跟踪
    └── cam_track_node.cpp
```

## 核心 AI 算法

### 1. 云端视觉语言大模型 (VLM)
- **豆包** (ByteDance): `doubao-1.5-vision-pro-250328` — 主力模型
- **Kimi** (Moonshot): `kimi-k2-0905-preview` — 备用模型
- **Yi-Vision** (01.AI): `yi-vision` — 第三备选
- 双图输入策略: 原始照片 + 二值化预处理图同时送入 VLM
- 三模型冗余切换保证任务可靠性

### 2. SLAM 与定位
- **Gmapping**: 基于粒子滤波的 2D LiDAR SLAM
- **Cartographer**: Google 图优化 SLAM
- **AMCL**: 自适应蒙特卡洛定位
- **自定义激光匹配定位**: 扫描-地图相关性定位

### 3. 路径规划
- **BFS 广度优先搜索**: 在 9×9 栅格图上进行最优路径搜索
- **动态障碍检测**: 实时检测墙体并更新地图
- **路径压缩**: 去除冗余共线路径点
- **PID 精确定位**: LiDAR 四面测距 + PID 对齐栅格中心

### 4. 计算机视觉
| 算法 | 应用 | 技术 |
|------|------|------|
| 火焰检测 | 识别火源目标 | HSV 颜色空间滤波 |
| 巡线 | 场地引导线跟踪 | Otsu 自适应阈值 + 最小二乘拟合 |
| 目标跟踪 | 移动物体跟踪 | KCF 核相关滤波 + Kalman Filter |
| 光流跟踪 | 特征点跟踪 | Lucas-Kanade 金字塔光流 |
| 人脸识别 | 人员检测识别 | EigenFace 特征脸 + Haar Cascade |
| AR 标签 | 二维码/标记识别 | AlvarMarker SDK |

### 5. 传感器融合
- **Madgwick AHRS**: 9 轴 IMU 姿态解算
- **Mahony AHRS**: 备用姿态解算算法
- **EKF 里程计融合**: 编码器 + IMU + LiDAR 多源融合

### 6. 语音交互
- **科大讯飞 MSC SDK**: 中文语音识别 (IAT) + 语音合成 (TTS)
- **阿里 DashScope**: `gummy-chat-v1` 语音助手
- **微软 Edge-TTS**: `zh-CN-XiaoxiaoNeural` 备用 TTS
- 唤醒词启动、线索播报、任务报告全程语音交互

## 任务流程

```
起点出发 → 自主导航 → 识别点拍照 → VLM 分析线索 
   ↓
多轮线索收集 → 解谜推理 → 确定任务区域 → 精确导航进入
   ↓
完成任务 → 终点区域 → 语音播报完成
```

## 获奖情况

- **第七届人工智能算法精英大赛**
- **全国一等奖**
- 证书：[第七届人工智能算法精英大赛国一等奖.png.pdf](../第七届人工智能算法精英大赛国一等奖.png.pdf)
