# h264-splitter
split h264 into single nalu

可以识别的分隔符：

{0x00, 0x00, 0x01}

{0x00, 0x00, 0x00, 0x01}

需要在编译时设置对应的编译参数

当前版本只适合小数据量测试使用，因为实现非常地简单粗暴，以后有时间再优化一下逻辑
