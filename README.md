# OHOS Porting Communities-开源鸿蒙系统移植社区（OPC）

目前该仓库存放有以下设备适配代码：  
树莓派4B  
小米6  
小米POCO F1  
香橙派5Plus  
FILY F12  

#### 介绍
该项目介绍，如何编译运行 [OpenHarmony 4.0 Release](https://gitee.com/openharmony/docs/blob/master/zh-cn/release-notes/OpenHarmony-v4.0-release.md)标准系统。  

# 搭建开发环境
## 1.1 硬件环境
准备一台装有ubuntu20.04系统X86主机，内存最低配置要求16G。 
## 1.2 下载repo脚本文件
1. 注册码云gitee账号。

2. 注册码云SSH公钥，请参考[码云帮助中心](https://gitee.com/help/articles/4191)。

3. 安装[git](https://git-scm.com/book/zh/v2/%E8%B5%B7%E6%AD%A5-%E5%AE%89%E8%A3%85-Git)客户端和[git-lfs](https://gitee.com/vcs-all-in-one/git-lfs?_from=gitee_search#downloading)并配置用户信息。
```
git config --global user.name "yourname"
git config --global user.email "your-email-address"
git config --global credential.helper store
```
4. 安装码云repo工具，可以执行如下命令。
```
curl -s https://gitee.com/oschina/repo/raw/fork_flow/repo-py3 > /usr/local/bin/repo  
pip3 install -i https://repo.huaweicloud.com/repository/pypi/simple requests
```
如果没有权限，可下载至其他目录，并将其配置到环境变量中chmod a+x /usr/local/bin/repo
```
mkdir ~/bin
curl https://gitee.com/oschina/repo/raw/fork_flow/repo-py3 -o ~/bin/repo 
chmod a+x ~/bin/repo
pip3 install -i https://repo.huaweicloud.com/repository/pypi/simple requests
```
```
vim ~/.bashrc               # 编辑环境变量
export PATH=~/bin:$PATH     # 在环境变量的最后添加一行repo路径信息
source ~/.bashrc            # 应用环境变量
```

## 1.3 获取OpenHarmony标准系统源码
通过repo + ssh 下载（需注册公钥，请参考[码云帮助中心](https://gitee.com/help/articles/4191)）。

```
export WORK_SPACE=/home/xxx/OpenHarmony #替换成自己定义的workspace路径
export PROJ_ROOT=$WORK_SPACE/4.0-Release
mkdir $WORK_SPACE
mkdir $PROJ_ROOT
cd $PROJ_ROOT
repo init -u https://gitee.com/openharmony/manifest -b OpenHarmony-4.0-Release --no-repo-verify
repo sync -c
repo forall -c 'git lfs pull'
```
## 1.4 获取编译工具链
参考[OpenHamony快速入门文档](https://gitee.com/openharmony/docs/tree/master/zh-cn/device-dev/quick-start)，基于命令行入门。
```
sudo apt-get update && sudo apt-get install binutils git git-lfs gnupg flex bison gperf build-essential zip curl zlib1g-dev gcc-multilib g++-multilib libc6-dev-i386 lib32ncurses5-dev x11proto-core-dev libx11-dev lib32z1-dev ccache libgl1-mesa-dev libxml2-utils xsltproc unzip m4 bc gnutls-bin python3.8 python3-pip ruby default-jdk libssl-dev libtinfo5 genext2fs u-boot-tools mtools mtd-utils scons gcc-arm-none-eabi abootimg
```
## 1.5 执行prebuilts
在源码根目录下执行脚本，安装编译器及二进制工具。
```
cd $PROJ_ROOT
bash build/prebuilts_download.sh
```

# 设备代码下载与整合
## 2.1 下载适配源码
进到项目根目录，下载设备源码：
```
git clone -b OpenHarmony-4.0-Release https://gitee.com/ohos-porting-communities/vendor_opc.git vendor/opc
git clone -b OpenHarmony-4.0-Release https://gitee.com/ohos-porting-communities/device_board_opc.git device/board/opc
git clone -b OpenHarmony-4.0-Release https://gitee.com/ohos-porting-communities/device_soc_opc.git device/soc/opc

#下载米6内核（835内核）
git clone https://gitee.com/ohos-porting-communities/linux_sagit.git kernel/linux/linux-sagit

#下载Poco F1内核（845内核）
git clone -b OpenHarmony-4.0-Release https://gitee.com/openharmony-dg/kernel_linux_6.3.git kernel/linux/linux-6.3

#下载香橙派5Plus内核
git clone https://gitee.com/ohos-porting-communities/linux_5.10_opi.git kernel/linux/linux-5.10-opi

#下载香橙派5Plus内核
git clone https://gitee.com/ohos-porting-communities/kernel_linux_5.10_f12.git kernel/linux/linux-5.10-f12

#因官方manifest在4.0分支去掉了eudev仓，poco f1电量检测依赖eudev，因此需要手动下载eudev
git clone -b OpenHarmony-4.0-Release https://gitee.com/openharmony/third_party_eudev.git third_party/eudev

```
根据编译的机型，选择合并补丁或适当修改源码：
```
补丁和说明放置在device/board/opc/common/patches目录下

代码修改说明：

#poco f1有两种机型，默认适配的是ebbg面板机型，tianma面板机型需要手动修改以下两个脚本文件
device/board/opc/beryllium/kernel/make_kernel.mk
device/board/opc/beryllium/kernel/make_bootimg.sh

#面板类型判断参考pmos的wiki
https://wiki.postmarketos.org/wiki/Xiaomi_POCO_F1_(xiaomi-beryllium)


#树莓派4B默认挂载配置为U盘启动方式，此为我为了方便调试进行的改动，
树莓派4B默认启动启动方式为从microSD卡启动，需要修改以下文件，切换挂载分区

device/board/opc/rpi4/cfg/fstab.required
device/board/opc/rpi4/cfg/fstab.rpi4

#香橙派5Plus因为我使用的显示器为4k分辨率，dpi参数较高，
编译前需要修改以下文件，修改dpi参数

vendor/opc/opi5plus/custom_conf/window/display_manager_config.xml


```

# 代码编译
## 3.1 OpenHarmony编译  
```
cd $PROJ_ROOT  

#小米POCO F1
./build.sh --product-name beryllium --ccache --no-prebuilt-sdk

#小米6
./build.sh --product-name sagit --ccache --no-prebuilt-sdk

#树莓派4B
./build.sh --product-name rpi4 --ccache --no-prebuilt-sdk

#香橙派5Plus
./build.sh --product-name opi5plus --ccache --no-prebuilt-sdk

#FILY F12
./build.sh --product-name f12 --ccache --no-prebuilt-sdk

```
 编译成功提示:
```
post_process
=====build beryllium successful.
```

编译生成的文件
```
$PROJ_ROOT/out/beryllium/packages/phone/images/system.img 
$PROJ_ROOT/out/beryllium/packages/phone/images/vendor.img
$PROJ_ROOT/out/beryllium/packages/phone/images/userdata.img
$PROJ_ROOT/out/beryllium/packages/phone/images/ramdisk.img
```


## 3.2 额外镜像编译
编译好OpenHarmony的文件之后使用以下命令内核并打包手机boot.img/树莓派4B完整镜像
```
#小米POCO F1
cd device/board/opc/beryllium/kernel
./build_kernel.sh

#小米6
cd device/board/opc/sagit/kernel
./build_kernel.sh

#树莓派4B
./build.sh --product-name rpi4 --ccache --no-prebuilt-sdk --build-target rpi_image


```

# 镜像烧写
手机镜像参考我的文章使用twrp刷入或者bootloader线刷  
https://ost.51cto.com/posts/20924

树莓派4B使用Win32DiskImager烧录整合好的rpi_image.img到microSD卡中  

香橙派5Plus和FILY F12参考主线设备rk3568的说明文档使用RKDevTool烧写  


# 开源许可协议  
Apache 2.0  

# 维护者邮箱 
开源鸿蒙系统移植社区  
598757652@qq.com  
