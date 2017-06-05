# StereoVision

## require: 
  * VS 2015
  * OpenCV 3.2
  
## input.yml

#### mod
操作模式
1 检测角点并保存图像到硬盘中
2 读取 stereo.yml 并显示立体匹配的效果

#### unit
主要用于判断是否为联合摄像机(智能车用双目单视频流)
若是联合摄像机则会调用另一个类来对单视频流进行预处理
通常分离式双摄像头(两个USB)设置为0

#### cameraNumberL(R)
左右摄像机的设备序号
可能跟摄像机的插入顺序有关
如果在打开时发现两个摄像机打开不对需要调换两个摄像头的设备序号
在上一步中如果设置了unit=1那么cameraNumberL即为要打开的摄像头 cameraNumberR设置为-1即可

#### frameWidth(Height)
摄像头分辨率

#### gridW(H)
角点数量的长宽

#### cutFrameNumber
要截取的图片数量