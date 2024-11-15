import sys
from typing import Optional
import PyQt5
from PyQt5.QtCore import QSize
from PyQt5.Qt import *
from src import *

# 文件编辑框
class EditingInterface(QWidget):
    _signal = PyQt5.QtCore.pyqtSignal(str)

    def __init__(self, name, data):
        super().__init__()
        self.resize(1200, 800)
        self.setWindowTitle(name)
        self.name = name
        self.setWindowIcon(QIcon('img/file.png'))

        self.resize(600, 400)
        self.text_edit = QTextEdit(self)  # 实例化一个QTextEdit对象
        self.text_edit.setText(data)  # 设置编辑框初始化时显示的文本
        self.text_edit.setPlaceholderText("在此输入文件内容")  # 设置占位字符串
        self.text_edit.textChanged.connect(self.changeMessage)  # 判断文本是否发生改变
        self.initialData = data

        # 创建布局
        self.h_layout = QHBoxLayout()
        self.v_layout = QVBoxLayout()

        self.v_layout.addWidget(self.text_edit)
        self.v_layout.addLayout(self.h_layout)

        # 创建状态栏
        self.status_bar = QStatusBar(self)
        self.v_layout.addWidget(self.status_bar)
        self.update_status_bar()

        self.setLayout(self.v_layout)

        self.setWindowModality(PyQt5.QtCore.Qt.ApplicationModal)

    def update_status_bar(self):
        text_length = len(self.text_edit.toPlainText())
        self.status_bar.showMessage(f'共 {text_length} 字')

    def closeEvent(self, event):
        # 如果打开后没有修改，则直接关闭即可
        if self.initialData == self.text_edit.toPlainText():
            event.accept()
            return

        reply = QMessageBox()
        reply.setWindowTitle('提示')
        reply.setWindowIcon(QIcon("img/file.png"))
        reply.setText('保存对文件 "' + self.name + '" 的更改吗？')
        reply.setStandardButtons(QMessageBox.Yes | QMessageBox.No | QMessageBox.Ignore)
        buttonY = reply.button(QMessageBox.Yes)
        buttonY.setText('保存')
        buttonN = reply.button(QMessageBox.No)
        buttonN.setText('不保存')
        buttonI = reply.button(QMessageBox.Ignore)
        buttonI.setText('取消')

        reply.exec_()

        if reply.clickedButton() == buttonI:
            event.ignore()
        elif reply.clickedButton() == buttonY:
            self._signal.emit(self.text_edit.toPlainText())
            event.accept()
        else:
            event.accept()

    def changeMessage(self):
        pass

# 属性显示框
class AttributeInterface(QWidget):
    def __init__(self, name, isFile, createTime, updateTime, child=0):
        super().__init__()
        self.setWindowTitle('属性')
        self.setWindowIcon(QIcon('img/attribute.png'))
        self.layout = QVBoxLayout(self)

        name_icon = QLabel()
        name_label = QLabel(name)
        if isFile:
            pixmap = QPixmap('img/file.png')
        else:
            pixmap = QPixmap('img/folder.png')

        # 调整图像大小
        scaled_pixmap = pixmap.scaled(50, 50, Qt.KeepAspectRatio)
        name_icon.setPixmap(scaled_pixmap)

        # 创建一个水平布局并添加图标和名称
        name_layout = QHBoxLayout()
        name_layout.addWidget(name_icon)
        name_layout.addWidget(name_label)
        name_layout.addItem(QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum))
        self.layout.addLayout(name_layout)

        # 创建时间
        self.layout.addWidget(QLabel(f"创建时间: {self.format_time(createTime)}"))

        # 更新时间或内部项目
        if isFile:
            self.layout.addWidget(QLabel(f"修改时间: {self.format_time(updateTime)}"))
        else:
            self.layout.addWidget(QLabel(f"内部项目: {child}"))

        # 设置主布局
        self.setLayout(self.layout)

    def format_time(self, t):
        return f"{t.tm_year}年{t.tm_mon}月{t.tm_mday}日 {t.tm_hour:02d}:{t.tm_min:02d}:{t.tm_sec:02d}"

