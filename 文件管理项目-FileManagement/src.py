import time
import os
import pickle

blockSize = 512  # 每个物理块大小
blockNum = 512  # 磁盘中物理块个数

# 物理块
class Block:
    def __init__(self, blockIndex: int, data=""):
        self.blockIndex = blockIndex  # 物理块的编号
        self.data = data  # 物理块中的数据

    def write(self, newData: str):
        # 将新数据写入物理块，如果新数据超过物理块的大小，只写入前blockSize个字符
        self.data = newData[:blockSize]
        # 返回未写入的数据
        return newData[blockSize:]

    def read(self):
        return self.data

    def isFull(self):
        return len(self.data) == blockSize

    def append(self, newData: str) -> str:
        remainSpace = blockSize - len(self.data)
        if remainSpace >= len(newData):
            self.data += newData
            return ""
        else:
            # 如果物理块已满，返回无法写入的部分
            self.data += newData[:remainSpace]
            return newData[remainSpace:]

    def clear(self):
        self.data = ""

# 文件分配表
class FAT:
    def __init__(self):
        self.fat = []
        for i in range(blockNum):
            self.fat.append(-2)

    # 寻找FAT表中的一个空闲位置
    def findBlank(self):
        for i in range(blockNum):
            if self.fat[i] == -2:
                return i
        return -1

    def write(self, data, disk):
        start = -1
        cur = -1

        while data != "":
            newLoc = self.findBlank()
            if newLoc == -1:
                raise Exception(print('磁盘空间不足!'))
                return
            if cur != -1:
                self.fat[cur] = newLoc
            else:
                start = newLoc
            cur = newLoc
            data = disk[cur].write(data)
            self.fat[cur] = -1

        return start

    def delete(self, start, disk):
        if start == -1:
            return

        while self.fat[start] != -1:
            disk[start].clear()
            las = self.fat[start]
            self.fat[start] = -2
            start = las

        self.fat[start] = -2
        disk[start].clear()

    def update(self, start, data, disk):
        # 更新从某个位置开始的所有数据，首先删除原有数据，然后写入新的数据
        self.delete(start, disk)
        return self.write(data, disk)

    def read(self, start, disk):
        data = ""
        while self.fat[start] != -1:
            data += disk[start].read()
            start = self.fat[start]
        data += disk[start].read()
        return data

    # 获得已经使用的块所占的百分比
    def get_usage_percentage(self):
        # 计算已经被占用的块的数量
        used_blocks = sum(1 for block in self.fat if block != -2)
        # 计算总的块的数量
        total_blocks = len(self.fat)
        # 计算并返回空间占用的百分比
        return (used_blocks / total_blocks) * 100

# 文件控制块
class FCB:
    def __init__(self, name, createTime, data, fat, disk):
        # 文件名
        self.name = name
        # 创建时间
        self.createTime = createTime
        # 最后修改时间
        self.updateTime = self.createTime
        # 根据data为其分配空间
        self.start = -1

    # 更新文件内容
    def update(self, newData, fat, disk):
        self.start = fat.update(self.start, newData, disk)

    # 删除文件
    def delete(self, fat, disk):
        fat.delete(self.start, disk)

    # 读取文件内容
    def read(self, fat, disk):
        if self.start == -1:
            return ""
        else:
            return fat.read(self.start, disk)

# 多级目录结点
class Catalog:
    def __init__(self, name, isFile, fat, disk, createTime, parent=None, data=""):
        # 结点名称
        self.name = name
        # 结点是否为文件类型
        self.isFile = isFile
        # 结点的父结点
        self.parent = parent
        # 结点的创建时间
        self.createTime = createTime
        # 结点的最后更新时间
        self.updateTime = createTime

        # 如果结点是文件夹类型
        if not self.isFile:
            # 存储子结点的列表
            self.children = []
        # 如果结点是文件类型，创建一个FCB来存储文件数据
        else:
            self.data = FCB(name, createTime, data, fat, disk)