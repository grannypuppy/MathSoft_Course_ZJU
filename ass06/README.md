此页面在[mathweb](https://zju_math.pages.zjusct.io/mathweb/applied/MathSoft/#_6)同步更新。

## 作业六要求&评分标准

**本次作业截止时间为6月1日23:59。**

### 作业要求

编译运行`gsl_test.c`并输出结果，并附上latex报告pdf，pdf中需要有程序运行结果截图，**截全屏，不要只截一个结果。**

**你必须使用我提供的`gsl-latest.tar.gz`来编译安装`libgsl`，不能使用`apt`安装，出了错误概不负责。**

以下是部分有用的编译流程：
```bash
tar -zxvf gsl-latest.tar.gz # 解压

# 进入解压后的文件夹
cd gsl-2.8

./configure --prefix=$HOME/usr/gsl # 配置安装路径

make -j4 # 编译运行，建议使用多线程，至于多少线程自己决定

make install # 安装
```

你一定要编译安装在`$HOME/usr/gsl`下。

你需要提交的文件包括

- **`make install`后的`gsl-2.8`下的`Makefile`，将其重命名为`Makefile.gsl`上传到你的`ass06`文件夹下。**

- 一份`Makefile`，用来编译`gsl_test.c`，请注意使用`$HOME`来代表你的用户目录，否则我这边将无法编译。

- `gsl_test.c`源文件。

- `report.tex/report.pdf`，包含运行结果截图，你的`prefix`等，以及你为了编译成功所做的其他必须的修改。

- **编译后的二进制文件。**


### 作业提交

在学在浙大上提交以下三种格式的任意一种链接即可
```bash
http://10.72.190.121:3000/shanxue/Note # 直接地址栏复制可能有问题，不推荐
http://10.72.190.121:3000/shanxue/Note.git
gitea@10.72.190.121:shanxue/Note.git
```