# 列表
class ListWidget(QListWidget):
    def __init__(self, curNode, parents, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        # 拖拽设置
        self.setAcceptDrops(True)
        self.setDragEnabled(True)
        self.setDragDropMode(QAbstractItemView.DragDrop)  # 设置拖放
        self.setSelectionMode(QAbstractItemView.ExtendedSelection)  # 设置选择多个
        self.setDefaultDropAction(Qt.CopyAction)
        # 双击可编辑
        self.edited_item = self.currentItem()
        self.close_flag = True
        self.currentItemChanged.connect(self.close_edit)

        self.curNode = curNode
        self.parents = parents
        # 正在被编辑状态
        self.isEdit = False

    def keyPressEvent(self, e: QKeyEvent) -> None:
        super().keyPressEvent(e)
        if e.key() == Qt.Key_Return:
            if self.close_flag:
                self.close_edit()
            self.close_flag = True

    def edit_new_item(self) -> None:
        self.close_flag = False
        self.close_edit()
        count = self.count()
        self.addItem('')
        item = self.item(count)
        self.edited_item = item
        self.openPersistentEditor(item)
        self.editItem(item)

    def item_double_clicked(self, modelindex: QModelIndex) -> None:
        return

    def editLast(self, index=-1) -> None:
        self.close_edit()
        item = self.item(self.count() - 1)
        self.setCurrentItem(item)
        self.edited_item = item
        self.openPersistentEditor(item)
        self.editItem(item)
        self.isEdit = True
        self.index = index

    def editSelected(self, index) -> None:
        self.close_edit()
        item = self.selectedItems()[-1]
        self.setCurrentItem(item)
        self.edited_item = item
        self.openPersistentEditor(item)
        self.editItem(item)
        self.isEdit = True
        self.index = index

    def close_edit(self, *_) -> None:
        if self.edited_item:
            self.isEdit = False
            self.closePersistentEditor(self.edited_item)
            # 检验是否重名
            while True:
                sameName = False
                for i in range(len(self.curNode.children) - 1):
                    if self.edited_item.text() == self.curNode.children[i].name and self.index != i:
                        self.edited_item.setText(self.edited_item.text() + "(2)")
                        sameName = True
                        break
                if not sameName:
                    break

            # 计算item在其父结点的下标

            self.curNode.children[self.index].name = self.edited_item.text()
            # 更新父目录
            self.parents.update_tree()

            self.edited_item = None

    def dragEnterEvent(self, e: QDragEnterEvent) -> None:
        if e.mimeData().hasText():
            if e.mimeData().text().startswith('file:///'):
                e.accept()
        else:
            e.ignore()

    def dragMoveEvent(self, e: QDragMoveEvent) -> None:
        e.accept()

    def dropEvent(self, e: QDropEvent) -> None:
        paths = e.mimeData().text().split('\n')
        for path in paths:
            path = path.strip()
            if len(path) > 8:
                self.addItem(path.strip()[8:])
        e.accept()


class FileSystemUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initProject()  # 读取外存中内容
        # 设置根目录为目录的第一个元素
        self.curNode = self.catalog[0]
        self.rootNode = self.curNode
        self.baseUrl = ['root']
        self.lastLoc = -1  # 用于返回上层目录
        # 初始化
        self.initUI()

    def initUI(self):
        # 窗体信息
        self.resize(1200, 800)
        self.setWindowTitle('File System')
        self.setWindowIcon(QIcon('img/folder.ico'))
        window_geometry = self.frameGeometry()
        center_point = QDesktopWidget().availableGeometry().center()
        window_geometry.moveCenter(center_point)
        self.move(window_geometry.topLeft())
        self.grid = QGridLayout()
        self.grid.setSpacing(10)
        self.widGet = QWidget()
        self.widGet.setLayout(self.grid)
        self.setCentralWidget(self.widGet)

        # 菜单栏
        menubar = self.menuBar()
        menubar.addAction('格式化', self.format)

        # 工具栏
        self.tool_bar = self.addToolBar('工具栏')

        # 返回键
        self.back_action = QAction(QIcon('img/back.png'), '&返回', self)
        self.back_action.setShortcut('Backspace')
        self.back_action.triggered.connect(self.backward)
        self.tool_bar.addAction(self.back_action)
        self.back_action.setEnabled(False)

        # 前进键
        self.forward_action = QAction(QIcon('img/forward.png'), '&前进', self)
        self.forward_action.triggered.connect(self.forward)
        self.tool_bar.addAction(self.forward_action)
        self.forward_action.setEnabled(False)
        self.tool_bar.addSeparator()

        # 当前所在路径
        self.cur_address = QLineEdit()
        self.cur_address.setText(' > root')
        self.cur_address.setReadOnly(True)
        self.cur_address.addAction(QIcon('img/folder.png'), QLineEdit.LeadingPosition)
        self.cur_address.setMinimumHeight(40)

        # 修改布局
        ptrLayout = QFormLayout()
        ptrLayout.addRow(self.cur_address)
        ptrWidget = QWidget()
        ptrWidget.setLayout(ptrLayout)
        ptrWidget.adjustSize()
        self.tool_bar.addWidget(ptrWidget)
        self.tool_bar.setMovable(False)

        # 地址树
        # 设置一个地址树部件
        self.tree = QTreeWidget()
        # 设置标题
        self.tree.setHeaderLabels(['快速访问'])
        # 设置列数
        self.tree.setColumnCount(1)
        # 建树
        self.build_tree()
        # 设置初始状态
        self.tree.setCurrentItem(self.rootItem)
        self.treeItem = [self.rootItem]
        # 绑定单击事件
        self.tree.itemClicked['QTreeWidgetItem*', 'int'].connect(self.click_item)
        # 将其位置绑定在第2行的第一列
        self.grid.addWidget(self.tree, 1, 0)

        self.listView = ListWidget(self.curNode, parents=self)
        self.listView.setMinimumWidth(800)
        self.listView.setViewMode(QListView.IconMode)
        self.listView.setIconSize(QSize(72, 72))
        self.listView.setGridSize(QSize(100, 100))
        self.listView.setResizeMode(QListView.Adjust)
        self.listView.setMovement(QListView.Static)
        self.listView.setEditTriggers(QAbstractItemView.AllEditTriggers)
        self.listView.doubleClicked.connect(self.open_file)

        # 加载当前路径文件
        self.load_cur_address()

        # 加载右击菜单
        self.listView.setContextMenuPolicy(Qt.CustomContextMenu)
        self.listView.customContextMenuRequested.connect(self.show_menu)

        # 将其位置绑定在第2行的第2列
        self.grid.addWidget(self.listView, 1, 1)

        # 建立一个标签，显示空间使用情况
        self.usage_label = QLabel(self)
        self.usage_label.setAlignment(Qt.AlignLeft)
        usage_percentage = self.fat.get_usage_percentage() * 100
        self.usage_label.setText(f"空间使用情况: {usage_percentage:.2f}%")
        self.grid.addWidget(self.usage_label, 2, 0, 1, 2)

        # 删除文件快捷键
        QShortcut(QKeySequence(self.tr("Delete")), self, self.delete)

        self.update_address_bar()

    def update_usage_label(self):
        usage_percentage = self.fat.get_usage_percentage() * 100
        self.usage_label.setText(f"空间使用情况: {usage_percentage:.2f}%")

    # 读外存中的内容
    def initProject(self):
        # 读取fat表
        if not os.path.exists('fat'):
            self.fat = FAT()
            self.fat.fat = [-2] * blockNum
            # 存储fat表
            with open('fat', 'wb') as f:
                f.write(pickle.dumps(self.fat))
        else:
            with open('fat', 'rb') as f:
                self.fat = pickle.load(f)

        # 读取disk表
        if not os.path.exists('disk'):
            self.disk = []
            for i in range(blockNum):
                self.disk.append(Block(i))
            # 存储disk表
            with open('disk', 'wb') as f:
                f.write(pickle.dumps(self.disk))
        else:
            with open('disk', 'rb') as f:
                self.disk = pickle.load(f)

        # 读取catalog表
        if not os.path.exists('catalog'):
            self.catalog = []
            self.catalog.append(Catalog("root", False, self.fat, self.disk, time.localtime(time.time())))
            # 存储
            with open('catalog', 'wb') as f:
                f.write(pickle.dumps(self.catalog))
        else:
            with open('catalog', 'rb') as f:
                self.catalog = pickle.load(f)

    def build_tree(self):
        self.tree.clear()

        # 递归构建树
        def buildTreeRecursive(node: Catalog, parent: QTreeWidgetItem):
            child = QTreeWidgetItem(parent)
            child.setText(0, node.name)

            if node.isFile:
                child.setIcon(0, QIcon('img/file.png'))
            else:
                if len(node.children) == 0:
                    child.setIcon(0, QIcon('img/folder.png'))
                else:
                    child.setIcon(0, QIcon('img/folderWithFile.png'))
                for i in node.children:
                    buildTreeRecursive(i, child)

            return child

        self.rootItem = buildTreeRecursive(self.catalog[0], self.tree)
        # 加载根节点的所有子控件
        self.tree.addTopLevelItem(self.rootItem)
        self.tree.expandAll()

    # 格式化
    def format(self):
        # 结束编辑
        self.listView.close_edit()

        # 提示框
        reply = QMessageBox()
        reply.setWindowIcon(QIcon("img/folder.png"))
        reply.setWindowTitle('提示')
        reply.setText('确定要格式化磁盘吗？此操作不可逆！')
        reply.setStandardButtons(QMessageBox.Yes | QMessageBox.No)
        buttonY = reply.button(QMessageBox.Yes)
        buttonY.setText('确定')
        buttonN = reply.button(QMessageBox.No)
        buttonN.setText('取消')
        reply.exec_()
        reply.show()

        if reply.clickedButton() == buttonN:
            return

        # 将FAT清空
        self.fat = FAT()
        self.fat.fat = [-2] * blockNum
        with open('fat', 'wb') as f:
            f.write(pickle.dumps(self.fat))

        # 清空磁盘
        self.disk = []
        for i in range(blockNum):
            self.disk.append(Block(i))
        with open('disk', 'wb') as f:
            f.write(pickle.dumps(self.disk))

        # 清空目录
        self.catalog = []
        self.catalog.append(Catalog("root", False, self.fat, self.disk, time.localtime(time.time())))
        with open('catalog', 'wb') as f:
            f.write(pickle.dumps(self.catalog))

        # 重新加载页面
        self.hide()
        self.main_window = FileSystemUI()
        self.main_window.show()

        self.update_tree()

    # 点击树点跳转
    def click_item(self, item, column):
        ways = [item]
        # 将切换跳转路径打印出来
        temp = item
        while temp.parent() is not None:
            temp = temp.parent()
            ways.append(temp)
        ways.reverse()
        # 回退到根节点
        while self.backward():
            pass
        # 将路径和路径树都作为根节点
        self.baseUrl = self.baseUrl[:1]
        self.treeItem = self.treeItem[:1]

        # 获得寻址路径
        for i in ways:
            if i == self.rootItem:
                continue
            # 前往该路径
            # 从curNode中查询item
            newNode = next((j for j in self.curNode.children if j.name == i.text(0)), None)
            # 前往路径j
            if newNode is not None and not newNode.isFile:
                self.curNode = newNode
                # 更新当前位置
                self.load_cur_address()
                self.listView.curNode = self.curNode
                self.baseUrl.append(newNode.name)

                # 更新路径
                selectedItem = next((self.treeItem[-1].child(j) for j in range(self.treeItem[-1].childCount()) if
                                     self.treeItem[-1].child(j).text(0) == newNode.name), None)
                if selectedItem is not None:
                    self.treeItem.append(selectedItem)
                    self.tree.setCurrentItem(selectedItem)
                else:
                    break

        # 更新地址栏内容
        self.update_address_bar()
        # 设置回退/前进键的可用状态
        self.back_action.setEnabled(self.curNode != self.rootNode)
        self.forward_action.setEnabled(False)
        # 将上一次的位置记为lastLoc
        self.lastLoc = -1

    # 打开文件
    def open_file(self, modelindex: QModelIndex) -> None:
        # 关闭可能正在进行的文件编辑
        self.listView.close_edit()

        try:
            # 尝试获取用户点击的项目
            item = self.listView.item(modelindex.row())
        except:
            # 如果出错，可能是因为用户使用了右键打开菜单
            # 如果没有选中的项目，直接返回
            if len(self.listView.selectedItems()) == 0:
                return
            # 否则，获取最后一个选中的项目
            item = self.listView.selectedItems()[-1]

        # 如果可以前进（即lastLoc不等于-1），并且nextStep为True
        if self.lastLoc != -1 and self.nextStep:
            # 获取lastLoc对应的项目，并重置lastLoc和nextStep
            item = self.listView.item(self.lastLoc)
            self.lastLoc = -1
            self.forward_action.setEnabled(False)
        self.nextStep = False

        # 在当前节点的子节点中查找与项目名称相同的节点
        newNode = None
        for i in self.curNode.children:
            if i.name == item.text():
                newNode = i
                break

        # 获取数据并向文件中写入新数据
        def getData(parameter):
            newNode.data.update(parameter, self.fat, self.disk)
            newNode.updateTime = time.localtime(time.time())

        # 如果找到的节点是文件
        if newNode.isFile:
            # 读取文件数据，并打开一个编辑窗口显示文件内容
            data = newNode.data.read(self.fat, self.disk)
            self.child = EditingInterface(newNode.name, data)
            self.child._signal.connect(getData)
            self.child.show()
            self.writeFile = newNode
        # 如果找到的节点是目录(文件夹)
        else:
            # 关闭可能正在进行的文件编辑
            self.listView.close_edit()

            # 更新当前节点，并加载新节点的文件
            self.curNode = newNode
            self.load_cur_address()
            self.listView.curNode = self.curNode

            # 更新当前路径
            self.baseUrl.append(newNode.name)

            # 在树状视图中找到新节点对应的项目，并设置为当前项目
            for i in range(self.treeItem[-1].childCount()):
                if self.treeItem[-1].child(i).text(0) == newNode.name:
                    selectedItem = self.treeItem[-1].child(i)
            self.treeItem.append(selectedItem)
            self.tree.setCurrentItem(selectedItem)
            self.back_action.setEnabled(True)

            # 更新地址栏
            self.update_address_bar()

        self.update_tree()

    # 返回上一级
    def backward(self):
        self.listView.close_edit()

        if self.rootNode == self.curNode:
            # 根节点无法返回
            return False

        # 记录上次所在位置
        for i in range(len(self.curNode.parent.children)):
            if self.curNode.parent.children[i].name == self.curNode.name:
                self.lastLoc = i
                self.forward_action.setEnabled(True)
                break

        self.curNode = self.curNode.parent
        # 更新当前位置
        self.load_cur_address()
        self.listView.curNode = self.curNode

        self.baseUrl.pop()
        self.treeItem.pop()
        self.tree.setCurrentItem(self.treeItem[-1])
        self.update_tree()
        self.update_address_bar()

        if self.curNode == self.rootNode:
            self.back_action.setEnabled(False)

        return True

    # 跳转下一级
    def forward(self):
        self.nextStep = True
        self.open_file(QModelIndex())

    # 更新地址栏
    def update_address_bar(self):
        self.statusBar().showMessage(str(len(self.curNode.children)) + '个项目')
        s = '> root'
        for i, item in enumerate(self.baseUrl):
            if i == 0:
                continue
            s += " > " + item
        self.cur_address.setText(s)
        self.update_tree()

    # 重命名
    def rename(self):
        if len(self.listView.selectedItems()) == 0:
            return
        # 获取最后一个被选中的
        self.listView.editSelected(self.listView.selectedIndexes()[-1].row())
        self.update_tree()

    # 删除文件/文件夹
    def delete(self):
        if len(self.listView.selectedItems()) == 0:
            return

        item = self.listView.selectedItems()[-1]
        index = self.listView.selectedIndexes()[-1].row()

        reply = QMessageBox()
        reply.setWindowIcon(QIcon("img/folder.ico"))
        reply.setWindowTitle('提示')

        if self.curNode.children[index].isFile:
            reply.setText('确定要删除文件 "' + item.text() + '" 吗？')
        else:
            reply.setText('确定要删除文件夹 "' + item.text() + '" 及其内部所有内容吗？')
        reply.setStandardButtons(QMessageBox.Yes | QMessageBox.No)
        buttonY = reply.button(QMessageBox.Yes)
        buttonY.setText('删除')
        buttonN = reply.button(QMessageBox.No)
        buttonN.setText('取消')

        reply.exec_()

        if reply.clickedButton() == buttonN:
            return

        # 删除文件
        self.listView.takeItem(index)
        del item

        # 递归删除文件
        def deleteFileRecursive(node):
            if node.isFile:
                node.data.delete(self.fat, self.disk)
            else:
                for i in node.children:
                    deleteFileRecursive(i)

        # 删除fat表中的内容
        deleteFileRecursive(self.curNode.children[index])
        self.curNode.children.remove(self.curNode.children[index])

        # 更新目录表
        def updateCatalog(node):
            if node.isFile:
                return [node]
            else:
                x = [node]
                for i in node.children:
                    x += updateCatalog(i)
                return x

        # 更新catalog表
        self.catalog = updateCatalog(self.rootNode)

        # 更新
        self.update_tree()

    # 创建文件夹
    def create_folder(self):

        self.item_1 = QListWidgetItem(QIcon("img/folder.png"), "新建文件夹")
        self.listView.addItem(self.item_1)
        self.listView.editLast()

        # 添加到目录表中
        newNode = Catalog(self.item_1.text(), False, self.fat, self.disk, time.localtime(time.time()), self.curNode)
        self.curNode.children.append(newNode)
        self.catalog.append(newNode)

        # 更新树
        self.update_tree()

    # 创建文件
    def create_file(self):
        self.item_1 = QListWidgetItem(QIcon("img/file.png"), "新建文件")
        self.listView.addItem(self.item_1)
        self.listView.editLast()

        # 添加到目录表中
        newNode = Catalog(self.item_1.text(), True, self.fat, self.disk, time.localtime(time.time()), self.curNode)
        self.curNode.children.append(newNode)
        self.catalog.append(newNode)

        # 更新树
        self.update_tree()

    # 右击项目，展示菜单
    def show_menu(self, point):
        # 创建一个菜单，并将其关联到列表视图
        menu = QMenu(self.listView)

        # 展示其属性
        def viewAttribute():
            # 查看当前路径属性
            if len(self.listView.selectedItems()) == 0:
                self.child = AttributeInterface(self.curNode.name, False, self.curNode.createTime,
                                                self.curNode.updateTime,
                                                len(self.curNode.children))

                self.child.show()
                return
            else:
                # 获取选中的最后一个
                node = self.curNode.children[self.listView.selectedIndexes()[-1].row()]
                if node.isFile:
                    self.child = AttributeInterface(node.name, node.isFile, node.createTime, node.updateTime, 0)
                else:
                    self.child = AttributeInterface(node.name, node.isFile, node.createTime, node.updateTime,
                                                    len(node.children))
                self.child.show()
                return

        # 如果用户选中元素
        if len(self.listView.selectedItems()) != 0:
            # 创建打开文件的操作
            action_open_file = QAction(QIcon('img/open.png'), '打开')
            action_open_file.triggered.connect(self.open_file)
            menu.addAction(action_open_file)

            # 创建删除文件的操作
            action_delete_file = QAction(QIcon('img/delete.png'), '删除')
            action_delete_file.triggered.connect(self.delete)
            menu.addAction(action_delete_file)

            # 创建重命名文件的操作
            action_rename_file = QAction(QIcon('img/rename.png'), '重命名')
            action_rename_file.triggered.connect(self.rename)
            menu.addAction(action_rename_file)

            # 创建查看属性的操作
            action_view_attributes = QAction(QIcon('img/attribute.png'), '属性')
            action_view_attributes.triggered.connect(viewAttribute)
            menu.addAction(action_view_attributes)


        # 用户没选中元素(即右键空白处)
        else:
            # 创建新建菜单
            createMenu = QMenu(menu)
            createMenu.setTitle('新建')
            createMenu.setIcon(QIcon('img/create.png'))

            # 创建新建文件夹的操作
            createFolderAction = QAction(QIcon('img/folder.png'), '文件夹')
            createFolderAction.triggered.connect(self.create_folder)
            createMenu.addAction(createFolderAction)

            # 创建新建文件的操作
            createFileAction = QAction(QIcon('img/file.png'), '文件')
            createFileAction.triggered.connect(self.create_file)
            createMenu.addAction(createFileAction)

            menu.addMenu(createMenu)

            # 创建查看属性的操作
            action_view_attributes = QAction(QIcon('img/attribute.png'), '属性')
            action_view_attributes.triggered.connect(viewAttribute)
            menu.addAction(action_view_attributes)

        # 显示菜单
        dest_point = self.listView.mapToGlobal(point)
        menu.exec_(dest_point)

    # 更新文件列表
    def update_tree(self):
        node = self.rootNode
        item = self.rootItem

        # 内部函数，用于递归更新树
        def updateTreeRecursive(node: Catalog, item: QTreeWidgetItem):
            item.setText(0, node.name)
            if node.isFile:
                item.setIcon(0, QIcon('img/file.png'))
            else:
                # 根据是否有子树设置图标
                if len(node.children) == 0:
                    item.setIcon(0, QIcon('img/folder.png'))
                else:
                    item.setIcon(0, QIcon('img/folder.png'))
                if item.childCount() < len(node.children):
                    # 增加一个新item即可
                    child = QTreeWidgetItem(item)
                elif item.childCount() > len(node.children):
                    # 一个一个找，删除掉对应元素
                    for i in range(item.childCount()):
                        if i == item.childCount() - 1:
                            item.removeChild(item.child(i))
                            break
                        if item.child(i).text(0) != node.children[i].name:
                            item.removeChild(item.child(i))
                            break
                for i in range(len(node.children)):
                    updateTreeRecursive(node.children[i], item.child(i))

        # 增加一个新item即可
        if item.childCount() < len(node.children):
            child = QTreeWidgetItem(item)
        # 删除掉对应元素
        elif item.childCount() > len(node.children):
            for i in range(item.childCount()):
                if i == item.childCount() - 1:
                    item.removeChild(item.child(i))
                    break
                if item.child(i).text(0) != node.children[i].name:
                    item.removeChild(item.child(i))
                    break
        # 更新根节点对应的数据
        for i in range(len(node.children)):
            updateTreeRecursive(node.children[i], item.child(i))

        updateTreeRecursive(node, item)
        # 更新空间占用文本
        self.update_usage_label()

    # 加载当前文件路径
    def load_cur_address(self):
        self.listView.clear()

        for i in self.curNode.children:
            if i.isFile:
                self.item_1 = QListWidgetItem(QIcon("img/file.png"), i.name)
                self.listView.addItem(self.item_1)
            else:
                if len(i.children) == 0:
                    self.item_1 = QListWidgetItem(QIcon("img/folder.png"), i.name)
                else:
                    self.item_1 = QListWidgetItem(QIcon("img/folder.png"), i.name)
                self.listView.addItem(self.item_1)

    # 关闭程序前询问
    def closeEvent(self, event):
        # 结束编辑
        self.listView.close_edit()

        reply = QMessageBox()
        reply.setWindowTitle('提示')
        reply.setWindowIcon(QIcon("img/folder.png"))
        reply.setText('您是否需将本次操作写入磁盘？')
        reply.setStandardButtons(QMessageBox.Yes | QMessageBox.Ignore | QMessageBox.No)
        buttonY = reply.button(QMessageBox.Yes)
        buttonY.setText('写入')
        buttonN = reply.button(QMessageBox.No)
        buttonN.setText('取消')
        buttonI = reply.button(QMessageBox.Ignore)
        buttonI.setText('不写入')

        reply.exec_()

        if reply.clickedButton() == buttonI:
            event.accept()
        elif reply.clickedButton() == buttonY:
            # 将内存中的文件存到本地
            # 存储fat表
            with open('fat', 'wb') as f:
                f.write(pickle.dumps(self.fat))
            # 存储disk表
            with open('disk', 'wb') as f:
                f.write(pickle.dumps(self.disk))
            # 存储
            with open('catalog', 'wb') as f:
                f.write(pickle.dumps(self.catalog))

            event.accept()
        else:
            event.ignore()

if __name__ == '__main__':
    app = QApplication(sys.argv)

    mainform = FileSystemUI()

    mainform.show()

    sys.exit(app.exec_())