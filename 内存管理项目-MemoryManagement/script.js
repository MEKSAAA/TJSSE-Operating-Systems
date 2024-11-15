// 页
class Page {
    constructor(id) {
        this.id = id;
        this.instructions = Array.from({ length: 10 }, (_, i) => id * 10 + i);
    }
}

// 内存块
class Memory{
    constructor(size){
        this.size=size;  // 内存的总页数
        this.method='FIFO';  //后续使用的置换算法
        this.frames=[];  // 存储当前在内存中的页
        this.lastUsedTime=[0,0,0,0];  // 记录每个页上次使用的时间
    }

    setMethod(method){
        this.method=method;
    }

    // 查看内存中是否有编号为id的指令
    hasInstruction(id,time){
        let i=this.frames.findIndex(page=>page.instructions.includes(id));  //检查页面的 instructions 数组中是否包含给定的编号为id指令
        if(i!==-1){
            if(this.method==='LRU')
                this.lastUsedTime[i]=time;
            return true;
        }
        return false;
    }
}

// 清除内存
function clearMemory()
{
    var table = document.getElementById("memory_table");
    // 删除所有行,表头除外
    while (table.rows.length>1) {
        table.deleteRow(1);
    }
}

function fifo()
{
    clearMemory();
    // 创建一个包含 32 个 Page 对象的数组
    let pages = Array.from({ length: 32 }, (_, i) => new Page(i));
    // 创建可容纳 4 页的内存
    let memory = new Memory(4);
    memory.setMethod('FIFO')
    let instructions=generateInstructions();
    let totalMiss=0;  // 缺页次数
    let visit=0;  // 当前访问次数
    var block=0;  // 记录内存块号
    for(let i of instructions){
        visit++;
        if(memory.hasInstruction(i,visit)){
            addToMemoryTable(visit,i,memory,"否",null,null);
        }
        else{
            totalMiss++;
            // 若内存已经装满，则需要置换，否则直接放入
            var pageRemoved=0;
            if(memory.frames.length<memory.size)
                pageRemoved=null;
            else{
                pageRemoved=memory.frames[block].id;
            }
            memory.frames[block]=pages[Math.floor(i/10)];
            memory.lastUsedTime[block]=visit;
            addToMemoryTable(visit,i,memory,"是",block,pageRemoved);
            block=(block+1)%4;
        }
    }
    return totalMiss/320;  //返回缺页率
}

function lru()
{
    clearMemory();
    // 创建一个包含 32 个 Page 对象的数组
    let pages = Array.from({ length: 32 }, (_, i) => new Page(i));
    // 创建可容纳 4 页的内存
    let memory = new Memory(4);
    memory.setMethod('LRU')
    let instructions=generateInstructions();
    let totalMiss=0;  // 缺页次数
    let visit=0;  // 当前访问次数
    var block=0;  // 记录内存块号
    for(let i of instructions){
        visit++;
        if(memory.hasInstruction(i,visit)){
            addToMemoryTable(visit,i,memory,"否",null,null);
        }
        else{
            totalMiss++;
            block=memory.frames.findIndex(page => page===undefined);  // 在内存中找到第一个未使用的内存块编号
            if(block===-1){
                block=memory.lastUsedTime.indexOf(Math.min(...memory.lastUsedTime));  // 找最早使用的索引
            }
            var pageRemoved=0;
            if(memory.frames[block]===undefined)
                pageRemoved=null;
            else
                pageRemoved=memory.frames[block].id;
            memory.frames[block]=pages[Math.floor(i/10)];
            memory.lastUsedTime[block]=visit;
            addToMemoryTable(visit,i,memory,"是",block,pageRemoved);
        }
    }
    return totalMiss/320;  //返回缺页率
}

// 生成指令
function generateInstructions()
{
    console.log("generateInstruction")
    let instructions = Array.from({ length: 320 }, (_, i) => i);
    let count=0; //记录执行次数
    let executionOrder=[];  //  按执行顺序存放的指令
    while(count<320){
        let m=Math.floor(Math.random()*320) // 随机生成 0-319
        executionOrder.push(m);
        count++;

        if(m+1<320&&count<320){
            executionOrder.push(m+1);
            count++;
        }

        // 前地址
        if(m>0&&count<320){
            let m1=Math.floor(Math.random()*m);  // 在0-m-1中选择
            executionOrder.push(m1);
            count++;

            if(m1+1<m&&count<320){
                executionOrder.push(m1+1);
                count++;
            }
        }

        // 后地址
        if(m+2<320&&count<320){
            let m2=m+2+Math.floor(Math.random()*(319-m-1));  // 在m+2-319中选择
            executionOrder.push(m2);
            count++;

            if(m2+1<320&&count<320){
                executionOrder.push(m2+1);
                count++;
            }
        }        
    }

    return executionOrder;
}

function addToMemoryTable(count,instructionId,memory,isMissing,blockInserted,pageRemoved)
{
    var table = document.getElementById('memory_table');

    // 创建一个新的行元素
    var row = document.createElement('tr');
    var block1 = (memory.frames[0] === undefined ? "-" : memory.frames[0].id.toString());
    var block2 = (memory.frames[1] === undefined ? "-" : memory.frames[1].id.toString());
    var block3 = (memory.frames[2] === undefined ? "-" : memory.frames[2].id.toString());
    var block4 = (memory.frames[3] === undefined ? "-" : memory.frames[3].id.toString());

    var insertBlock = (blockInserted === null ? "-" : (blockInserted + 1).toString());
    var removePage = (pageRemoved === null ? "-" : pageRemoved.toString());

    // 创建并添加数据到新的单元格
    var data = [count, instructionId, block1, block2, block3, block4, isMissing, insertBlock, removePage];
    for (let i = 0; i < data.length; i++) {
        var cell = document.createElement('td');
        cell.textContent = data[i];

        // 根据条件为单元格添加相应的CSS类
        if (isMissing === "是") {
            cell.classList.add('miss-cell');
        } else if (isMissing === "否") {
            cell.classList.add('exist-cell');
        }
        if (insertBlock !== "-") {
            if (i === (blockInserted + 2)) { // 第3到第6列对应插入的块
                cell.classList.add('change-cell');
            }
        }
        row.appendChild(cell);
    }

    // 将新行添加到表格
    table.appendChild(row);
}

// jQuery事件处理
$(document).ready(function () {
    $("#fifo_button").click(function () {
        let missRate = fifo();
        $("#fifo_page_miss_count").text(missRate * 320);
        $("#fifo_page_miss_rate").text(missRate.toFixed(2));
        $(this).text("重启");
    });

    $("#lru_button").click(function () {
        let missRate = lru();
        $("#lru_page_miss_count").text(missRate * 320);
        $("#lru_page_miss_rate").text(missRate.toFixed(2));
        $(this).text("重启");
    });
});
